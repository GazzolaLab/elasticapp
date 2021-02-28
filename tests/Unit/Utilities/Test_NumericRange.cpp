//
// Created by Tejaswin Parthasarathy on 1/11/21.
//

#include "Framework/TestingFramework.hpp"
#include "Utilities/NumericRange.hpp"

namespace {
  using namespace elastica;

  /// [is_numeric_range_example]
  using numericRT = decltype(numeric_range(2, 10));
  static_assert(elastica::tt::IsNumericRange<numericRT>::value, "Failure!");

  using not_a_numericRT = decltype(2 * 4);
  static_assert(not elastica::tt::IsNumericRange<not_a_numericRT>::value,
                "Failure!");
  /// [is_numeric_range_example]

  ELASTICA_TEST_CASE("Unit.Simulator.Utilities.NumericRange.constructor",
                     "[Unit][Simulator][Utilities]") {
    {
      detail::NumericRangeImpl<float> a({1.0, 10.0, 2.0});
      CHECK(a.start_ == approx(1.0));
      CHECK(a.stop_ == approx(10.0));
      CHECK(a.step_ == approx(2.0));
    }
    {
      detail::NumericRange a{1, 4, 2};
      CHECK(a.start_ == 1);
      CHECK(a.stop_ == 4);
      CHECK(a.step_ == 2);
    }
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Utilities.NumericRange.make_numeric",
                     "[Unit][Simulator][Utilities]") {
    {
      auto a = numeric_range(2, 10);
      CHECK(a.start_ == approx(2));
      CHECK(a.stop_ == approx(10));
      CHECK(a.step_ == approx(1));
    }

    {
      /// [numeric_range_example]
      // includes 2, but not 10 with a step of 2
      // so it enumerates [ 2, 4, 6, 8 ]
      auto a = numeric_range(2, 10, 2);
      /// [numeric_range_example]
      CHECK(a.start_ == 2);
      CHECK(a.stop_ == 10);
      CHECK(a.step_ == 2);
    }
  }
}  // namespace
