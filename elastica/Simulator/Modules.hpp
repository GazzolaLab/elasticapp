//==============================================================================
/*!
//  \file
//  \brief Simulation modules
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

#include "Simulator/Interface/Connections.hpp"
#include "Simulator/Interface/Constraints.hpp"
// #include "Simulator/Interface/Environments.hpp"
#include "Simulator/Interface/Forcing.hpp"

//==============================================================================
//
//  DOXYGEN DOCUMENTATION
//
//==============================================================================

//******************************************************************************
/*!\defgroup modules Modules
 * \ingroup simulator
 * \brief Top-level modules for assembling a simulator
 *
 * Modules are entry-points for customizing a simulation in \elastica. By
 * including a module within a ::elastica::interface::Simulator, one gets access
 * to the different modular entities (such as those in @ref connections,
 * @ref constraints, @ref forcing etc.) within the context of assembling
 * a simulation.
 */
//******************************************************************************

namespace elastica {
  //****************************************************************************
  /*!\brief Top-level modules for assembling a simulator
   * \ingroup modules
   */
  namespace modules {}
  //****************************************************************************
}  // namespace elastica
