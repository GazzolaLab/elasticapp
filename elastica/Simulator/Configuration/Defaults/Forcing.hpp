//==============================================================================
/*!
//  \file
//  \brief Default Forcing configuration
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
#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"
#include "Simulator/Configuration/ModuleConfiguration/Forcing.hpp"
#include "Utilities/TMPL.hpp"  // for registrants

//==============================================================================
//
//  CONFIGURATION
//
//==============================================================================
#include "Configuration/Forcing.hpp"

namespace elastica {

  namespace configuration {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Default configuration of forcing in the simulation
     * \ingroup default_config
     *
     * \details
     * DefaultForcingConfiguration is the default compile-time customization
     * of @ref forcing in \elastica
     * It is default-constructible and the default settings can be changed using
     * ELASTICA_DEFAULT_EXECUTION_POLICY_FOR_FORCING() and
     * ELASTICA_DEFAULT_REGISTRARS_FOR_FORCING()
     */
    class DefaultForcingConfiguration
        : public ForcingConfiguration<
              ELASTICA_DEFAULT_EXECUTION_POLICY_FOR_FORCING,
              ELASTICA_DEFAULT_REGISTRARS_FOR_FORCING> {};
    //**************************************************************************

  }  // namespace configuration

}  // namespace elastica
