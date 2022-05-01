//
// Created by Tejaswin Parthasarathy on 1/13/21.
//

#include <cmath>
#include <random>

#include "Framework/TestingFramework.hpp"
#include "Utilities/DefineTypes.h"
#include "Utilities/Geometry/Angles.hpp"
#include "Utilities/Geometry/GetClosestLineSegmentPoints.hpp"
#include "Utilities/Math/Dist.hpp"
#include "Utilities/Math/Normalize.hpp"
#include "Utilities/Math/Vec3.hpp"

namespace {
  using namespace elastica;

  Vec3 generate_random_direction() {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_real_distribution<real_t> distribution(0.0, 180.0);

    const real_t angle = geometry::deg2rad(distribution(generator));
    const real_t in_plane_angle = geometry::deg2rad(distribution(generator));
    return Vec3{std::sin(angle) * std::cos(in_plane_angle),
                std::sin(angle) * std::sin(in_plane_angle), std::cos(angle)};
  }

  ELASTICA_TEST_CASE("Unit.Utilities.Geometry.get_closest_line_segment_points",
                     "[Unit][Utilities]") {
    SECTION("Parallel lines") {
      const real_t distance = 10.0;
      auto check_parallel = [=](real_t along_a, real_t along_b) {
        const Vec3 direction = generate_random_direction();

        //      const Vec3 direction = {-0.97974818468590862,
        //      0.0017541652783870899,
        //                              0.20022591617680954};

        const Vec3 a1_seed{1.0, 0.0, 0.0};
        const Vec3 b1_seed =
            a1_seed + 9.0 * normalize(Vec3{0.2, 0.4, 0.5} % direction);
        //        std::tie(a1_seed, b1_seed) = [&]() {
        //          const Vec3 a1_seed{1.0, 0.0, 0.0};
        //          const Vec3 b1_seed{5.0, 2.0, 8.0};
        //          const Vec3 ab = b1_seed - a1_seed;
        //          const Vec3 perp = normalize(ab % direction);
        //          return std::make_tuple(a1_seed, a1_seed + perp * 9.0);
        //        }();

        const Vec3 a1 = a1_seed - distance * along_a * direction;
        const Vec3 a2 = a1_seed + distance * (1.0 - along_a) * direction;
        const Vec3 b1 = b1_seed - distance * along_b * direction;
        const Vec3 b2 = b1_seed + distance * (1.0 - along_b) * direction;

        Vec3 cp1, cp2;
        auto result =
            geometry::get_closest_line_segment_points(a1, a2, b1, b2, cp1, cp2);

        // how much is b displaced
        CHECK(dist(cp1, cp2) == approx(9.0));
        return result;
      };

      SECTION("Interior") {
        for (auto along : {0.2, 0.4, 0.6, 0.8}) {
          auto result = check_parallel(along, along);
          // This really depends on the code path and the direction so we dont
          // check it

          //          CHECK(result.first_line_status ==
          //                geometry::get_closest_line_segment_points.MIDDLE);
          //          // the code first checks for left and passes it
          //          CHECK(result.second_line_status ==
          //                geometry::get_closest_line_segment_points.LEFT);
        }
      }  // interior

      SECTION("Edges") {
        for (auto along : {0.2, 0.4, 0.6, 0.8}) {
          auto result = check_parallel(along, 0.0);
          //          CHECK(result.first_line_status ==
          //                geometry::get_closest_line_segment_points.MIDDLE);
          //          CHECK(result.second_line_status ==
          //                geometry::get_closest_line_segment_points.LEFT);

          result = check_parallel(0.0, along);
          //          CHECK(result.first_line_status ==
          //                geometry::get_closest_line_segment_points.LEFT);
          //          CHECK(result.second_line_status ==
          //                geometry::get_closest_line_segment_points.MIDDLE);
        }

        for (auto along : {0.2, 0.4, 0.6, 0.8}) {
          auto result = check_parallel(along, 1.0);
          //          CHECK(result.first_line_status ==
          //                geometry::get_closest_line_segment_points.MIDDLE);
          //          CHECK(result.second_line_status ==
          //                geometry::get_closest_line_segment_points.RIGHT);

          result = check_parallel(1.0, along);
          //          CHECK(result.first_line_status ==
          //                geometry::get_closest_line_segment_points.RIGHT);
          //          CHECK(result.second_line_status ==
          //                geometry::get_closest_line_segment_points.MIDDLE);
        }
      }  // edges

      // point should lie on line
      //      CHECK(normalize(cp1 - a1) == direction);
      //      CHECK(normalize(cp2 - b1) == direction);

      //      CHECK(cp1 == a1);
      //      CHECK(result.first_line_status ==
      //      geometry::get_closest_line_segment_points.LEFT);
      //      CHECK(result.second_line_status ==
      //      geometry::get_closest_line_segment_points.LEFT);
      //
      //      CHECK(cp2 == b1);
    }  // parallel
    SECTION("Perpendicular lines") {
      const real_t distance = 10.0;
      auto check_perpendicular = [=](real_t along_a, real_t along_b) {
        const Vec3 direction = generate_random_direction();

        const Vec3 a1 = {1.0, 0.0, 0.0};
        const Vec3 a2 = a1 + 10.0 * direction;
        const Vec3 expected_cp1 = a1 + along_a * (a2 - a1);

        const Vec3 normal_direction =
            normalize(generate_random_direction() % direction);
        const Vec3 expected_cp2 = expected_cp1 + normal_direction * distance;

        const Vec3 b_direction = normal_direction % direction;
        const Vec3 b1 = expected_cp2 - 5.0 * b_direction * along_b;
        const Vec3 b2 = expected_cp2 + 5.0 * (1.0 - along_b) * b_direction;

        REQUIRE_FALSE(b_direction == direction);
        REQUIRE_FALSE(b_direction == -direction);
        REQUIRE(dist(expected_cp2, expected_cp1) == approx(distance));

        Vec3 cp1, cp2;
        auto result =
            geometry::get_closest_line_segment_points(a1, a2, b1, b2, cp1, cp2);

        // point should lie on line
        CHECK(normalize(cp1 - a1) % direction == Vec3{0.0, 0.0, 0.0});
        CHECK(normalize(cp2 - b1) % b_direction == Vec3{0.0, 0.0, 0.0});

        CHECK(dist(cp1, cp2) == approx(distance));
        CHECK(cp1 == expected_cp1);
        CHECK(cp2 == expected_cp2);

        return result;
      };
      SECTION("Interior") {
        for (auto along : {0.2, 0.4, 0.6, 0.8}) {
          auto result = check_perpendicular(along, along);
          CHECK(result.first_line_status ==
                geometry::get_closest_line_segment_points.MIDDLE);
          CHECK(result.second_line_status ==
                geometry::get_closest_line_segment_points.MIDDLE);
        }
      }  // interior
      SECTION("Edges") {
        for (auto along : {0.2, 0.4, 0.6, 0.8}) {
          auto result = check_perpendicular(along, 0.0);
          CHECK(result.first_line_status ==
                geometry::get_closest_line_segment_points.MIDDLE);
          CHECK(result.second_line_status ==
                geometry::get_closest_line_segment_points.LEFT);

          result = check_perpendicular(0.0, along);
          CHECK(result.first_line_status ==
                geometry::get_closest_line_segment_points.LEFT);
          CHECK(result.second_line_status ==
                geometry::get_closest_line_segment_points.MIDDLE);
        }

        for (auto along : {0.2, 0.4, 0.6, 0.8}) {
          auto result = check_perpendicular(along, 1.0);
          CHECK(result.first_line_status ==
                geometry::get_closest_line_segment_points.MIDDLE);
          CHECK(result.second_line_status ==
                geometry::get_closest_line_segment_points.RIGHT);

          result = check_perpendicular(1.0, along);
          CHECK(result.first_line_status ==
                geometry::get_closest_line_segment_points.RIGHT);
          CHECK(result.second_line_status ==
                geometry::get_closest_line_segment_points.MIDDLE);
        }

        // from a custom problem
        {
          const real_t capsule_radius(4.0);
          const real_t capsule_length(2.0 * capsule_radius);
          const Vec3 capsule_axis = normalize(Vec3{2.0, 1.0, 0.0});
          const Vec3 capsule_position{5.0, 3.0, 6.0};

          const Vec3 a1 =
              capsule_position - 0.5 * capsule_axis * capsule_length;
          const Vec3 a2 = a1 + capsule_axis * capsule_length;

          const real_t second_capsule_radius = 2.0;
          const real_t second_capsule_length = 4.0 * second_capsule_radius;

          const real_t angle = geometry::deg2rad(0.0);
          const real_t in_plane_angle = geometry::deg2rad(0.0);
          const Vec3 perp_direction{std::sin(angle) * std::cos(in_plane_angle),
                                    std::sin(angle) * std::sin(in_plane_angle),
                                    std::cos(angle)};
          const Vec3 radial_direction = capsule_axis % perp_direction;

          const Vec3 second_capsule_position =
              capsule_position +
              (capsule_radius +
               1.0 * (second_capsule_radius + 0.5 * second_capsule_length)) *
                  radial_direction;
          const Vec3 second_capsule_axis = radial_direction;

          const Vec3 b1 = second_capsule_position -
                          0.5 * second_capsule_axis * second_capsule_length;
          const Vec3 b2 = second_capsule_position +
                          0.5 * second_capsule_axis * second_capsule_length;

          Vec3 cp1, cp2;

          geometry::get_closest_line_segment_points(a1, a2, b1, b2, cp1, cp2);
          CHECK(cp1 == capsule_position);
          CHECK(cp2 == b1);
          CHECK(length(cp2 - cp1) ==
                approx(capsule_radius + second_capsule_radius));
        }

      }  // edges
    }    // perp

    SECTION("Intersecting") {
      SECTION("At the edges") {
        const Vec3 a1 = {1.0, 2.0, 3.0};
        const Vec3 d1 = generate_random_direction();
        const Vec3 d2 = generate_random_direction();
        const Vec3 a2 = a1 + 10.0 * d1;
        const Vec3 b2 = a1 + 10.0 * d2;

        Vec3 cp1, cp2;
        auto result =
            geometry::get_closest_line_segment_points(a1, a2, a1, b2, cp1, cp2);
        CHECK(result.first_line_status ==
              geometry::get_closest_line_segment_points.LEFT);
        CHECK(result.second_line_status ==
              geometry::get_closest_line_segment_points.LEFT);
        CHECK(cp1 == a1);
        CHECK(cp2 == a1);

        result =
            geometry::get_closest_line_segment_points(a2, a1, a1, b2, cp1, cp2);
        CHECK(result.first_line_status ==
              geometry::get_closest_line_segment_points.RIGHT);
        CHECK(result.second_line_status ==
              geometry::get_closest_line_segment_points.LEFT);
        CHECK(cp1 == a1);
        CHECK(cp2 == a1);

        result =
            geometry::get_closest_line_segment_points(a1, a2, b2, a1, cp1, cp2);
        CHECK(result.first_line_status ==
              geometry::get_closest_line_segment_points.LEFT);
        CHECK(result.second_line_status ==
              geometry::get_closest_line_segment_points.RIGHT);
        CHECK(cp1 == a1);
        CHECK(cp2 == a1);

        result =
            geometry::get_closest_line_segment_points(a2, a1, b2, a1, cp1, cp2);
        CHECK(result.first_line_status ==
              geometry::get_closest_line_segment_points.RIGHT);
        CHECK(result.second_line_status ==
              geometry::get_closest_line_segment_points.RIGHT);
        CHECK(cp1 == a1);
        CHECK(cp2 == a1);
      }
      SECTION("Interior") {
        auto check_intersecting = [](real_t along_a, real_t along_b) {
          const Vec3 intersection_point = {1.0, 2.0, 3.0};
          const Vec3 d1 = generate_random_direction();
          const Vec3 d2 = generate_random_direction();

          const Vec3 a1 = intersection_point - along_a * 10.0 * d1;
          const Vec3 a2 = intersection_point + (1.0 - along_a) * 10.0 * d1;
          const Vec3 b1 = intersection_point - along_b * 10.0 * d2;
          const Vec3 b2 = intersection_point + (1.0 - along_b) * 10.0 * d2;

          Vec3 cp1, cp2;
          auto result = geometry::get_closest_line_segment_points(a1, a2, b1,
                                                                  b2, cp1, cp2);
          CHECK(result.first_line_status ==
                geometry::get_closest_line_segment_points.MIDDLE);
          CHECK(result.second_line_status ==
                geometry::get_closest_line_segment_points.MIDDLE);
          CHECK(cp1 == intersection_point);
          CHECK(cp2 == intersection_point);
        };
        const real_t alongs[] = {0.2, 0.4, 0.6, 0.8};
        for (auto along_a : alongs)
          for (auto along_b : alongs)
            check_intersecting(along_a, along_b);
      }
    }

    SECTION("Converging") {
      SECTION("With first end closest for both lines") {
        auto check_converging = [](real_t along_a, real_t along_b) {
          const Vec3 potential_intersection_point = {1.0, 2.0, 3.0};
          const Vec3 d1 = generate_random_direction();
          const Vec3 d2 = generate_random_direction();
          const real_t cos_angle = (dot(d1, d2));
          REQUIRE(cos_angle < 1.0);

          // cosine formula
          const real_t a = along_a * 10.0;
          const real_t b = along_b * 10.0;
          const real_t expected_length =
              std::sqrt(a * a + b * b - 2 * a * b * cos_angle);

          const Vec3 a1 = potential_intersection_point + a * d1;
          const Vec3 a2 = a1 + 10.0 * d1;
          const Vec3 b1 = potential_intersection_point + b * d2;
          const Vec3 b2 = b1 + 10.0 * d2;

          Vec3 cp1, cp2;
          auto result = geometry::get_closest_line_segment_points(a1, a2, b1,
                                                                  b2, cp1, cp2);
          CHECK(result.first_line_status ==
                geometry::get_closest_line_segment_points.LEFT);
          CHECK(result.second_line_status ==
                geometry::get_closest_line_segment_points.LEFT);
          // CHECK(cp2 == b1);
          CHECK(dist(cp1, cp2) == approx(expected_length));
        };

        const real_t alongs[] = {0.2, 0.4, 0.6, 0.8};
        for (auto along : alongs)
          check_converging(along, along);
      }

      SECTION("With first end closest for second line") {
        auto check_converging = [](real_t along_a, real_t along_b) {
          // b > a is the pre condition
          const Vec3 potential_intersection_point = {1.0, 2.0, 3.0};
          const Vec3 d1 = generate_random_direction();
          const Vec3 d2 = generate_random_direction();
          const real_t cos_angle = (dot(d1, d2));
          const real_t sin_angle = std::sqrt(1.0 - cos_angle * cos_angle);
          REQUIRE(cos_angle < 1.0);

          // cosine formula
          const real_t a = along_a * 10.0;
          const real_t b = along_b * 10.0;

          const Vec3 a1 = potential_intersection_point + a * d1;
          const Vec3 a2 = a1 + 10.0 * d1;
          const Vec3 b1 = potential_intersection_point + b * d2;
          const Vec3 b2 = b1 + 10.0 * d2;

          const real_t expected_length =
              std::min(dist(a1, b1), std::abs(b * sin_angle));

          Vec3 cp1, cp2;
          auto result = geometry::get_closest_line_segment_points(a1, a2, b1,
                                                                  b2, cp1, cp2);
          // This can be left or middle depending on the configuration
          //          CHECK(result.first_line_status ==
          //                geometry::get_closest_line_segment_points.MIDDLE);
          CHECK(result.second_line_status ==
                geometry::get_closest_line_segment_points.LEFT);
          // CHECK(cp2 == b1);
          // Dont know what to check here.
          // CHECK(dist(cp1, cp2) == approx(expected_length));
        };
        const real_t alongs[] = {0.2, 0.4, 0.6, 0.8};
        for (auto along : alongs)
          check_converging(along, along + 0.1);
      }
    }
  }

}  // namespace
