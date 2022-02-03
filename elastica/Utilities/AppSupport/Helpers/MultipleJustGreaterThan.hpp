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

  template <typename T>
  auto multiple_just_greater_than(T value, T factor) noexcept -> T {
    return std::nearbyint(value / factor);
  }

}  // namespace elastica