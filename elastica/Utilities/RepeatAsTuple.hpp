//==============================================================================
/*!
//  \file
//  \brief Defines a function that constructs a tuple by repeating an argument
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

#include <tuple>    // forward as tuple
#include <utility>  // index sequences

namespace detail {

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  /*!\brief Helper for repeating an element Size times
   * \ingroup utiles
   */
  struct RepeatHelper {
    template <typename T, std::size_t... Is>
    static inline constexpr decltype(auto) apply(std::index_sequence<Is...>,
                                                 T&& t) {
      return std::make_tuple(((void)Is, t)..., std::forward<T>(t));
    }
  };
  /*! \endcond */
  //****************************************************************************

}  // namespace detail

//******************************************************************************
/*!\brief Create a `std::tuple<T, T, T, ... (Size)>{t, t, ... (Size)}`
 * \ingroup utils
 *
 * \details
 * Repeats an element `t` of type `T` `Size` times to form a tuple of `t`s
 *
 * \example
 * \snippet Test_RepeatAsTuple.cpp repeat_eg
 *
 * \tparam Size the size of the array
 * \param t The element to be repeated
 */
template <std::size_t Size, typename T>
inline constexpr decltype(auto) repeat_as_tuple(T&& t) {
  return detail::RepeatHelper::apply(
      std::make_index_sequence<(Size == 0 ? Size : Size - 1)>{},
      std::forward<T>(t));
}
//******************************************************************************
