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
#include "Simulator/Interface/Constraints/Configuration.hpp"
#include "Simulator/Interface/Constraints/Constrainer.hpp"
#include "Simulator/Interface/Constraints/ConstrainerBase.hpp"
#include "Simulator/Interface/Constraints/Constraint.hpp"
#include "Simulator/Interface/Constraints/ConstraintBase.hpp"
#include "Simulator/Interface/Constraints/ConstraintUUID.hpp"
#include "Simulator/Interface/Constraints/ExecutionPolicy.hpp"
#include "Simulator/Interface/Protocols/Constraints.hpp"
/// Serialization support
#include "Simulator/IO/Serialization/Types.hpp"
///
#include "ErrorHandling/NestedExceptions.hpp"
#include "Utilities/CRTP.hpp"
#include "Utilities/Logging.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/ProtocolHelpers.hpp"

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
              SimTraits, Constraints<SimTraits, Sim>> {
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
      //************************************************************************

      //**Friendships***********************************************************
      //! Friend the Algorithm
      friend Algorithm;
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
          : n_total_constraints(0UL), constraints_(), constrainers_() {
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
            constrainers_(std::move(other.constrainers_)){};
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
          // std::make_unique<ConstraintImpl>(idx); Do not use make_unique here
          // because Constraints' constructor is protected by design
          using CONSTrainT = ::elastica::interface::constraints::Constraint<
              System, ConstraintsRegistrars>;
          constraints_.push_back(ConstraintID(
              new CONSTrainT(n_total_constraints, system, std::move(name))));
        } catch (const std::exception& e) {
          abort_with_backtrace(e);
        }

        // 3. increment counter
        ++n_total_constraints;

        return static_cast<::elastica::interface::constraints::Constraint<
            System, ConstraintsRegistrars>&>(*(constraints_.back()));
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
      inline constexpr auto constrainers() const& noexcept
          -> std::vector<ConstrainerID> const& {
        return constrainers_;
      }
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
        using UUID = ::elastica::interface::constraints::ConstraintUUID;
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
        try {
          check_and_report_errors();

          constrainers_.reserve(n_total_constraints);
          std::transform(constraints_.cbegin(), constraints_.cend(),
                         std::back_inserter(constrainers_),
                         [](const ConstraintID& constraint) {
                           return constraint->finalize();
                         });
        } catch (...) {
          const char* error_message =
              "Preparing the constraints module failed because";
          std::throw_with_nested(std::runtime_error(error_message));
        }

        // constraints are no longer needed, remove them
        constraints_.clear();
        constraints_.shrink_to_fit();

        // to be safe, constrain initially
        apply_constraints_on_primitives(time_v);
        apply_constraints_on_rates(time_v);
      }
      //************************************************************************

     private:
      //**Member variables******************************************************
      //! The total number of registered constraints
      SizeType n_total_constraints;
      //! The temporary constraint information before finalizing
      std::vector<ConstraintID> constraints_;
      //! The constraint information applied on runtime
      std::vector<ConstrainerID> constrainers_;
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace modules

}  // namespace elastica
