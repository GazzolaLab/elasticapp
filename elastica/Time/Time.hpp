//==============================================================================
/*!
//  \file
//  \brief Inclusion of all time components of \elastica
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

#include <utility>

#include "Time/Types.hpp"
//
#include "Time/Protocols.hpp"
#include "Time/SimulationTime.hpp"
#include "Time/SymplecticSteppers.hpp"
#include "Time/TypeTraits.hpp"

//==============================================================================
//
//  DOXYGEN DOCUMENTATION
//
//==============================================================================

//******************************************************************************
/*!\defgroup time Time
 * \brief Utilities for temporal marching in \elastica
 *
 * The Time module contains functionality for stepping an \elastica simulation
 * forward (or backward!) in time. It additionally contains numerical algorithms
 * for marching simulations.
 */
//******************************************************************************

namespace elastica {

  //****************************************************************************
  /*!\brief Numerical algorithms for time-stepping
  * \ingroup time
  */
  namespace timestepper {}
  //****************************************************************************

}  // namespace elastica
