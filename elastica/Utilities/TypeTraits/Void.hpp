//==============================================================================
/*!
//  \file
//  \brief Implementation of cpp17 void_t type trait
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

namespace cpp17 {

  /*!\brief Given a set of types, returns `void`
  // \ingroup type_traits
  //
  // \details
  // Given a list of types, returns `void`. This is very useful for
  // controlling name lookup resolution via partial template specialization.
  //
  // \usage
  // For any set of types `Ti`,
     \code
       using result = cpp17::void_t<T0, T1, T2, T3>;
     \endcode
  //
  // \metareturns
  // void
  //
  // \semantics
  // For any set of types `Ti`,
     \code
       using result = void;
     \endcode
  //
  // \example
  // \snippet Utilities/Test_TypeTraits.cpp void_t_example
  //
  // \see std::void_t
  //
  // \tparam Ts the set of types
  */
  template <typename... Ts>
  using void_t = void;

}  // namespace cpp17
