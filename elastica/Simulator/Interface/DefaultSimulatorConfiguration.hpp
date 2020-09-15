//==============================================================================
/*!
//  \file
//  \brief Definition of Default Simulator Configuration
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

// The base simulator configuration
#include "Simulator/Interface/SimulatorConfiguration.hpp"

// The abilities
#include "ModuleSettings/Domain.hpp"
#include "ModuleSettings/Systems.hpp"
#include "ModuleSettings/Environments.hpp"
// #include "ModuleSettings/Parallel.hpp"
// #include "ModuleSettings/Connections.hpp" // Needed?
// #include "ModuleSettings/Constraints.hpp" // Needed?

namespace elastica {

  namespace interface {

    //==========================================================================
    //
    //  ALIAS DECLARATIONS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Alias declaration for the default configuration of all simulations
    // \ingroup simulator
    //
    // The DefaultSimulatorConfiguration alias declaration serves as the main
    // customization point of a default Elastica++ simulation. It configures the
    // SimulatorConfiguration with default values as marked in the configuration
    // module. The following code example shows how to configure different
    // default simulations.

       \code
       // A simulation configuration with bare minimum requirements
       using BareMinimumDefaultSimulatorConfiguration =
           SimulatorConfiguration<defaults::DomainConfiguration,
                                  defaults::SystemsConfiguration>;

       // A simulation configuration with more modules
       using SimulatorConfigurationWithEnvironments =
           SimulatorConfiguration<defaults::DomainConfiguration,
                                  defaults::SystemsConfiguration,
                                  defaults::EnvironmentsConfiguration>;
       \endcode

    // \see SimulatorConfiguration make_simulator
    */
    using DefaultSimulatorConfiguration =
        SimulatorConfiguration<defaults::DomainConfiguration,
                               defaults::SystemsConfiguration,
                               defaults::EnvironmentsConfiguration>;
    //**************************************************************************

  }  // namespace interface

}  // namespace elastica