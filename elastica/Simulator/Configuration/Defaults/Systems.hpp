//==============================================================================
/*!
//  \file
//  \brief Default systems configuration
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
#include "Simulator/Configuration/Defaults/Types.hpp"
///
////////// Forward declarations come first
#include "Systems/RigidBody/Types.hpp"
//////////
#include "Simulator/Configuration/ModuleConfiguration/BlockingPolicy.hpp"
#include "Simulator/Configuration/ModuleConfiguration/IterationPolicy.hpp"
#include "Simulator/Configuration/ModuleConfiguration/Systems.hpp"
// #include "Systems/Tags.hpp"
#include "Utilities/TMPL.hpp"  // needed because the Macro is a typelist

//==============================================================================
//
//  SYSTEM CONFIGURATION
//
//==============================================================================
#include "Configuration/Systems.hpp"

namespace elastica {

  namespace configuration {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Default configuration for iteration over systems in the simulation
     * \ingroup default_config
     *
     * It is default-constructible and the default settings can be changed using
     * ELASTICA_DEFAULT_ITERATION_POLICY_ACROSS_SYSTEM_TYPES() and
     * ELASTICA_DEFAULT_ITERATION_POLICY_FOR_EACH_SYSTEM_TYPE().
     */
    struct DefaultIterationPolicy
        : IterationPolicy<
              ELASTICA_DEFAULT_ITERATION_POLICY_ACROSS_SYSTEM_TYPES,
              ELASTICA_DEFAULT_ITERATION_POLICY_FOR_EACH_SYSTEM_TYPE, void> {
     private:
      using Parent = IterationPolicy<
          ELASTICA_DEFAULT_ITERATION_POLICY_ACROSS_SYSTEM_TYPES,
          ELASTICA_DEFAULT_ITERATION_POLICY_FOR_EACH_SYSTEM_TYPE, void>;

     public:
      using Parent::Parent;
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Default configuration of systems in the simulation
     * \ingroup default_config
     *
     * \details
     * DefaultSystemsConfiguration is the default compile-time customization
     * of @ref systems in \elastica
     *
     * \details
     * DefaultForcingConfiguration is the default compile-time customization
     * of @ref connections in \elastica.
     * It is default-constructible and the default settings can be changed using
     * ELASTICA_DEFAULT_ADMISSIBLE_PLUGINS_FOR_SYSTEMS() and
     * ELASTICA_DEFAULT_BLOCKING_POLICY_FOR_SYSTEMS(). The iteration policy
     * in this case is DefaultIterationPolicy, see its documentation for
     * customizing it.
     */
    class DefaultSystemsConfiguration
        : public SystemsConfiguration<
              ELASTICA_DEFAULT_ADMISSIBLE_PLUGINS_FOR_SYSTEMS,
              ELASTICA_DEFAULT_BLOCKING_POLICY_FOR_SYSTEMS,
              DefaultIterationPolicy> {
     private:
      //**Type definitions******************************************************
      //! Parent type
      using Parent =
          SystemsConfiguration<ELASTICA_DEFAULT_ADMISSIBLE_PLUGINS_FOR_SYSTEMS,
                               ELASTICA_DEFAULT_BLOCKING_POLICY_FOR_SYSTEMS,
                               DefaultIterationPolicy>;

     public:
      //! Inherit constructors
      DefaultSystemsConfiguration() noexcept
          : Parent(
                ELASTICA_DEFAULT_BLOCKING_POLICY_FOR_SYSTEMS{
                    default_blocking_policy_size()},
                DefaultIterationPolicy{}){};
      ~DefaultSystemsConfiguration() = default;
      //************************************************************************
    };
    //**************************************************************************

    // DONE Type trait/protocol for valid Systems Configuration

  }  // namespace configuration

}  // namespace elastica
