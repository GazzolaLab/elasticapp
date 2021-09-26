//
// Created by Tejaswin Parthasarathy on 1/14/21.
//
#include <cmath>

#include "Framework/TestingFramework.hpp"
#include "Utilities/Geometry/angles.hpp"

namespace {
  using namespace elastica::geometry;
  constexpr float M_PI_F = static_cast<float>(M_PI);

  ELASTICA_TEST_CASE("Unit.Utilities.Geometry.deg2rad", "[Unit][Utilities]") {
    CHECK(approx(deg2rad(0.0F)) == 0.0F);
    CHECK(approx(deg2rad(0.0)) == 0.0);
    CHECK(approx(deg2rad(45.0F)) == 0.25F * M_PI_F);
    CHECK(approx(deg2rad(45.0)) == 0.25 * M_PI);
    CHECK(approx(deg2rad(90.0F)) == 0.5F * M_PI_F);
    CHECK(approx(deg2rad(90.0)) == 0.5 * M_PI);
    CHECK(approx(deg2rad(180.0F)) == 1.0F * M_PI_F);
    CHECK(approx(deg2rad(180.0)) == 1.0 * M_PI);
  }

  ELASTICA_TEST_CASE("Unit.Utilities.Geometry.rad2deg", "[Unit][Utilities]") {
    CHECK(approx(rad2deg(0.0F)) == 0.0F);
    CHECK(approx(rad2deg(0.0)) == 0.0);
    CHECK(approx(rad2deg(0.25F * M_PI_F)) == 45.0F);
    CHECK(approx(rad2deg(0.25 * M_PI)) == 45.0);
    CHECK(approx(rad2deg(0.5F * M_PI_F)) == 90.0F);
    CHECK(approx(rad2deg(0.5 * M_PI)) == 90.0);
    CHECK(approx(rad2deg(1.0F * M_PI_F)) == 180.0F);
    CHECK(approx(rad2deg(1.0 * M_PI)) == 180.0);
  }

}  // namespace
