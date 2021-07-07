//
// Created by Tejaswin Parthasarathy on 6/15/21.
//
#include "Utilities/TypeTraits/IsConvertibleToA.hpp"

namespace {

  /// [is_convertible_to_a_example]
  template <class T>
  struct Base {};
  struct Base_D1 : Base<int> {};
  // CRTP
  struct Base_D2 : Base<Base_D2> {};
  struct Base_D1_D1 : Base_D1 {};
  struct NotDerived {};

  static_assert(::tt::is_convertible_to_a<Base, Base_D1>::value,
                "is_convertible_to_a failure!");
  static_assert(::tt::is_convertible_to_a<Base, Base_D2>::value,
                "is_convertible_to_a failure!");
  static_assert(::tt::is_convertible_to_a<Base, Base_D1_D1>::value,
                "is_convertible_to_a failure!");
  static_assert(not ::tt::is_convertible_to_a<Base, NotDerived>::value,
                "is_convertible_to_a failure!");

  // private inheritance should not work
  struct Base_D3 : private Base<int> {};
  static_assert(not ::tt::is_convertible_to_a<Base, Base_D3>::value,
                "is_convertible_to_a failure!");

  // multiple template arguments are not a problem
  template <typename A, typename B>
  struct BaseWithMultipleArgs : public Base<A> {};
  struct Base_D4 : public BaseWithMultipleArgs<int, char> {};

  static_assert(::tt::is_convertible_to_a<Base, Base_D4>::value,
                "is_convertible_to_a failure!");
  static_assert(::tt::is_convertible_to_a<BaseWithMultipleArgs, Base_D4>::value,
                "is_convertible_to_a failure!");
  /// [is_convertible_to_a_example]

}  // namespace