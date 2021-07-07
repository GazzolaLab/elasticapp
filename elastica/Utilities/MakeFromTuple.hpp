//==============================================================================
/*!
//  \file
//  \brief Implementation of c++-17 `std::make_from_tuple`
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
#include <tuple>
#include <type_traits>
#include <utility>

#include "Utilities/Invoke.hpp"

namespace cpp17 {

  //****************************************************************************
  /*!\brief Construct an object of type `T`, using the elements of the tuple `t`
   * as the arguments to the constructor.
   * \ingroup utils
   *
   * \example
   * \snippet Test_MakeFromTuple.cpp make_from_tuple_eg
   *
   * \param t tuple whose elements to be used as arguments to the constructor of
   * T
   * \tparam T type to be constructed
   * \return The constructed T object.
   */
  template <class T, class Tuple>
  constexpr auto make_from_tuple(Tuple&& t) -> T {
    return index_apply<std::tuple_size<std::remove_reference_t<Tuple>>::value>(
        [t = std::forward<Tuple>(t)](auto... Is) mutable -> T {
          static_assert(
              std::is_constructible<T, decltype(std::get<Is>(
                                           std::declval<Tuple>()))...>::value,
              "Cannot construct object");
          return T(std::get<Is>(std::forward<Tuple>(t))...);
        });
  }
  //****************************************************************************

}  // namespace cpp17