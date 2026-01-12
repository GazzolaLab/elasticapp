//==============================================================================
/*!
//  \file
//  \brief Simulator module to interface with Explicit time-steppers
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
///
#include "Simulator/Interface/TimeSteppers/Types.hpp"
/// Temporal interfaces
// #include "Time/ExplicitSteppers.hpp"
#include "Time/SimulationTime.hpp"
/// Utilities
#include "Utilities/AsConst.hpp"
#include "Utilities/CRTP.hpp"
#include "Utilities/Logging.hpp"
#include "Utilities/NonCopyable.hpp"

namespace elastica {

  namespace interface {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Simulator module to interface with explicit time-steppers
     * \ingroup interface
     *
     * The InterfaceWithExplicitSteppers module is an enhancement to the
     * \elastica Simulator and is responsible for integrating the simulator with
     * a explicit numerical temporal integration algorithm.
     *
     * \tparam SimTraits Options for configuration of the Simulator
     * \tparam Sim Simulator
     *
     * \see make_simulator(), Simulator
     */
    template <class SimTraits, class Sim>
    class InterfaceWithExplicitSteppers
        : public CRTPHelper<Sim, InterfaceWithExplicitSteppers>
    // public ::tt::ConformsTo<protocols::TimeStepper>, // TODO!
    // public NonCopyable // Trivially copyable, so leave it alone
    {
     private:
      //**Compile-time checks***************************************************
      //
      //************************************************************************

      //**Type definitions******************************************************
      //! CRTP Type
      using CRTP = CRTPHelper<Sim, InterfaceWithExplicitSteppers>;
      using CRTP::self;  //<! CRTP methods
      //************************************************************************

     public:
      //**Type definitions******************************************************
      //! Type of simulation time
      using TimeType = typename SimTraits::TimeType;
      //! Type of simulation time delta
      using TimeDeltaType = typename SimTraits::TimeDeltaType;
      //************************************************************************

     protected:
      //**Constructors**********************************************************
      /*!\name Constructors */
      //@{
      //************************************************************************
      /*!\brief Constructor of the InterfaceWithExplicitSteppers class.
       */
      InterfaceWithExplicitSteppers() {
        using namespace ::elastica::logging;
        log<info>([](auto const& log) {
          log << "Successfully initialized InterfaceWithExplicitSteppers "
                 "module";
        });
      };
      //************************************************************************
      //@}
      //************************************************************************

     public:
      //**Destructor************************************************************
      /*!\name Destructor */
      //@{
      ~InterfaceWithExplicitSteppers() {
        using namespace ::elastica::logging;
        log<info>([](auto const& log) {
          log << "Successfully destroyed the InterfaceWithExplicitSteppers";
        });
      };
      //@}
      //************************************************************************

     protected:
      //**Interfaces************************************************************
      /*!\name Interface methods */
      //@{

      //************************************************************************
      /*!\brief Update simluator over one time-step
       *
       * \param time_stepper Explicit time-stepper to use for update
       * \param time_v Current simulation time, to be updated to the latest time
       * at the end of the function
       * \param dt Time-step of the simulation
       */
      template <typename ExplicitAlgorithm>
      auto step(::elastica::timestepper::ExplicitStepper<
          ExplicitAlgorithm> const& time_stepper,
                TimeType& time_v, TimeDeltaType const dt) noexcept -> void {
        // always marked noexcept
        // TODO : can we have one more indirection layer to put a computation
        // graph here, instead of manually declaring the steps?
        using ::elastica::timestepper::dispatch_based_on;
        using ::elastica::timestepper::apply_substeps_of;

        apply_substeps_of(
            time_stepper, [&, dt = dt](auto const& substep) noexcept {
              dispatch_based_on(
                  substep,
                  [&, &time_v = std::as_const(time_v),
                      dt = dt](auto const& kinematic_update_substep) noexcept {
                    // parallel invoke 4 available system collection
                    // parallel for each system in system collection
                    // do kinematic_update_substep
                    self().apply_over_systems(
                        [=, &kinematic_update_substep](auto& system) noexcept {
                          kinematic_update_substep(system, time_v, dt);
                        });
                  },
                  [&, &time_v = std::as_const(time_v),
                      dt = dt](auto const& dynamic_update_substep) noexcept {
                    // parallel pipeline here, based on strategy

                    // parallel for each constraint
                    // apply constraint to primitives
                    // needed here because time needs to be incremented first
                    self().apply_post_kinematic_updates(time_v);

                    // parallel invoke 4 available system collection
                    // parallel for each system in system collection
                    // compute internal dynamics
                    self().apply_over_systems([=](auto& system) noexcept {
                      // this should be determined based on the interface met
                      // by systems such as Cosserat rods
                      system.compute_internal_dynamics(time_v);
                      // dynamic_update_substep(system, time_v, dt);
                    });

                    // parallel for each connection
                    // apply connection to primitives

                    // parallel for each environment
                    // apply environment forces to primitives

                    // parallel for each controls
                    // apply control forces to primitives
                    self().apply_pre_dynamic_update_substeps(time_v);

                    // parallel invoke 4 available system collection
                    // parallel for each system in system collection
                    // do dynamic_update_substep
                    self().apply_over_systems(
                        [=, &dynamic_update_substep](auto& system) noexcept {
                          dynamic_update_substep(system, time_v, dt);
                        });

                    // parallel for each constraint
                    // apply constraint to derivatives
                    self().apply_post_dynamic_update_substeps(time_v);
                  },
                  [&, dt = dt](auto const& time_update_substep) noexcept {
                    time_v += time_update_substep(dt);
                  });
            });
      }
      //************************************************************************

      //@}
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace interface

}  // namespace elastica
