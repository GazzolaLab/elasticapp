//
// Created by Tejaswin Parthasarathy on 9/26/21.
//

#include <cmath>

#include "Framework/TestingFramework.hpp"
#include "Utilities/DefineTypes.h"
#include "Utilities/Math/Normalize.hpp"

namespace {

  ELASTICA_TEST_CASE("Unit.Utilities.Math.is_normalized", "[Unit][Utilities]") {
    using namespace elastica;

    CHECK(is_normalized(Vec3{1.0, 0.0, 0.0}));
    CHECK(is_normalized(Vec3{0.0, 1.0, 0.0}));
    CHECK(is_normalized(Vec3{0.0, 0.0, 1.0}));

    CHECK(is_normalized(Vec3{0.8, 0.6, 0.0}));
    CHECK(is_normalized(Vec3{0.0, -0.6, 0.8}));
    CHECK(is_normalized(Vec3{0.1 / 0.9, -0.8 / 0.9, 0.4 / 0.9}));

    CHECK_FALSE(is_normalized(Vec3{1.0, 1.0, 1.0}));
  }

}