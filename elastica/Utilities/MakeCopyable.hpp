//==============================================================================
/*!
//  \file
//  \brief Utility for "copying" non-copyable objects
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
// \snippet Test_MakeCopyable.cpp make_copyable_function
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
