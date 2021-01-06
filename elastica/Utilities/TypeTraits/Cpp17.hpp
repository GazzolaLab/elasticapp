//==============================================================================
/*!
//  \file
//  \brief Implementation of some cpp17 type traits
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
#include <type_traits>
// void_t
#include "Utilities/TypeTraits/Void.hpp"

namespace cpp17 {
  /*!
   * \ingroup UtilitiesGroup
   * \brief Mark a return type as being "void". In C++17 void is a regular type
   * under certain circumstances, so this can be replaced by `void` then.
   *
   * The proposal is available
   * [here](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0146r1.html)
   */
  struct void_type {};

  /// \ingroup TypeTraitsGroup
  /// \brief A compile-time boolean
  ///
  /// \usage
  /// For any bool `B`
  /// \code
  /// using result = cpp17::bool_constant<B>;
  /// \endcode
  ///
  /// \see std::bool_constant std::integral_constant std::true_type
  /// std::false_type
  template <bool B>
  using bool_constant = std::integral_constant<bool, B>;

  // @{
  /// \ingroup TypeTraitsGroup
  /// \brief A logical AND on the template parameters
  ///
  /// \details
  /// Given a list of ::bool_constant template parameters computes their
  /// logical AND. If the result is `true` then derive off of std::true_type,
  /// otherwise derive from std::false_type. See the documentation for
  /// std::conjunction for more details.
  ///
  /// \usage
  /// For any set of types `Ti` that are ::bool_constant like
  /// \code
  /// using result = cpp17::conjunction<T0, T1, T2>;
  /// \endcode
  /// \pre For all types `Ti`, `Ti::value` is a `bool`
  ///
  /// \metareturns
  /// ::bool_constant
  ///
  /// \semantics
  /// If `T::value != false` for all `Ti`, then
  /// \code
  /// using result = cpp17::bool_constant<true>;
  /// \endcode
  /// otherwise
  /// \code
  /// using result = cpp17::bool_constant<false>;
  /// \endcode
  ///
  /// \example
  /// \snippet Utilities/Test_TypeTraits.cpp conjunction_example
  /// \see std::conjunction, disjunction, std::disjunction
  template <class...>
  struct conjunction : std::true_type {};
  /// \cond HIDDEN_SYMBOLS
  template <class B1>
  struct conjunction<B1> : B1 {};
  template <class B1, class... Bn>
  struct conjunction<B1, Bn...>
      : std::conditional_t<static_cast<bool>(B1::value), conjunction<Bn...>,
                           B1> {};
  /// \endcond
  /// \see std::conjunction
  template <class... B>
  constexpr bool conjunction_v = conjunction<B...>::value;
  // @}

  // @{
  /// \ingroup TypeTraitsGroup
  /// \brief A logical OR on the template parameters
  ///
  /// \details
  /// Given a list of ::bool_constant template parameters computes their
  /// logical OR. If the result is `true` then derive off of std::true_type,
  /// otherwise derive from std::false_type. See the documentation for
  /// std::conjunction for more details.
  ///
  /// \usage
  /// For any set of types `Ti` that are ::bool_constant like
  /// \code
  /// using result = cpp17::disjunction<T0, T1, T2>;
  /// \endcode
  /// \pre For all types `Ti`, `Ti::value` is a `bool`
  ///
  /// \metareturns
  /// ::bool_constant
  ///
  /// \semantics
  /// If `T::value == true` for any `Ti`, then
  /// \code
  /// using result = cpp17::bool_constant<true>;
  /// \endcode
  /// otherwise
  /// \code
  /// using result = cpp17::bool_constant<false>;
  /// \endcode
  ///
  /// \example
  /// \snippet Utilities/Test_TypeTraits.cpp disjunction_example
  /// \see std::disjunction, conjunction, std::conjunction
  template <class...>
  struct disjunction : std::false_type {};
  /// \cond HIDDEN_SYMBOLS
  template <class B1>
  struct disjunction<B1> : B1 {};
  template <class B1, class... Bn>
  struct disjunction<B1, Bn...>
      : std::conditional_t<static_cast<bool>(B1::value), B1,
                           disjunction<Bn...>> {};
  /// \endcond
  /// \see std::disjunction
  template <class... B>
  constexpr bool disjunction_v = disjunction<B...>::value;
  // @}

  /// \ingroup TypeTraitsGroup
  /// \brief Negate a ::bool_constant
  ///
  /// \details
  /// Given a ::bool_constant returns the logical NOT of it.
  ///
  /// \usage
  /// For a ::bool_constant `B`
  /// \code
  /// using result = cpp17::negate<B>;
  /// \endcode
  ///
  /// \metareturns
  /// ::bool_constant
  ///
  /// \semantics
  /// If `B::value == true` then
  /// \code
  /// using result = cpp17::bool_constant<false>;
  /// \endcode
  ///
  /// \example
  /// \snippet Utilities/Test_TypeTraits.cpp negation_example
  /// \see std::negation
  /// \tparam B the ::bool_constant to negate
  template <class B>
  struct negation : bool_constant<!B::value> {};

  /*!
   * \ingroup TypeTraitsGroup
   * \brief Variable template for is_same
   */
  template <typename T, typename U>
  constexpr bool is_same_v = std::is_same<T, U>::value;

}  // namespace cpp17
