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

  ELASTICA_TEST_CASE("Unit.Simulator.Utilities.NumericAt.constructor",
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

  ELASTICA_TEST_CASE("Unit.Simulator.Utilities.NumericAt",
                     "[Unit][Simulator][Utilities]") {
    test_at_with_lvalue_inputs();
    test_at_with_prvalue_inputs();
    test_at_with_xvalue_inputs();
  }

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Utilities.NumericAt.sort_and_check_duplicates",
      "[Unit][Simulator][Utilities]") {
    {
      // No throw
      /// [scd_example]
      auto b = sort_and_check_duplicates(numeric_at(124UL, 22UL, 34UL, 42UL));
      /// [scd_example]
    }
    {
      // throw
      CHECK_THROWS_WITH(
          sort_and_check_duplicates(numeric_at(22UL, 22UL, 1UL, 42UL)),
          Catch::Contains("Repeated elements"));
    }
  }

}  // namespace
