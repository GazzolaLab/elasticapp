//==============================================================================
/*!
//  \file
//  \brief Simulator module to interface with Symplectic time-steppers
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

///
#include "Simulator/Interface/TimeSteppers/Types.hpp"
//
#include "Simulator/Interface/TimeSteppers/Metadata.hpp"
//
#include "Systems/common/Access.hpp"  // to qualify elastica::torques
/// Temporal interfaces
#include "Time/Protocols.hpp"
#include "Time/SimulationTime.hpp"
#include "Time/SymplecticSteppers.hpp"
#include "Time/TimeStepID.hpp"
/// Utils
#include "Utilities/CRTP.hpp"
#include "Utilities/Logging.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/StdVectorHelpers.hpp"

namespace elastica {

  namespace interface {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Simulator module to interface with symplectic time-steppers
     * \ingroup interface
     *
     * The InterfaceWithSymplecticSteppers module is an enhancement to the
     * \elastica Simulator and is responsible for integrating the simulator with
     * a symplectic numerical temporal integration algorithm.
     *
     * \tparam SimTraits Options for configuration of the Simulator
     * \tparam Sim Simulator
     *
     * \see make_simulator(), Simulator
     */
    template <class SimTraits, class Sim>
    class InterfaceWithSymplecticSteppers
        : public CRTPHelper<Sim, InterfaceWithSymplecticSteppers>,
          public ::tt::ConformsTo<
              ::elastica::timestepper::protocols::SymplecticSteppable>
    // public NonCopyable // Trivially copyable, so leave it alone
    {
     private:
      //**Compile-time checks***************************************************
      //
      //************************************************************************

      //**Type definitions******************************************************
      //! CRTP Type
      using CRTP = CRTPHelper<Sim, InterfaceWithSymplecticSteppers>;
      using CRTP::self;  //<! CRTP methods
      //************************************************************************

     public:
      //**Type definitions******************************************************
      //! Type of simulation time
      using TimeType = typename SimTraits::TimeType;
      //! Type of simulation time delta
      using TimeDeltaType = typename SimTraits::TimeDeltaType;
      //! Type of current iteration
      using TimeIDType = TimeStepID;
      //************************************************************************

     protected:
      //**Constructors**********************************************************
      /*!\name Constructors */
      //@{
      //************************************************************************
      /*!\brief Constructor of the InterfaceWithSymplecticSteppers class.
       */
      InterfaceWithSymplecticSteppers() {
        using namespace ::elastica::logging;
        log<info>([](auto const& log) {
          log << "Successfully initialized InterfaceWithSymplecticSteppers "
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
      ~InterfaceWithSymplecticSteppers() {
        using namespace ::elastica::logging;
        log<info>([](auto const& log) {
          log << "Successfully destroyed the InterfaceWithSymplecticSteppers";
        });
      };
      //@}
      //************************************************************************

     public:
      //**Interfaces************************************************************
      /*!\name Interface methods */
      //@{

      //************************************************************************
      /*!\brief Update simulator over one time-step
       *
       * \param time_stepper Symplectic time-stepper to use for updates
       * \param time_v Current simulation time, to be updated to the latest time
       * at the end of the function
       * \param dt Time-step of the simulation
       * \param iteration The current iteration of the simulation
       */
      template <typename SymplecticAlgorithm>
      auto step(::elastica::timestepper::SymplecticStepper<
                    SymplecticAlgorithm> const& time_stepper,
                TimeType& time_v, TimeDeltaType const dt,
                TimeIDType& iteration) noexcept -> void {
        ELASTICA_ASSERT(iteration.step_threshold() ==
                            time_stepper.n_time_updates_per_step(),
                        R"error(
Time ID does not have expected number of substeps. The problem is because the
time-stepper you are using has more/less number of substeps. To rectify this
mistake, consider using the following idiom when constructing the time-step :
::elastica::TimeStepId time_step{time_stepper.n_time_updates_per_step()};)error");

        // always marked noexcept
        // TODO : can we have one more indirection layer to put a computation
        // graph here, instead of manually declaring the steps?
        using ::elastica::timestepper::apply_substeps_of;
        using ::elastica::timestepper::dispatch_based_on;

        apply_substeps_of(
            time_stepper, [&, dt = dt](auto const& substep) noexcept {
              dispatch_based_on(
                  substep,
                  [&, &time_v = std::as_const(time_v),
                   dt = dt](auto const& kinematic_update_substep) noexcept {
                    // parallel invoke 4 available system collection
                    // parallel for each system in system collection
                    // do kinematic_update_substep
                    self().for_each_block_update_kinematics(
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
                    self().for_each_block_compute_internal_dynamics(
                        [=](auto& system) noexcept {
                          // this should be determined based on the interface
                          // met by systems such as Cosserat rods
                          using ::elastica::external_loads;
                          using ::elastica::external_torques;
                          external_loads(system) = 0.0;
                          external_torques(system) = 0.0;
                          system.compute_internal_dynamics(time_v);
                          // dynamic_update_substep(system, time_v, dt);
                        });

                    // parallel for each connection
                    // apply connection to primitives

                    // parallel for each environment
                    // apply environment forces to primitives

                    // parallel for each controls
                    // apply control forces to primitives
                    self().apply_pre_dynamic_updates(time_v);

                    // parallel invoke 4 available system collection
                    // parallel for each system in system collection
                    // do dynamic_update_substep
                    self().for_each_block_update_dynamics(
                        [=, &dynamic_update_substep](auto& system) noexcept {
                          dynamic_update_substep(system, time_v, dt);
                        });

                    // parallel for each constraint
                    // apply constraint to derivatives
                    self().apply_post_dynamic_updates(time_v);
                  },
                  [&, dt = dt](auto const& time_update_substep) noexcept {
                    time_v += time_update_substep(dt);
                    ++iteration;
                  });
            });
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Update simulator over one time-step
       *
       * \param time_stepper Symplectic time-stepper to use for updates
       * \param time_v Current simulation time, to be updated to the latest time
       * at the end of the function
       * \param dt Time-step of the simulation
       */
      template <typename SymplecticAlgorithm>
      auto step(::elastica::timestepper::SymplecticStepper<
                    SymplecticAlgorithm> const& time_stepper,
                TimeType& time_v, TimeDeltaType const dt) noexcept -> void {
        TimeIDType time_step{time_stepper.n_time_updates_per_step()};
        step(time_stepper, time_v, dt, time_step);
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Generates the algorithm metadata
       *
       * \return outstream
       */
      auto algorithm_metadata() const /*noexcept*/ -> AlgorithmMetadata {
        ::elastica::timestepper::SymplecticStepper<
            ::elastica::timestepper::symplectic::PositionVerlet>
            time_stepper{};

        using ::elastica::timestepper::apply_substeps_of;
        using ::elastica::timestepper::dispatch_based_on;

        using Metadata = decltype(self().metadata_update_kinematics());
        std::vector<Metadata> metadata;

        // replicate the computation graph
        apply_substeps_of(time_stepper, [&](auto const& substep) /*noexcept*/ {
          dispatch_based_on(
              substep,
              [&](auto const& /*kinematic_update_substep*/) /* noexcept*/
              {
                // parallel invoke 4 available system collection
                // parallel for each system in system collection
                // do kinematic_update_substep
                metadata.push_back(self().metadata_update_kinematics());
              },
              [&](auto const& /*dynamic_update_substep*/) /* noexcept */ {
                // parallel pipeline here, based on strategy

                // parallel for each constraint
                // apply constraint to primitives
                // needed here because time needs to be incremented
                // first
                metadata.push_back(self().metadata_post_kinematic_updates());

                // parallel invoke 4 available system collection
                // parallel for each system in system collection
                // compute internal dynamics
                metadata.push_back(self().metadata_compute_internal_dynamics());

                // parallel for each connection
                // apply connection to primitives

                // parallel for each environment
                // apply environment forces to primitives

                // parallel for each controls
                // apply control forces to primitives
                metadata.push_back(self().metadata_pre_dynamic_updates());

                // parallel invoke 4 available system collection
                // parallel for each system in system collection
                // do dynamic_update_substep
                metadata.push_back(self().metadata_update_dynamics());

                // parallel for each constraint
                // apply constraint to derivatives
                metadata.push_back(self().metadata_post_dynamic_updates());
              },
              [&](auto const& /*time_update_substep*/) noexcept {
                metadata.push_back(
                    Metadata{{"UpdateTime", "Update simulator-wide time"}});
              });
        });

        return {flatten(metadata)};
      }
      //@}
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace interface

}  // namespace elastica