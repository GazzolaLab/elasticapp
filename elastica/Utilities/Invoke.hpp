//==============================================================================
/*!
//  \file
//  \brief Defines std::invoke and friends
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

#include <cstdint>
#include <utility>
#include <tuple>

namespace index_apply_detail {

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  /*!\brief Implementation of index apply
  // \ingroup UtilitiesGroup
  */
  template <class F, std::size_t... Is>
  constexpr decltype(auto)
  index_apply_impl(F&& f, std::index_sequence<Is...> /* meta */) noexcept(
      noexcept(f(std::integral_constant<std::size_t, Is>{}...))) {
    return f(std::integral_constant<std::size_t, Is>{}...);
  }
  /*! \endcond */
  //****************************************************************************

}  // namespace index_apply_detail

//******************************************************************************
/*!\brief Invokes a function on compile-time indices
// \ingroup UtilitiesGroup
//
// \details
// Invoke the Callable object `f` with `N` integer parameters `t`, like so
// `f(0, 1, ..., N - 1)`.
//
// \param f : Callable object to be invoked on the indices
//
// \tparam N : The compile-time number of indices
//
// \example
// \snippet Test_Invoke.cpp index_apply_example
//
// index_apply() can also be used in a `constexpr` context, but is less useful
// in C++14 (but more useful in C++17 with constexpr lambdas)
//
// \see apply()
*/
template <std::size_t N, class F>
constexpr decltype(auto) index_apply(F&& f) noexcept(noexcept(
    index_apply_detail::index_apply_impl(std::forward<F>(f),
                                         std::make_index_sequence<N>{}))) {
  return index_apply_detail::index_apply_impl(std::forward<F>(f),
                                              std::make_index_sequence<N>{});
}
//******************************************************************************
