//==============================================================================
/*!
//  \file
//  \brief Implementation of module for Forcing
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

/// All forcing types
#include "Simulator/Forcing/Types.hpp"
#include "Simulator/Interface/Forcing/Types.hpp"
/// Interface methods
#include "Simulator/Interface/Forcing/BatchingPolicy.hpp"
#include "Simulator/Interface/Forcing/Configuration.hpp"
#include "Simulator/Interface/Forcing/ExecutionPolicy.hpp"
#include "Simulator/Interface/Forcing/Forcer.hpp"
#include "Simulator/Interface/Forcing/ForcerBase.hpp"
#include "Simulator/Interface/Forcing/Forcing.hpp"
#include "Simulator/Interface/Forcing/ForcingBase.hpp"
#include "Simulator/Interface/Forcing/ForcingUUID.hpp"
#include "Simulator/Interface/Protocols/Forcing.hpp"
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
    /*!\brief The forcing module
     * \ingroup modules forcing
     *
     * The Forcing module is an enhancement to the \elastica Simulator
     * and is responsible for dynamically applying forces and torques to
     * different entities.
     *
     * \tparam SimTraits Options for configuration of the Simulator
     * \tparam Sim Simulator
     *
     * \usage
     * To use Forcing, pass it as template argument to
     * ::elastica::make_simulator() as shown below. This needs to be accompanied
     * by an instantiation of
     * ::elastica::configuration::ForcingConfiguration as an argument to
     * ::elastica::make_simulator().
     *
     * \code
     * auto forcing_configuration =
     * ::elastica::configuration::DefaultForcingConfiguration{};
     *
     * auto sim =
     * ::elastica::make_simulator<::elastica::modules::Forcing>(
     * ::elastica::configuration::DefaultSystemsConfiguration{},
     * ::elastica::configuration::DefaultDomainConfiguration{},
     *  forcing_configuration
     * );
     * \endcode
     * And the simulator `sim` now has capabilities of forcing systems. The
     * sole entry point of forcing is the #force method for which we
     * show examples below.
     *
     * \example
     * For the setup of a system shown below...
     * \snippet Forcing/Test_Module.cpp force_setup
     * ...one can launch a forcing of a system in one of many ways, each
     * intended to promote readability and be as clear about the intent as
     * possible, as shown below:
     * \snippet Forcing/Test_Module.cpp force_eg
     * \snippet Forcing/Test_Module.cpp force_eg_curry
     * \snippet Forcing/Test_Module.cpp force_eg_no_curry
     * \snippet Forcing/Test_Module.cpp force_eg_with_name
     *
     * It is useful to get a handle back to the forcing to manipulate/record
     * it later on in the client script. For this purpose, after completing the
     * forcing, one gets a key which can be used to retrieve the forcing
     * after finalizing the simulator. This is shown below:
     * \snippet Forcing/Test_Module.cpp query_eg
     *
     * \see ::elastica::make_simulator()
     */
    template <class SimTraits,  // Simulator options
              class Sim>        // Simulator class
    class Forcing
        : public CRTPHelper<Sim, Forcing>,
          public ::tt::ConformsTo<::elastica::interface::protocols::Forcing>,
          public NonCopyable,
          private detail::HasForcingConfiguration<SimTraits>,
          protected ::elastica::interface::forcing::ExecutionPolicy<
              SimTraits, Forcing<SimTraits, Sim>>,
          protected ::elastica::interface::forcing::BatchingPolicy<SimTraits> {
     private:
      //**Compile-time checks***************************************************
      //
      //************************************************************************

      //**Type definitions******************************************************
      //! This type
      using This = Forcing<SimTraits, Sim>;
      //! CRTP Type
      using CRTP = CRTPHelper<Sim, Forcing>;
      //! CRTP methods
      using CRTP::self;
      //! Internal size type
      using SizeType = std::size_t;
      //! Internal type of index
      using IndexType = std::size_t;
      //! Algorithm type
      using Algorithm =
          ::elastica::interface::forcing::ExecutionPolicy<SimTraits, This>;
      //! Batch type
      using BatchAlgorithm =
          ::elastica::interface::forcing::BatchingPolicy<SimTraits>;
      //! UUID type
      using UUID = ::elastica::interface::forcing::ForcingUUID;
      //! Map from system to uuid
      using UUIDMap = ::elastica::interface::detail::UUIDMap<UUID>;
      //! Map from uuid to batch
      using UUIDToBatch = ::elastica::interface::detail::UUIDToBatch<UUID>;
      //************************************************************************

     public:
      //**Type definitions******************************************************
      //! Type of simulation time
      using TimeType = typename SimTraits::TimeType;  // public for conformation
      //! ID Type of forcing applicator
      using ForcerID =
          std::unique_ptr<::elastica::interface::forcing::ForcerBase>;
      //! Type of forcing registrars
      using ForcingRegistrars = typename SimTraits::ForcingRegistrars;
      //! ID Type of forcing
      using ForcingID = std::unique_ptr<
          ::elastica::interface::forcing::ForcingBase<ForcingRegistrars>>;
      //! A batch of forcers
      using ForcerBatch = std::vector<ForcerID>;
      //! Batches of forcers
      using ForcerBatches = std::vector<ForcerBatch>;
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
      /*!\brief Constructor of the Forcing class.
       */
      // TODO : mark protected
      Forcing()
          : n_total_forcing(0UL),
            forcing_(),
            forcer_batches_(),
            uuid_map_(),
            batch_from() {
        using namespace ::elastica::logging;
        log<info>([](auto const& log) {
          log << "Successfully initialized the Forcing module";
        });
      };
      //************************************************************************

      //************************************************************************
      /*!\brief Move constructor of the Forcing class.
       *
       * \details This is not needed strictly (as it is ensured to be a
       * singleton class by the Simulator). However, while testing it is useful.
       */
      Forcing(Forcing&& other) noexcept
          : n_total_forcing(other.n_total_forcing),
            forcing_(std::move(other.forcing_)),
            forcer_batches_(std::move(other.forcer_batches_)),
            uuid_map_(std::move(other.uuid_map_)),
            batch_from(std::move(other.batch_from)){};
      //************************************************************************
      //@}
      //************************************************************************

     public:
      //**Destructor************************************************************
      /*!\name Destructor */
      //@{
      ~Forcing() {
        using namespace ::elastica::logging;
        log<info>([](auto const& log) {
          log << "Successfully cleaned up the Forcing module";
        });
      };
      //@}
      //************************************************************************

      //**Interfaces************************************************************
      /*!\name Interface methods */
      //@{
     protected:
      //************************************************************************
      /*!\brief Check if the system can be forced
       *
       * \param first_system The system to be forced
       * \exception std::nested exception, if forcing is not possible
       */
      template <typename System>
      inline void check_if_system_can_be_forced(System const& system) const {
        // TODO : Have names for system to print in backtrace
        const char* error_message = "Attempts to force system failed because ";

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
      /*!\brief Force a system
       *
       * \details
       * force() is the only interface for forcing systems within a
       * \elastica simulation.
       *
       * \param system The system to be forced
       * \param name An optional name for the force, useful for debugging
       * \exception std::runtime_error If `system` is not registered in the
       * simulator
       * \return Mutable reference to an internal
       * ::elastica::interface::forcing::Forcing object
       *
       * \example
       * For the setup of a system shown below...
       * \snippet Forcing/Test_Module.cpp force_setup
       * ...and after launching a force(), one can complete the forcing
       * information in multiple ways, seen below for the same system:
       * \snippet Forcing/Test_Module.cpp force_eg
       * \snippet Forcing/Test_Module.cpp force_eg_curry
       * \snippet Forcing/Test_Module.cpp force_eg_no_curry
       * \snippet Forcing/Test_Module.cpp force_eg_with_name
       */
      template <typename System
                // Requires <tt::is_block_slice_v<System>> = nullptr
                >
      inline auto force(System const& system, std::string name = "")
          -> ::elastica::interface::forcing::Forcing<System,
                                                     ForcingRegistrars>& {
        try {
          // 1.
          check_if_system_can_be_forced(system);

          // 2. if so, good create a forcing object, append and return to user
          // ForcingType forcing_ = std::make_unique<ForcingImpl>(idx);
          using FORcing =
              ::elastica::interface::forcing::Forcing<System,
                                                      ForcingRegistrars>;
          forcing_.emplace_back(std::make_unique<FORcing>(
              n_total_forcing, system, std::move(name)));

        } catch (const std::exception& e) {
          abort_with_backtrace(e);
        }

        // 3. increment counter
        ++n_total_forcing;

        // 4. map it within
        uuid_map_.map(system).with(forcing_.back()->id());

        return static_cast<::elastica::interface::forcing::Forcing<
            System, ForcingRegistrars>&>(*(forcing_.back()));
      }
      //************************************************************************

      protected :
          //************************************************************************
          /*! \cond ELASTICA_INTERNAL */
          /*!\brief Retrieval of a forcer from a unique key
           *
           * \param forcing_uuid The key using which we retrieve the forcings
           */
          inline auto
          retrieve_from_query(UUID const& forcing_uuid) noexcept ->
          typename ForcerID::pointer {
        /*
         * dev note:
         * 1. the retrieval is noexcept as by construction the
         * forcing uuid here is < size of array.
         * 2. if we batch the logic here changes and we need to do a search.
         */
        ForcerBatch& batch = forcer_batches_[batch_from[forcing_uuid]];
        // its bound to find it, so no need checking
        return std::find_if(std::begin(batch), std::end(batch),
                            [&](ForcerID const& forcer_id) {
                              // if same
                              using ::elastica::interface::detail::id;
                              // don't compare ids directly since its changed
                              return id(forcing_uuid) == id(forcer_id->id());
                            })
            ->get();
      }
      /*! \endcond */
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Query the simulator for a created forcing
       *
       * \details
       * query() is the interface for querying for forcings created within a
       * \elastica simulation. It takes as input the key generated from the
       * constrain() hierarchy and returns the forcing that it corresponds to.
       * This is useful if you want to manipulate the forcings (such as for
       * storing some temporal data) using the events mechanism of \elastica.
       *
       * It should be noted that a successful query can only be completed after
       * finalizing the simulator and no additional forcings can be
       * launched). Failure to finalize() results in a runtime error.
       *
       * \param query_key The key of the query generated from the constrain()
       * process
       * \exception Nested exception if failed to query
       *
       * \example
       * For the setup of systems shown below...
       * \snippet Forcing/Test_Module.cpp force_setup
       * and after completing the forcing, one gets a key which can be used
       * to retrieve the forcing object after finalizing
       * \snippet Forcing/Test_Module.cpp query_eg
       */
      template <typename ForcingType>
      inline auto query(
          ::elastica::interface::forcing::ForcingQueryID<ForcingType> const
              query_key) -> ForcingType& {
        using ::elastica::interface::detail::id;  // from include in query
        const char* error_message =
            "Attempts to query a forcing failed because ";
        try {
          this->self().check_if_final();
        } catch (...) {
          std::throw_with_nested(std::runtime_error(error_message));
        }

        // convert a void ptr to a ForcingType* and then dereference it
        void* unsafe_forcing =
            This::retrieve_from_query(id(query_key))->get_forcing_unsafe();
        return *static_cast<ForcingType*>(unsafe_forcing);
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply forces and torques to a system
       *
       * \param time_v The current time of simulation
       */
      inline auto apply_dynamics(TimeType time_v) const noexcept -> void {
        // this is internal so a member function suffices.
        auto const& algorithm =
            self().template get_algorithm<typename Algorithm::ForceSystems>();
        algorithm(time_v);
      };
      //************************************************************************

      //************************************************************************
      /*!\brief Gets the number of forcing registered in the present
       * simulator
       */
      inline auto n_forcings() const noexcept -> SizeType {
        return n_total_forcing;
      }
      //************************************************************************

     protected:
      //************************************************************************
      /*!\brief Gets the forcer buffer
       */
      inline constexpr auto forcer_batches() & noexcept -> ForcerBatches& {
        return forcer_batches_;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Gets the forcer buffer
       */
      inline constexpr auto forcer_batches() const& noexcept
          -> ForcerBatches const& {
        return forcer_batches_;
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
                    typename BatchAlgorithm::ForcingBatcher>();
        return algorithm(n_forcings());
      };
      //************************************************************************

      //@}
      //************************************************************************

     protected:
      //************************************************************************
      /*!\brief Check status of forcing
       *
       * \details
       * Check for any missing forcing information, and report it to the user
       * in a sane fashion.
       *
       * \exception std::runtime_error If any forcing information is
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
         *     sim.force(e)
         *   \endcode
         * Notice that there is an attempt to force the rod, yet actual
         * forcing information is not received. The simulator thinks they
         * have forced, and tries to access the forcing generator.
         * Unfortunately its a nullptr which upon dereferencing results in a
         * segfault
         */
        struct ForcingInfo {
          UUID uuid;
          std::string forcing_typename;
          std::string system_typename;

          ForcingInfo(UUID id, std::string ft, std::string st)
              : uuid(std::move(id)),
                forcing_typename(std::move(ft)),
                system_typename(std::move(st)) {}
        };
        using FaultyForcingsInfo = std::vector<ForcingInfo>;
        FaultyForcingsInfo faulty_forcings_info;

        // assume 10% is faulty
        const std::size_t n_faulty_guess(n_total_forcing / 10);
        faulty_forcings_info.reserve(n_faulty_guess);
        std::for_each(forcing_.cbegin(), forcing_.cend(),
                      [&faulty_forcings_info](ForcingID const& forcing) {
                        if (forcing->is_not_valid())
                          faulty_forcings_info.emplace_back(
                              forcing->id(), forcing->concrete_forcing_type(),
                              forcing->concrete_system_type());
                      });

        // Prune list of faulty forcings
        if (not faulty_forcings_info.empty()) {
          std::ostringstream s_stream;
          s_stream << "Faults detected in the following forcings!\n"
                   << "They are listed by their ids and names below \n";

          for (auto const& forcing_info : faulty_forcings_info) {
            auto const& forcing_uuid = forcing_info.uuid;
            auto const& concrete_typename = forcing_info.forcing_typename;
            auto const& system_typename = forcing_info.system_typename;
            s_stream << "\t"
                     << "ID : " << forcing_uuid.uuid;
            if (not forcing_uuid.name.empty())
              s_stream << "\t Forcing Name : " << forcing_uuid.name;
            if (not concrete_typename.empty())
              s_stream << "\t Forcing Type : " << concrete_typename;
            if (not system_typename.empty())
              s_stream << "\t System Type : " << system_typename;
            // TODO Have an option to tag/name each rod so that its easier to
            // detect which rod has errors.
            s_stream << "\n";
          }

          s_stream << "Did you forget to call the `with()` method while "
                      "attempting to force?\n"
                      "Did you give the right indices using the at() method?";

          throw std::runtime_error(s_stream.str());
        }
      }
      //************************************************************************

      //************************************************************************
      /*!\brief The finalize method.
       *
       * \details
       * The finalize method is the interface to finalize the forcing module
       * of elastica before launching into temporal integration. It checks for
       * any missing information, then assembles all forcing information
       * received thus far to apply forcing performantly
       *
       * \param time_v The time to finalize, usually 0.0
       * \exception std::runtime_error If any forcing information is
       * missing/faulty
       */
      void finalize(TimeType time_v) {
        if (n_total_forcing) {
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

            forcer_batches_.reserve(n_batches());
            forcer_batches_.resize(n_batches());

            for (ForcingID const& forcing : forcing_) {
              // guaranteed to be [0, partition_size() - 1]
              const std::size_t batch_number = batch_from[forcing->id()];

              // careful in pushing back unique ptr
              forcer_batches_[batch_number].push_back(forcing->finalize());
            }

            //          forcers_.reserve(n_total_forcing);
            //          std::transform(
            //              forcing_.cbegin(), forcing_.cend(),
            //              std::back_inserter(forcers_),
            //              [](const ForcingID& forcing) { return
            //              forcing->finalize(); });

          } catch (...) {
            const char* error_message =
                "Preparing the forcing module failed because";
            std::throw_with_nested(std::runtime_error(error_message));
          }
        } else {
          const char* warning_message =
              "No forcing added even though the module was "
              "included! Consider removing ::elastica::modules::Forcing "
              "from ::elastica::make_simulator for potentially improved "
              "performance";
          using namespace ::elastica::logging;
          log<warning>([&](auto const& log) { log << warning_message; });

          std::cerr << "Warning : " << warning_message
                    << std::endl;  // flush it
        }

        // forcing are no longer needed, remove them
        forcing_.clear();
        forcing_.shrink_to_fit();

        // to be safe, force initially
        apply_dynamics(time_v);
      }
      //************************************************************************

     protected:
      //**Reset methods*********************************************************
      /*!\name Reset methods*/
      //@{

      //************************************************************************
      /*!\brief Resets the forcing module
       *
       * \param time_v The time of reset
       * \exception std::runtime_error
       */
      void reset_module(TimeType /*time_v*/) {
        n_total_forcing = 0UL;
        forcing_.clear();  // no need, but okay for symmetry
        forcer_batches().clear();
        uuid_map_.clear();
        batch_from.clear();
      }
      //************************************************************************

      //@}
      //************************************************************************

     private:
      //**Member variables******************************************************
      //! The total number of registered forcing
      SizeType n_total_forcing;
      //! The temporary forcing information before finalizing
      std::vector<ForcingID> forcing_;
      //! The forcing information applied on runtime
      ForcerBatches forcer_batches_;
      //! A map from systems to IDs for
      UUIDMap uuid_map_;
      //! A map from uuid to batch
      UUIDToBatch batch_from;
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace modules

}  // namespace elastica
