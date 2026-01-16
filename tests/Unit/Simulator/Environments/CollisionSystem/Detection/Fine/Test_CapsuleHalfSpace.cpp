//
// Created by Tejaswin Parthasarathy on 8/8/22.
//
#include <cmath>

#include "Framework/TestingFramework.hpp"
//
#include "Simulator/Environments/CollisionSystem/Detection/Fine/Kernels/CapsuleHalfSpace.hpp"
//
#include "Simulator/Environments/CollisionSystem/Contact/Contact.hpp"
#include "Simulator/Environments/CollisionSystem/Contact/ContactVectorAdapter.hpp"
//
#include "Simulator/Frames.hpp"
#include "Simulator/Materials.hpp"
//
#include "Utilities/Math/Orthogonal.hpp"
#include "Utilities/Math/Vec3.hpp"
//
#include "Utilities/Geometry/Angles.hpp"
#include "Utilities/Geometry/AxisAngleRotation.hpp"

namespace {

  using namespace ::elastica;
  using namespace ::elastica::collision;
  using ::elastica::collision::detection::fine::do_collide;

  using App = Approx;
  using CV = ContactVectorAdapter<Contact>;

  void clear(CV& contacts_vector) { contacts_vector.clear(); }

  ELASTICA_TEST_CASE(
      "Unit.Environments.FineDetection.MaxContacts.collide_capsule_half_space",
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

    // half-space plane is infinite, so only guarantee precision till
    // contact_threshold
    auto contact_approx = App::custom().margin(0.1 * contact_threshold);

    SECTION("Bodies do not contact") {
      SECTION("when placed far away") {
        Vec3 half_space_position = {0.0, 0.0, -3.0};  // -2 is the capsule tip
        Vec3 half_space_normal = {0.0, 0.0, 1.0};

        HalfSpacePrimitive second_half_space(1UL, 1UL, half_space_position,
                                             half_space_normal, iron);
        do_collide(first_capsule, second_half_space, contacts);
        CHECK(contacts.empty());

        const real_t angle = geometry::deg2rad(45.0);
        const real_t in_half_space_angle = geometry::deg2rad(10.0);
        const Vec3 radial_direction{
            std::sin(angle) * std::cos(in_half_space_angle),
            std::sin(angle) * std::sin(in_half_space_angle), std::cos(angle)};
        const Vec3 another_half_space_position =
            capsule_position +
            radial_direction * capsule_radius * 2;  // 2 for well-separation
        HalfSpacePrimitive another_half_space(
            2UL, 2UL, another_half_space_position, -radial_direction, iron);
        do_collide(first_capsule, another_half_space, contacts);
        CHECK(contacts.empty());

      }  // far away
      SECTION("when placed closeby") {
        Vec3 const& half_space_normal = capsule_axis;

        // if 1 put 1.0 * contact threshold here it fails the test as it goes
        // just less than the contact threshold
        Vec3 half_space_position =
            capsule_position -
            half_space_normal * (0.5 * capsule_length + capsule_radius +
                                 1.1 * contact_threshold);
        HalfSpacePrimitive second_half_space(1UL, 1UL, half_space_position,
                                             half_space_normal, iron);
        do_collide(first_capsule, second_half_space, contacts);
        CHECK(contacts.empty());

        const real_t angle = geometry::deg2rad(45.0);
        const real_t in_half_space_angle = geometry::deg2rad(10.0);
        const Vec3 perpendicular_direction{
            std::sin(angle) * std::cos(in_half_space_angle),
            std::sin(angle) * std::sin(in_half_space_angle), std::cos(angle)};
        const Vec3 radial_direction{
            normalize(capsule_axis % perpendicular_direction)};

        const Vec3 another_half_space_position =
            capsule_position +
            radial_direction * (capsule_radius + 1.1 * contact_threshold);
        HalfSpacePrimitive another_half_space(
            2UL, 2UL, another_half_space_position, -radial_direction, iron);
        do_collide(first_capsule, another_half_space, contacts);
        CHECK(contacts.empty());

      }  // placed close
    }    // no contact

    SECTION("Bodies penetrate") {
      SECTION("at spherical end of capsule") {
        // if 1 put 1.0 * contact threshold here it fails the test as it goes
        // just less than the contact threshold
        auto run_test_for =
            [&](const real_t angle_between_capsule_and_half_space_normal,
                const real_t expected_penetration_depth) {
              // real_t expected_penetration_depth = 0.25 * capsule_radius;

              // rotate boaut normal
              const Rot3 R = elastica::geometry::rotate_via_axis_angle(
                  get_unit_vector_along(LagrangianFrame::Normal),
                  angle_between_capsule_and_half_space_normal, false);
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
                        angle_between_capsule_and_half_space_normal))));

              Vec3 const& half_space_normal = rotated_axis;

              // test on either side of capsule
              constexpr int lower_side = -1;
              constexpr int upper_side = 1;
              for (auto half_space_side : {lower_side, upper_side}) {
                const Vec3 half_space_position =
                    capsule_position +
                    half_space_side * (0.5 * capsule_length) * capsule_axis +
                    // also multiply here since we need to make sure the
                    // half_space does not lie on the same side of center
                    half_space_side *
                        (capsule_radius - expected_penetration_depth) *
                        half_space_normal;
                HalfSpacePrimitive second_half_space(
                    1UL, 1UL, half_space_position,
                    -half_space_side * half_space_normal, iron);
                do_collide(first_capsule, second_half_space, contacts);

                CAPTURE(half_space_side);
                CAPTURE(angle_between_capsule_and_half_space_normal);
                CHECK_FALSE(contacts.empty());
                // negative distances are interpenetrating so stick with the
                // sign convention
                CHECK(contacts[0].get_first_body() == &first_capsule);
                CHECK(contacts[0].get_second_body() == &second_half_space);
                CHECK(contacts[0].get_distance() ==
                      contact_approx(-expected_penetration_depth));
                auto const expected_contact_position =
                    half_space_position + half_space_side * half_space_normal *
                                              0.5 * expected_penetration_depth;
                CHECK(contacts[0].get_position() == expected_contact_position);
                CHECK(contacts[0].get_normal() ==
                      (-half_space_side * half_space_normal));

                clear(contacts);
              }
            };
        SECTION("closest point outside halfspace") {
          const real_t expected_penetration_depth = 0.25 * capsule_radius;
          for (auto angle : {0., 15., 30., 45., 60., 75.}) {
            run_test_for(angle, expected_penetration_depth);
            run_test_for(-angle, expected_penetration_depth);
          }
        }
        SECTION("closest point inside halfspace") {
          const real_t expected_penetration_depth = 1.25 * capsule_radius;
          for (auto angle : {0., 15., 30., 45., 60., 75.}) {
            run_test_for(angle, expected_penetration_depth);
            run_test_for(-angle, expected_penetration_depth);
          }
        }
      }  // spherical

      SECTION("at cylindrical part of capsule") {
        auto run_test_for = [&](Vec3 const& half_space_normal,
                                real_t const expected_penetration_depth) {
          const Vec3 half_space_position =
              capsule_position +
              (capsule_radius - expected_penetration_depth) * half_space_normal;

          HalfSpacePrimitive second_half_space(1UL, 1UL, half_space_position,
                                               -half_space_normal, iron);

          do_collide(first_capsule, second_half_space, contacts);

          CHECK_FALSE(contacts.empty());
#ifndef _ELASTICA_CAPSULE_HALFSPACE_SPHERE_CHECK
          REQUIRE(std::size(contacts) == 1UL);
#else
          REQUIRE(std::size(contacts) == 2UL);
#endif

          // negative distances are interpenetrating so stick with the
          // sign convention
          CHECK(contacts[0].get_first_body() == &first_capsule);
          CHECK(contacts[0].get_second_body() == &second_half_space);
          CHECK(contacts[0].get_distance() ==
                contact_approx(-expected_penetration_depth));
#ifndef _ELASTICA_CAPSULE_HALFSPACE_SPHERE_CHECK
          CHECK(contacts[0].get_position() ==
                (capsule_position +
                 (capsule_radius - 0.5 * expected_penetration_depth) *
                     half_space_normal));
#else
          CHECK(contacts[0].get_position() ==
                (half_space_position + 0.5 * capsule_length * capsule_axis));
#endif
          CHECK(contacts[0].get_normal() == (-half_space_normal));

#ifdef _ELASTICA_CAPSULE_HALFSPACE_SPHERE_CHECK
          CHECK(contacts[1].get_first_body() == &first_capsule);
          CHECK(contacts[1].get_second_body() == &second_half_space);
          CHECK(contacts[1].get_distance() ==
                approx(-expected_penetration_depth));
          CHECK(contacts[1].get_position() ==
                (half_space_position - 0.5 * capsule_length * capsule_axis));
          CHECK(contacts[1].get_normal() == (-half_space_normal));
#endif

          clear(contacts);
        };

        const Rot3 capR = std::as_const(first_capsule).get_rotation_matrix();

        SECTION("Centerline outside hafspace") {
          real_t const expected_penetration_depth = 0.25 * capsule_radius;
          SECTION("half_space normal == capsule normal") {
            const Vec3 half_space_normal{
                capR(cast_along(LagrangianFrame::Normal),
                     cast_along(EulerianFrame::X)),
                capR(cast_along(LagrangianFrame::Normal),
                     cast_along(EulerianFrame::Y)),
                capR(cast_along(LagrangianFrame::Normal),
                     cast_along(EulerianFrame::Z))};
            run_test_for(half_space_normal, expected_penetration_depth);
          }

          SECTION("half_space normal == capsule binormal") {
            const Vec3 half_space_normal{
                capR(cast_along(LagrangianFrame::Binormal),
                     cast_along(EulerianFrame::X)),
                capR(cast_along(LagrangianFrame::Binormal),
                     cast_along(EulerianFrame::Y)),
                capR(cast_along(LagrangianFrame::Binormal),
                     cast_along(EulerianFrame::Z))};
            run_test_for(half_space_normal, expected_penetration_depth);
          }
        }

        SECTION("Centerline inside halfspace") {
          real_t const expected_penetration_depth = 1.25 * capsule_radius;
          SECTION("half_space normal == capsule normal") {
            const Vec3 half_space_normal{
                capR(cast_along(LagrangianFrame::Normal),
                     cast_along(EulerianFrame::X)),
                capR(cast_along(LagrangianFrame::Normal),
                     cast_along(EulerianFrame::Y)),
                capR(cast_along(LagrangianFrame::Normal),
                     cast_along(EulerianFrame::Z))};
            run_test_for(half_space_normal, expected_penetration_depth);
          }

          SECTION("half_space normal == capsule binormal") {
            const Vec3 half_space_normal{
                capR(cast_along(LagrangianFrame::Binormal),
                     cast_along(EulerianFrame::X)),
                capR(cast_along(LagrangianFrame::Binormal),
                     cast_along(EulerianFrame::Y)),
                capR(cast_along(LagrangianFrame::Binormal),
                     cast_along(EulerianFrame::Z))};
            run_test_for(half_space_normal, expected_penetration_depth);
          }
        }

      }  // cylinder
    }    // penetrating case

    // TODO : bodies touch each other section
  }

}  // namespace
