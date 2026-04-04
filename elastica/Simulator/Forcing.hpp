//==============================================================================
/*!
//  \file
//  \brief Inclusion of all forcing components of \elastica
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
 * \brief Implementation of different external forces that dynamically influence
 * degrees of freedom of an \elastica entity  (rods, rigid bodies etc.).
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
