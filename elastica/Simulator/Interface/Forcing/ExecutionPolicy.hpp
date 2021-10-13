//==============================================================================
/*!
//  \file
//  \brief Algorithm execution policies for the \elastica Forcing module
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
#include <tuple>  // for construction support

///
#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"
#include "Simulator/Interface/Forcing/Types.hpp"
///
#include "Parallel/for_each.hpp"  // uses elastica's, not GLIBCXX
///
#include "Utilities/CRTP.hpp"
#include "Utilities/MakeFromTuple.hpp"
#include "Utilities/Size.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace interface {

    namespace forcing {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      // TODO : should be batched if parallelized

      //************************************************************************
      /*!\brief Execution policy for Forcing module
       * \ingroup interface
       *
       * This class represents the policy of execution for applying forcing
       * to systems within the ambit of the Forcing module. As such, the
       * policies span a spectrum for purely sequential execution to parallel,
       * asynchronous execution.
       *
       * \tparam ForcingTraits Options for configuration of the Forcing
       * Module
       * \tparam ForcingModule The forcing module of the Simulator
       *
       * \see elastica::interface::Forcing
       */
      template <class ForcingTraits, class ForcingModule>
      class ExecutionPolicy
          : public CRTPHelper<ForcingModule, ExecutionPolicy> {
       private:
        //**Type definitions****************************************************
        //! CRTP Type
        using CRTP = CRTPHelper<ForcingModule, ExecutionPolicy>;
        //! CRTP methods
        using CRTP::self;
        //! Type of simulation time
        using TimeType = typename ForcingTraits::TimeType;
        //**********************************************************************

       protected:
        //**Type definitions****************************************************
        //! Definition of operator for connecting systems
        template <typename Policy, class Dummy = void>
        struct ForceSystemsOp;
        //! Sequential connection op
        using ForceSystemsInSequence =
            ForceSystemsOp<::elastica::configuration::sequential_policy>;
        //! Parallel connection op
        using ForceSystemsInParallel =
            ForceSystemsOp<::elastica::configuration::parallel_policy>;
        //! Hybrid connection op
        using ForceSystemsInHybrid =
            ForceSystemsOp<::elastica::configuration::hybrid_policy>;
        //**********************************************************************

        //**Forcing class*******************************************************
        /*!\name Forcing functor */
        //@{

        //**********************************************************************
        /*!\brief Force systems in sequence
         */
        template <class Dummy>
        struct ForceSystemsOp<::elastica::configuration::sequential_policy,
                              Dummy> {
          //********************************************************************
          /*!\brief Policy constructor.
           */
          ForceSystemsOp(ForcingModule* sim, ForcingTraits const& /*meta*/)
              : module_handle(*sim) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy constructor (copy constructed)
           */
          explicit ForceSystemsOp(
              std::tuple<ForcingModule*, ForcingTraits const&> args)
              : ForceSystemsOp(
                    cpp17::make_from_tuple<ForceSystemsOp>(std::move(args))) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy destructor.
           */
          ~ForceSystemsOp() = default;
          //********************************************************************

          //********************************************************************
          /*!\brief Execute forcing
           *
           * \param time_v The current time of simulation
           */
          inline auto operator()(TimeType time_v) const noexcept -> void {
            for (auto const& batch : module_handle.forcer_batches()) {
              for (auto const& forcer : batch) {
                forcer->apply_forces(time_v);
                forcer->apply_torques(time_v);
              }
            }
          }
          //********************************************************************

          //**Member variables**************************************************
          //! Handle to the ForcingModule
          ForcingModule const& module_handle;
          //********************************************************************
        };
        //**********************************************************************

        //**********************************************************************
        /*!\brief Force systems in parallel
         */
        template <class Dummy>
        struct ForceSystemsOp<::elastica::configuration::parallel_policy,
                              Dummy> {
          //********************************************************************
          /*!\brief Policy constructor.
           */
          ForceSystemsOp(ForcingModule* sim, ForcingTraits const& /*meta*/)
              : module_handle(*sim) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy constructor (copy constructed)
           */
          explicit ForceSystemsOp(
              std::tuple<ForcingModule*, ForcingTraits const&> args)
              : ForceSystemsOp(
                    cpp17::make_from_tuple<ForceSystemsOp>(std::move(args))) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy destructor.
           */
          ~ForceSystemsOp() = default;
          //********************************************************************

          //********************************************************************
          /*!\brief Execute forcing
           *
           * \param time_v The current time of simulation
           */
          inline auto operator()(TimeType time_v) const noexcept -> void {
            // ideally this will call the serial functor multiple times
            // over, but that might not scale due to construction costs (one
            // ptr)
            // DONE : parallelism across batches
            using ::elastica::size;
            ::elastica::parallel::for_each(
                0UL, size(module_handle.forcer_batches()),
                [&, time_v](std::size_t const idx) {
                  auto const& batch = module_handle.forcer_batches()[idx];
                  for (auto const& forcer : batch) {
                    forcer->apply_forces(time_v);
                    forcer->apply_torques(time_v);
                  }
                });
          }
          //********************************************************************

          //**Member variables**************************************************
          //! Handle to the ForcingModule
          ForcingModule const& module_handle;
          //********************************************************************
        };
        //**********************************************************************

        //**********************************************************************
        /*!\brief Hybridly execute algorithm to force systems
         */
        template <class Dummy>
        struct ForceSystemsOp<::elastica::configuration::hybrid_policy, Dummy> {
          //********************************************************************
          /*!\brief Policy constructor.
           */
          ForceSystemsOp(ForcingModule* sim, ForcingTraits const& traits)
              : force_systems_in_sequence_(sim, traits),
                force_systems_in_parallel_(sim, traits),
                module_handle(*sim),
                parallelism_threshold_(traits.forcing_parallelism_threshold()) {
          }
          //********************************************************************

          //********************************************************************
          /*!\brief Policy constructor (copy constructed)
           */
          explicit ForceSystemsOp(
              std::tuple<ForcingModule*, ForcingTraits const&> args)
              : ForceSystemsOp(
                    cpp17::make_from_tuple<ForceSystemsOp>(std::move(args))) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy destructor.
           */
          ~ForceSystemsOp() = default;
          //********************************************************************

          //********************************************************************
          /*!\brief Execute forcing of primitives
           *
           * \param time_v The current time of simulation
           */
          inline auto operator()(TimeType time_v) const noexcept -> void {
            // Is this if() check viable when put in a flow graph?
            if (module_handle.n_forcings() < parallelism_threshold_)
              force_systems_in_sequence_(time_v);
            else
              force_systems_in_parallel_(time_v);
          }
          //********************************************************************

          //**Member variables**************************************************
          //! Serial forcing functor
          ForceSystemsInSequence force_systems_in_sequence_;
          //! Parallel forcing functor
          ForceSystemsInParallel force_systems_in_parallel_;
          //! Handle to the ForcingModule
          ForcingModule const& module_handle;
          //! Threshold to switch between serial and parallel execution
          const std::size_t parallelism_threshold_;
          //********************************************************************
        };
        //**********************************************************************

        //@}
        //**********************************************************************

       protected:
        //**Algorithms**********************************************************
        /*! \cond ELASTICA_INTERNAL */
        /*!\name Algorithms for forcing*/
        //@{
        struct ForceSystems {
          //**Type definitions**************************************************
          //! Type for tagged tuple
          using type =
              ForceSystemsOp<typename ForcingTraits::ForcingExecutionPolicy>;
          //********************************************************************
        };
        //@}
        /*! \endcond */
        //**********************************************************************

       public:
        //**Algorithmic order***************************************************
        /*!\name Algorithmic order of steps within a temporal loop*/
        //@{
        //! Algorithms to be carried out after the kinematic update step
        using PostKinematicUpdateSteps = tmpl::list<>;
        //! Algorithms to be carried out before the dynamic update step
        using PreDynamicUpdateSteps = tmpl::list<ForceSystems>;
        //! Algorithms to be carried out after the dynamic update step
        using PostDynamicUpdateSteps = tmpl::list<>;
        //@}
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace forcing

  }  // namespace interface

}  // namespace elastica
