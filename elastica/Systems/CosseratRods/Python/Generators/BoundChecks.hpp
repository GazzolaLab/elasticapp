//==============================================================================
/*!
//  \file
//  \brief Helpers to check bounds of Rod  data types
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include <cstddef>
#include <stdexcept>
#include <string>

namespace py_bindings {

  //****************************************************************************
  /*!\brief Check if a Cosserat rod variable access is in bounds. Throws
   * std::runtime_error if it is not.
   * \ingroup python_bindings
   */
  inline void variable_bounds_check(std::size_t const limit,
                                    std::size_t const index) {
    if (index > limit) {
      throw std::runtime_error{
          "Out of bounds access (" + std::to_string(index) +
          ") into variable of size " + std::to_string(limit)};
    }
  }
  //****************************************************************************

}  // namespace py_bindings