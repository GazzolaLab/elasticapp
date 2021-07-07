//
// Created by Tejaswin Parthasarathy on 6/2/21.
//
#include <tuple>
#include <memory>

#include "Framework/TestingFramework.hpp"
#include "Utilities/RepeatAsTuple.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"

namespace {

  ELASTICA_TEST_CASE("Unit.Utilities.RepeatAsTuple", "[Unit][Utilities]") {
    // [repeat_eg]
    int x = 2;
    auto repeated_x = repeat_as_tuple<3UL>(x);
    static_assert(
        cpp17::is_same_v<decltype(repeated_x), std::tuple<int, int, int>>,
        "Repeat failure!");
    CHECK(repeated_x == std::make_tuple(x, x, x));
    // [repeat_eg]

    // check if single argument perfectly forwards
    auto pi = std::make_unique<int>(2);
    auto repeated_pi = repeat_as_tuple<1UL>(std::move(pi));
    CHECK(*std::get<0UL>(repeated_pi) == 2);
  }

}  // namespace
