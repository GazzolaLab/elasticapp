//
// Created by Tejaswin Parthasarathy on 6/2/21.
//
#include <memory>
#include <tuple>

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

    // check for strucst
    auto a = [x = 20]() { return x; };
    auto repeated_a = repeat_as_tuple<3UL>(a);
    CHECK(std::get<0UL>(repeated_a)() == 20);

    // check if zero arguments make sense even with custom types
    auto another_pi = std::make_unique<int>(5);
    auto repeated_another_pi = repeat_as_tuple<0UL>(std::move(pi));
    static_assert(cpp17::is_same_v<decltype(repeated_another_pi), std::tuple<>>,
                  "Meta-failure");

    // auto another_func = [x = 10]() { return x; };
    auto repeated_func = repeat_as_tuple<0UL>(a);
    static_assert(cpp17::is_same_v<decltype(repeated_func), std::tuple<>>,
                  "Meta-failure");
  }

}  // namespace
