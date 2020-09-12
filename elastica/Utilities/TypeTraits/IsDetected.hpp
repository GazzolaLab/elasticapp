//==============================================================================
/*!
//  \file blaze/util/typetraits/is_detected.h
//  \brief Header file for the is_detected type trait
//
//  Copyright (C) 2012-2020 Klaus Iglberger - All Rights Reserved
//
//  This file is part of the Blaze library. You can redistribute it and/or
modify it under
//  the terms of the New (Revised) BSD License. Redistribution and use in source
and binary
//  forms, with or without modification, are permitted provided that the
following conditions
//  are met:
//
//  1. Redistributions of source code must retain the above copyright notice,
this list of
//     conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice,
this list
//     of conditions and the following disclaimer in the documentation and/or
other materials
//     provided with the distribution.
//  3. Neither the names of the Blaze development group nor the names of its
contributors
//     may be used to endorse or promote products derived from this software
without specific
//     prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY
//  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES
//  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
NO EVENT
//  SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT,
//  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED
//  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR
//  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN
//  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH
//  DAMAGE.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include <type_traits>

#include "Utilities/TypeTraits/Void.hpp"

namespace tt {

  //============================================================================
  //
  //  CLASS DEFINITION
  //
  //============================================================================

  //****************************************************************************
  /*!\brief Default result type for the detected_t type trait.
  // \ingroup type_traits
  */
  struct NoSuchType {};
  //****************************************************************************

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  /*!\brief Auxiliary helper struct for the is_detected type trait.
  // \ingroup type_traits
  */
  template <typename Default, typename AlwaysVoid,
            template <typename...> class OP, typename... Ts>
  struct Detector {
    using value_type = std::false_type;
    using type = Default;
  };
  /*! \endcond */
  //****************************************************************************

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  /*!\brief Specialization of the Detector class template for a successful
  // evaluation of \a OP.
  // \ingroup type_traits
  */
  template <typename Default, template <typename...> class OP, typename... Ts>
  struct Detector<Default, cpp17::void_t<OP<Ts...> >, OP, Ts...> {
    using value_type = std::true_type;
    using type = OP<Ts...>;
  };
  /*! \endcond */
  //****************************************************************************

  //****************************************************************************
  /*!\brief Compile time detection of type properties.
  // \ingroup type_traits
  //
  // The detected_or type trait determines if the given \a OP can be evaluated
  // for the given types \a Ts. In case \a OP can be successfully evaluated,
  // detected_t evaluates to OP<Ts...>. Otherwise it evaluates to Default.

     \code
     template<class T>
     using Diff_t = typename T::difference_type;

     struct C { using difference_type = int; };
     struct D { };

     template< typename T >
     using difference_type = typename detected_or<ptrdiff_t, Diff_t, T>::type;

     static_assert( IsSame_v< difference_type<C>, int > ,
                    "C's difference_type should be int!" );
     static_assert( IsSame_v< difference_type<D>, ptrdiff_t >,
                    "D's difference_type should be ptrdiff_t!");
     \endcode
  */
  template <typename Default, template <typename...> class OP, typename... Ts>
  using detected_or = Detector<Default, cpp17::void_t<>, OP, Ts...>;
  //****************************************************************************

  //****************************************************************************
  /*!\brief Compile time detection of type properties.
  // \ingroup type_traits
  //
  // The is_detected type trait determines if the given \a OP can be evaluated
  // for the given types \a Ts. In case \a OP can be successfully evaluated,
  // is_detected is an alias for Truetype. Otherwise it is an alias for
  // Falsetype.

     \code
     template<class T>
     using CopyAssign_t = decltype(
                              std::declval<T&>() = std::declval<const T&>()
                          );

     struct A { };
     struct B { void operator=( const B& ) = delete; };

     static_assert( is_detected_v<CopyAssign_t, A> ,
                    "A should be copy assignable!" );
     static_assert( !is_detected_v<CopyAssign_t, B>,
                    "B should not be copy assignable!");
     \endcode

     \code
     template<class T>
     using Diff_t = typename T::difference_type;

     struct C { using difference_type = int; };
     struct D { };

     static_assert( is_detected_v<Diff_t, C> ,
                    "C should have a difference type!" );
     static_assert( !is_detected_v<Diff_t, D>,
                    "D should not have a difference type!");
     \endcode
  */
  template <template <typename...> class OP, typename... Ts>
  using is_detected = typename detected_or<NoSuchType, OP, Ts...>::value_type;
  //****************************************************************************

  //****************************************************************************
  /*!\brief Compile time detection of type properties.
  // \ingroup type_traits
  //
  // The detected_t type trait determines if the given \a OP can be evaluated
  // for the given types \a Ts. In case \a OP can be successfully evaluated,
  // detected_or evaluates to OP<Ts...>. Otherwise it evaluates to NoSuchType.

     \code
     template<class T>
     using Diff_t = typename T::difference_type;

     struct C { using difference_type = int; };
     struct D { };

     static_assert( IsSame_v< detected_t<Diff_t, C>, int > ,
                    "C's difference_type should be int!" );
     static_assert( IsSame_v< detected_t<Diff_t, D>, NoSuchType > ,
                    "D should not have a difference type!");
     \endcode
  */
  template <template <typename...> class OP, typename... Ts>
  using detected_t = typename detected_or<NoSuchType, OP, Ts...>::type;
  //****************************************************************************

  //****************************************************************************
  /*!\brief Auxiliary variable template for the is_detected type trait.
  // \ingroup type_traits
  //
  // The is_detected_v variable template provides a convenient shortcut to
  // access the nested \a value of the is_detected class template. For
  // instance, given the template \a OP and the two types \a T1 and \a T2 the
  // following two statements are identical:

     \code
     constexpr bool value1 = elastica::tt::is_detected<OP,T1,T2>::value;
     constexpr bool value2 = elastica::tt::is_detected_v<OP,T1,T2>;
     \endcode
  */
  template <template <typename...> class OP, typename... Ts>
  constexpr bool is_detected_v = is_detected<OP, Ts...>::value;
  //****************************************************************************

  //****************************************************************************
  /*!\brief Auxiliary variable template for the detected_or type trait.
  // \ingroup type_traits
  //
  // The detected_or_t variable template provides a convenient shortcut to
  // access the nested \a type of the detected_or class template. For instance,
  // given the type \a Default, the template \a OP and the two types \a T1 and
  // \a T2 the following two statements are identical:

     \code
     using type1 = typename blaze::detected_or<Default,OP,T1,T2>::type;
     using type2 = blaze::detected_or_t<Default,OP,T1,T2>;
     \endcode
  */
  template <typename Default, template <typename...> class OP, typename... Ts>
  using detected_or_t = typename detected_or<Default, OP, Ts...>::type;
  //****************************************************************************

}  // namespace tt
