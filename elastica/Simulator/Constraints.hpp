//==============================================================================
/*!
//  \file
//  \brief Inclusion of all constraint components of \elastica
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
#include "Simulator/Constraints/Types.hpp"
///
#include "Simulator/Constraints/Constraint.hpp"
#include "Simulator/Constraints/MakeConstraint.hpp"
#include "Simulator/Constraints/Registration.hpp"
// #include "Simulator/Constraints/Others.hpp"

// include interface for all constraints finally
#include "Simulator/Interface/Constraints.hpp"

//==============================================================================
//
//  DOXYGEN DOCUMENTATION
//
//==============================================================================

//******************************************************************************
/*!\defgroup constraints Constraints
 * \brief Implementation of constraints (i.e . boundary conditions) imposing
 * degrees of freedom of an \elastica entity (such as rods, rigid bodies etc.).
 *
 */
//******************************************************************************

namespace elastica {

  //****************************************************************************
  /*!\brief Constraint data-structures and subroutines
  // \ingroup constraints
  */
  namespace constraints {}
  //****************************************************************************

}  // namespace elastica
