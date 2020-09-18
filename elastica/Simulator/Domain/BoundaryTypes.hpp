//==============================================================================
/*!
//  \file
//  \brief Boundary types implementation
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

#include <type_traits>

#include "Simulator/Domain/Types.hpp"

namespace elastica {

  //============================================================================
  //
  //  CLASS DEFINITION
  //
  //============================================================================

  //****************************************************************************
  /*!\brief Struct representing an unbounded boundary condition.
  // \ingroup domain
  //
  // The UnboundedBoundary is a struct that marks a particular direction within
  // the Domain as having an unbounded boundary condition within the \elastica
  // library.
  //
  // \note This class is derived from std::false_type for ease in implementing
  // and/or logic when needed
  */
  struct UnboundedBoundary : std::false_type {};
  //****************************************************************************

  //****************************************************************************
  /*!\brief Struct representing a periodic boundary condition.
  // \ingroup domain
  //
  // The UnboundedBoundary is a struct that marks a particular direction within
  // the Domain as having an periodic boundary condition within the \elastica
  // library.
  //
  // \note This class is derived from std::true_type for ease in implementing
  // and/or logic when needed
  */
  struct PeriodicBoundary : std::true_type {};
  //****************************************************************************

}  // namespace elastica