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
  // the Domain as having an unbounded boundary condition within the Elastica++
  // library.
  */
  struct UnboundedBoundary : std::false_type {};
  //****************************************************************************

  //****************************************************************************
  /*!\brief Struct representing a periodic boundary condition.
  // \ingroup domain
  //
  // The UnboundedBoundary is a struct that marks a particular direction within
  // the Domain as having an periodic boundary condition within the Elastica++
  // library.
  */
  struct PeriodicBoundary : std::true_type {};
  //****************************************************************************

  /* NOTE : These are derived from true/false so that its easier to do and/or
   * logic
   */

}  // namespace elastica