//
// Created by Tejaswin Parthasarathy on 9/27/21.
//

#include "Framework/TestingFramework.hpp"
#include "Utilities/Clamp.hpp"

namespace {

  ELASTICA_TEST_CASE("Unit.Utilities.clamp", "[Unit][Utilities]") {
    int max_val = 10;
    int min_val = -10;
    int x = 5;

    CHECK(cpp17::clamp(x, min_val, max_val) == x);

    x = 8;
    CHECK(cpp17::clamp(x, min_val, max_val) == x);

    x = -8;
    CHECK(cpp17::clamp(x, min_val, max_val) == x);

    x = 20;
    CHECK(cpp17::clamp(x, min_val, max_val) == max_val);

    x = -11;
    CHECK(cpp17::clamp(x, min_val, max_val) == min_val);

  }
}
