//
// Created by Tejaswin Parthasarathy on 1/12/21.
//

#include <vector>

#include "Framework/TestingFramework.hpp"
#include "Utilities/End.hpp"

namespace {

  using namespace elastica;

  ELASTICA_TEST_CASE("Unit.Utilities.End.end", "[Unit][Utilities]") {
    /// [end_example]
    auto a = std::vector<int>{1, 2, 3};
    CHECK(elastica::end(a) == std::end(a));
    /// [end_example]
  }

  ELASTICA_TEST_CASE("Unit.Utilities.End.end_indexing", "[Unit][Utilities]") {
    static_assert(std::is_same<decltype(end - 2), from_end>::value, "Oops!");
    /// [end_indexing_example]
    CHECK((elastica::end - 2).i == 2);
    CHECK((elastica::end - 10).i == 10);
    CHECK(((elastica::end - 10) - 2).i == 12);

    // also constexpr!
    static_assert((elastica::end - 2).i == 2, "End meta-failure!");
    /// [end_indexing_example]
  }

  // [[OutputRegex, The regex that should be found in the output]]
  ELASTICA_TEST_CASE("Unit.Utilities.End.end_indexing_assert",
                     "[Unit][Utilities]") {
    // Does not work in release mode, wonder why
    // ASSERTION_TEST();
    // TODO : after correcting output tests
    //    ignore_unused(end - (-2));

    CHECK(true);
  }

  ELASTICA_TEST_CASE("Unit.Utilities.End.end_indexing_equality",
                     "[Unit][Utilities]") {
    /// [end_equality_comparison_eg]
    CHECK((elastica::end - 2) == (elastica::end - 2));
    CHECK(((elastica::end - 10) - 2) == (elastica::end - 12));
    static_assert((elastica::end - 2) == (elastica::end - 2),
                  "End meta-failure!");
    /// [end_equality_comparison_eg]

    /// [end_inequality_comparison_eg]
    CHECK((elastica::end - 2) != (elastica::end - 3));
    CHECK(((elastica::end - 10) - 2) != (elastica::end - 15));
    static_assert((elastica::end - 2) != (elastica::end - 3),
                  "End meta-failure!");
    /// [end_inequality_comparison_eg]
  }

}  // namespace
