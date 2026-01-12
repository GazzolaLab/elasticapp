//
// Created by Tejaswin Parthasarathy on 1/6/21.
//

#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/TypeTraits/IsInvokable.hpp"

namespace {

  int IGNORE_UNUSED simple_static_function() { return int{}; }
  int IGNORE_UNUSED simple_static_function_r() { return 42; }
  int IGNORE_UNUSED simple_static_function_r_with_arg(int v) { return v; }
  const int& IGNORE_UNUSED simple_static_function_r_with_ref_arg(const int& v) {
    return v;
  }

  /// [is_invocable_example]
  static_assert(std::is_invocable<int()>::value, "is_invocable failure");
  /// [is_invocable_example]

  /// [is_invocable_v_example]
  static_assert(std::is_invocable_v<int()> ==
                    std::is_invocable<int()>::value,
                "is_invocable_v failure");
  /// [is_invocable_v_example]

  static_assert(std::is_invocable<decltype(simple_static_function)>::value,
                "is_invocable failure");
  static_assert(std::is_invocable<decltype(simple_static_function_r)>::value,
                "is_invocable failure");
  static_assert(std::is_invocable<decltype(simple_static_function_r_with_arg),
                                    int>::value,
                "is_invocable failure");
  static_assert(
      std::is_invocable<decltype(simple_static_function_r_with_ref_arg),
                          int>::value,
      "is_invocable failure");

  // also test for false negatives
  static_assert(
      !std::is_invocable<decltype(simple_static_function), int>::value,
      "unit test fail");
  struct S {
    int x;
    float f;
    char c;
  };
  static_assert(
      !std::is_invocable<decltype(simple_static_function_r), S>::value,
      "unit test fail");
  static_assert(
      !std::is_invocable<decltype(simple_static_function_r_with_arg)>::value,
      "unit test fail");

  /// [is_invocable_r_example]
  static_assert(std::is_invocable_r<int, int()>::value,
                "is_invocable_r failure");
  static_assert(std::is_invocable_r<void, void(int), int>::value,
                "is_invocable_r failure");
  /// [is_invocable_r_example]

  /// [is_invocable_r_v_example]
  static_assert(std::is_invocable_r_v<void, void(int), int> ==
                    std::is_invocable_r<void, void(int), int>::value,
                "is_invocable_v failure");
  /// [is_invocable_r_v_example]

  static_assert(
      std::is_invocable_r<void, decltype(simple_static_function)>::value,
      "is_invocable_r failure");
  static_assert(
      std::is_invocable_r<int, decltype(simple_static_function_r)>::value,
      "is_invocable_r failure");
  static_assert(
      std::is_invocable_r<void, decltype(simple_static_function_r)>::value,
      "is_invocable_r failure");
  static_assert(std::is_invocable_r<
                    const int&, decltype(simple_static_function_r_with_arg),
                    const int&>::value,
                "is_invocable_r failure");

  // also test for false negatives
  static_assert(!std::is_invocable_r<void, decltype(simple_static_function),
                                       int>::value,
                "is_invocable_r failure");
  static_assert(
      !std::is_invocable_r<int, decltype(simple_static_function_r), S>::value,
      "is_invocable_r failure");
  static_assert(!std::is_invocable_r<
                    const int&, decltype(simple_static_function_r_with_arg),
                    const S&>::value,
                "is_invocable_r failure");

  /// [is_nothrow_callable]
  auto IGNORE_UNUSED simple_lambda = [](int x) -> int { return x; };
  auto IGNORE_UNUSED simple_lambda_nothrow = [](int x) noexcept -> int {
    return x;
  };
  static_assert(
      !std::is_nothrow_callable<decltype(simple_lambda), int>::value,
      "is_nothrow_callable failure");
  static_assert(
      std::is_nothrow_callable<decltype(simple_lambda_nothrow), int>::value,
      "is_nothrow_callable failure");
  /// [is_nothrow_callable]

  /// [is_nothrow_invocable_example]
  struct MayThrow {
    void operator()(float f);
  };
  struct NoThrow {
    void operator()(float f) noexcept;
  };
  static_assert(!std::is_nothrow_invocable<MayThrow, float>::value,
                "is_nothrow_invocable failure");
  static_assert(std::is_nothrow_invocable<NoThrow, float>::value,
                "is_nothrow_invocable failure");
  /// [is_nothrow_invocable_example]

  /// [is_nothrow_invocable_v_example]
  static_assert(std::is_nothrow_invocable_v<int(int), int> ==
                    std::is_nothrow_invocable<int(int), int>::value,
                "is_nothrow_invocable_v failure");
  /// [is_nothrow_invocable_v_example]

  /// [is_nothrow_invocable_r_example]
  static_assert(!std::is_nothrow_invocable_r<int, int()>::value,
                "is_nothrow_invocable_r failure");
  auto IGNORE_UNUSED simple_lambda_nothrow_voidr = [](int x) noexcept -> void {
    elastica::ignore_unused(x);
  };
  static_assert(std::is_nothrow_invocable_r<
                    void, decltype(simple_lambda_nothrow_voidr), int>::value,
                "is_nothrow_invocable_r failure");
  /// [is_nothrow_invocable_r_example]

  /// [is_nothrow_invocable_v_example]
  static_assert(
      std::is_nothrow_invocable_r_v<int, int(float), float> ==
          std::is_nothrow_invocable_r<int, int(float), float>::value,
      "is_nothrow_invocable_r_v failure");
  /// [is_nothrow_invocable_v_example]

}  // namespace
