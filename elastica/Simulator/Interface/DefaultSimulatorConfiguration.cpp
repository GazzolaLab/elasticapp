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

//******************************************************************************
// Includes
//******************************************************************************

#include "Simulator/Interface/DefaultSimulatorConfiguration.hpp"

namespace elastica {

  namespace interface {

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
    template struct SimulatorConfiguration<defaults::DomainConfiguration,
                                           defaults::SystemsConfiguration,
                                           defaults::EnvironmentsConfiguration>;
    //**************************************************************************

  }  // namespace interface

}  // namespace elastica
