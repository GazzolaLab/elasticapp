//
// Created by Tejaswin Parthasarathy on 1/21/21.
//

#include <type_traits>

#include "Utilities/TypeTraits/Void.hpp"

/// [void_t_example]
static_assert(std::is_same<cpp17::void_t<char, bool, double>, void>::value,
              "Failed testing type trait void_t");

struct A {
  A(const A&) = delete;
};
template <class T>
using copy_assign_t = decltype(std::declval<T&>() = std::declval<const T&>());
// results in void type even if not well-formed
static_assert(std::is_same<cpp17::void_t<copy_assign_t<A>>, void>::value,
              "Failed testing type trait void_t");
/// [void_t_example]
