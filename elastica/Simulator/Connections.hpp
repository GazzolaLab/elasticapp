//==============================================================================
/*!
//  \file
//  \brief Inclusion of all connection components of \elastica
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
#include "Simulator/Connections/Types.hpp"
///
#include "Simulator/Connections/Connection.hpp"
#include "Simulator/Connections/MakeConnection.hpp"
#include "Simulator/Connections/Registration.hpp"
// include interface for all constraints finally
#include "Simulator/Interface/Connections.hpp"

//==============================================================================
//
//  DOXYGEN DOCUMENTATION
//
//==============================================================================

//******************************************************************************
/*!\defgroup connections Connections
 *
 * The connections module contains implementations of connectivity (joints)
 * linking two \elastica entities (rods, rigid bodies, etc.).
 */
//******************************************************************************

namespace elastica {

  //****************************************************************************
  /*!\brief Connection data-structures and subroutines
  // \ingroup connections
  */
  namespace connections {}
  //****************************************************************************

}  // namespace elastica
