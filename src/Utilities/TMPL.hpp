#pragma once
#include <type_traits>

namespace tmpl {
  // Types list
  template <class... T>
  struct list {};

  // Values list
  template <typename T, T... Values>
  using integral_list = list<std::integral_constant<T, Values>...>;

  // Empty list
  using empty_sequence = tmpl::list<>;
}  // namespace tmpl
