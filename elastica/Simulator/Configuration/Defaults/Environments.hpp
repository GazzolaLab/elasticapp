//==============================================================================
/*!
//  \file
//  \brief Default environments configuration
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
#include "Simulator/Configuration/ModuleConfiguration/Environments.hpp"
#include "Simulator/Environments/Primitives/Types.hpp"  // needed for template instantiation
#include "Simulator/Environments/Tags.hpp"  // needed for template instantiation
#include "Utilities/TMPL.hpp"               // tags are type lists

//==============================================================================
//
//  ENVIRONMENT CONFIGURATION
//
//==============================================================================
#include "Configuration/Environments.hpp"

namespace elastica {

  namespace configuration {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Default configuration of environments in the simulation
     * \ingroup default_config
     *
     * \details
     * DefaultEnvironmentsConfiguration is the default compile-time
     * customization of @ref environments in \elastica
     * It is default-constructible and the default settings can be changed using
     * ELASTICA_DEFAULT_ADMISSIBLE_ENVIRONMENT_TAGS(),
     * ELASTICA_DEFAULT_ADMISSIBLE_IMPOSED_PRIMITIVES(),
     * ELASTICA_DEFAULT_EXECUTION_POLICY_FOR_ENVIRONMENTS().
     */
    struct DefaultEnvironmentsConfiguration
        : public EnvironmentsConfiguration<
              ELASTICA_DEFAULT_ADMISSIBLE_ENVIRONMENT_TAGS,
              ELASTICA_DEFAULT_ADMISSIBLE_IMPOSED_PRIMITIVES,
              ELASTICA_DEFAULT_EXECUTION_POLICY_FOR_ENVIRONMENTS,
              DefaultGravityFieldUserConfiguration> {
     private:
      //**Type definitions******************************************************
      //! Parent type
      using Parent = EnvironmentsConfiguration<
          ELASTICA_DEFAULT_ADMISSIBLE_ENVIRONMENT_TAGS,
          ELASTICA_DEFAULT_ADMISSIBLE_IMPOSED_PRIMITIVES,
          ELASTICA_DEFAULT_EXECUTION_POLICY_FOR_ENVIRONMENTS,
          DefaultGravityFieldUserConfiguration>;

     public:
      //! Inherit constructors
      DefaultEnvironmentsConfiguration() noexcept
          : Parent(DefaultGravityFieldUserConfiguration{}){};
      ~DefaultEnvironmentsConfiguration() = default;
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace configuration

}  // namespace elastica
