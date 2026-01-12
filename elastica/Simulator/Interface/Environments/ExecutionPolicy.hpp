//==============================================================================
/*!
//  \file
//  \brief Algorithm execution policies for the \elastica Environments module
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

#include "ModuleSettings/Profiling.hpp"
//
#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"
#include "Simulator/Configuration/ModuleConfiguration/Streams.hpp"  // string
//
#include "Simulator/Environments/Common.hpp"
//
#include "Simulator/Interface/Affordances/detail/EmptyUpdateSteps.hpp"
#include "Simulator/Interface/Environments/Types.hpp"
///
#include "Utilities/AsConst.hpp"
#include "Utilities/Invoke.hpp"
#include "Utilities/MakeFromTuple.hpp"
#include "Utilities/MakeString.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace interface {

    namespace environments {

      //************************************************************************
      inline std::string env_algorithm_desc(std::size_t n_environments) {
        return std::string(MakeString{} << "Apply effects from "
                                        << n_environments << " environments");
      }
      //************************************************************************

      //************************************************************************
      template <typename Policy>
      inline std::string env_algorithm_desc(std::size_t n_environments) {
        return env_algorithm_desc(n_environments) + " using " +
               configuration::to_string(Policy{});
      }
      //************************************************************************

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Execution policy for Environments module
       * \ingroup interface
       *
       * \details
       * ExecutionPolicy class represents the policy of execution for
       * applying environments to systems within the ambit of the environments
       * module. As such, the policies span a spectrum for purely sequential
       * execution to parallel, asynchronous execution.
       *
       * \tparam EnvironmentTraits Options for configuration of the Environments
       * Module
       * \tparam TotalSimulator The environments module of the Simulator
       *
       * \see elastica::interface::Environments
       */
      template <class EnvironmentsTraits, class TotalSimulator>
      class ExecutionPolicy
          : public ::elastica::modules::detail::EmptyUpdateSteps<
                EnvironmentsTraits,
                ExecutionPolicy<EnvironmentsTraits, TotalSimulator>> {
       private:
        //**Type definitions****************************************************
        //! Type of simulation time
        using TimeType = typename EnvironmentsTraits::TimeType;
        //**********************************************************************

       protected:
        //**Type definitions****************************************************
        //! Definition of operator for applying environmental forces
        template <typename Policy, class Dummy = void>
        struct ApplyEnvironmentForcesOp;
        //! Apply environmental forces sequentially
        using ApplyEnvironmentForcesOpInSequence = ApplyEnvironmentForcesOp<
            ::elastica::configuration::sequential_policy>;
        //! Apply environmental forces parallely
        using ApplyEnvironmentForcesOpInParallel = ApplyEnvironmentForcesOp<
            ::elastica::configuration::parallel_policy>;
        //**********************************************************************

        //**Environment functors************************************************
        /*!\name Environment functors*/
        //@{

        //**********************************************************************
        /*!\brief Apply environmental forces, processing each environment
         * sequentially
         */
        template <class Dummy>
        struct ApplyEnvironmentForcesOp<
            ::elastica::configuration::sequential_policy, Dummy> {
          //********************************************************************
          /*!\brief Policy constructor.
           */
          explicit ApplyEnvironmentForcesOp(TotalSimulator* sim,
                                            EnvironmentsTraits const& traits)
              : module_handle(*sim) {
            /*
             * Workaround to initialize environments from traits.
             * Here the "algorithm" initializes whats required
             */
            std::apply(
                [&](auto&... envs) {
                  using ::elastica::environments::initialize_parameters;
                  EXPAND_PACK_LEFT_TO_RIGHT(
                      (initialize_parameters(*envs, traits)));
                },
                module_handle.environments());
          }
          //********************************************************************

          //********************************************************************
          /*!\brief Policy constructor (copy constructed)
           */
          explicit ApplyEnvironmentForcesOp(
              std::tuple<TotalSimulator*, EnvironmentsTraits const&> args)
              : ApplyEnvironmentForcesOp(
                    std::make_from_tuple<ApplyEnvironmentForcesOp>(
                        std::move(args))) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy destructor.
           */
          ~ApplyEnvironmentForcesOp() = default;
          //********************************************************************

          //********************************************************************
          /*!\brief Execute connection of primitives
          //
          // \param time_v The current time of simulation
          */
          inline auto operator()(TimeType time_v) const noexcept -> void {
            // fill logic
            // More complex environments, expansion of system collection of
            // parameter packs done inside

            // This requires primitives to be in-built into environments
            module_handle.for_each_primitive(
                [](auto& p) { p.reset_forces_and_torques(); });
            module_handle.for_each_imposed_primitive(
                [](auto& p) { p.reset_forces_and_torques(); });

            tmpl::for_each<typename TotalSimulator::AdmissibleEnvironmentTags>(
                [this, time_v](auto v) {
                  using T = tmpl::type_from<decltype(v)>;
                  ELASTICA_PROFILING_SECTION {
                    this->module_handle.template get_environment_timer<T>()
                        .start();
                  }

                  ((this->module_handle).template get_environment<T>())
                      ->resolve(this->module_handle, time_v);

                  ELASTICA_PROFILING_SECTION {
                    this->module_handle.template get_environment_timer<T>()
                        .stop();
                  }
                });
          }
          //********************************************************************

          //********************************************************************
          /*!\brief Stream operator
           */
          friend std::ostream& operator<<(std::ostream& os,
                                          ApplyEnvironmentForcesOp const& op) {
            return os << env_algorithm_desc<
                       ::elastica::configuration::sequential_policy>(
                       op.module_handle.n_environments());
          }
          //********************************************************************

          //**Member variables**************************************************
          //! Handle to the Environments Module
          TotalSimulator& module_handle;
          //**********************************************************************
        };
        //************************************************************************

        //************************************************************************
        /*!\brief Apply environmental forces, processing each environment in
         * parallel sequentially
         */
        template <class Dummy>
        struct ApplyEnvironmentForcesOp<
            ::elastica::configuration::parallel_policy, Dummy> {
          //********************************************************************
          /*!\brief Policy constructor.
           */
          explicit ApplyEnvironmentForcesOp(TotalSimulator* sim,
                                            EnvironmentsTraits const& traits)
              : module_handle(*sim) {
            /*
             * Workaround to initialize environments from traits.
             * Here the "algorithm" initializes whats required
             */
            std::apply(
                [&](auto&... envs) {
                  using ::elastica::environments::initialize_parameters;
                  EXPAND_PACK_LEFT_TO_RIGHT(
                      (initialize_parameters(*envs, traits)));
                },
                module_handle.environments());
          }
          //********************************************************************

          //********************************************************************
          /*!\brief Policy constructor (copy constructed)
           */
          explicit ApplyEnvironmentForcesOp(
              std::tuple<TotalSimulator*, EnvironmentsTraits const&> args)
              : ApplyEnvironmentForcesOp(
                    std::make_from_tuple<ApplyEnvironmentForcesOp>(
                        std::move(args))) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy destructor.
           */
          ~ApplyEnvironmentForcesOp() = default;
          //********************************************************************

          //********************************************************************
          /*!\brief Execute connection of primitives
          //
          // \param time_v The current time of simulation
          */
          inline auto operator()(TimeType time_v) const noexcept -> void {
            // This requires primitives to be in-built into environments
            module_handle.for_each_primitive(
                [](auto& p) { p.reset_forces_and_torques(); });
            module_handle.for_each_imposed_primitive(
                [](auto& p) { p.reset_forces_and_torques(); });

            // Unsure if parallel processing is safe-here.
            tmpl::for_each<typename TotalSimulator::AdmissibleEnvironmentTags>(
                [this, time_v](auto v) {
                  using T = tmpl::type_from<decltype(v)>;
                  ELASTICA_PROFILING_SECTION {
                    this->module_handle.template get_environment_timer<T>()
                        .start();
                  }

                  ((this->module_handle).template get_environment<T>())
                      ->resolve(this->module_handle, time_v);

                  ELASTICA_PROFILING_SECTION {
                    this->module_handle.template get_environment_timer<T>()
                        .stop();
                  }
                });
          }
          //********************************************************************

          //********************************************************************
          /*!\brief Stream operator
           */
          friend std::ostream& operator<<(std::ostream& os,
                                          ApplyEnvironmentForcesOp const& op) {
            return os << env_algorithm_desc<
                       ::elastica::configuration::parallel_policy>(
                       op.module_handle.n_environments());
          }
          //********************************************************************

          //**Member variables**************************************************
          //! Handle to the Environments Module
          TotalSimulator& module_handle;
          //********************************************************************
        };
        //**********************************************************************

        //@}
        //**********************************************************************

       protected:
        //**Algorithms**********************************************************
        /*! \cond ELASTICA_INTERNAL */
        /*!\name Algorithms for environments*/
        //@{
        struct ApplyEnvironmentForces {
          //**Type definitions**************************************************
          //! Type for tagged tuple
          using type = ApplyEnvironmentForcesOp<
              typename EnvironmentsTraits::EnvironmentsExecutionPolicy>;
          //********************************************************************

          //**Info method*******************************************************
          static std::string metadata(
              TotalSimulator const& module_handle) noexcept {
            return env_algorithm_desc<
                typename EnvironmentsTraits::EnvironmentsExecutionPolicy>(
                module_handle.n_environments());
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
        using PreDynamicUpdateSteps = tmpl::list<ApplyEnvironmentForces>;
        //@}
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace environments

  }  // namespace interface

}  // namespace elastica