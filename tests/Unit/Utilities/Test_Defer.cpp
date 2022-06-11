//
// Created by Tejaswin Parthasarathy on 1/21/21.
//

#include <utility>

#include "Framework/TestingFramework.hpp"
#include "Utilities/Defer.hpp"
#include "Utilities/IgnoreUnused.hpp"

namespace {

  using namespace elastica;

  ELASTICA_TEST_CASE("Unit.Utilities.Defer.defer", "[Unit][Utilities]") {
    EMPTY_TEST();

    /// [defer_example]
    // marks a pair as deferred to a later point in time
    auto p = std::make_pair(elastica::defer, elastica::defer);
    /// [defer_example]
    ignore_unused(p);
  }

  ELASTICA_TEST_CASE("Unit.Utilities.Defer.defer_composition",
                     "[Unit][Utilities]") {
    /// [defer_composition_example]
    auto a = std::pair<int, int>{1, 2};
    // defers can be composed and return a defer type
    static_assert(std::is_same<decltype(elastica::defer(a)),
                               decltype(elastica::defer)>::value,
                  "Oops!");
    CHECK(&elastica::defer(a) == &elastica::defer);
    CHECK(&elastica::defer(elastica::defer(a)) == &elastica::defer);
    /// [defer_composition_example]
  }

}  // namespace