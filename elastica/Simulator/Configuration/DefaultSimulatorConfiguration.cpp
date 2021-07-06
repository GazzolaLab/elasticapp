//==============================================================================
/*!
//  \file
//  \brief Instantiations of Default Simulator Configuration
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

//******************************************************************************
// Includes
//******************************************************************************

#include "Simulator/Configuration/DefaultSimulatorConfiguration.hpp"

namespace elastica {

  namespace configuration {

    //==========================================================================
    //
    //  EXPLICIT INSTANTIATIONS
    //
    //==========================================================================

    /*
    // Explicitly instantiate only the classes we want to be defined.
    https://stackoverflow.com/questions/2351148/explicit-template-instantiation-when-is-it-used
    // Sad but true :
    https://stackoverflow.com/questions/11705407/explicit-template-instantiation-through-a-typedef
    */

    //**************************************************************************
    /*!\brief Explicit instantiation of DefaultSimulatorConfiguration
    // \ingroup simulator
    */
    // template class SimulatorConfiguration<
    //     configuration::DefaultDomainConfiguration,
    //     configuration::DefaultSystemsConfiguration,
    //     configuration::DefaultEnvironmentsConfiguration>;
    //**************************************************************************

    //**************************************************************************
    /*!\brief Default constructor.
     *
     * \note We specifically instantiate a default constructor here for
     * make_simulator() to work out of the box.
     */
    // DefaultSimulatorConfiguration::DefaultSimulatorConfiguration() = default;
//        : Parent(
//              DefaultConnectionsConfiguration{},
//              DefaultConstraintsConfiguration{}, DefaultDomainConfiguration{},
//              DefaultEnvironmentsConfiguration{}, DefaultForcingConfiguration{},
//              DefaultSystemsConfiguration{}){};
    //**************************************************************************

  }  // namespace configuration

}  // namespace elastica
