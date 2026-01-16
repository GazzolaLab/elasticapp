//
// Created by Tejaswin Parthasarathy on 8/8/22.
//
#include <algorithm>
#include <cmath>
#include <memory>

#include "Framework/TestingFramework.hpp"
//
#include "Simulator/Environments/CollisionSystem/Detection/Fine/Kernels/CapsulePlane.hpp"
//
#include "Simulator/Environments/CollisionSystem/Contact/Contact.hpp"
#include "Simulator/Environments/CollisionSystem/Contact/ContactVectorAdapter.hpp"
//
#include "Simulator/Frames.hpp"
#include "Simulator/Materials.hpp"
//
#include "Utilities/Math/Length.hpp"
#include "Utilities/Math/Orthogonal.hpp"
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/TMPL.hpp"
//
#include "Utilities/Geometry/Angles.hpp"
#include "Utilities/Geometry/AxisAngleRotation.hpp"

namespace {

  using namespace ::elastica;
  using namespace ::elastica::collision;
  using ::elastica::collision::detection::fine::do_collide;

  using CV = ContactVectorAdapter<Contact>;

  void clear(CV& contacts_vector) { contacts_vector.clear(); }

  ELASTICA_TEST_CASE(
      "Unit.Environments.FineDetection.MaxContacts.collide_capsule_plane",
      "[Unit][Environments]") {
    CV contacts;
    REQUIRE(contacts.empty());

    Vec3 capsule_position{5.0, 3.0, 6.0};
    real_t capsule_radius(4.0);
    real_t capsule_length(8.0);
    Vec3 capsule_axis{0.0, 0.0, 1.0};

    // aligned along z axis
    const auto R3(make_orthogonal_bases_from_tangent(capsule_axis));

    CapsulePrimitive first_capsule(0UL, 0UL, true, capsule_position, R3,
                                   capsule_radius, capsule_length, iron);

    Vec3 velocity{1.0, 0.0, 0.0};
    Vec3 omega{0.0, 0.0, 0.0};
    first_capsule.transfer_impl(capsule_position, R3, velocity, omega);

    SECTION("Bodies do not contact") {
      SECTION("when placed far away") {
        Vec3 plane_position = {-10.0, 10.0, 10.0};
        Vec3 plane_normal = {0.0, 1.0, 0.0};

        SECTION("when plane is infinite") {
          PlanePrimitive second_plane(1UL, 1UL, plane_position, plane_normal,
                                      iron);
          do_collide(first_capsule, second_plane, contacts);
          CHECK(contacts.empty());

          const real_t angle = geometry::deg2rad(45.0);
          const real_t in_plane_angle = geometry::deg2rad(10.0);
          const Vec3 radial_direction{
              std::sin(angle) * std::cos(in_plane_angle),
              std::sin(angle) * std::sin(in_plane_angle), std::cos(angle)};
          const Vec3 another_plane_position =
              capsule_position +
              radial_direction * capsule_radius * 2;  // 2 for well-separation
          PlanePrimitive another_plane(2UL, 2UL, another_plane_position,
                                       -radial_direction, iron);
          do_collide(first_capsule, another_plane, contacts);
          CHECK(contacts.empty());
        }  // infinite

        SECTION("when plane is finite") {
          PlanePrimitive second_plane(1UL, 1UL, true, plane_position,
                                      plane_normal, {2.0, 1.0}, iron);

          do_collide(first_capsule, second_plane, contacts);
          CHECK(contacts.empty());

          const real_t angle = geometry::deg2rad(45.0);
          const real_t in_plane_angle = geometry::deg2rad(10.0);
          const Vec3 perpendicular_direction{
              std::sin(angle) * std::cos(in_plane_angle),
              std::sin(angle) * std::sin(in_plane_angle), std::cos(angle)};

          const Vec3 radial_direction{
              normalize(capsule_axis % perpendicular_direction)};
          const Vec3 another_plane_position =
              capsule_position + radial_direction * 2 * capsule_radius;
          PlanePrimitive another_plane(2UL, 2UL, true, another_plane_position,
                                       -radial_direction, {1.0, 1.0}, iron);
          do_collide(first_capsule, another_plane, contacts);
          CHECK(contacts.empty());
        }  // finite

      }  // far away

      SECTION("when placed closeby") {
        Vec3 plane_normal = capsule_axis;

        SECTION("when plane is infinite") {
          // if 1 put 1.0 * contact threshold here it fails the test as it goes
          // just less than the contact threshold
          Vec3 plane_position =
              capsule_position -
              plane_normal * (0.5 * capsule_length + capsule_radius +
                              1.1 * contact_threshold);
          PlanePrimitive second_plane(1UL, 1UL, plane_position, plane_normal,
                                      iron);
          do_collide(first_capsule, second_plane, contacts);
          CHECK(contacts.empty());

          const real_t angle = geometry::deg2rad(45.0);
          const real_t in_plane_angle = geometry::deg2rad(10.0);
          const Vec3 perpendicular_direction{
              std::sin(angle) * std::cos(in_plane_angle),
              std::sin(angle) * std::sin(in_plane_angle), std::cos(angle)};
          const Vec3 radial_direction{
              normalize(capsule_axis % perpendicular_direction)};

          const Vec3 another_plane_position =
              capsule_position +
              radial_direction * (capsule_radius + 1.1 * contact_threshold);
          PlanePrimitive another_plane(2UL, 2UL, another_plane_position,
                                       -radial_direction, iron);
          do_collide(first_capsule, another_plane, contacts);
          CHECK(contacts.empty());
        }  // infinite

        SECTION("when plane is finite") {
          // if 1 put 1.0 * contact threshold here it fails the test as it goes
          // just less than the contact threshold
          Vec3 plane_position =
              capsule_position -
              plane_normal * (0.5 * capsule_length + capsule_radius +
                              1.1 * contact_threshold);
          PlanePrimitive second_plane(1UL, 1UL, true, plane_position,
                                      plane_normal, {2.0, 1.0}, iron);

          do_collide(first_capsule, second_plane, contacts);
          CHECK(contacts.empty());

          const real_t angle = geometry::deg2rad(45.0);
          const real_t in_plane_angle = geometry::deg2rad(10.0);
          const Vec3 perpendicular_direction{
              std::sin(angle) * std::cos(in_plane_angle),
              std::sin(angle) * std::sin(in_plane_angle), std::cos(angle)};
          const Vec3 radial_direction{
              normalize(capsule_axis % perpendicular_direction)};

          const Vec3 another_plane_position =
              capsule_position +
              radial_direction * (capsule_radius + 1.1 * contact_threshold);
          PlanePrimitive another_plane(2UL, 2UL, true, another_plane_position,
                                       -radial_direction, {1.0, 1.0}, iron);
          do_collide(first_capsule, another_plane, contacts);
          CHECK(contacts.empty());
        }  // finite

      }  // placed close

    }  // no contact

    SECTION("Bodies penetrate") {
      SECTION("at spherical end of capsule") {
        SECTION("when plane is infinite") {
          // if 1 put 1.0 * contact threshold here it fails the test as it goes
          // just less than the contact threshold
          auto run_test_for =
              [&](const real_t angle_between_capsule_and_plane_normal) {
                real_t expected_penetration_depth = 0.25 * capsule_radius;

                // rotate boaut normal
                const Rot3 R = elastica::geometry::rotate_via_axis_angle(
                    get_unit_vector_along(LagrangianFrame::Normal),
                    angle_between_capsule_and_plane_normal, false);
                const Rot3 tiltedR =
                    R * std::as_const(first_capsule).get_rotation_matrix();

                const Vec3 rotated_axis{
                    tiltedR(cast_along(LagrangianFrame::Tangent),
                            cast_along(EulerianFrame::X)),
                    tiltedR(cast_along(LagrangianFrame::Tangent),
                            cast_along(EulerianFrame::Y)),
                    tiltedR(cast_along(LagrangianFrame::Tangent),
                            cast_along(EulerianFrame::Z))};
                // check the angle is indeed correct
                CHECK(std::acos(elastica::dot(rotated_axis, capsule_axis)) ==
                      // I don't see sign here
                      approx(std::abs(geometry::deg2rad(
                          angle_between_capsule_and_plane_normal))));

                const Vec3 plane_normal = rotated_axis;

                // test on either side of capsule
                constexpr int lower_side = -1;
                constexpr int upper_side = 1;
                for (auto plane_side : {lower_side, upper_side}) {
                  const Vec3 plane_position =
                      capsule_position +
                      plane_side * (0.5 * capsule_length) * capsule_axis +
                      // also multiply here since we need to make sure the plane
                      // does not lie on the same side of center
                      plane_side *
                          (capsule_radius - expected_penetration_depth) *
                          plane_normal;
                  PlanePrimitive second_plane(1UL, 1UL, plane_position,
                                              -plane_side * plane_normal, iron);
                  do_collide(first_capsule, second_plane, contacts);

                  CAPTURE(plane_side);
                  CAPTURE(angle_between_capsule_and_plane_normal);
                  CHECK_FALSE(contacts.empty());
                  // negative distances are interpenetrating so stick with the
                  // sign convention
                  CHECK(contacts[0].get_first_body() == &first_capsule);
                  CHECK(contacts[0].get_second_body() == &second_plane);
                  CHECK(contacts[0].get_distance() ==
                        approx(-expected_penetration_depth));
                  CHECK(contacts[0].get_position() == plane_position);
                  CHECK(contacts[0].get_normal() ==
                        (-plane_side * plane_normal));

                  clear(contacts);
                }
              };
          for (auto angle : {0., 15., 30., 45., 60., 75.}) {
            run_test_for(angle);
            run_test_for(-angle);
          }
        }  // inf

        if (false) {
          SECTION("when plane is finite") {
            auto run_test_for =
                [&](const real_t angle_between_capsule_and_plane_normal) {
                  real_t expected_penetration_depth = 0.25 * capsule_radius;

                  // rotate boaut normal
                  const Rot3 R = elastica::geometry::rotate_via_axis_angle(
                      get_unit_vector_along(LagrangianFrame::Normal),
                      angle_between_capsule_and_plane_normal, false);
                  const Rot3 tiltedR =
                      R * std::as_const(first_capsule).get_rotation_matrix();

                  const Vec3 rotated_axis{
                      tiltedR(cast_along(LagrangianFrame::Tangent),
                              cast_along(EulerianFrame::X)),
                      tiltedR(cast_along(LagrangianFrame::Tangent),
                              cast_along(EulerianFrame::Y)),
                      tiltedR(cast_along(LagrangianFrame::Tangent),
                              cast_along(EulerianFrame::Z))};
                  // check the angle is indeed correct
                  CHECK(std::acos(elastica::dot(rotated_axis, capsule_axis)) ==
                        // I don't see sign here
                        approx(std::abs(geometry::deg2rad(
                            angle_between_capsule_and_plane_normal))));

                  const Vec3 plane_normal = rotated_axis;

                  constexpr int lower_side = -1;
                  constexpr int upper_side = 1;
                  for (auto plane_side : {lower_side, upper_side}) {
                    // place the plane on the middle and one of the four edges
                    // does not matter
                    const real_t plane_span = 10.0;
                    const Vec3 expected_contact_point =
                        capsule_position +
                        plane_side * (0.5 * capsule_length) * capsule_axis +
                        // also multiply here since we need to make sure the
                        // plane does not lie on the same side of center
                        plane_side *
                            (capsule_radius - expected_penetration_depth) *
                            plane_normal;

                    auto run_scenario = [&](const Vec3 plane_position) {
                      PlanePrimitive another_plane(
                          1UL, 1UL, true, plane_position,
                          -plane_side * plane_normal, {plane_span, plane_span},
                          iron);

                      do_collide(first_capsule, another_plane, contacts);

                      CAPTURE(plane_side);
                      CAPTURE(angle_between_capsule_and_plane_normal);
                      CAPTURE(plane_position);

                      CHECK_FALSE(contacts.empty());
                      // negative distances are interpenetrating so stick with
                      // the sign convention
                      CHECK(contacts[0].get_first_body() == &first_capsule);
                      CHECK(contacts[0].get_second_body() == &another_plane);
                      CHECK(contacts[0].get_distance() ==
                            approx(-expected_penetration_depth));
                      CHECK(contacts[0].get_position() ==
                            expected_contact_point);
                      CHECK(contacts[0].get_normal() ==
                            (-plane_side * plane_normal));

                      clear(contacts);
                    };  // run_scenario

                    auto plane_span_directions = [&]() {
                      PlanePrimitive second_plane(
                          1UL, 1UL, expected_contact_point,
                          -plane_side * plane_normal, iron);
                      // plane in middle
                      const Rot3 planeR =
                          std::as_const(second_plane).get_rotation_matrix();
                      const Vec3 plane_span_tangent{
                          planeR(cast_along(LagrangianFrame::Tangent),
                                 cast_along(EulerianFrame::X)),
                          planeR(cast_along(LagrangianFrame::Tangent),
                                 cast_along(EulerianFrame::Y)),
                          planeR(cast_along(LagrangianFrame::Tangent),
                                 cast_along(EulerianFrame::Z))};
                      const Vec3 plane_span_binormal{
                          planeR(cast_along(LagrangianFrame::Binormal),
                                 cast_along(EulerianFrame::X)),
                          planeR(cast_along(LagrangianFrame::Binormal),
                                 cast_along(EulerianFrame::Y)),
                          planeR(cast_along(LagrangianFrame::Binormal),
                                 cast_along(EulerianFrame::Z))};
                      CHECK(elastica::dot(plane_span_binormal, plane_normal) ==
                            approx(0.));
                      CHECK(elastica::dot(plane_span_tangent, plane_normal) ==
                            approx(0.));

                      return std::make_pair(plane_span_tangent,
                                            plane_span_binormal);
                    }();

                    {
                      // plane is in the middle
                      const Vec3 plane_position =
                          expected_contact_point +
                          plane_span_directions.first * 0.2 * plane_span +
                          plane_span_directions.second * 0.2 * plane_span;
                      run_scenario(plane_position);
                    }

                    {  // contact point is at "left" edge
                      const Vec3 plane_position =
                          expected_contact_point +
                          plane_span_directions.first * 2.0 * plane_span +
                          plane_span_directions.second * 0.2 * plane_span;
                      run_scenario(plane_position);
                    }

                  }  // side
                };

            for (auto angle : {0., 15., 30., 45., 60., 75.}) {
              run_test_for(angle);
              run_test_for(-angle);
            }

          }  // finite

        }  // path has not been coded yet!

      }  // spherical
      SECTION("at cylindrical part of capsule") {
        SECTION("with two contacts recorded") {
          auto run_test_for = [&](Vec3 const& plane_normal) {
            real_t expected_penetration_depth = 0.25 * capsule_radius;

            const Vec3 plane_position =
                capsule_position +
                (capsule_radius - expected_penetration_depth) * plane_normal;

            PlanePrimitive second_plane(1UL, 1UL, plane_position, -plane_normal,
                                        iron);

            do_collide(first_capsule, second_plane, contacts);

            CHECK_FALSE(contacts.empty());
            REQUIRE(std::size(contacts) == 2UL);

            // negative distances are interpenetrating so stick with the
            // sign convention
            CHECK(contacts[0].get_first_body() == &first_capsule);
            CHECK(contacts[0].get_second_body() == &second_plane);
            CHECK(contacts[0].get_distance() ==
                  approx(-expected_penetration_depth));
            CHECK(contacts[0].get_position() ==
                  (plane_position + 0.5 * capsule_length * capsule_axis));
            CHECK(contacts[0].get_normal() == (-plane_normal));

            CHECK(contacts[1].get_first_body() == &first_capsule);
            CHECK(contacts[1].get_second_body() == &second_plane);
            CHECK(contacts[1].get_distance() ==
                  approx(-expected_penetration_depth));
            CHECK(contacts[1].get_position() ==
                  (plane_position - 0.5 * capsule_length * capsule_axis));
            CHECK(contacts[1].get_normal() == (-plane_normal));

            clear(contacts);
          };

          const Rot3 capR =
              std::as_const(first_capsule).get_rotation_matrix();

          SECTION("plane normal == capsule normal") {
            const Vec3 plane_normal{capR(cast_along(LagrangianFrame::Normal),
                                         cast_along(EulerianFrame::X)),
                                    capR(cast_along(LagrangianFrame::Normal),
                                         cast_along(EulerianFrame::Y)),
                                    capR(cast_along(LagrangianFrame::Normal),
                                         cast_along(EulerianFrame::Z))};
            run_test_for(plane_normal);
          }

          SECTION("plane normal == capsule binormal") {
            const Vec3 plane_normal{capR(cast_along(LagrangianFrame::Binormal),
                                         cast_along(EulerianFrame::X)),
                                    capR(cast_along(LagrangianFrame::Binormal),
                                         cast_along(EulerianFrame::Y)),
                                    capR(cast_along(LagrangianFrame::Binormal),
                                         cast_along(EulerianFrame::Z))};
            run_test_for(plane_normal);
          }
        }  // two contacts

      }  // cylinder

    }  // penetrating case

    // TODO : bodies touch each other section
  }

}  // namespace