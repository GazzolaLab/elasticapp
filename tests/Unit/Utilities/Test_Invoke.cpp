//
// Created by Tejaswin Parthasarathy on 1/5/21.
//
#include <utility>

#include "Framework/TestingFramework.hpp"
#include "Utilities/Invoke.hpp"
#include "Utilities/TMPL.hpp"

namespace {
  int simple_static_function(){};
  int simple_static_function_r() { return 42; }
  int simple_static_function_r_with_arg(int v) { return v; }
  const int& simple_static_function_r_with_ref_arg(const int& v) { return v; }

  ELASTICA_TEST_CASE("Unit.Utilities.invoke", "[Unit][Utilities]") {
    // From
    // https://github.com/BlackMATov/invoke.hpp/blob/main/untests/invoke_tests.cpp
    cpp17::invoke(simple_static_function_r);
    CHECK(cpp17::invoke(simple_static_function_r) == 42);
    CHECK(cpp17::invoke(simple_static_function_r_with_arg, 42) == 42);
    {
      int v = 42;
      CHECK(&cpp17::invoke(simple_static_function_r_with_ref_arg, v) == &v);
    }

    /// [invoke_example]
    CHECK(cpp17::invoke([]() { return 42; }) == 42);
    // sum of 2
    CHECK(cpp17::invoke([](int x, int y) { return x + y; }, 19, 23) == 42);
    // sum of first 9 integers
    CHECK(cpp17::invoke(
              [](auto... args) {
                int sum = 0;
                EXPAND_PACK_LEFT_TO_RIGHT((sum += args));
                return sum;
              },
              1, 2, 3, 4, 5, 6, 7, 8, 9) == 45);
    /// [invoke_example]
  }

  /// [invoke_constexpr]
  constexpr int two_sum(int x, int y) { return x + y; }

  // sum of two integers in a constexpr context
  // integral constants have implicit constexpr conversions to its type
  static_assert(cpp17::invoke(two_sum, std::integral_constant<int, 8>{},
                              std::integral_constant<int, 9>{}) == 17,
                "Constexpr invoke failure");

  // sum of first 9 integers in a constexpr context
  template <int... Is>
  constexpr int variadic_sum() {
    int sum = 0;
    EXPAND_PACK_LEFT_TO_RIGHT(sum += Is);
    return sum;
  }

  template <int... Is>
  constexpr int variadic_sum_helper(std::integer_sequence<int, Is...>) {
    // to help deduction of variadic sum template
    return cpp17::invoke(variadic_sum<Is...>);
  }

  static_assert(variadic_sum_helper(std::make_integer_sequence<int, 5>{}) == 10,
                "Constexpr invoke failure");
  /// [invoke_constexpr]

  ELASTICA_TEST_CASE("Unit.Utilities.apply", "[Unit][Utilities]") {
    // From
    // https://github.com/BlackMATov/invoke.hpp/blob/main/untests/invoke_tests.cpp
    cpp17::apply(simple_static_function_r, std::make_tuple());
    CHECK(cpp17::apply(simple_static_function_r, std::make_tuple()) == 42);
    CHECK(cpp17::apply(simple_static_function_r_with_arg,
                       std::make_tuple(42)) == 42);
    {
      int v = 42;
      CHECK(&cpp17::apply(simple_static_function_r_with_ref_arg,
                          std::make_tuple(std::ref(v))) == &v);
    }
    /// [apply_example]
    CHECK(cpp17::apply([]() { return 42; }, std::make_tuple()) == 42);
    // sum of 2
    CHECK(cpp17::apply([](int x, int y) { return x + y; },
                       std::make_tuple(19, 23)) == 42);
    // sum of 2, but with a pair
    CHECK(cpp17::apply([](int x, int y) { return x + y; },
                       std::make_pair(16, 26)) == 42);
    // sum of first 9 integers
    CHECK(cpp17::apply(
              [](auto... args) {
                int sum = 0;
                EXPAND_PACK_LEFT_TO_RIGHT((sum += args));
                return sum;
              },
              std::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9)) == 45);
    /// [apply_example]
  }

  /// [apply_constexpr]
  // sum of two integers in a constexpr context
  static_assert(
      cpp17::apply(two_sum, std::make_pair(std::integral_constant<int, 8>{},
                                           std::integral_constant<int, 9>{})) ==
          17,
      "Constexpr apply failure");

  // factorial in a constexpr context
  template <typename... Is>
  constexpr int product(Is... is) {
    int product = 1;
    EXPAND_PACK_LEFT_TO_RIGHT(product *= is);
    return product;
  }

  static_assert(cpp17::apply(product<int, int, int, int>,
                             std::make_tuple(1, 2, 3, 4)) == 24,
                "Constexpr apply failure");
  /// [apply_constexpr]

  ELASTICA_TEST_CASE("Unit.Utilities.index_apply", "[Unit][Utilities]") {
    index_apply<0>(simple_static_function_r);
    CHECK(index_apply<0>(simple_static_function_r) == 42);
    CHECK(index_apply<1>(simple_static_function_r_with_arg) == 0);

    {
      int v = 42;
      auto t = std::make_tuple(std::ref(v));
      CHECK(&index_apply<1>([&t](auto I) -> decltype(auto) {
        return std::get<I>(t);
      }) == &v);
    }

    /// [index_apply_example]
    // sum of first 4 in a sequence (allows to evaluate slices)
    CHECK(
        index_apply<4>([t = std::make_tuple(1, 2, 3, 4, 5, 6, 7)](auto... Is) {
          int sum = 0;
          EXPAND_PACK_LEFT_TO_RIGHT((sum += std::get<Is>(t)));
          return sum;
        }) == 10);
    /// [index_apply_example]
  }

}  // namespace
