//
// Created by Tejaswin Parthasarathy on 8/7/22.
//
#include "Framework/TestingFramework.hpp"
#include "Simulator/Environments/Primitives/HalfSpacePrimitive.hpp"
#include "Simulator/Materials.hpp"
#include "Utilities/Math/Infinity.hpp"
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/Thresholds.hpp"
//
#include "Simulator/Environments/Primitives/Test_AccessAPI.hpp"

namespace {

  using namespace elastica;
  using namespace elastica::collision;

  ELASTICA_TEST_CASE("Unit.Environments.Primitives.HalfSpacePrimitive",
                     "[Unit][Simulator][Environments]") {
    // calculates bounding box
    { HalfSpacePrimitive half_space(0UL, 0UL, true, iron); }

    {
      Vec3 anchor_position = {0.0, 5.0, 6.0};
      Vec3 normal = {0.0, 0.0, 1.0};

      HalfSpacePrimitive half_space(0UL, 0UL, anchor_position, normal, iron);
      CHECK(half_space.get_normal() == normal);
      CHECK(half_space.get_AABB()[2] ==
            approx(6.0 - half_space.get_span() - contact_threshold));
      // The right tolerance is tight, so we only check for upto 0.1 *
      // contact_threshold
      CHECK(half_space.get_AABB()[5] ==
            approx(6.0 + contact_threshold).epsilon(0.1 * contact_threshold));
    }

    {
      Vec3 anchor_position = {0.0, 5.0, 6.0};
      Vec3 normal = {0.0, 0.0, -1.0};

      HalfSpacePrimitive half_space(0UL, 0UL, anchor_position, normal, iron);
      CHECK(half_space.get_normal() == normal);
      CHECK(half_space.get_AABB()[2] ==
            approx(6.0 - contact_threshold).epsilon(0.1 * contact_threshold));
      // The right tolerance is tight, so we only check for upto 0.1 *
      // contact_threshold
      CHECK(half_space.get_AABB()[5] ==
            approx(6.0 + half_space.get_span() + contact_threshold));
    }

    {
      Vec3 anchor_position = {0.0, 5.0, 6.0};
      Vec3 normal = {1.0, 0.0, 0.0};

      HalfSpacePrimitive half_space(0UL, 0UL, anchor_position, normal, iron);
      CHECK(half_space.get_normal() == normal);
      CHECK(half_space.get_AABB()[0] ==
            approx(0.0 - half_space.get_span() - contact_threshold));
      // The right tolerance is tight, so we only check for upto 0.1 *
      // contact_threshold
      CHECK(half_space.get_AABB()[3] ==
            approx(0.0 + contact_threshold).epsilon(0.1 * contact_threshold));
    }

    {
      Vec3 anchor_position = {0.0, 5.0, 6.0};
      Vec3 normal = {-1.0, 0.0, 0.0};

      HalfSpacePrimitive half_space(0UL, 0UL, anchor_position, normal, iron);
      CHECK(half_space.get_normal() == normal);
      CHECK(half_space.get_AABB()[0] ==
            approx(0.0 - contact_threshold).epsilon(0.1 * contact_threshold));
      // The right tolerance is tight, so we only check for upto 0.1 *
      // contact_threshold
      CHECK(half_space.get_AABB()[3] ==
            approx(0.0 + half_space.get_span() + contact_threshold));
    }

    {
      Vec3 anchor_position = {0.0, 5.0, 6.0};
      Vec3 normal = {0.0, 1.0, 0.0};

      HalfSpacePrimitive half_space(0UL, 0UL, anchor_position, normal, iron);
      CHECK(half_space.get_normal() == normal);
      CHECK(half_space.get_AABB()[1] ==
            approx(5.0 - half_space.get_span() - contact_threshold));
      // The right tolerance is tight, so we only check for upto 0.1 *
      // contact_threshold
      CHECK(half_space.get_AABB()[4] ==
            approx(5.0 + contact_threshold).epsilon(0.1 * contact_threshold));
    }

    {
      Vec3 anchor_position = {0.0, 5.0, 6.0};
      Vec3 normal = {0.0, -1.0, 0.0};

      HalfSpacePrimitive half_space(0UL, 0UL, anchor_position, normal, iron);
      CHECK(half_space.get_normal() == normal);
      CHECK(half_space.get_AABB()[1] ==
            approx(5.0 - contact_threshold).epsilon(0.1 * contact_threshold));
      // The right tolerance is tight, so we only check for upto 0.1 *
      // contact_threshold
      CHECK(half_space.get_AABB()[4] ==
            approx(5.0 + half_space.get_span() + contact_threshold));
    }

    {
      Vec3 anchor_position = {0.0, 5.0, 6.0};
      auto normals = {0.0, 0.0, 1.0};
      auto binormals = {0.0, 1.0, 0.0};
      auto tangents = {1.0, 0.0, 0.0};
      // implicit convention
      Rot3 Q = {normals, binormals, tangents};

      HalfSpacePrimitive half_space(0UL, 0UL, true, anchor_position, Q, iron);

      CHECK(half_space.get_normal() == Vec3{0.0, 0.0, 1.0});
      CHECK(half_space.get_AABB() ==
            typename HalfSpacePrimitive::AABB(
                0.0 - 0.5 * half_space.get_span() - contact_threshold,
                5.0 - 0.5 * half_space.get_span() - contact_threshold,
                6.0 - half_space.get_span() - contact_threshold,
                0.0 + 0.5 * half_space.get_span() + contact_threshold,
                5.0 + 0.5 * half_space.get_span() + contact_threshold,
                6.0 + contact_threshold));
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Environments.Primitives.HalfSpacePrimitive.access_API",
      "[Unit][Simulator][Environments]") {
    // clang-format on
    EMPTY_TEST();

    Vec3 anchor_position = {0.0, 5.0, 6.0};
    auto normals = {0.0, 0.0, 1.0};
    auto binormals = {0.0, 1.0, 0.0};
    auto tangents = {1.0, 0.0, 0.0};
    // implicit convention
    Rot3 Q = {normals, binormals, tangents};

    HalfSpacePrimitive half_space(0UL, 0UL, true, anchor_position, Q, iron);

    ::testing::check_access_API(std::as_const(half_space));
  }

}  // namespace
