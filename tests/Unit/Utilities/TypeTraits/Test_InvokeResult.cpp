//
// Created by Tejaswin Parthasarathy on 1/6/21.
//

#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/TypeTraits/InvokeResult.hpp"

namespace {

  /// [invoke_result_example]
  struct S {
    double operator()(char, int&);
  };

  static_assert(std::is_same<cpp17::tt::invoke_result<S, char, int&>::type,
                             double>::value,
                "invoke_result failure");
  /// [invoke_result_example]

  /// [invoke_result_t_example]
  struct P {
    float operator()(int) { return 1.0; }
  };

  static_assert(
      std::is_same<cpp17::tt::invoke_result_t<P, int>,
                   typename cpp17::tt::invoke_result<P, int>::type>::value,
      "invoke_result_t failure");
  /// [invoke_result_t_example]

  IGNORE_UNUSED void simple_static_function() {}

  IGNORE_UNUSED int simple_static_function_r() { return 42; }

  IGNORE_UNUSED int simple_static_function_r_with_arg(int v) { return v; }

  IGNORE_UNUSED const int& simple_static_function_r_with_ref_arg(const int& v) {
    return v;
  }

  static_assert(
      std::is_same<void, cpp17::tt::invoke_result_t<
                             decltype(simple_static_function)>>::value,
      "invoke_result failure");
  static_assert(
      std::is_same<int, cpp17::tt::invoke_result_t<
                            decltype(simple_static_function_r)>>::value,
      "invoke_result failure");
  static_assert(
      std::is_same<
          int, cpp17::tt::invoke_result_t<
                   decltype(simple_static_function_r_with_arg), int>>::value,
      "invoke_result failure");
  static_assert(
      std::is_same<const int&,
                   cpp17::tt::invoke_result_t<
                       decltype(simple_static_function_r_with_ref_arg),
                       const int&>>::value,
      "invoke_result failure");

}  // namespace