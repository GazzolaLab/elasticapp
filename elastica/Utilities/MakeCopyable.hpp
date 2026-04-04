//==============================================================================
/*!
//  \file
//  \brief Utility for "copying" non-copyable objects
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

#include <memory>
#include <type_traits>

//******************************************************************************
/*!\brief Creates a fake "copy" of a non-copyable Callable
// \ingroup UtilitiesGroup
//
// \details
// Creates a reference to a non-copyable Callable `f`, wraps it in a new
// callable and returns it instead
//
// \param f : Callable object to be copied
//
// \example
// Wrap `f` in a shared_ptr and return a lambda that forwards calls.
*/
template <class F>
auto make_copyable_function(F&& f) /*  noexcept */ {
  using dF = std::decay_t<F>;
  auto spf = std::make_shared<dF>(std::forward<F>(f));
  return [spf](auto&&... args) -> decltype(auto) {
    return (*spf)(decltype(args)(args)...);
  };
}
//******************************************************************************
