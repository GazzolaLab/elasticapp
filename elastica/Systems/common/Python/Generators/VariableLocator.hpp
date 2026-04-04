//==============================================================================
/*!
//  \file
//  \brief Custom type to make a variable locator in the Python side
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//******************************************************************************

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include <pybind11/pybind11.h>
//

namespace py_bindings {

  template <typename T>
  struct VariableLocator {
    constexpr auto data() & noexcept -> T& { return item_; }
    constexpr auto data() const& noexcept -> T const& { return item_; }
    T& item_;
  };

}  // namespace py_bindings