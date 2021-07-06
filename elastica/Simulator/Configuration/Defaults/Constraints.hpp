//==============================================================================
/*!
//  \file
//  \brief Default Constraints configuration
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
#include "Simulator/Configuration/ModuleConfiguration/Constraints.hpp"
#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"
#include "Utilities/TMPL.hpp"  // for registrants

//==============================================================================
//
//  CONFIGURATION
//
//==============================================================================
#include "Configuration/Constraints.hpp"

namespace elastica {

  namespace configuration {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Default configuration of constraints in the simulation
     * \ingroup default_config
     *
     * \details
     * DefaultConstraintsConfiguration is the default compile-time customization
     * of @ref connections in \elastica
     * It is default-constructible and the default settings can be changed using
     * ELASTICA_DEFAULT_EXECUTION_POLICY_FOR_CONSTRAINTS() and
     * ELASTICA_DEFAULT_REGISTRARS_FOR_CONSTRAINTS()
     */
    class DefaultConstraintsConfiguration
        : public ConstraintsConfiguration<
              ELASTICA_DEFAULT_EXECUTION_POLICY_FOR_CONSTRAINTS,
              ELASTICA_DEFAULT_REGISTRARS_FOR_CONSTRAINTS> {};
    //**************************************************************************


  }  // namespace configuration

}  // namespace elastica
