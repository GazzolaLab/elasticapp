//
// Created by Tejaswin Parthasarathy on 9/25/21.
//
#include "Framework/TestingFramework.hpp"
#include "Simulator/Environments/Primitives/Protocols.hpp"
#include "Simulator/Environments/Primitives/SpherePrimitive.hpp"
#include "Simulator/Materials.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/Math/Rot3.hpp"
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace {

  using namespace elastica;
  using namespace elastica::collision;

  static_assert(
      ::tt::assert_conforms_to<SpherePrimitive, protocols::ConvexPrimitive>,
      "Does not conform to convex protocol!");

  ELASTICA_TEST_CASE("Unit.Environments.Primitives.SpherePrimitive",
                     "[Unit][Simulator][Environments]") {
    Vec3 position{5.0, 3.0, 6.0};
    real_t radius(4.0);
    // calculates bounding box
    SpherePrimitive first_sphere(0UL, 0UL, true, iron);
    SpherePrimitive sphere(0UL, 0UL, true, position, radius, iron);

    CHECK(sphere.get_AABB() ==
          typename SpherePrimitive::AABB(
              1.0 - contact_threshold, -1.0 - contact_threshold,
              2.0 - contact_threshold, 9.0 + contact_threshold,
              7.0 + contact_threshold, 10.0 + contact_threshold));

    //    CHECK(sphere.get_AABB()[0] == approx(1.0));
    //    CHECK(sphere.get_AABB()[3] == approx(9.0));
    //    CHECK(sphere.get_AABB()[1] == approx(-1.0));
    //    CHECK(sphere.get_AABB()[4] == approx(7.0));
    //    CHECK(sphere.get_AABB()[2] == approx(2.0));
    //    CHECK(sphere.get_AABB()[5] == approx(10.0));

    CHECK(sphere.get_radius() == approx(radius));
  }

  ELASTICA_TEST_CASE("Unit.Environments.Primitives.SpherePrimitive.support",
                     "[Unit][Simulator][Environments]") {
    Vec3 position{5.0, 3.0, 6.0};
    real_t radius(4.0);
    // calculates bounding box
    SpherePrimitive sphere(0UL, 0UL, true, position, radius, iron);

    SECTION("support vector") {
      CHECK(sphere.support(Vec3{1.0, 0.0, 0.0}) == Vec3{9.0, 3.0, 6.0});
      CHECK(sphere.support(Vec3{0.0, 1.0, 0.0}) == Vec3{5.0, 7.0, 6.0});
      CHECK(sphere.support(Vec3{0.0, 0.0, 1.0}) == Vec3{5.0, 3.0, 10.0});
      CHECK(sphere.support(Vec3{1.0 / 9.0, 4.0 / 9.0, 8.0 / 9.0}) ==
            Vec3{5.0 + (4.0 / 9.0), 3.0 + (4.0 * 4.0) / 9.0,
                 6.0 + (8.0 * 4.0 / 9.0)});

      //
      CHECK(sphere.support(Vec3{-1.0, 0.0, 0.0}) == Vec3{1.0, 3.0, 6.0});
      CHECK(sphere.support(Vec3{0.0, -1.0, 0.0}) == Vec3{5.0, -1.0, 6.0});
      CHECK(sphere.support(Vec3{0.0, 0.0, -1.0}) == Vec3{5.0, 3.0, 2.0});
      CHECK(sphere.support(Vec3{-1.0 / 9.0, -4.0 / 9.0, -8.0 / 9.0}) ==
            Vec3{5.0 - (4.0 / 9.0), 3.0 - (4.0 * 4.0) / 9.0,
                 6.0 - (8.0 * 4.0 / 9.0)});
    }
    SECTION("support vector with threshold") {
      CHECK(sphere.support_contact_threshold(Vec3{1.0, 0.0, 0.0}) ==
            Vec3{9.0 + contact_threshold, 3.0, 6.0});
      CHECK(sphere.support_contact_threshold(Vec3{0.0, 1.0, 0.0}) ==
            Vec3{5.0, 7.0 + contact_threshold, 6.0});
      CHECK(sphere.support_contact_threshold(Vec3{0.0, 0.0, 1.0}) ==
            Vec3{5.0, 3.0, 10.0 + contact_threshold});
      //
      CHECK(sphere.support_contact_threshold(Vec3{-1.0, 0.0, 0.0}) ==
            Vec3{1.0 - contact_threshold, 3.0, 6.0});
      CHECK(sphere.support_contact_threshold(Vec3{0.0, -1.0, 0.0}) ==
            Vec3{5.0, -1.0 - contact_threshold, 6.0});
      CHECK(sphere.support_contact_threshold(Vec3{0.0, 0.0, -1.0}) ==
            Vec3{5.0, 3.0, 2.0 - contact_threshold});
    }
  }

  ELASTICA_TEST_CASE("Unit.Environments.Primitives.SpherePrimitive.transfer",
                     "[Unit][Simulator][Environments]") {
    Vec3 position{5.0, 3.0, 6.0};
    real_t radius(4.0);
    SpherePrimitive sphere(0UL, 0UL, true, position, radius, iron);

    real_t new_radius(2.0 * radius);
    ::elastica::ignore_unused(new_radius);
    Vec3 new_position = 2.0 * position;
    Vec3 velocity = Vec3{1.0, 0.0, 1.0};
    Vec3 omega = Vec3{1.0, 2.0, 1.0};
    sphere.transfer(/*new_radius, */ new_position, velocity, omega);

    //    CHECK(sphere.get_radius() == approx(new_radius));
    CHECK(sphere.get_position() == (new_position));
    CHECK(sphere.get_velocity() == (velocity));
    CHECK(sphere.get_angular_velocity() == (omega));

    // check bounding box updated
    CHECK(sphere.get_AABB() ==
          typename SpherePrimitive::AABB(
              6.0 - contact_threshold, 2.0 - contact_threshold,
              8.0 - contact_threshold, 14.0 + contact_threshold,
              10.0 + contact_threshold, 16.0 + contact_threshold));

    Rot3 new_rotation{{1.0, 0.0, 0.0}, {0.0, 0.8, 0.6}, {0.0, -0.6, 0.8}};

    sphere.transfer(new_radius, new_position, new_rotation, velocity, omega);

    //    CHECK(sphere.get_radius() == approx(new_radius));
    CHECK(sphere.get_position() == (new_position));
    CHECK(sphere.get_rotation_matrix() == (new_rotation));
    CHECK(sphere.get_velocity() == (velocity));
    CHECK(sphere.get_angular_velocity() == (omega));

    // check bounding updated for new radius
    CHECK(sphere.get_AABB() ==
          typename SpherePrimitive::AABB(
              2.0 - contact_threshold, -2.0 - contact_threshold,
              4.0 - contact_threshold, 18.0 + contact_threshold,
              14.0 + contact_threshold, 20.0 + contact_threshold));
  }

}  // namespace