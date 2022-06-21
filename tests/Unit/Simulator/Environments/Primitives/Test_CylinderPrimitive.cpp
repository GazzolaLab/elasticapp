//
// Created by Tejaswin Parthasarathy on 9/25/21.
//
#include <cmath>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Environments/Primitives/CylinderPrimitive.hpp"
#include "Simulator/Environments/Primitives/Protocols.hpp"
#include "Simulator/Materials.hpp"
#include "Utilities/AsConst.hpp"
#include "Utilities/Geometry/Angles.hpp"
#include "Utilities/Math/Identity.hpp"
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/ProtocolHelpers.hpp"
//
#include "Simulator/Environments/Primitives/Test_AccessAPI.hpp"

namespace {

  using namespace elastica;
  using namespace elastica::collision;

  static_assert(
      ::tt::assert_conforms_to<CylinderPrimitive, protocols::ConvexPrimitive>,
      "Does not conform to convex protocol!");

  ELASTICA_TEST_CASE("Unit.Environments.Primitives.CylinderPrimitive",
                     "[Unit][Simulator][Environments]") {
    Vec3 position{5.0, 3.0, 6.0};
    real_t radius(4.0);
    real_t length(3.0);

    // calls the d
    CylinderPrimitive first_cylinder(0UL, 0UL, true, iron);
    CylinderPrimitive cylinder(0UL, 0UL, true, position, Vec3{1.0, 1.1, 1.2},
                               radius, length, iron);

    CHECK(cylinder.get_radius() == approx(radius));
    CHECK(cylinder.get_length() == approx(length));
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Environments.Primitives.CylinderPrimitive.access_API",
      "[Unit][Simulator][Environments]") {
    // clang-format on
    EMPTY_TEST();

    Vec3 position{5.0, 3.0, 6.0};
    real_t radius(4.0);
    real_t length(3.0);

    CylinderPrimitive cylinder(0UL, 0UL, true, position, Vec3{1.0, 1.1, 1.2},
                               radius, length, iron);
    ::testing::check_access_API(cylinder);
    ::testing::check_access_API(cpp17::as_const(cylinder));
  }

  ELASTICA_TEST_CASE(
      "Unit.Environments.Primitives.CylinderPrimitive.calculate_bounding_box",
      "[Unit][Simulator][Environments]") {
    Vec3 position{5.0, 3.0, 6.0};
    real_t radius(4.0);
    real_t length(3.0);

    SECTION("Aligned with the axis") {
      {
        Vec3 axis{0.0, 1.0, 0.0};

        CylinderPrimitive cylinder(0UL, 0UL, true, position, axis, radius,
                                   length, iron);
        CHECK(cylinder.get_AABB() ==
              typename CylinderPrimitive::AABB(
                  1.0 - contact_threshold, 1.5 - contact_threshold,
                  2.0 - contact_threshold, 9.0 + contact_threshold,
                  4.5 + contact_threshold, 10.0 + contact_threshold));
      }

      {
        Vec3 axis{1.0, 0.0, 0.0};

        CylinderPrimitive cylinder(0UL, 0UL, true, position, axis, radius,
                                   length, iron);
        CHECK(cylinder.get_AABB() ==
              typename CylinderPrimitive::AABB(
                  3.5 - contact_threshold, -1.0 - contact_threshold,
                  2.0 - contact_threshold, 6.5 + contact_threshold,
                  7.0 + contact_threshold, 10.0 + contact_threshold));
      }

      {
        Vec3 axis{0.0, 0.0, 1.0};

        CylinderPrimitive cylinder(0UL, 0UL, true, position, axis, radius,
                                   length, iron);
        // the cylinder box is loose so contains is good enough?
        CHECK(cylinder.get_AABB().contains(typename CylinderPrimitive::AABB(
            1.0 - contact_threshold, -1.0 - contact_threshold,
            4.5 - contact_threshold, 9.0 + contact_threshold,
            7.0 + contact_threshold, 7.5 + contact_threshold)));
      }
    }

    SECTION("Not aligned with the axis") {
      {
        Vec3 axis{0.8, 0.6, 0.0};

        CylinderPrimitive cylinder(0UL, 0UL, true, position, axis, radius,
                                   length, iron);
        CHECK(cylinder.get_AABB() ==
              typename CylinderPrimitive::AABB(
                  5.0 - (1.5 * 0.8) - (4 * 0.6) - contact_threshold,
                  3.0 - (1.5 * 0.6) - (4 * 0.8) - contact_threshold,
                  2.0 - contact_threshold,
                  5.0 + (1.5 * 0.8) + (4 * 0.6) + contact_threshold,
                  3.0 + (1.5 * 0.6) + (4 * 0.8) + contact_threshold,
                  10.0 + contact_threshold));
      }
      {
        Vec3 axis{1.0, 1.0, 1.0};
        real_t coeff = 1.0 / std::sqrt(3.0);

        CylinderPrimitive cylinder(0UL, 0UL, true, position, axis, radius,
                                   length, iron);

        // TODO very weak fit, why?
        CHECK(cylinder.get_AABB() ==  //.contains
              (typename CylinderPrimitive::AABB(
                  5.0 - (1.5 * 1.0 * coeff) - (4 * 1.0 * coeff) -
                      contact_threshold,
                  3.0 - (1.5 * 1.0 * coeff) - (4 * 1.0 * coeff) -
                      contact_threshold,
                  6.0 - (1.5 * 1.0 * coeff) - (4 * 1.0 * coeff) -
                      contact_threshold,
                  5.0 + (1.5 * 1.0 * coeff) + (4 * 1.0 * coeff) +
                      contact_threshold,
                  3.0 + (1.5 * 1.0 * coeff) + (4 * 1.0 * coeff) +
                      contact_threshold,
                  6.0 + (1.5 * 1.0 * coeff) + (4 * 1.0 * coeff) +
                      contact_threshold)));
      }
    }
  }

  ELASTICA_TEST_CASE("Unit.Environments.Primitives.CylinderPrimitive.support",
                     "[Unit][Simulator][Environments]") {
    Vec3 position{5.0, 3.0, 6.0};
    real_t radius(4.0);
    real_t length(6.0);

    SECTION("Centerline aligned with axis") {
      // Vec3 axis{0.0, 0.0, 1.0};

      CylinderPrimitive cylinder(0UL, 0UL, true, position, identity3(), radius,
                                 length, iron);

      SECTION("Furthest point on midplane circle") {
        CHECK(cylinder.support(Vec3{1.0, 0.0, 0.0}) == Vec3{9.0, 3.0, 6.0});
        CHECK(cylinder.support(Vec3{-1.0, 0.0, 0.0}) == Vec3{1.0, 3.0, 6.0});
        CHECK(cylinder.support(Vec3{0.0, 1.0, 0.0}) == Vec3{5.0, 7.0, 6.0});
        CHECK(cylinder.support(Vec3{0.0, -1.0, 0.0}) == Vec3{5.0, -1.0, 6.0});
        CHECK(cylinder.support(
                  Vec3{1.0 / std::sqrt(2), 1.0 / std::sqrt(2), 0.0}) ==
              Vec3{5.0 + 4.0 / std::sqrt(2), 3.0 + 4.0 / std::sqrt(2), 6.0});
      }

      SECTION("Furthest point on centerline axis") {
        CHECK(cylinder.support(Vec3{0.0, 0.0, 1.0}) == Vec3{5.0, 3.0, 9.0});
        CHECK(cylinder.support(Vec3{0.0, 0.0, -1.0}) == Vec3{5.0, 3.0, 3.0});
      }

      //
      SECTION("Furthest point on boundary between edge and circular plane") {
        CHECK(cylinder.support(Vec3{0.8, 0.0, 0.6}) == Vec3{9.0, 3.0, 9.0});
        CHECK(cylinder.support(Vec3{0.8, 0.0, -0.6}) == Vec3{9.0, 3.0, 3.0});
        CHECK(cylinder.support(Vec3{0.0, 0.8, 0.6}) == Vec3{5.0, 7.0, 9.0});
        CHECK(cylinder.support(Vec3{0.0, 0.8, -0.6}) == Vec3{5.0, 7.0, 3.0});

        // on the edge of the circular plane, but not aligned
        CHECK(cylinder.support(Vec3{0.8 * 0.8, 0.8 * 0.6, 0.6}) ==
              Vec3{5.0 + (0.8 * 4), 3.0 + (0.6 * 4), 9.0});
        CHECK(cylinder.support(Vec3{0.8 * 0.6, 0.8 * 0.8, -0.6}) ==
              Vec3{5.0 + (0.6 * 4), 3.0 + (0.8 * 4), 3.0});
      }

      SECTION("Furthest point in the interior of the circular plane") {
        // inside the circular plane
        real_t angle = ::elastica::geometry::deg2rad(75.0);
        // I would have thought this was the closest vector, but all other
        // implementations and theory seem to disagree
        //      CHECK(cylinder.support(Vec3{std::cos(angle), 0.0,
        //      std::sin(angle)}) ==
        //            Vec3{5.0 + (3.0 / std::tan(angle)), 3.0, 9.0});
        CHECK(cylinder.support(Vec3{std::cos(angle), 0.0, std::sin(angle)}) ==
              Vec3{5.0 + (4.0), 3.0, 9.0});

        real_t another_angle = ::elastica::geometry::deg2rad(60.0);
        CHECK(cylinder.support(Vec3{std::cos(angle) * std::cos(another_angle),
                                    std::cos(angle) * std::sin(another_angle),
                                    -std::sin(angle)}) ==
              Vec3{5.0 + (4.0 * std::cos(another_angle)),
                   3.0 + (4.0 * std::sin(another_angle)), 3.0});
      }

      SECTION("Furthest point in the edge the circular at some height") {
        real_t angle = ::elastica::geometry::deg2rad(10.0);
        // I would have thought this was the closest vector, but all other
        // implementations and theory seem to disagree
        //      CHECK(cylinder.support(Vec3{std::cos(angle), 0.0,
        //      std::sin(angle)}) ==
        //            Vec3{5.0 + (3.0 / std::tan(angle)), 3.0, 9.0});
        CHECK(cylinder.support(Vec3{std::cos(angle), 0.0, std::sin(angle)}) ==
              Vec3{5.0 + (4.0), 3.0, 9.0});

        real_t another_angle = ::elastica::geometry::deg2rad(60.0);
        CHECK(cylinder.support(Vec3{std::cos(angle) * std::cos(another_angle),
                                    std::cos(angle) * std::sin(another_angle),
                                    -std::sin(angle)}) ==
              Vec3{5.0 + (4.0 * std::cos(another_angle)),
                   3.0 + (4.0 * std::sin(another_angle)), 3.0});
      }
    }

    // TODO
    //    SECTION("Centerline not aligned with axis") {
    //
    //    }

    SECTION("Support with contact thresholding") {
      CylinderPrimitive cylinder(0UL, 0UL, true, position, identity3(), radius,
                                 length, iron);
      CHECK(cylinder.support_contact_threshold(Vec3{0.8, 0.0, 0.6}) ==
            Vec3{9.0 + 0.8 * contact_threshold, 3.0,
                 9.0 + 0.6 * contact_threshold});
    }
  }

  ELASTICA_TEST_CASE("Unit.Environments.Primitives.CylinderPrimitive.transfer",
                     "[Unit][Simulator][Environments]") {
    Vec3 position{5.0, 3.0, 6.0};
    real_t radius(4.0);
    real_t length(6.0);
    CylinderPrimitive cylinder(0UL, 0UL, true, position, identity3(), radius,
                               length, iron);

    Vec3 new_position = 2.0 * position;
    Rot3 new_rotation = {{0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 0.0, 0.0}};
    Vec3 velocity = Vec3{1.0, 0.0, 1.0};
    Vec3 omega = Vec3{1.0, 2.0, 1.0};
    cylinder.transfer(/*new_radius, */ new_position, new_rotation, velocity,
                      omega);

    CHECK(cylinder.get_position() == (new_position));
    CHECK(cylinder.get_rotation_matrix() == (new_rotation));
    CHECK(cylinder.get_velocity() == (velocity));
    CHECK(cylinder.get_angular_velocity() == (omega));

    // check bounding box updated
    // [10, 6, 12] +/- whatever
    CHECK(cylinder.get_AABB() ==
          typename CylinderPrimitive::AABB(
              10.0 - 3.0 - contact_threshold, 6.0 - 4.0 - contact_threshold,
              12.0 - 4.0 - contact_threshold, 10.0 + 3.0 + contact_threshold,
              6.0 + 4.0 + contact_threshold, 12.0 + 4.0 + contact_threshold));
  }

}  // namespace