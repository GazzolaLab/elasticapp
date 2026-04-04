//==============================================================================
/*!
//  \file
//  \brief Default Connections configuration
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
#include "Simulator/Configuration/Defaults/Types.hpp"
///
#include "Simulator/Configuration/ModuleConfiguration/Connections.hpp"
#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"
#include "Utilities/TMPL.hpp"  // for registrants

//==============================================================================
//
//  CONFIGURATION
//
//==============================================================================
#include "Configuration/Connections.hpp"

namespace elastica {

  namespace configuration {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Default configuration of connections in the simulation
     * \ingroup default_config
     *
     * \details
     * DefaultConnectionsConfiguration is the default compile-time customization
     * of @ref connections in \elastica.
     * It is default-constructible and the default settings can be changed using
     * ELASTICA_DEFAULT_EXECUTION_POLICY_FOR_CONNECTIONS() and
     * ELASTICA_DEFAULT_REGISTRARS_FOR_CONNECTIONS()
     */
    class DefaultConnectionsConfiguration
        : public ConnectionsConfiguration<
              ELASTICA_DEFAULT_EXECUTION_POLICY_FOR_CONNECTIONS,
              ELASTICA_DEFAULT_REGISTRARS_FOR_CONNECTIONS> {};
    //**************************************************************************

  }  // namespace configuration

}  // namespace elastica
