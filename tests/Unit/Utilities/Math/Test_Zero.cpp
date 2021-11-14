//
// Created by Tejaswin Parthasarathy on 11/9/21.
//

#include <cmath>

#include "Framework/TestingFramework.hpp"
#include "Utilities/DefineTypes.h"
#include "Utilities/Math/Zero.hpp"

namespace {

  ELASTICA_TEST_CASE("Unit.Utilities.Math.is_zero_length",
                     "[Unit][Utilities]") {
    using namespace elastica;
    CHECK_FALSE(is_zero_length(Vec3{1.0, 0.0, 0.0}));
    CHECK_FALSE(is_zero_length(Vec3{0.0, 1.0, 0.0}));
    CHECK_FALSE(is_zero_length(Vec3{0.0, 0.0, 1.0}));

    CHECK(is_zero_length(zero3()));
  }

}  // namespace