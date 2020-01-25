#pragma once
#include <type_traits>

#include "TypeTraits.hpp"

namespace tmpl {
  // Types list
  template <class... T>
  struct list {};

  // Values list
  template <typename T, T... Values>
  using integral_list = list<std::integral_constant<T, Values>...>;

  // Empty list
  using empty_sequence = tmpl::list<>;

  //============================================================================
  //
  // MANIPULATING TYPELISTS
  //
  //============================================================================

  //****************************************************************************
  // Push back a type on to a list
  //****************************************************************************

  namespace detail {
    template <class TypeList,  // Type of the type list
              class... T>      // Types to be appended
    struct PushBackImpl;

    template <template <class...> class TypeList,  // Type list specializaiton
              class... U,                          // Types in the Typelist
              class... T>  // Types to be added to the Typelist
    struct PushBackImpl<TypeList<U...>, T...> {
      using type = TypeList<U..., T...>;
    };
  }  // namespace detail

  template <class TL, class... T>
  using push_back = typename detail::PushBackImpl<TL, T...>::type;

  //****************************************************************************
  // Push front a type on to a list
  //****************************************************************************

  namespace detail {
    template <class TypeList,  // Type of the type list
              class... T>      // Types to be appended
    struct PushFrontImpl;

    template <template <class...> class TypeList,  // Type list specializaiton
              class... U,                          // Types in the Typelist
              class... T>  // Types to be added to the Typelist
    struct PushFrontImpl<TypeList<U...>, T...> {
      using type = TypeList<T..., U...>;
    };
  }  // namespace detail

  template <class TL, class... T>
  using push_front = typename detail::PushFrontImpl<TL, T...>::type;

  //============================================================================
  //
  // SEARCHING TYPELISTS
  //
  //============================================================================
  namespace detail {
    template <class TypeList,  // Type of the type lists
              class T>         // Type to be searched
    struct ContainsImpl;

    template <template <class...> class TypeList,  // Type list specializaiton
              class... U,                          // Types in the Typelist
              class T>                             // Type to be searched
    struct ContainsImpl<TypeList<U...>, T>
        : public tt::detail::IsContained<T, U...> {};
  }  // namespace detail

  template <class TL, class T>
  constexpr bool contains = detail::ContainsImpl<TL, T>::value;

}  // namespace tmpl
