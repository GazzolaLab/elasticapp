//
// Created by Tejaswin Parthasarathy on 9/25/21.
//
#include "Framework/TestingFramework.hpp"
#include "Simulator/Environments/Primitives/PlanePrimitive.hpp"
#include "Simulator/Materials.hpp"
#include "Utilities/AsConst.hpp"
#include "Utilities/Math/Infinity.hpp"
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/Thresholds.hpp"
//
#include "Simulator/Environments/Primitives/Test_AccessAPI.hpp"

namespace {

  using namespace elastica;
  using namespace elastica::collision;

  ELASTICA_TEST_CASE("Unit.Environments.Primitives.PlanePrimitive",
                     "[Unit][Simulator][Environments]") {
    // calculates bounding box
    { PlanePrimitive plane(0UL, 0UL, true, iron); }

    {
      Vec3 position = {0.0, 5.0, 6.0};
      Vec3 normal = {0.0, 0.0, 1.0};

      PlanePrimitive plane(0UL, 0UL, position, normal, iron);
      CHECK(plane.get_normal() == normal);
      CHECK(plane.get_AABB()[2] == approx(6.0 - contact_threshold));
      CHECK(plane.get_AABB()[5] == approx(6.0 + contact_threshold));
    }

    {
      Vec3 position = {0.0, 5.0, 6.0};
      auto normals = {0.0, 0.0, 1.0};
      auto binormals = {0.0, 1.0, 0.0};
      auto tangents = {1.0, 0.0, 0.0};
      // implicit convention
      Rot3 Q = {normals, binormals, tangents};

      PlanePrimitive plane(0UL, 0UL, true, position, Q, {10.0, 20.0}, iron);

      CHECK(plane.get_normal() == Vec3{0.0, 0.0, 1.0});
      CHECK(plane.get_AABB() ==
            typename PlanePrimitive::AABB(
                -5.0 - contact_threshold, -5.0 - contact_threshold,
                6.0 - contact_threshold, 5.0 + contact_threshold,
                15.0 + contact_threshold, 6.0 + contact_threshold));
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Environments.Primitives.PlanePrimitive.access_API",
      "[Unit][Simulator][Environments]") {
    // clang-format on
    EMPTY_TEST();

    Vec3 position = {0.0, 5.0, 6.0};
    auto normals = {0.0, 0.0, 1.0};
    auto binormals = {0.0, 1.0, 0.0};
    auto tangents = {1.0, 0.0, 0.0};
    // implicit convention
    Rot3 Q = {normals, binormals, tangents};

    PlanePrimitive plane(0UL, 0UL, true, position, Q, {10.0, 20.0}, iron);

    ::testing::check_access_API(std::as_const(plane));
  }

}  // namespace