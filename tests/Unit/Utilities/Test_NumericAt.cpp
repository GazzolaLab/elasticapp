//
// Created by Tejaswin Parthasarathy on 12/29/20.
//

#include <array>
#include <type_traits>

#include "Framework/TestingFramework.hpp"
#include "Utilities/NumericAt.hpp"

namespace {

  using namespace elastica;
  using namespace elastica::detail;

  ELASTICA_TEST_CASE("Unit.Simulator.Utilities.At.constructor",
                     "[Unit][Simulator][Utilities]") {
    numeric_at_t<2> a({1, 2});
    CHECK(a.args[0] == 1);
    CHECK(a.args[1] == 2);
    CHECK(a.size() == 2);
    CHECK(a.data()[0] == 1);
    CHECK(a.data()[1] == 2);
  }

  void test_at_with_lvalue_inputs() {
    std::size_t x = 1, y = 2, z = 3;
    auto b{numeric_at(x, y, z)};
    static_assert(std::is_same<decltype(b), numeric_at_t<3>>::value, "Oops!");
    // invariance in inputs
    CHECK(x == 1);
    CHECK(y == 2);
    CHECK(z == 3);

    CHECK(b.args[0] == x);
    CHECK(b.args[1] == y);
    CHECK(b.args[2] == z);

    auto c{numeric_at(x)};
    static_assert(std::is_same<decltype(c), numeric_at_t<1>>::value, "Oops!");
    CHECK(c.args[0] == x);
  }

  void test_at_with_prvalue_inputs() {
    /// [numeric_at_example]
    auto b(numeric_at(1UL, 2UL, 3UL, 4UL));
    /// [numeric_at_example]
    static_assert(std::is_same<decltype(b), numeric_at_t<4>>::value, "Oops!");
    CHECK(b.args[0] == 1);
    CHECK(b.args[1] == 2);
    CHECK(b.args[2] == 3);
    CHECK(b.args[3] == 4);
  }

  void test_at_with_xvalue_inputs() {
    std::size_t y = 2;
    std::size_t z = 3;
    auto b{numeric_at(std::move(y), std::move(z))}; /* NOLINT*/
    static_assert(std::is_same<decltype(b), numeric_at_t<2>>::value, "Oops!");
    CHECK(b.args[0] == 2);
    CHECK(b.args[1] == 3);
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Utilities.At",
                     "[Unit][Simulator][Utilities]") {
    test_at_with_lvalue_inputs();
    test_at_with_prvalue_inputs();
    test_at_with_xvalue_inputs();
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Utilities.At.sort_and_check_duplicates",
                     "[Unit][Simulator][Utilities]") {
    {
      // No throw
      /// [scd_example]
      auto b = sort_and_check_duplicates(numeric_at(124UL, 22UL, 34UL, 42UL));
      /// [scd_example]
    }
    {
      // throw
      CHECK_THROWS_WITH(sort_and_check_duplicates(numeric_at(22UL, 22UL, 1UL, 42UL)),
                        Catch::Contains("Repeated elements"));
    }
  }

  /*
  void test_sanitize_for_lvalues() {
    auto b{numeric_at(124UL, 22UL, 34Ul, 42UL)};
    auto c(sanitize(b, 100));
    CHECK(b.args[0] == 124);
    CHECK(b.args[1] == 22);
    CHECK(b.args[2] == 34);
    CHECK(b.args[3] == 42);
    for (std::size_t i = 0UL; i < 4UL; ++i)
      CHECK(c.args[i] == b.args[i]);
  }

  void test_sanitize_for_prvalues() {
    auto c(sanitize(numeric_at(124UL, 22UL, 34UL, 42UL), 100));
    CHECK(c.args[0] == 124);
    CHECK(c.args[1] == 22);
    CHECK(c.args[2] == 34);
    CHECK(c.args[3] == 42);
  }

  void test_sanitize_for_xvalues() {
    auto b(numeric_at(124UL, 22UL, 34UL, 42UL));
    auto c(sanitize(std::move(b), 100));
    CHECK(c.args[0] == 124);
    CHECK(c.args[1] == 22);
    CHECK(c.args[2] == 34);
    CHECK(c.args[3] == 42);
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Utilities.At.sanitize_invariance",
                     "[Unit][Simulator][Utilities]") {
    test_sanitize_for_lvalues();
    test_sanitize_for_prvalues();
    test_sanitize_for_xvalues();
  }


  void test_negative_indices_in_sanitize_for_lvalues() {
    auto b{numeric_at(124, -22, -34, 42, -1, -2, -3)};
    auto c(sanitize(b, 100));
    CHECK(c.args[0] == b.args[0]);
    CHECK(c.args[3] == b.args[3]);

    // check for no modification to b
    CHECK(b.args[4] == -1);
    CHECK(b.args[5] == -2);
    CHECK(b.args[6] == -3);

    // border
    CHECK(c.args[4] == 99);
    CHECK(c.args[5] == 98);
    CHECK(c.args[6] == 97);

    // well into it
    CHECK(b.args[1] == -22);
    CHECK(b.args[2] == -34);
    CHECK(c.args[1] == 78);
    CHECK(c.args[2] == 66);
  }

  void test_negative_indices_in_sanitize_for_prvalues() {
    auto c(sanitize(numeric_at(124, -22, -34, 42), 100));
    CHECK(c.args[0] == 124);
    CHECK(c.args[1] == 78);
    CHECK(c.args[2] == 66);
    CHECK(c.args[3] == 42);
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Utilities.At.sanitize_negative_indices",
                     "[Unit][Simulator][Utilities]") {
    test_negative_indices_in_sanitize_for_lvalues();
    test_negative_indices_in_sanitize_for_prvalues();
  }

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Utilities.At.sanitize_check_range_invariance",
      "[Unit][Simulator][Utilities]") {
    auto c(check_range(numeric_at(24UL, 32UL, 1UL, 99UL), 100));
    CHECK(c.args[0] == 24);
    CHECK(c.args[1] == 32);
    CHECK(c.args[2] == 1);
    CHECK(c.args[3] == 99);
  }

  // [[OutputRegex, "as the number of degrees of freedom is only 100"]]
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Utilities.At.check_range_throws_for_negative_indices",
      "[Unit][Simulator][Utilities]") {
    ERROR_TEST();
    CHECK_THROWS_AS(check_range(numeric_at(24, -32, 1, 99), 100),
                    std::range_error);
    //    auto c(check_range(numeric_at(24, -32, 1, 99), 100));
  }

  // clang-format off
  // [[OutputRegex, "as the number of degrees of freedom is only 100"]]
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Utilities.At.check_range_throws_for_out_of_bound_indices",
      "[Unit][Simulator][Utilities]") {
    // clang-format on
    ERROR_TEST();
    CHECK_THROWS_AS(check_range(numeric_at(24, 32, 1, 199), 100),
                    std::range_error);
    //    auto c(check_range(numeric_at(24, 32, 1, 199), 100));
  }

  // clang-format off
  // [[OutputRegex, "as the number of degrees of freedom is only 100"]]
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Utilities.At.check_range_throws_and_reports_all_incorrect_indices",
      "[Unit][Simulator][Utilities]") {
    // clang-format on
    ERROR_TEST();
    CHECK_THROWS_AS(check_range(numeric_at(124, -32, 1, 199), 100),
                    std::range_error);
    //    auto c(check_range(numeric_at(124, -32, 1, 199), 100));
  }
  */

}  // namespace
