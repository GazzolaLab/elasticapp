//==============================================================================
/*!
//  \file
//  \brief Inclusion of all forcing components of \elastica
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
#include "Simulator/Forcing/Types.hpp"
///
#include "Simulator/Forcing/Forcing.hpp"
#include "Simulator/Forcing/MakeForcing.hpp"
#include "Simulator/Forcing/Registration.hpp"

// include interface for all forcing finally
#include "Simulator/Interface/Forcing/Module.hpp"

//==============================================================================
//
//  DOXYGEN DOCUMENTATION
//
//==============================================================================

//******************************************************************************
/*!\defgroup forcing Forcing
 *
 * The forcing module contains implementations of different external forcings
 * dynamically affecting various degrees of freedom of an \elastica entity
 * (rods, rigid bodies etc.).
 */
//******************************************************************************

namespace elastica {

  //****************************************************************************
  /*!\brief Forcing data-structures and subroutines
  // \ingroup forcing
  */
  namespace forcing {}
  //****************************************************************************

}  // namespace elastica
