//==============================================================================
/*!
//  \file
//  \brief Implementation of module for Constraints
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <algorithm>
#include <exception>  // nested
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

/// All constraint types
#include "Simulator/Constraints/Types.hpp"
#include "Simulator/Interface/Constraints/Types.hpp"
/// Interface methods
#include "Simulator/Interface/Constraints/BatchingPolicy.hpp"
#include "Simulator/Interface/Constraints/Configuration.hpp"
#include "Simulator/Interface/Constraints/Constrainer.hpp"
#include "Simulator/Interface/Constraints/ConstrainerBase.hpp"
#include "Simulator/Interface/Constraints/Constraint.hpp"
#include "Simulator/Interface/Constraints/ConstraintBase.hpp"
#include "Simulator/Interface/Constraints/ConstraintUUID.hpp"
#include "Simulator/Interface/Constraints/ExecutionPolicy.hpp"
#include "Simulator/Interface/Protocols/Constraints.hpp"
#include "Simulator/Interface/Wrappers/UUIDMap.hpp"
/// Serialization support
#include "Simulator/IO/Serialization/Types.hpp"
///
#include "ErrorHandling/NestedExceptions.hpp"
#include "Utilities/CRTP.hpp"
#include "Utilities/KarmarkarKarpPartition.hpp"
#include "Utilities/Logging.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/StdVectorHelpers.hpp"

namespace elastica {

  namespace modules {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief The constraints module
     * \ingroup modules constraints
     *
     * The Constraints module is an enhancement to the \elastica Simulator
     * and is responsible for constraining the degrees of freedom of different
     * entities. One can also think of them as implementing different boundary
     * conditions on rods (or) rigid bodies.
     *
     * \tparam SimTraits Options for configuration of the Simulator
     * \tparam Sim Simulator
     *
     * \usage
     * To use Constraints, pass it as template argument to
     * ::elastica::make_simulator() as shown below. This needs to be accompanied
     * by an instantiation of
     * ::elastica::configuration::ConstraintsConfiguration as an argument to
     * ::elastica::make_simulator().
     *
     * \code
     * auto constraints_configuration =
     * ::elastica::configuration::DefaultConstraintsConfiguration{};
     *
     * auto sim =
     * ::elastica::make_simulator<::elastica::modules::Constraints>(
     * ::elastica::configuration::DefaultSystemsConfiguration{},
     * ::elastica::configuration::DefaultDomainConfiguration{},
     *  constraints_configuration
     * );
     * \endcode
     * And the simulator `sim` now has capabilities of constraining systems. The
     * sole entry point of constraints is the #constrain method for which we
     * show examples below.
     *
     * \example
     * For the setup of a system shown below...
     * \snippet Constraints/Test_Module.cpp constrain_setup
     * ...one can launch a constraint of a system in one of many ways, each
     * intended to promote readability and be as clear about the intent as
     * possible, as shown below:
     * \snippet Constraints/Test_Module.cpp constrain_eg
     * \snippet Constraints/Test_Module.cpp constrain_eg_curry
     * \snippet Constraints/Test_Module.cpp constrain_eg_no_curry
     * \snippet Constraints/Test_Module.cpp constrain_eg_with_name
     *
     * It is useful to get a handle back to the constraint to manipulate/record
     * it later on in the client script. For this purpose, after completing the
     * constraint, one gets a key which can be used to retrieve the constraint
     * after finalizing the simulator. This is shown below:
     * \snippet Constraints/Test_Module.cpp query_eg
     *
     * \see ::elastica::make_simulator()
     */
    template <class SimTraits, class Sim>
    class Constraints
        : public CRTPHelper<Sim, Constraints>,
          public ::tt::ConformsTo<
              ::elastica::interface::protocols::Constraints>,
          public NonCopyable,
          private detail::HasConstraintsConfiguration<SimTraits>,
          protected ::elastica::interface::constraints::ExecutionPolicy<
              SimTraits, Constraints<SimTraits, Sim>>,
          protected ::elastica::interface::constraints::BatchingPolicy<
              SimTraits> {
     private:
      //**Compile-time checks***************************************************
      //
      //************************************************************************

      //**Type definitions******************************************************
      //! This type
      using This = Constraints<SimTraits, Sim>;
      //! CRTP Type
      using CRTP = CRTPHelper<Sim, Constraints>;
      //! CRTP methods
      using CRTP::self;
      //! Internal size type
      using SizeType = typename SimTraits::SizeType;
      //! Internal type of index
      using IndexType = typename SimTraits::IndexType;
      //! Algorithm type
      using Algorithm =
          ::elastica::interface::constraints::ExecutionPolicy<SimTraits, This>;
      //! Batch type
      using BatchAlgorithm =
          ::elastica::interface::constraints::BatchingPolicy<SimTraits>;
      //! UUID type
      using UUID = ::elastica::interface::constraints::ConstraintUUID;
      //! Map from system to uuid
      using UUIDMap = ::elastica::interface::detail::UUIDMap<UUID>;
      //! Map from uuid to batch
      using UUIDToBatch = ::elastica::interface::detail::UUIDToBatch<UUID>;
      //************************************************************************

     public:
      //**Type definitions******************************************************
      //! Type of simulation time
      using TimeType = typename SimTraits::TimeType;  // public for conformation
      //! ID Type of constraint applicator
      using ConstrainerID =
          std::unique_ptr<::elastica::interface::constraints::ConstrainerBase>;
      //! Type of constraint registrars
      using ConstraintsRegistrars = typename SimTraits::ConstraintsRegistrars;
      //! ID Type of constraints
      using ConstraintID =
          std::unique_ptr<::elastica::interface::constraints::ConstraintBase<
              ConstraintsRegistrars>>;
      //! A batch of constrainers
      using ConstrainerBatch = std::vector<ConstrainerID>;
      //! Batches of constrainers
      using ConstrainerBatches = std::vector<ConstrainerBatch>;
      //************************************************************************

      //**Friendships***********************************************************
      //! Friend the Algorithm
      friend Algorithm;
      //! Friend the batcher (not necessary)
      friend BatchAlgorithm;
      //! Friendship with Serialize
      friend struct ::elastica::io::Serialize<This>;
      //************************************************************************

     protected:
      //**Constructors**********************************************************
      /*!\name Constructors */
      //@{
      //************************************************************************
      /*!\brief Constructor of the Constraints class.
       */
      Constraints()
          : n_total_constraints(0UL),
            constraints_(),
            constrainer_batches_(),
            uuid_map_(),
            batch_from() {
        using namespace ::elastica::logging;
        log<info>([](auto const& log) {
          log << "Successfully initialized the Constraint module";
        });
      };
      //************************************************************************

      //************************************************************************
      /*!\brief Move constructor of the Constraints class.
       *
       * \details This is not needed strictly (as it is ensured to be a
       * singleton class by the Simulator). However, while testing it is useful.
       */
      Constraints(Constraints&& other) noexcept
          : n_total_constraints(other.n_total_constraints),
            constraints_(std::move(other.constraints_)),
            constrainer_batches_(std::move(other.constrainer_batches_)),
            uuid_map_(std::move(other.uuid_map_)),
            batch_from(std::move(other.batch_from)){};
      //************************************************************************

      //@}
      //************************************************************************

     public:
      //**Destructor************************************************************
      /*!\name Destructor */
      //@{

      //************************************************************************
      /*!\brief Destructor
       */
      ~Constraints() {
        using namespace ::elastica::logging;
        log<info>([](auto const& log) {
          log << "Successfully cleaned up the Constraint module";
        });
      };
      //************************************************************************

      //@}
      //************************************************************************

      //**Interfaces************************************************************
      /*!\name Interface methods */
      //@{
     protected:
      //************************************************************************
      /*!\brief Check if the system can be constrained
       *
       * \param first_system The system to be constrained
       * \exception std::nested exception, if constraint is not possible
       */
      template <typename System>
      inline void check_if_system_can_be_constrained(
          System const& system) const {
        // TODO : Have names for system to print in backtrace
        const char* error_message =
            "Attempts to constrain system failed because ";

        [this, error_message](auto const& sys) {
          try {
            // 0. Check if not final
            this->self().check_if_not_final();
            // 1. First check if system is registered in the simulation
            this->self().confirm_as_registered(sys);
          } catch (...) {
            std::throw_with_nested(std::runtime_error(error_message));
          };
        }(system);
      }
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Constrain a system
       *
       * \details
       * constrain() is the only interface for constraining systems within a
       * \elastica simulation.
       *
       * \param system The system to be constrained
       * \param name An optional name for the constrain, useful for debugging
       * \exception std::runtime_error If `system` is not registered in the
       * simulator
       * \return Mutable reference to an internal
       * ::elastica::interface::constraints::Constraint object
       *
       * \example
       * For the setup of a system shown below...
       * \snippet Constraints/Test_Module.cpp constrain_setup
       * ...and after launching a constrain(), one can complete the constraint
       * information in multiple ways, seen below for the same system:
       * \snippet Constraints/Test_Module.cpp constrain_eg
       * \snippet Constraints/Test_Module.cpp constrain_eg_curry
       * \snippet Constraints/Test_Module.cpp constrain_eg_no_curry
       * \snippet Constraints/Test_Module.cpp constrain_eg_with_name
       */
      template <typename System
                //  Requires<tt::is_block_slice_v<System>> = nullptr
                >
      inline auto constrain(System const& system, std::string name = "")
          -> ::elastica::interface::constraints::Constraint<
              System, ConstraintsRegistrars>& {
        try {
          // 1.
          check_if_system_can_be_constrained(system);

          // 2. if so, good create a constraint object, append and return to
          // user ConstraintType constraint_ =
          // std::make_unique<ConstraintImpl>(idx);
          using CONSTrainT = ::elastica::interface::constraints::Constraint<
              System, ConstraintsRegistrars>;
          constraints_.emplace_back(std::make_unique<CONSTrainT>(
              n_total_constraints, system, std::move(name)));
        } catch (const std::exception& e) {
          abort_with_backtrace(e);
        }

        // 3. increment counter
        ++n_total_constraints;

        // 4. map it within
        uuid_map_.map(system).with(constraints_.back()->id());

        return static_cast<::elastica::interface::constraints::Constraint<
            System, ConstraintsRegistrars>&>(*(constraints_.back()));
      }
      //************************************************************************

      protected :
          //************************************************************************
          /*! \cond ELASTICA_INTERNAL */
          /*!\brief Retrieval of a constrainer from a unique key
           *
           * \param constraint_uuid The key using which we retrieve the
           * constraints
           */
          inline auto
          retrieve_from_query(UUID const& constraint_uuid) noexcept ->
          typename ConstrainerID::pointer {
        /*
         * dev note:
         * 1. the retrieval is noexcept as by construction the
         * constraint uuid here is < size of array.
         * 2. if we batch the logic here changes and we need to do a search.
         */
        ConstrainerBatch& batch =
            constrainer_batches_[batch_from[constraint_uuid]];
        // its bound to find it, so no need checking
        return std::find_if(std::begin(batch), std::end(batch),
                            [&](ConstrainerID const& constrainer_id) {
                              // if same
                              using ::elastica::interface::detail::id;
                              // don't compare ids directly since its changed
                              return id(constraint_uuid) ==
                                     id(constrainer_id->id());
                            })
            ->get();
      }
      /*! \endcond */
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Query the simulator for a created constraint
       *
       * \details
       * query() is the interface for querying for constraints created within a
       * \elastica simulation. It takes as input the key generated from the
       * constrain() hierarchy and returns the constraint that it corresponds
       * to. This is useful if you want to manipulate the constraints (such as
       * for storing some temporal data) using the events mechanism of
       * \elastica.
       *
       * It should be noted that a successful query can only be completed after
       * finalizing the simulator and no additional constraints can be
       * launched). Failure to finalize() results in a runtime error.
       *
       * \param query_key The key of the query generated from the constrain()
       * process
       * \exception Nested exception if failed to query
       *
       * \example
       * For the setup of systems shown below...
       * \snippet Constraints/Test_Module.cpp constrain_setup
       * and after completing the constraint, one gets a key which can be used
       * to retrieve the constraint object after finalizing
       * \snippet Constraints/Test_Module.cpp query_eg
       */
      template <typename ConstraintType>
      inline auto query(::elastica::interface::constraints::ConstraintQueryID<
                        ConstraintType> const query_key) -> ConstraintType& {
        using ::elastica::interface::detail::id;  // from include in query
        const char* error_message =
            "Attempts to query a constraint failed because ";
        try {
          this->self().check_if_final();
        } catch (...) {
          std::throw_with_nested(std::runtime_error(error_message));
        }

        // convert a void ptr to a ConstraintType* and then dereference it
        void* unsafe_constraint =
            This::retrieve_from_query(id(query_key))->get_constraint_unsafe();
        return *static_cast<ConstraintType*>(unsafe_constraint);
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Constrain primitives (positions, directors) of a system
       *
       * \param time_v The current time of simulation
       */
      inline auto apply_constraints_on_primitives(
          TimeType time_v) const noexcept -> void {
        // this is internal so a member function suffices.
        auto const& algorithm =
            self()
                .template get_algorithm<
                    typename Algorithm::ConstrainPrimitives>();
        algorithm(time_v);
      };
      //************************************************************************

      //************************************************************************
      /*!\brief Constrain rates (velocities, angular velocity) of all registered
       * systems
       *
       * \param time_v The current time of simulation
       */
      inline auto apply_constraints_on_rates(TimeType time_v) const noexcept
          -> void {
        // this is internal so a member function suffices.
        auto const& algorithm =
            self().template get_algorithm<typename Algorithm::ConstrainRates>();
        algorithm(time_v);
      };
      //************************************************************************

      //************************************************************************
      /*!\brief Gets the number of constraints registered in the present
       * simulator
       */
      inline auto n_constraints() const noexcept -> SizeType {
        return n_total_constraints;
      }
      //************************************************************************

     protected:
      //************************************************************************
      /*!\brief Gets the constrainer buffer
       */
      inline constexpr auto constrainer_batches() & noexcept
          -> ConstrainerBatches& {
        return constrainer_batches_;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Gets the constrainer buffer
       */
      inline constexpr auto constrainer_batches() const& noexcept
          -> ConstrainerBatches const& {
        return constrainer_batches_;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Gets the uuid->batch mapping
       */
      inline constexpr auto batch_map() & noexcept -> UUIDToBatch& {
        return batch_from;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Gets the uuid->batch mapping
       */
      inline constexpr auto batch_map() const& noexcept -> UUIDToBatch const& {
        return batch_from;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Gets the total number of batches
       */
      inline auto n_batches() const noexcept -> std::size_t {
        // this is internal so a member function suffices.
        auto const& algorithm =
            self()
                .template get_batching_algorithm<
                    typename BatchAlgorithm::ConstraintsBatcher>();
        return algorithm(n_constraints());
      };
      //************************************************************************

      //@}
      //************************************************************************

     protected:
      //************************************************************************
      /*!\brief Check status of constraints
       *
       * \details
       * Check for any missing constraint information, and report it to the user
       * in a sane fashion.
       *
       * \exception std::runtime_error If any constraint information is
       * missing/faulty
       * \return void
       */
      auto check_and_report_errors() const -> void {
        /*
         * Here we first check if f_ is empty
         * This happens when users do something along the lines of
         *   \code
         *     e = rod()
         *     sim.push_back(e)
         *     sim.constrain(e)
         *   \endcode
         * Notice that there is an attempt to constrain the rod, yet actual
         * constraint information is not received. The simulator thinks they
         * have constrained, and tries to access the constraint generator.
         * Unfortunately its a nullptr which upon dereferencing results in a
         * segfault
         */
        struct ConstraintInfo {
          UUID uuid;
          std::string constraint_typename;
          std::string system_typename;

          ConstraintInfo(UUID id, std::string ct, std::string st)
              : uuid(std::move(id)),
                constraint_typename(std::move(ct)),
                system_typename(std::move(st)) {}
        };
        using FaultyConstraintsInfo = std::vector<ConstraintInfo>;
        FaultyConstraintsInfo faulty_constraints_info;

        // assume 10% is faulty
        const std::size_t n_faulty_guess(n_total_constraints / 10);
        faulty_constraints_info.reserve(n_faulty_guess);
        std::for_each(
            constraints_.cbegin(), constraints_.cend(),
            [&faulty_constraints_info](ConstraintID const& constraint) {
              if (constraint->is_not_valid())
                faulty_constraints_info.emplace_back(
                    constraint->id(), constraint->concrete_constraint_type(),
                    constraint->concrete_system_type());
            });

        // Prune list of faulty constraints
        if (not faulty_constraints_info.empty()) {
          std::ostringstream s_stream;
          s_stream << "Faults detected in the following constraints!.\n"
                   << "They are listed by their ids and names below \n";

          for (auto const& constraint_info : faulty_constraints_info) {
            auto const& constraint_uuid = constraint_info.uuid;
            auto const& concrete_typename = constraint_info.constraint_typename;
            auto const& system_typename = constraint_info.system_typename;
            s_stream << "\t"
                     << "ID : " << constraint_uuid.uuid;
            if (not constraint_uuid.name.empty())
              s_stream << "\t Constraint Name : " << constraint_uuid.name;
            if (not concrete_typename.empty())
              s_stream << "\t Constraint Type : " << concrete_typename;
            if (not system_typename.empty())
              s_stream << "\t System Type : " << system_typename;
            // TODO Have an option to tag/name each rod so that its easier to
            // detect which rod has errors.
            s_stream << "\n";
          }

          s_stream << "Did you forget to call the `with()` method while "
                      "attempting to constrain?\n"
                      "Did you give the right indices using the at() method?";

          throw std::runtime_error(s_stream.str());
        }
      }
      //************************************************************************

      //************************************************************************
      /*!\brief The finalize method.
       *
       * \details
       * The finalize method is the interface to finalize the constraints module
       * of elastica before launching into temporal integration. It checks for
       * any missing information, then assembles all constraint information
       * received thus far to apply constraints performantly
       *
       * \param time_v The time to finalize, usually 0.0
       * \exception std::runtime_error If any constraint information is
       * missing/faulty
       */
      void finalize(TimeType time_v) {
        if (n_total_constraints) {
          try {
            check_and_report_errors();

            // logic for batching
            batch_from = [](auto uuids_batched_per_system,
                            std::size_t partition_size) {
              auto partition_result = elastica::karmarkar_karp_partition(
                  uuids_batched_per_system, partition_size,
                  [](auto const& v) { return ::elastica::size(v); });
              using UUIDSubsets =
                  decltype(::flatten(partition_result.subsets()));
              UUIDSubsets flattened_uuid_subsets;
              flattened_uuid_subsets.reserve(partition_size);
              // flatten the inner-most subset dimension
              for (auto& sub : partition_result.subsets()) {
                flattened_uuid_subsets.emplace_back(::flatten(std::move(sub)));
              }
              return ::elastica::interface::detail::make_uuid_to_batch_map(
                  std::move(flattened_uuid_subsets));
            }(std::move(uuid_map_).as_vector(), n_batches());

            constrainer_batches_.reserve(n_batches());
            constrainer_batches_.resize(n_batches());

            for (ConstraintID const& constraint : constraints_) {
              // guaranteed to be [0, partition_size() - 1]
              const std::size_t batch_number = batch_from[constraint->id()];

              // careful in pushing back unique ptr
              constrainer_batches_[batch_number].push_back(
                  constraint->finalize());
            }

          } catch (...) {
            const char* error_message =
                "Preparing the constraints module failed because";
            std::throw_with_nested(std::runtime_error(error_message));
          }
        } else {
          const char* warning_message =
              "No constraints added even though the module was "
              "included! Consider removing ::elastica::modules::Constraints "
              "from ::elastica::make_simulator for potentially improved "
              "performance";
          using namespace ::elastica::logging;
          log<warning>([&](auto const& log) { log << warning_message; });

          std::cerr << "Warning : " << warning_message
                    << std::endl;  // flush it
        }

        // constraints are no longer needed, remove them
        constraints_.clear();
        constraints_.shrink_to_fit();

        // to be safe, constrain initially
        apply_constraints_on_primitives(time_v);
        apply_constraints_on_rates(time_v);
      }
      //************************************************************************

     protected:
      //**Reset methods*********************************************************
      /*!\name Reset methods*/
      //@{

      //************************************************************************
      /*!\brief Resets the module
       *
       * \param time_v The time of reset
       * \exception std::runtime_error
       */
      void reset_module(TimeType /*time_v*/) {
        n_total_constraints = 0UL;
        constraints_.clear();  // no need, but okay for symmetry
        constrainer_batches().clear();
        uuid_map_.clear();
        batch_from.clear();
      }
      //************************************************************************

      //@}
      //************************************************************************

     private:
      //**Member variables******************************************************
      //! The total number of registered constraints
      SizeType n_total_constraints;
      //! The temporary constraint information before finalizing
      std::vector<ConstraintID> constraints_;
      //! The forcing information applied on runtime
      ConstrainerBatches constrainer_batches_;
      //! A map from systems to IDs for
      UUIDMap uuid_map_;
      //! A map from uuid to batch
      UUIDToBatch batch_from;
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace modules

}  // namespace elastica
