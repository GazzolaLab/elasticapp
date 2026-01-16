//
// Created by Tejaswin Parthasarathy on 9/26/21.
//

#include <cmath>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Environments/Primitives/Protocols.hpp"
#include "Simulator/Environments/Primitives/detail/CapsulePrimitiveBase.hpp"
#include "Simulator/Materials.hpp"
#include "Utilities/Geometry/Angles.hpp"
#include "Utilities/Math/Identity.hpp"
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/ProtocolHelpers.hpp"
//
#include "Simulator/Environments/Primitives/Test_AccessAPI.hpp"

namespace {

  using namespace elastica;
  using namespace elastica::collision;
  using namespace elastica::collision::detail;

  static_assert(::tt::assert_conforms_to<CapsulePrimitiveBase,
                                         protocols::ConvexPrimitive>,
                "Does not conform to convex protocol!");

  ELASTICA_TEST_CASE("Unit.Environments.Primitives.CapsulePrimitiveBase",
                     "[Unit][Simulator][Environments]") {
    Vec3 position{5.0, 3.0, 6.0};
    real_t radius(4.0);
    real_t length(3.0);

    // calls the d
    CapsulePrimitiveBase first_capsule(0UL, 0UL, true, iron);
    CapsulePrimitiveBase capsule(0UL, 0UL, true, position, Vec3{1.0, 1.1, 1.2},
                                 radius, length, iron);

    CHECK(capsule.get_radius() == approx(radius));
    CHECK(capsule.get_length() == approx(length));
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Environments.Primitives.CapsulePrimitiveBase.access_API",
      "[Unit][Simulator][Environments]") {
    // clang-format on
    EMPTY_TEST();

    Vec3 position{5.0, 3.0, 6.0};
    real_t radius(4.0);
    real_t length(3.0);

    CapsulePrimitiveBase capsule(0UL, 0UL, true, position, Vec3{1.0, 1.1, 1.2},
                                 radius, length, iron);
    ::testing::check_access_API(capsule);
    ::testing::check_access_API(std::as_const(capsule));
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Environments.Primitives.CapsulePrimitiveBase.calculate_bounding_box",
      "[Unit][Simulator][Environments]") {
    // clang-format on
    Vec3 position{5.0, 3.0, 6.0};
    real_t radius(4.0);
    real_t length(12.0);

    SECTION("Aligned with the axis") {
      {
        Vec3 axis{0.0, 1.0, 0.0};

        CapsulePrimitiveBase capsule(0UL, 0UL, true, position, axis, radius,
                                     length, iron);
        CHECK(capsule.get_AABB() ==
              typename CapsulePrimitiveBase::AABB(
                  1.0 - contact_threshold, -7.0 - contact_threshold,
                  2.0 - contact_threshold, 9.0 + contact_threshold,
                  13.0 + contact_threshold, 10.0 + contact_threshold));
      }

      {
        Vec3 axis{1.0, 0.0, 0.0};

        CapsulePrimitiveBase capsule(0UL, 0UL, true, position, axis, radius,
                                     length, iron);
        CHECK(capsule.get_AABB() ==
              typename CapsulePrimitiveBase::AABB(
                  -5.0 - contact_threshold, -1.0 - contact_threshold,
                  2.0 - contact_threshold, 15.0 + contact_threshold,
                  7.0 + contact_threshold, 10.0 + contact_threshold));
      }

      {
        Vec3 axis{0.0, 0.0, 1.0};

        CapsulePrimitiveBase capsule(0UL, 0UL, true, position, axis, radius,
                                     length, iron);
        // the capsule box is loose so contains is good enough?
        CHECK(capsule.get_AABB() ==
              (typename CapsulePrimitiveBase::AABB(
                  1.0 - contact_threshold, -1.0 - contact_threshold,
                  -4.0 - contact_threshold, 9.0 + contact_threshold,
                  7.0 + contact_threshold, 16.0 + contact_threshold)));
      }
    }

    SECTION("Not aligned with the axis") {
      {
        Vec3 axis{0.8, 0.6, 0.0};

        CapsulePrimitiveBase capsule(0UL, 0UL, true, position, axis, radius,
                                     length, iron);
        CHECK(capsule.get_AABB() ==
              typename CapsulePrimitiveBase::AABB(
                  5.0 - (6.0 * 0.8) - 4 - contact_threshold,
                  3.0 - (6.0 * 0.6) - 4 - contact_threshold,
                  2.0 - contact_threshold,
                  5.0 + (6.0 * 0.8) + 4 + contact_threshold,
                  3.0 + (6.0 * 0.6) + 4 + contact_threshold,
                  10.0 + contact_threshold));
      }
      {
        Vec3 axis{1.0, 1.0, 1.0};
        real_t coeff = 1.0 / std::sqrt(3.0);

        CapsulePrimitiveBase capsule(0UL, 0UL, true, position, axis, radius,
                                     length, iron);

        // TODO very weak fit, why?
        CHECK(capsule.get_AABB() ==  //.contains
              (typename CapsulePrimitiveBase::AABB(
                  5.0 - (6.0 * 1.0 * coeff) - (4 * 1.0) - contact_threshold,
                  3.0 - (6.0 * 1.0 * coeff) - (4 * 1.0) - contact_threshold,
                  6.0 - (6.0 * 1.0 * coeff) - (4 * 1.0) - contact_threshold,
                  5.0 + (6.0 * 1.0 * coeff) + (4 * 1.0) + contact_threshold,
                  3.0 + (6.0 * 1.0 * coeff) + (4 * 1.0) + contact_threshold,
                  6.0 + (6.0 * 1.0 * coeff) + (4 * 1.0) + contact_threshold)));
      }
    }
  }

  ELASTICA_TEST_CASE(
      "Unit.Environments.Primitives.CapsulePrimitiveBase.support",
      "[Unit][Simulator][Environments]") {
    Vec3 position{5.0, 3.0, 6.0};
    real_t radius(4.0);
    real_t length(12.0);
    Vec3 upper_cap_point{5.0, 3.0, 12.0};
    Vec3 lower_cap_point{5.0, 3.0, 0.0};

    SECTION("Centerline aligned with axis") {
      // Vec3 axis{0.0, 0.0, 1.0};

      CapsulePrimitiveBase capsule(0UL, 0UL, true, position, identity3(),
                                   radius, length, iron);

      SECTION("Furthest point on midplane circle") {
        CHECK(capsule.support(Vec3{1.0, 0.0, 0.0}) == Vec3{9.0, 3.0, 6.0});
        CHECK(capsule.support(Vec3{-1.0, 0.0, 0.0}) == Vec3{1.0, 3.0, 6.0});
        CHECK(capsule.support(Vec3{0.0, 1.0, 0.0}) == Vec3{5.0, 7.0, 6.0});
        CHECK(capsule.support(Vec3{0.0, -1.0, 0.0}) == Vec3{5.0, -1.0, 6.0});
        CHECK(capsule.support(
                  Vec3{1.0 / std::sqrt(2), 1.0 / std::sqrt(2), 0.0}) ==
              Vec3{5.0 + 4.0 / std::sqrt(2), 3.0 + 4.0 / std::sqrt(2), 6.0});
      }

      SECTION("Furthest point on centerline axis") {
        CHECK(capsule.support(Vec3{0.0, 0.0, 1.0}) == Vec3{5.0, 3.0, 16.0});
        CHECK(capsule.support(Vec3{0.0, 0.0, -1.0}) == Vec3{5.0, 3.0, -4.0});
      }

      //
      SECTION("Furthest point on spherical shell") {
        // this direction is beyond the circular edge
        real_t first_direction = 4.0;
        real_t second_direction = 6.0;
        real_t hypotenuse = std::sqrt(first_direction * first_direction +
                                      second_direction * second_direction);
        first_direction /= hypotenuse;
        second_direction /= hypotenuse;
        CHECK(capsule.support(Vec3{first_direction, 0.0, second_direction}) ==
              upper_cap_point +
                  4.0 * Vec3{first_direction, 0.0, second_direction});
        CHECK(capsule.support(Vec3{first_direction, 0.0, -second_direction}) ==
              lower_cap_point +
                  4.0 * Vec3{first_direction, 0.0, -second_direction});
        CHECK(capsule.support(Vec3{0.0, first_direction, second_direction}) ==
              upper_cap_point +
                  4.0 * Vec3{0.0, first_direction, second_direction});
        CHECK(capsule.support(Vec3{0.0, first_direction, -second_direction}) ==
              lower_cap_point +
                  4.0 * Vec3{0.0, first_direction, -second_direction});

        real_t angle_in_plane = ::elastica::geometry::deg2rad(75.0);

        // on the edge of the circular plane, but not aligned
        auto d =
            Vec3{first_direction * std::cos(angle_in_plane),
                 first_direction * std::sin(angle_in_plane), second_direction};
        CHECK(capsule.support(d) == upper_cap_point + 4.0 * d);
        CHECK(capsule.support(-d) == lower_cap_point - 4.0 * d);
      }

      //      SECTION("Furthest point in the edge the circular at some height")
      //      {
      //        real_t angle = ::elastica::geometry::deg2rad(10.0);
      //        // I would have thought this was the closest vector, but all
      //        other
      //        // implementations and theory seem to disagree
      //        //      CHECK(capsule.support(Vec3{std::cos(angle), 0.0,
      //        //      std::sin(angle)}) ==
      //        //            Vec3{5.0 + (3.0 / std::tan(angle)), 3.0, 9.0});
      //
      //        auto d = Vec3{std::cos(angle), 0.0, std::sin(angle)};
      //        CHECK(capsule.support(d) == Vec3{5.0 + (4.0), 3.0, 9.0});
      //
      //        real_t another_angle = ::elastica::geometry::deg2rad(60.0);
      //        auto another_d = Vec3{std::cos(angle) * std::cos(another_angle),
      //                              std::cos(angle) * std::sin(another_angle),
      //                              -std::sin(angle)};
      //        CHECK(capsule.support(another_d) ==
      //              Vec3{5.0 + (4.0 * std::cos(another_angle)),
      //                   3.0 + (4.0 * std::sin(another_angle)), 3.0});
      //      }
    }

    // TODO
    //    SECTION("Centerline not aligned with axis") {
    //
    //    }

    SECTION("Support with contact thresholding") {
      CapsulePrimitiveBase capsule(0UL, 0UL, true, position, identity3(),
                                   radius, length, iron);
      real_t angle = ::elastica::geometry::deg2rad(75.0);
      real_t first_direction = std::cos(angle);
      real_t second_direction = std::sin(angle);

      CHECK(capsule.support(Vec3{0.0, first_direction, -second_direction}) ==
            lower_cap_point + contact_threshold +
                4.0 * Vec3{0.0, first_direction, -second_direction});
    }
  }

}  // namespace
