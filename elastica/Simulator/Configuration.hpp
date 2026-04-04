//==============================================================================
/*!
//  \file
//  \brief Inclusion of all configuration components of \elastica
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
#include "Simulator/Configuration/Types.hpp"
///
#include "Simulator/Configuration/ModuleConfiguration.hpp"
#include "Simulator/Configuration/Defaults.hpp"
#include "Simulator/Configuration/SimulatorConfiguration.hpp"
#include "Simulator/Configuration/DefaultSimulatorConfiguration.hpp"
#include "Simulator/Configuration/TypeTraits.hpp"

//==============================================================================
//
//  DOXYGEN DOCUMENTATION
//
//==============================================================================

//******************************************************************************
/*!\defgroup config Configuration
 * \brief Customization of \elastica simulation
 *
 * The configuration module contains functionality to tune (algorithms at
 * compile-time, with runtime parameters) \elastica to the client's
 * needs and requirements.
 */
//******************************************************************************

namespace elastica {

  //****************************************************************************
  /*!\brief Configuration functions and meta-functions
  // \ingroup config
  */
  namespace configuration {}
  //****************************************************************************

}  // namespace elastica
