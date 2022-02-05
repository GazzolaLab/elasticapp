//==============================================================================
/*!
//  \file
//  \brief Calculates a multiple just greater than
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include <cmath>

namespace elastica {

  //****************************************************************************
  /*!\brief Calculates a multiple of `factor` just greater than `value`
  // \ingroup application_support
  //
  // \details
  // Used idiomatically in \elastica to calculate the number of time-steps need
  // for a time `value` given a `dt` factor.
  //
  // \param value  Functions as dividend
  // \param factor Functions as divisor
  //
  // \example
  // \snippet Test_MultipleJustGreaterThan.cpp multiple_greater_eg
  */
  template <typename T>
  auto multiple_just_greater_than(T value, T factor) noexcept -> T {
    // TODO : refactor magic number here
    return std::ceil((value - T(1e-14)) / factor);
  }
  //****************************************************************************

}  // namespace elastica