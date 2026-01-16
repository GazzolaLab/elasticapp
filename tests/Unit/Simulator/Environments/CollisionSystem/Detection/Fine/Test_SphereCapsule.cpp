//
// Created by Tejaswin Parthasarathy on 8/8/22.
//
#include <cmath>

#include "Framework/TestingFramework.hpp"
//
#include "Simulator/Environments/CollisionSystem/Detection/Fine/Kernels/SphereCapsule.hpp"
//
#include "Simulator/Environments/CollisionSystem/Contact/Contact.hpp"
#include "Simulator/Environments/CollisionSystem/Contact/ContactVectorAdapter.hpp"
#include "Simulator/Frames.hpp"
#include "Simulator/Materials.hpp"
#include "Utilities/Geometry/Angles.hpp"
#include "Utilities/Math/Orthogonal.hpp"
#include "Utilities/Math/Vec3.hpp"

namespace {

  using namespace ::elastica;
  using namespace ::elastica::collision;
  using ::elastica::collision::detection::fine::do_collide;

  using CV = ContactVectorAdapter<Contact>;

  void clear(CV& contacts_vector) { contacts_vector.clear(); }

  ELASTICA_TEST_CASE(
      "Unit.Environments.FineDetection.MaxContacts.collide_sphere_capsule",
      "[Unit][Environments]") {
    CV contacts;
    REQUIRE(contacts.empty());

    Vec3 position{5.0, 3.0, 6.0};
    real_t radius(4.0);
    SpherePrimitive first_sphere(0UL, 0UL, true, position, radius, iron);

    Vec3 velocity{1.0, 0.0, 0.0};
    Vec3 omega{0.0, 0.0, 0.0};
    first_sphere.transfer(position, velocity, omega);

    const auto R1(make_orthogonal_bases_from_tangent(
        get_unit_vector_along(elastica::EulerianFrame::X)));
    const auto R2(make_orthogonal_bases_from_tangent(
        get_unit_vector_along(elastica::EulerianFrame::Y)));
    const auto R3(make_orthogonal_bases_from_tangent(
        get_unit_vector_along(elastica::EulerianFrame::Z)));

    SECTION("Bodies do not contact") {
      SECTION("When placed far away") {
        const real_t capsule_radius = radius;
        const real_t capsule_length = 2.0 * radius;
        const Vec3 capsule_axis = {1.0, 0.0, 0.0};
        // 3 r because its r_s + (1/2 l + r)_c = 3r
        const Vec3 capsule_position = 3.0 * radius + 10.0 + position;
        CapsulePrimitive second_capsule(1UL, 1UL, true, capsule_position,
                                        capsule_axis, capsule_radius,
                                        capsule_length, iron);
        // always fill in before colliding because we need to make sure that
        // relative velocities are defined
        second_capsule.transfer(second_capsule.get_position(),
                                second_capsule.get_rotation_matrix(), velocity,
                                omega);

        do_collide(first_sphere, second_capsule, contacts);
        CHECK(contacts.empty());
      }
      SECTION("When placed very close") {
        const real_t capsule_radius = radius;
        const real_t capsule_length = 2.0 * radius;
        const Vec3 capsule_position{
            position[0] + 3.0 * radius + contact_threshold, position[1],
            position[2]};
        CapsulePrimitive second_capsule(1UL, 1UL, true, capsule_position, R1,
                                        capsule_radius, capsule_length, iron);
        // always fill in before colliding because we need to make sure that
        // relative velocities are defined
        second_capsule.transfer(second_capsule.get_position(),
                                second_capsule.get_rotation_matrix(), velocity,
                                omega);

        do_collide(first_sphere, second_capsule, contacts);
        CHECK(contacts.empty());

        const Vec3 pos1{position[0] + 2.0 * radius + contact_threshold,
                        position[1], position[2]};
        second_capsule.transfer(pos1, R2, velocity, omega);
        do_collide(first_sphere, second_capsule, contacts);
        CHECK(contacts.empty());

        second_capsule.transfer(pos1, R3, velocity, omega);
        do_collide(first_sphere, second_capsule, contacts);
        CHECK(contacts.empty());

        const Vec3 pos2{position[0],
                        position[1] + 3.0 * radius + contact_threshold,
                        position[2]};
        second_capsule.transfer(pos2, R2, velocity, omega);
        do_collide(first_sphere, second_capsule, contacts);
        CHECK(contacts.empty());

        const Vec3 pos3{position[0],
                        position[1] + 2.0 * radius + contact_threshold,
                        position[2]};
        second_capsule.transfer(pos3, R1, velocity, omega);
        do_collide(first_sphere, second_capsule, contacts);
        CHECK(contacts.empty());

        second_capsule.transfer(pos3, R3, velocity, omega);
        do_collide(first_sphere, second_capsule, contacts);
        CHECK(contacts.empty());

        const Vec3 pos4{position[0], position[1],
                        position[2] + 3.0 * radius + contact_threshold};
        second_capsule.transfer(pos4, R3, velocity, omega);
        do_collide(first_sphere, second_capsule, contacts);
        CHECK(contacts.empty());

        const Vec3 pos5{position[0], position[1],
                        position[2] + 2.0 * radius + contact_threshold};
        second_capsule.transfer(pos5, R1, velocity, omega);
        do_collide(first_sphere, second_capsule, contacts);
        CHECK(contacts.empty());

        second_capsule.transfer(pos5, R2, velocity, omega);
        do_collide(first_sphere, second_capsule, contacts);
        CHECK(contacts.empty());
      }
    }
    SECTION("Bodies penetrate each other") {
      const real_t capsule_radius = radius;
      const real_t capsule_length = 2.0 * radius;

      SECTION("Capsule penetrates with its spherical region") {
        const Vec3 capsule_position{position[0] + radius + 1.75 * radius,
                                    position[1], position[2]};
        CapsulePrimitive second_capsule(1UL, 1UL, true, capsule_position, R1,
                                        capsule_radius, capsule_length, iron);
        // always fill in before colliding because we need to make sure that
        // relative velocities are defined
        second_capsule.transfer(second_capsule.get_position(),
                                second_capsule.get_rotation_matrix(), velocity,
                                omega);

        do_collide(first_sphere, second_capsule, contacts);

        CHECK_FALSE(contacts.empty());
        // negative distances are interpenetrating so stick with the sign
        // convention
        CHECK(contacts[0].get_first_body() == &first_sphere);
        CHECK(contacts[0].get_second_body() == &second_capsule);
        CHECK(contacts[0].get_distance() == approx(-0.25 * radius));
        CHECK(contacts[0].get_position() ==
              position + Vec3{radius - 0.25 / 2 * radius, 0.0, 0.0});
        CHECK(contacts[0].get_normal() == Vec3{-1.0, 0.0, 0.0});

        clear(contacts);

        const real_t angle = geometry::deg2rad(30.0);
        const real_t in_plane_angle = geometry::deg2rad(80.0);
        const Vec3 capsule_axis{std::sin(angle) * std::cos(in_plane_angle),
                                std::sin(angle) * std::sin(in_plane_angle),
                                std::cos(angle)};
        const Rot3 tiltedR = make_orthogonal_bases_from_tangent(capsule_axis);
        const Vec3 pos1 = position + (radius + 1.75 * radius) * capsule_axis;

        second_capsule.transfer(pos1, tiltedR, velocity, omega);
        do_collide(first_sphere, second_capsule, contacts);

        CHECK_FALSE(contacts.empty());
        // negative distances are interpenetrating so stick with the sign
        // convention
        CHECK(contacts[0].get_first_body() == &first_sphere);
        CHECK(contacts[0].get_second_body() == &second_capsule);
        CHECK(contacts[0].get_distance() == approx(-0.25 * radius));
        CHECK(contacts[0].get_position() ==
              position + (radius - 0.25 / 2 * radius) * capsule_axis);
        CHECK(contacts[0].get_normal() == -capsule_axis);

        clear(contacts);
      }
      SECTION("Capsule penetrates with its cylindrical region") {
        const Vec3 pos1{position[0] + radius + 0.75 * radius, position[1],
                        position[2]};
        CapsulePrimitive second_capsule(1UL, 1UL, true, pos1, R1,
                                        capsule_radius, capsule_length, iron);
        second_capsule.transfer(second_capsule.get_position(), R2, velocity,
                                omega);

        do_collide(first_sphere, second_capsule, contacts);

        CHECK_FALSE(contacts.empty());
        // negative distances are interpenetrating so stick with the sign
        // convention
        CHECK(contacts[0].get_first_body() == &first_sphere);
        CHECK(contacts[0].get_second_body() == &second_capsule);
        CHECK(contacts[0].get_distance() == approx(-0.25 * radius));
        CHECK(contacts[0].get_position() ==
              position + Vec3{radius - 0.25 / 2 * radius, 0.0, 0.0});
        CHECK(contacts[0].get_normal() == Vec3{-1.0, 0.0, 0.0});

        clear(contacts);

        const real_t angle = geometry::deg2rad(30.0);
        const real_t in_plane_angle = geometry::deg2rad(80.0);
        const Vec3 radial_direction{std::sin(angle) * std::cos(in_plane_angle),
                                    std::sin(angle) * std::sin(in_plane_angle),
                                    std::cos(angle)};
        // perp tp radial direction
        //        const Vec3 capsule_axis = blaze::column<1UL>(
        //            blaze::trans(make_orthogonal_bases_from_tangent(radial_direction)));
        const Rot3 tiltedR =
            make_orthogonal_bases_from_normal(radial_direction);
        const Vec3 pos2 = position + (radius + 0.6 * radius) * radial_direction;

        second_capsule.transfer(pos2, tiltedR, velocity, omega);

        do_collide(first_sphere, second_capsule, contacts);

        CHECK_FALSE(contacts.empty());
        // negative distances are interpenetrating so stick with the sign
        // convention
        CHECK(contacts[0].get_first_body() == &first_sphere);
        CHECK(contacts[0].get_second_body() == &second_capsule);
        CHECK(contacts[0].get_distance() == approx(-0.4 * radius));
        CHECK(contacts[0].get_position() ==
              position + (radius - 0.4 / 2 * radius) * radial_direction);
        CHECK(contacts[0].get_normal() == -radial_direction);

        clear(contacts);
      }
    }
    SECTION("Bodies touch each other") {
      const real_t capsule_radius = radius;
      const real_t capsule_length = 2.0 * radius;

      SECTION("Capsule touches with its spherical region") {
        const Vec3 u = {1.0, 0.0, 0.0};
        const Vec3 capsule_position = position + 3.0 * radius * u;
        CapsulePrimitive second_capsule(1UL, 1UL, true, capsule_position, R1,
                                        capsule_radius, capsule_length, iron);
        // always fill in before colliding because we need to make sure that
        // relative velocities are defined
        second_capsule.transfer(second_capsule.get_position(),
                                second_capsule.get_rotation_matrix(), velocity,
                                omega);

        do_collide(first_sphere, second_capsule, contacts);

        CHECK(contacts[0].get_first_body() == &first_sphere);
        CHECK(contacts[0].get_second_body() == &second_capsule);
        CHECK(contacts[0].get_distance() == approx(0.0));
        CHECK(contacts[0].get_position() == position + (radius)*u);
        CHECK(contacts[0].get_normal() == -u);

        clear(contacts);

        const real_t angle = geometry::deg2rad(30.0);
        const real_t in_plane_angle = geometry::deg2rad(80.0);
        const Vec3 radial_direction{std::sin(angle) * std::cos(in_plane_angle),
                                    std::sin(angle) * std::sin(in_plane_angle),
                                    std::cos(angle)};

        const Rot3 tiltedR =
            make_orthogonal_bases_from_tangent(radial_direction);
        const Vec3 pos2 = position + (3.0 * radius) * radial_direction;

        second_capsule.transfer(pos2, tiltedR, velocity, omega);

        do_collide(first_sphere, second_capsule, contacts);

        CHECK(contacts[0].get_first_body() == &first_sphere);
        CHECK(contacts[0].get_second_body() == &second_capsule);
        CHECK(contacts[0].get_distance() == approx(0.0));
        CHECK(contacts[0].get_position() ==
              position + (radius)*radial_direction);
        CHECK(contacts[0].get_normal() == -radial_direction);

        clear(contacts);
      }
      SECTION("Capsule touches with its cylindrical region") {
        const Vec3 pos1{position[0] + 2.0 * radius, position[1], position[2]};
        CapsulePrimitive second_capsule(1UL, 1UL, true, pos1, R1,
                                        capsule_radius, capsule_length, iron);
        second_capsule.transfer(second_capsule.get_position(), R2, velocity,
                                omega);

        do_collide(first_sphere, second_capsule, contacts);

        CHECK_FALSE(contacts.empty());
        // negative distances are interpenetrating so stick with the sign
        // convention
        CHECK(contacts[0].get_first_body() == &first_sphere);
        CHECK(contacts[0].get_second_body() == &second_capsule);
        CHECK(contacts[0].get_distance() == approx(0.0));
        CHECK(contacts[0].get_position() ==
              position + radius * Vec3{1.0, 0.0, 0.0});
        CHECK(contacts[0].get_normal() == Vec3{-1.0, 0.0, 0.0});

        clear(contacts);

        const real_t angle = geometry::deg2rad(45.0);
        const real_t in_plane_angle = geometry::deg2rad(10.0);
        const Vec3 radial_direction{std::sin(angle) * std::cos(in_plane_angle),
                                    std::sin(angle) * std::sin(in_plane_angle),
                                    std::cos(angle)};
        // perp tp radial direction
        //        const Vec3 capsule_axis = blaze::column<1UL>(
        //            blaze::trans(make_orthogonal_bases_from_tangent(radial_direction)));
        const Rot3 tiltedR =
            make_orthogonal_bases_from_normal(radial_direction);
        const Vec3 pos2 = position + (2.0 * radius) * radial_direction;

        second_capsule.transfer(pos2, tiltedR, velocity, omega);

        do_collide(first_sphere, second_capsule, contacts);

        CHECK_FALSE(contacts.empty());
        // negative distances are interpenetrating so stick with the sign
        // convention
        CHECK(contacts[0].get_first_body() == &first_sphere);
        CHECK(contacts[0].get_second_body() == &second_capsule);
        CHECK(contacts[0].get_distance() == approx(0.0));
        CHECK(contacts[0].get_position() ==
              position + (radius)*radial_direction);
        CHECK(contacts[0].get_normal() == -radial_direction);

        clear(contacts);
      }
    }
  }

}  // namespace
