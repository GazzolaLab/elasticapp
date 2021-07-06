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
#include "Utilities/TypeTraits/Cpp17.hpp"

namespace elastica {

  //============================================================================
  //
  //  CLASS DEFINITION
  //
  //============================================================================

  //****************************************************************************
  /*!\brief struct representing an unbounded boundary condition.
   * \ingroup domain
   *
   * IsUnbounded marks a particular direction as having an unbounded
   * boundary condition within the Domain of the \elastica library.
   *
   * \note This class is derived from std::false_type for ease in implementing
   * and/or logic when needed, see tt::SimulationDomainType
   */
  struct IsUnbounded : std::false_type {};
  //****************************************************************************

  //****************************************************************************
  /*!\brief struct representing a periodic boundary condition.
   * \ingroup domain
   *
   * IsPeriodic marks a particular direction as having a periodic boundary
   * condition within the Domain of the \elastica library.
   *
   * \note This class is derived from std::true_type for ease in implementing
   * and/or logic when needed, see tt::SimulationDomainType
   */
  struct IsPeriodic : std::true_type {};
  //****************************************************************************

}  // namespace elastica