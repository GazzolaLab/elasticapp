//==============================================================================
/*!
//  \file
//  \brief Configuration for the Systems module of an \elastica Simulator
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
#include "Simulator/Configuration/ModuleConfiguration/Types.hpp"
///
#include "Simulator/Configuration/ModuleConfiguration/BlockingPolicy.hpp"
#include "Simulator/Configuration/ModuleConfiguration/IterationPolicy.hpp"
#include "Simulator/Configuration/Protocols/Systems.hpp"
//
#include "Systems/Types.hpp"  // For types to be registered
//
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/TMPL.hpp"  // for registrants
#include "Utilities/TypeTraits/Cpp20.hpp"

//==============================================================================
//
//  SYSTEM CONFIGURATION
//
//==============================================================================
#include "Configuration/Systems.hpp"  // for defaulted template list

namespace elastica {

  namespace configuration {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Configuration of systems within the Simulator module
     * \ingroup config
     *
     * \details
     * SystemsConfiguration denotes the compile-time customization point of the
     * systems modeled (such as types of cosserat rods, rigid bodies etc.)
     * within an ::elastica::interface::Simulator. It offers the ability to
     *
     * 1) customize the simulation with custom systems via the template
     * parameter.
     * 2) control (via policy classes) how new systems will be added
     * to blocks
     * 3) control (via policy classes) how systems will be iterated
     * upon (in serial/parallel) when executing some kernel over them.
     *
     * \tparam SystemPlugins Tags for indicating which systems need to be
     * assembled in the current \elastica simulation.
     * \tparam BlockingStrategy Policy parameter for how systems are blocked
     * within a simulator, see ::elastica::configuration::BlockingPolicy
     * \tparam IterationStrategy Policy parameter for how systems are iterated
     * upon  within a simulator, see ::elastica::configuration::IterationPolicy
     *
     * \see make_systems_configuration() for idiomatic construction
     */
    template <typename SystemPlugins, typename BlockingStrategy,
              typename IterationStrategy>
    class SystemsConfiguration
        : public ::tt::ConformsTo<protocols::SystemsConfiguration> {
     private:
      //**Type definitions******************************************************
      //! This type
      using This = SystemsConfiguration<SystemPlugins, BlockingStrategy,
                                        IterationStrategy>;
      //************************************************************************

     public:
      //**Type definitions******************************************************
      //! Tags of admissible systems in the default simulation
      using AdmissibleSystemPlugins =
          tmpl::transform<SystemPlugins, cpp20::remove_cvref<tmpl::_1>>;
      //! Should we check the system plugins for conformance
      using check_plugins_for_conformance = std::true_type;
      //! Policy for blocking
      using BlockingPolicy = BlockingStrategy;
      //! Policy for iteration
      using IterationPolicy = IterationStrategy;
      //************************************************************************

      //**Constructor***********************************************************
      /*!\name Constructor*/
      //@{
     public:
      //************************************************************************
      /*!\brief The default constructor.
       */
      constexpr SystemsConfiguration() = delete;
      //************************************************************************

      //************************************************************************
      /*!\brief The sole constructor.
       *
       * \param p Blocking policy
       * \param i Iteration policy
       */
      constexpr explicit SystemsConfiguration(BlockingPolicy p,
                                              IterationPolicy i) noexcept
          : blocking_policy_(std::move(p)), iteration_policy_(std::move(i)){};
      //************************************************************************

      //@}
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Obtains the policy for blocking
       */
      inline auto blocking_policy() const& noexcept -> BlockingPolicy const& {
        return This::blocking_policy_;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Obtains the policy for blocking
       */
      inline auto blocking_policy() && noexcept -> BlockingPolicy&& {
        return static_cast<BlockingPolicy&&>(This::blocking_policy_);
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Obtains the policy for iteration
       */
      inline auto iteration_policy() const& noexcept -> IterationPolicy const& {
        return This::iteration_policy_;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Obtains the policy for iteration
       */
      inline auto iteration_policy() && noexcept -> IterationPolicy&& {
        return static_cast<IterationPolicy&&>(This::iteration_policy_);
      }
      //************************************************************************

     private:
      //**Member variables******************************************************
      //! Policy parameter for blocking systems
      BlockingPolicy blocking_policy_;
      //! Policy parameter for iteration over systems
      IterationPolicy iteration_policy_;
      //************************************************************************
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Idiomatically makes a systems configuration
     * \ingroup config
     *
     * \example
     * \snippet ModuleConfiguration/Test_Systems.cpp seq_eg
     *
     * \param blocking A policy for determining blocking, see @ref blocking
     * \param iteration A policy for determining iterations on systems, see @ref
     * iteration
     * \tparam SystemPlugins A tmpl::list of tags of admissible systems
     * \return SystemsConfiguration
     */
    template <typename SystemPlugins =
                  ELASTICA_DEFAULT_ADMISSIBLE_PLUGINS_FOR_SYSTEMS,
              typename BlockingCriteria, typename ExecutionStrategyOverSystems,
              typename ExecutionStrategyForEachSystem,
              typename IterationCriterion>
    inline constexpr decltype(auto) make_systems_configuration(
        BlockingPolicy<BlockingCriteria> blocking,
        IterationPolicy<ExecutionStrategyOverSystems,
                        ExecutionStrategyForEachSystem, IterationCriterion>
            iteration) noexcept {
      using RT = SystemsConfiguration<
          SystemPlugins, BlockingPolicy<BlockingCriteria>,
          IterationPolicy<ExecutionStrategyOverSystems,
                          ExecutionStrategyForEachSystem, IterationCriterion>>;
      return RT{std::move(blocking), std::move(iteration)};
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Idiomatically makes a systems configuration
     * \ingroup config
     *
     * \param iteration A policy for determining iterations on systems, see @ref
     * iteration
     * \param blocking A policy for determining blocking, see @ref blocking
     * \tparam SystemPlugins A tmpl::list of tags of admissible systems
     * \return SystemsConfiguration
     */
    template <typename SystemPlugins =
                  ELASTICA_DEFAULT_ADMISSIBLE_PLUGINS_FOR_SYSTEMS,
              typename BlockingCriteria, typename ExecutionStrategyOverSystems,
              typename ExecutionStrategyForEachSystem,
              typename IterationCriterion>
    inline constexpr decltype(auto) make_systems_configuration(
        IterationPolicy<ExecutionStrategyOverSystems,
                        ExecutionStrategyForEachSystem, IterationCriterion>
            iteration,
        BlockingPolicy<BlockingCriteria> blocking) noexcept {
      return make_systems_configuration<SystemPlugins>(std::move(blocking),
                                                       std::move(iteration));
    }
    //**************************************************************************

  }  // namespace configuration

}  // namespace elastica