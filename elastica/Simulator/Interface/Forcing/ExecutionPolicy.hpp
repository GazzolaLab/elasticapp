//==============================================================================
/*!
//  \file
//  \brief Algorithm execution policies for the \elastica Forcing module
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
#include <tuple>  // for construction support

///
#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"
#include "Simulator/Configuration/ModuleConfiguration/Streams.hpp"  // string
#include "Simulator/Interface/Affordances/detail/EmptyUpdateSteps.hpp"
#include "Simulator/Interface/Forcing/Types.hpp"
///
#include "Parallel/for_each.hpp"  // uses elastica's, not GLIBCXX
///
#include "Utilities/CRTP.hpp"
#include "Utilities/MakeString.hpp"
#include "Utilities/Size.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace interface {

    namespace forcing {

      //************************************************************************
      inline std::string forcing_algorithm_desc(std::size_t n_forcings) {
        return std::string(MakeString{} << "Apply " << n_forcings
                                        << " external forces");
      }
      //************************************************************************

      //************************************************************************
      template <typename Policy>
      inline std::string forcing_algorithm_desc(std::size_t n_forcings) {
        return forcing_algorithm_desc(n_forcings) + " using " +
               configuration::to_string(Policy{});
      }
      //************************************************************************

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

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
          : public CRTPHelper<ForcingModule, ExecutionPolicy>,
            public ::elastica::modules::detail::EmptyUpdateSteps<
                ForcingTraits, ExecutionPolicy<ForcingTraits, ForcingModule>> {
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
                    std::make_from_tuple<ForceSystemsOp>(std::move(args))) {}
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

          //********************************************************************
          /*!\brief Stream operator
           */
          friend std::ostream& operator<<(std::ostream& os,
                                          ForceSystemsOp const& op) {
            return os << forcing_algorithm_desc<
                       ::elastica::configuration::sequential_policy>(
                       op.module_handle.n_forcings());
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
                    std::make_from_tuple<ForceSystemsOp>(std::move(args))) {}
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
            ::elastica::parallel::for_each(module_handle.forcer_batches(),
                                           [time_v](auto const& batch) {
                                             for (auto const& forcer : batch) {
                                               forcer->apply_forces(time_v);
                                               forcer->apply_torques(time_v);
                                             }
                                           });
          }
          //********************************************************************

          //********************************************************************
          /*!\brief Stream operator
           */
          friend std::ostream& operator<<(std::ostream& os,
                                          ForceSystemsOp const& op) {
            return os << forcing_algorithm_desc<
                       ::elastica::configuration::parallel_policy>(
                       op.module_handle.n_forcings());
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
                    std::make_from_tuple<ForceSystemsOp>(std::move(args))) {}
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

          //********************************************************************
          /*!\brief Stream operator
           */
          friend std::ostream& operator<<(std::ostream& os,
                                          ForceSystemsOp const& op) {
            return os << forcing_algorithm_desc<
                       ::elastica::configuration::hybrid_policy>(
                       op.module_handle.n_forcings());
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

          //**Info method*******************************************************
          static std::string metadata(
              ForcingModule const& module_handle) noexcept {
            return forcing_algorithm_desc<
                typename ForcingTraits::ForcingExecutionPolicy>(
                module_handle.n_forcings());
          }
          //********************************************************************
        };
        //@}
        /*! \endcond */
        //**********************************************************************

       public:
        //**Algorithmic order***************************************************
        /*!\name Algorithmic order of steps within a temporal loop*/
        //@{
        //! Algorithms to be carried out before the dynamic update step
        using PreDynamicUpdateSteps = tmpl::list<ForceSystems>;
        //@}
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace forcing

  }  // namespace interface

}  // namespace elastica
