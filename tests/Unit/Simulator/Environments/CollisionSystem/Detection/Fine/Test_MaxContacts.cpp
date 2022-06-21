//
// Created by Tejaswin Parthasarathy on 9/27/21.
//
#include <algorithm>
#include <cmath>
#include <memory>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Environments/CollisionSystem/Contact/Contact.hpp"
#include "Simulator/Environments/CollisionSystem/Contact/ContactVectorAdapter.hpp"
#include "Simulator/Environments/CollisionSystem/Detection/Fine/MaxContacts.hpp"
#include "Simulator/Frames.hpp"
#include "Simulator/Materials.hpp"
#include "Utilities/AsConst.hpp"
#include "Utilities/Geometry/Angles.hpp"
#include "Utilities/Math/Length.hpp"
#include "Utilities/Math/Orthogonal.hpp"
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/Size.hpp"
#include "Utilities/TMPL.hpp"

namespace {

  //#define TEST_GJK_EPA_ALGORITHM

  using namespace ::elastica;
  using namespace ::elastica::collision;
  using ::elastica::collision::detection::fine::MaxContacts;

  class TestMaxContacts : public MaxContacts {
   public:
    using MaxContacts::do_collide;
    using MaxContacts::do_collide_with_capsule;
  };

  // using CID = Contact*;
  using CV = ContactVectorAdapter<Contact>;

  void clear(CV& contacts_vector) {
    // Mem pooling is not used anymore
    // ugly hack for C style mem pooling
    //    std::for_each(contacts_vector.begin(), contacts_vector.end(),
    //                  std::default_delete<Contact>());
    contacts_vector.clear();
  }

  ELASTICA_TEST_CASE(
      "Unit.Environments.FineDetection.MaxContacts.collide_sphere_sphere",
      "[Unit][Environments]") {
    CV contacts;
    REQUIRE(contacts.empty());

    Vec3 position{5.0, 3.0, 6.0};
    real_t radius(4.0);
    SpherePrimitive first_sphere(0UL, 0UL, true, position, radius, iron);

    Vec3 velocity{1.0, 0.0, 0.0};
    Vec3 omega{0.0, 0.0, 0.0};
    first_sphere.transfer(position, velocity, omega);

    SECTION("Bodies do not contact") {
      SECTION("When placed far away") {
        SpherePrimitive second_sphere(1UL, 1UL, true, radius + 1.0 + position,
                                      radius, iron);
        TestMaxContacts::do_collide(first_sphere, second_sphere, contacts);
        CHECK(contacts.empty());
      }

      SECTION("When placed very close") {
        const real_t angle = geometry::deg2rad(30.0);
        const real_t in_plane_angle = geometry::deg2rad(80.0);
        const Vec3 uv{std::sin(angle) * std::cos(in_plane_angle),
                      std::sin(angle) * std::sin(in_plane_angle),
                      std::cos(angle)};
        SpherePrimitive second_sphere(
            1UL, 1UL, true, position + (2.0 * radius + contact_threshold) * uv,
            radius, iron);
        TestMaxContacts::do_collide(first_sphere, second_sphere, contacts);
        CHECK(contacts.empty());
      }
    }
    SECTION("Bodies penetrate each other") {
      {
        Vec3 second_position{position[0] + radius + 0.75 * radius, position[1],
                             position[2]};
        SpherePrimitive second_sphere(1UL, 1UL, true, second_position, radius,
                                      iron);
        second_sphere.transfer(second_position, velocity, omega);

        TestMaxContacts::do_collide(first_sphere, second_sphere, contacts);

        CHECK_FALSE(contacts.empty());
        // negative distances are interpenetrating so stick with the sign
        // convention
        CHECK(contacts[0].get_first_body() == &first_sphere);
        CHECK(contacts[0].get_second_body() == &second_sphere);
        CHECK(contacts[0].get_distance() == approx(-0.25 * radius));
        CHECK(contacts[0].get_position() ==
              position + Vec3{0.75 * radius + 0.25 / 2 * radius, 0.0, 0.0});
        CHECK(contacts[0].get_normal() == Vec3{-1.0, 0.0, 0.0});
      }

      clear(contacts);

      {
        Vec3 second_position{position[0], position[1] + radius + 0.75 * radius,
                             position[2]};
        SpherePrimitive second_sphere(1UL, 1UL, true, second_position, radius,
                                      iron);
        second_sphere.transfer(second_position, velocity, omega);

        TestMaxContacts::do_collide(first_sphere, second_sphere, contacts);

        CHECK_FALSE(contacts.empty());
        // negative distances are interpenetrating so stick with the sign
        // convention
        CHECK(contacts[0].get_first_body() == &first_sphere);
        CHECK(contacts[0].get_second_body() == &second_sphere);
        CHECK(contacts[0].get_distance() == approx(-0.25 * radius));
        CHECK(contacts[0].get_position() ==
              position + Vec3{0.0, 0.75 * radius + 0.25 / 2 * radius, 0.0});
        CHECK(contacts[0].get_normal() == Vec3{0.0, -1.0, 0.0});
      }

      clear(contacts);

      {
        Vec3 second_position{position[0], position[1],
                             position[2] - radius - 0.75 * radius};
        SpherePrimitive second_sphere(1UL, 1UL, true, second_position, radius,
                                      iron);
        second_sphere.transfer(second_position, velocity, omega);

        TestMaxContacts::do_collide(first_sphere, second_sphere, contacts);

        CHECK_FALSE(contacts.empty());
        // negative distances are interpenetrating so stick with the sign
        // convention
        CHECK(contacts[0].get_first_body() == &first_sphere);
        CHECK(contacts[0].get_second_body() == &second_sphere);
        CHECK(contacts[0].get_distance() == approx(-0.25 * radius));
        CHECK(contacts[0].get_position() ==
              position - Vec3{0.0, 0.0, 0.75 * radius + 0.25 / 2 * radius});
        CHECK(contacts[0].get_normal() == Vec3{0.0, 0.0, 1.0});
      }

      clear(contacts);

      {
        const real_t angle = geometry::deg2rad(60.0);
        const real_t in_plane_angle = geometry::deg2rad(75.0);
        const Vec3 uv{std::sin(angle) * std::cos(in_plane_angle),
                      std::sin(angle) * std::sin(in_plane_angle),
                      std::cos(angle)};
        const Vec3 second_position = position + (1 + 0.75) * radius * uv;
        SpherePrimitive second_sphere(1UL, 1UL, true, second_position, radius,
                                      iron);
        second_sphere.transfer(second_position, velocity, omega);

        TestMaxContacts::do_collide(first_sphere, second_sphere, contacts);

        CHECK_FALSE(contacts.empty());
        // negative distances are interpenetrating so stick with the sign
        // convention
        CHECK(contacts[0].get_first_body() == &first_sphere);
        CHECK(contacts[0].get_second_body() == &second_sphere);
        CHECK(contacts[0].get_distance() == approx(-0.25 * radius));
        CHECK(contacts[0].get_position() ==
              position + (0.75 + 0.125) * radius * uv);
        CHECK(contacts[0].get_normal() == -uv);
      }

      clear(contacts);
    }
    SECTION("Bodies touch each other") {
      {
        const real_t angle = geometry::deg2rad(60.0);
        const real_t in_plane_angle = geometry::deg2rad(75.0);
        const Vec3 uv{std::sin(angle) * std::cos(in_plane_angle),
                      std::sin(angle) * std::sin(in_plane_angle),
                      std::cos(angle)};
        const Vec3 second_position = position + 2.0 * radius * uv;
        SpherePrimitive second_sphere(1UL, 1UL, true, second_position, radius,
                                      iron);
        second_sphere.transfer(second_position, velocity, omega);

        TestMaxContacts::do_collide(first_sphere, second_sphere, contacts);

        CHECK_FALSE(contacts.empty());
        // negative distances are interpenetrating so stick with the sign
        // convention
        CHECK(contacts[0].get_first_body() == &first_sphere);
        CHECK(contacts[0].get_second_body() == &second_sphere);
        // I thought this would be contact threshold, but nope
        CHECK(contacts[0].get_distance() == approx(0.0));
        CHECK(contacts[0].get_position() == position + radius * uv);
        CHECK(contacts[0].get_normal() == -uv);
      }
      clear(contacts);
    }
  }

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

        TestMaxContacts::do_collide(first_sphere, second_capsule, contacts);
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

        TestMaxContacts::do_collide(first_sphere, second_capsule, contacts);
        CHECK(contacts.empty());

        const Vec3 pos1{position[0] + 2.0 * radius + contact_threshold,
                        position[1], position[2]};
        second_capsule.transfer(pos1, R2, velocity, omega);
        TestMaxContacts::do_collide(first_sphere, second_capsule, contacts);
        CHECK(contacts.empty());

        second_capsule.transfer(pos1, R3, velocity, omega);
        TestMaxContacts::do_collide(first_sphere, second_capsule, contacts);
        CHECK(contacts.empty());

        const Vec3 pos2{position[0],
                        position[1] + 3.0 * radius + contact_threshold,
                        position[2]};
        second_capsule.transfer(pos2, R2, velocity, omega);
        TestMaxContacts::do_collide(first_sphere, second_capsule, contacts);
        CHECK(contacts.empty());

        const Vec3 pos3{position[0],
                        position[1] + 2.0 * radius + contact_threshold,
                        position[2]};
        second_capsule.transfer(pos3, R1, velocity, omega);
        TestMaxContacts::do_collide(first_sphere, second_capsule, contacts);
        CHECK(contacts.empty());

        second_capsule.transfer(pos3, R3, velocity, omega);
        TestMaxContacts::do_collide(first_sphere, second_capsule, contacts);
        CHECK(contacts.empty());

        const Vec3 pos4{position[0], position[1],
                        position[2] + 3.0 * radius + contact_threshold};
        second_capsule.transfer(pos4, R3, velocity, omega);
        TestMaxContacts::do_collide(first_sphere, second_capsule, contacts);
        CHECK(contacts.empty());

        const Vec3 pos5{position[0], position[1],
                        position[2] + 2.0 * radius + contact_threshold};
        second_capsule.transfer(pos5, R1, velocity, omega);
        TestMaxContacts::do_collide(first_sphere, second_capsule, contacts);
        CHECK(contacts.empty());

        second_capsule.transfer(pos5, R2, velocity, omega);
        TestMaxContacts::do_collide(first_sphere, second_capsule, contacts);
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

        TestMaxContacts::do_collide(first_sphere, second_capsule, contacts);

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
        TestMaxContacts::do_collide(first_sphere, second_capsule, contacts);

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

        TestMaxContacts::do_collide(first_sphere, second_capsule, contacts);

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

        TestMaxContacts::do_collide(first_sphere, second_capsule, contacts);

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

        TestMaxContacts::do_collide(first_sphere, second_capsule, contacts);

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

        TestMaxContacts::do_collide(first_sphere, second_capsule, contacts);

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

        TestMaxContacts::do_collide(first_sphere, second_capsule, contacts);

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

        TestMaxContacts::do_collide(first_sphere, second_capsule, contacts);

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

  ELASTICA_TEST_CASE(
      "Unit.Environments.FineDetection.MaxContacts.collide_sphere_plane",
      "[Unit][Environments]") {
    CV contacts;
    REQUIRE(contacts.empty());

    Vec3 position{5.0, 3.0, 6.0};
    real_t radius(4.0);
    SpherePrimitive first_sphere(0UL, 0UL, true, position, radius, iron);

    Vec3 velocity{1.0, 0.0, 0.0};
    Vec3 omega{0.0, 0.0, 0.0};
    first_sphere.transfer(position, velocity, omega);

    SECTION("Bodies do not contact") {
      SECTION("When placed far away") {
        Vec3 plane_position = {0.0, 0.0, 0.0};
        Vec3 plane_normal = {0.0, 0.0, 1.0};

        SECTION("Infinite plane") {
          PlanePrimitive second_plane(1UL, 1UL, plane_position, plane_normal,
                                      iron);
          TestMaxContacts::do_collide(first_sphere, second_plane, contacts);
          CHECK(contacts.empty());

          const real_t angle = geometry::deg2rad(45.0);
          const real_t in_plane_angle = geometry::deg2rad(10.0);
          const Vec3 radial_direction{
              std::sin(angle) * std::cos(in_plane_angle),
              std::sin(angle) * std::sin(in_plane_angle), std::cos(angle)};
          const Vec3 another_plane_position =
              position + radial_direction * 2 * radius;
          PlanePrimitive another_plane(2UL, 2UL, another_plane_position,
                                       -radial_direction, iron);
          TestMaxContacts::do_collide(first_sphere, another_plane, contacts);
          CHECK(contacts.empty());
        }

        SECTION("Finite plane") {
          PlanePrimitive second_plane(1UL, 1UL, true, plane_position,
                                      plane_normal, {2.0, 1.0}, iron);

          TestMaxContacts::do_collide(first_sphere, second_plane, contacts);
          CHECK(contacts.empty());

          const real_t angle = geometry::deg2rad(45.0);
          const real_t in_plane_angle = geometry::deg2rad(10.0);
          const Vec3 radial_direction{
              std::sin(angle) * std::cos(in_plane_angle),
              std::sin(angle) * std::sin(in_plane_angle), std::cos(angle)};
          const Vec3 another_plane_position =
              position + radial_direction * 2 * radius;
          PlanePrimitive another_plane(2UL, 2UL, true, another_plane_position,
                                       -radial_direction, {1.0, 1.0}, iron);
          TestMaxContacts::do_collide(first_sphere, another_plane, contacts);
          CHECK(contacts.empty());
        }
      }
      SECTION("When placed close") {
        Vec3 plane_normal = {0.0, 0.0, 1.0};

        SECTION("Infinite plane") {
          // if 1 put 1.0 * contact threshold here it fails the test as it goes
          // just less than the contact threshold
          Vec3 plane_position =
              position - plane_normal * (radius + 1.1 * contact_threshold);
          PlanePrimitive second_plane(1UL, 1UL, plane_position, plane_normal,
                                      iron);
          TestMaxContacts::do_collide(first_sphere, second_plane, contacts);
          CHECK(contacts.empty());

          const real_t angle = geometry::deg2rad(45.0);
          const real_t in_plane_angle = geometry::deg2rad(10.0);
          const Vec3 radial_direction{
              std::sin(angle) * std::cos(in_plane_angle),
              std::sin(angle) * std::sin(in_plane_angle), std::cos(angle)};
          const Vec3 another_plane_position =
              position + radial_direction * (radius + 1.1 * contact_threshold);
          PlanePrimitive another_plane(2UL, 2UL, another_plane_position,
                                       -radial_direction, iron);
          TestMaxContacts::do_collide(first_sphere, another_plane, contacts);
          CHECK(contacts.empty());
        }
        SECTION("Finite plane") {
          // if 1 put 1.0 * contact threshold here it fails the test as it goes
          // just less than the contact threshold
          Vec3 plane_position =
              position - plane_normal * (radius + 1.1 * contact_threshold);
          PlanePrimitive second_plane(1UL, 1UL, true, plane_position,
                                      plane_normal, {2.0, 1.0}, iron);

          TestMaxContacts::do_collide(first_sphere, second_plane, contacts);
          CHECK(contacts.empty());

          const real_t angle = geometry::deg2rad(45.0);
          const real_t in_plane_angle = geometry::deg2rad(10.0);
          const Vec3 radial_direction{
              std::sin(angle) * std::cos(in_plane_angle),
              std::sin(angle) * std::sin(in_plane_angle), std::cos(angle)};
          const Vec3 another_plane_position =
              position + radial_direction * (radius + 1.1 * contact_threshold);
          PlanePrimitive another_plane(2UL, 2UL, true, another_plane_position,
                                       -radial_direction, {1.0, 1.0}, iron);
          TestMaxContacts::do_collide(first_sphere, another_plane, contacts);
          CHECK(contacts.empty());
        }
      }
    }
    SECTION("Bodies penetrate each other") {
      SECTION("Infinite plane") {
        // if 1 put 1.0 * contact threshold here it fails the test as it goes
        // just less than the contact threshold
        Vec3 plane_normal = {0.0, 0.0, 1.0};
        Vec3 plane_position = position + plane_normal * (0.6 * radius);
        PlanePrimitive second_plane(1UL, 1UL, plane_position, -plane_normal,
                                    iron);
        TestMaxContacts::do_collide(first_sphere, second_plane, contacts);

        CHECK_FALSE(contacts.empty());
        // negative distances are interpenetrating so stick with the sign
        // convention
        CHECK(contacts[0].get_first_body() == &first_sphere);
        CHECK(contacts[0].get_second_body() == &second_plane);
        CHECK(contacts[0].get_distance() == approx(-0.4 * radius));
        CHECK(contacts[0].get_position() ==
              position + (0.6 * radius) * plane_normal);
        CHECK(contacts[0].get_normal() == -plane_normal);

        clear(contacts);

        const real_t angle = geometry::deg2rad(45.0);
        const real_t in_plane_angle = geometry::deg2rad(10.0);
        const Vec3 radial_direction{std::sin(angle) * std::cos(in_plane_angle),
                                    std::sin(angle) * std::sin(in_plane_angle),
                                    std::cos(angle)};
        // can be something quite different
        plane_normal = {0.0, 1.0, 0.0};
        const Vec3 another_plane_position =
            position + radial_direction * (0.8 * radius);
        PlanePrimitive another_plane(2UL, 2UL, another_plane_position,
                                     plane_normal, iron);
        TestMaxContacts::do_collide(first_sphere, another_plane, contacts);

        CHECK_FALSE(contacts.empty());
        // negative distances are interpenetrating so stick with the sign
        // convention
        CHECK(contacts[0].get_first_body() == &first_sphere);
        CHECK(contacts[0].get_second_body() == &another_plane);
        const real_t projected((0.8 * radius) *
                               dot(radial_direction, plane_normal));
        CHECK(contacts[0].get_distance() == approx(-radius + projected));
        CHECK(contacts[0].get_position() ==
              position + projected * -plane_normal);
        CHECK(contacts[0].get_normal() == plane_normal);

        clear(contacts);
      }
      SECTION("Finite plane") {
        // if 1 put 1.0 * contact threshold here it fails the test as it goes
        // just less than the contact threshold
        Vec3 plane_normal = {0.0, 0.0, 1.0};
        // shift arbitrary in x direction
        Vec3 plane_position = position + plane_normal * (0.6 * radius) +
                              0.4 * radius * Vec3{1.0, 0.0, 0.0} +
                              0.2 * radius * Vec3{0.0, 1.0, 0.0};
        // does not matter
        PlanePrimitive second_plane(1UL, 1UL, true, plane_position,
                                    -plane_normal, {radius, radius}, iron);
        TestMaxContacts::do_collide(first_sphere, second_plane, contacts);

        CHECK_FALSE(contacts.empty());
        // negative distances are interpenetrating so stick with the sign
        // convention
        CHECK(contacts[0].get_first_body() == &first_sphere);
        CHECK(contacts[0].get_second_body() == &second_plane);
        CHECK(contacts[0].get_distance() == approx(-0.4 * radius));
        CHECK(contacts[0].get_position() ==
              position + (0.6 * radius) * plane_normal);
        CHECK(contacts[0].get_normal() == -plane_normal);

        clear(contacts);

        const real_t angle = geometry::deg2rad(45.0);
        const real_t in_plane_angle = geometry::deg2rad(10.0);
        const Vec3 radial_direction{std::sin(angle) * std::cos(in_plane_angle),
                                    std::sin(angle) * std::sin(in_plane_angle),
                                    std::cos(angle)};
        const Vec3 another_plane_position =
            position + radial_direction * (0.8 * radius);
        PlanePrimitive another_plane(2UL, 2UL, another_plane_position,
                                     plane_normal, iron);
        TestMaxContacts::do_collide(first_sphere, another_plane, contacts);

        CHECK_FALSE(contacts.empty());
        // negative distances are interpenetrating so stick with the sign
        // convention
        CHECK(contacts[0].get_first_body() == &first_sphere);
        CHECK(contacts[0].get_second_body() == &another_plane);
        const real_t projected(0.8 * radius *
                               dot(radial_direction, plane_normal));
        CHECK(contacts[0].get_distance() == approx(-(radius - projected)));
        CHECK(contacts[0].get_position() ==
              position + projected * -plane_normal);
        CHECK(contacts[0].get_normal() == plane_normal);

        clear(contacts);
      }
    }
    SECTION("Bodies touch each other") {
      Vec3 plane_normal = {0.0, 0.0, 1.0};

      SECTION("Infinite plane") {
        // if 1 put 1.0 * contact threshold here it fails the test as it goes
        // just less than the contact threshold
        Vec3 plane_position = position - plane_normal * (radius);
        PlanePrimitive second_plane(1UL, 1UL, plane_position, plane_normal,
                                    iron);
        TestMaxContacts::do_collide(first_sphere, second_plane, contacts);

        CHECK_FALSE(contacts.empty());
        // negative distances are interpenetrating so stick with the sign
        // convention
        CHECK(contacts[0].get_first_body() == &first_sphere);
        CHECK(contacts[0].get_second_body() == &second_plane);
        CHECK(contacts[0].get_distance() == approx(0.0));
        CHECK(contacts[0].get_position() == second_plane.get_position());
        CHECK(contacts[0].get_normal() == plane_normal);

        clear(contacts);

        const real_t angle = geometry::deg2rad(32.0);
        const real_t in_plane_angle = geometry::deg2rad(15.0);
        const Vec3 radial_direction{std::sin(angle) * std::cos(in_plane_angle),
                                    std::sin(angle) * std::sin(in_plane_angle),
                                    std::cos(angle)};
        const Vec3 another_plane_position =
            position + radial_direction * (radius);
        PlanePrimitive another_plane(2UL, 2UL, another_plane_position,
                                     -radial_direction, iron);
        TestMaxContacts::do_collide(first_sphere, another_plane, contacts);

        CHECK_FALSE(contacts.empty());
        // negative distances are interpenetrating so stick with the sign
        // convention
        CHECK(contacts[0].get_first_body() == &first_sphere);
        CHECK(contacts[0].get_second_body() == &another_plane);
        CHECK(contacts[0].get_distance() == approx(0.0));
        CHECK(contacts[0].get_position() == another_plane.get_position());
        CHECK(contacts[0].get_normal() == -radial_direction);

        clear(contacts);
      }
      SECTION("Finite plane") {
        // if 1 put 1.0 * contact threshold here it fails the test as it goes
        // just less than the contact threshold
        Vec3 plane_tangent{1.0, 1.0, 0.0};
        const real_t tangent_span = 20.0;
        Vec3 plane_position = position - plane_normal * (radius) +
                              plane_tangent * tangent_span * 0.4;
        PlanePrimitive second_plane(1UL, 1UL, true, plane_position,
                                    plane_normal, {tangent_span, tangent_span},
                                    iron);

        TestMaxContacts::do_collide(first_sphere, second_plane, contacts);

        CHECK_FALSE(contacts.empty());
        // negative distances are interpenetrating so stick with the sign
        // convention
        CHECK(contacts[0].get_first_body() == &first_sphere);
        CHECK(contacts[0].get_second_body() == &second_plane);
        CHECK(contacts[0].get_distance() == approx(0.0));
        CHECK(contacts[0].get_position() == position - plane_normal * radius);
        CHECK(contacts[0].get_normal() == plane_normal);

        clear(contacts);

        const real_t angle = geometry::deg2rad(45.0);
        const real_t in_plane_angle = geometry::deg2rad(10.0);
        const Vec3 radial_direction{std::sin(angle) * std::cos(in_plane_angle),
                                    std::sin(angle) * std::sin(in_plane_angle),
                                    std::cos(angle)};
        const Vec3 another_plane_position =
            position + radial_direction * radius;
        PlanePrimitive another_plane(2UL, 2UL, true, another_plane_position,
                                     -radial_direction, {1.0, 1.0}, iron);
        TestMaxContacts::do_collide(first_sphere, another_plane, contacts);

        CHECK_FALSE(contacts.empty());
        // negative distances are interpenetrating so stick with the sign
        // convention
        CHECK(contacts[0].get_first_body() == &first_sphere);
        CHECK(contacts[0].get_second_body() == &another_plane);
        CHECK(contacts[0].get_distance() == approx(0.0));
        CHECK(contacts[0].get_position() == another_plane_position);
        CHECK(contacts[0].get_normal() == -radial_direction);

        clear(contacts);
      }
    }
  }

  ELASTICA_TEST_CASE(
      "Unit.Environments.FineDetection.MaxContacts.collide_sphere_cylinder",
      "[Unit][Environments]") {
    EMPTY_TEST();

#if defined(TEST_GJK_EPA_ALGORITHM)
    CV contacts;
    REQUIRE(contacts.empty());

    Vec3 position{5.0, 3.0, 6.0};
    real_t radius(4.0);
    SpherePrimitive first_sphere(0UL, 0UL, true, position, radius, iron);

    Vec3 velocity{1.0, 0.0, 0.0};
    Vec3 omega{0.0, 0.0, 0.0};
    first_sphere.transfer(position, velocity, omega);

    const auto R1(make_orthogonal_bases_from_tangent(Vec3{1.0, 0.0, 0.0}));
    const auto R2(make_orthogonal_bases_from_tangent(Vec3{0.0, 1.0, 0.0}));
    const auto R3(make_orthogonal_bases_from_tangent(Vec3{0.0, 0.0, 1.0}));

    SECTION("Bodies penetrate each other") {
      const real_t cylinder_radius = radius;
      const real_t cylinder_length = 2.0 * radius;

      //      SECTION("Cylinder penetrates with its spherical region") {
      //        const Vec3 cylinder_position{position[0] + radius + 1.75 *
      //        radius,
      //                                    position[1], position[2]};
      //        CylinderPrimitive second_cylinder(1UL, 1UL, true,
      //        cylinder_position, R1,
      //                                        cylinder_radius,
      //                                        cylinder_length, iron);
      //        // always fill in before colliding because we need to make sure
      //        that
      //        // relative velocities are defined
      //        second_cylinder.transfer(second_cylinder.get_position(),
      //                                second_cylinder.get_rotation_matrix(),
      //                                velocity, omega);
      //
      //        TestMaxContacts::do_collide(first_sphere, second_cylinder,
      //        contacts);
      //
      //        CHECK_FALSE(contacts.empty());
      //        // negative distances are interpenetrating so stick with the
      //        sign
      //        // convention
      //        CHECK(contacts[0].get_first_body() == &first_sphere);
      //        CHECK(contacts[0].get_second_body() == &second_cylinder);
      //        CHECK(contacts[0].get_distance() == approx(-0.25 * radius));
      //        CHECK(contacts[0].get_position() ==
      //              position + Vec3{radius - 0.25 / 2 * radius, 0.0, 0.0});
      //        CHECK(contacts[0].get_normal() == Vec3{-1.0, 0.0, 0.0});
      //
      //        clear(contacts);
      //
      //        const real_t angle = geometry::deg2rad(30.0);
      //        const real_t in_plane_angle = geometry::deg2rad(80.0);
      //        const Vec3 cylinder_axis{std::sin(angle) *
      //        std::cos(in_plane_angle),
      //                                std::sin(angle) *
      //                                std::sin(in_plane_angle),
      //                                std::cos(angle)};
      //        const Rot3 tiltedR =
      //        make_orthogonal_bases_from_tangent(cylinder_axis); const Vec3
      //        pos1 = position + (radius + 1.75 * radius) * cylinder_axis;
      //
      //        second_cylinder.transfer(pos1, tiltedR, velocity, omega);
      //        TestMaxContacts::do_collide(first_sphere, second_cylinder,
      //        contacts);
      //
      //        CHECK_FALSE(contacts.empty());
      //        // negative distances are interpenetrating so stick with the
      //        sign
      //        // convention
      //        CHECK(contacts[0].get_first_body() == &first_sphere);
      //        CHECK(contacts[0].get_second_body() == &second_cylinder);
      //        CHECK(contacts[0].get_distance() == approx(-0.25 * radius));
      //        CHECK(contacts[0].get_position() ==
      //              position + (radius - 0.25 / 2 * radius) * cylinder_axis);
      //        CHECK(contacts[0].get_normal() == -cylinder_axis);
      //
      //        clear(contacts);
      //      }
      SECTION("Cylinder penetrates with its cylindrical region") {
        //        CylinderPrimitive first_cylinder(0UL, 0UL, true, position, R2,
        //        radius, 20 * cylinder_length, iron);
        //        first_cylinder.transfer(first_cylinder.get_position(), R2,
        //        velocity,
        //                                 omega);

        const Vec3 pos1{position[0] + radius + 0.75 * radius, position[1],
                        position[2]};
        CylinderPrimitive second_cylinder(
            1UL, 1UL, true, pos1, R3, cylinder_radius, cylinder_length, iron);
        second_cylinder.transfer(second_cylinder.get_position(), R3, velocity,
                                 omega);

        //        TestMaxContacts::do_collide(first_cylinder, second_cylinder,
        //        contacts);
        //
        //        CHECK_FALSE(contacts.empty());
        //        // negative distances are interpenetrating so stick with the
        //        sign
        //        // convention
        //        CHECK(contacts[0].get_first_body() == &first_cylinder);
        //        CHECK(contacts[0].get_second_body() == &second_cylinder);
        //        CHECK(contacts[0].get_distance() == approx(-0.25 * radius));
        //        CHECK(contacts[0].get_position() ==
        //              position + Vec3{radius - 0.25 / 2 * radius, 0.0, 0.0});
        //        CHECK(contacts[0].get_normal() == Vec3{-1.0, 0.0, 0.0});
        //
        //        clear(contacts);

        TestMaxContacts::do_collide(first_sphere, second_cylinder, contacts);

        CHECK_FALSE(contacts.empty());
        // negative distances are interpenetrating so stick with the
        // sign
        // convention
        CHECK(contacts[0].get_first_body() == &first_sphere);
        CHECK(contacts[0].get_second_body() == &second_cylinder);
        CHECK(contacts[0].get_distance() == approx(-0.25 * radius));
        CHECK(contacts[0].get_position() ==
              position + Vec3{radius - 0.25 / 2 * radius, 0.0, 0.0});
        CHECK(contacts[0].get_normal() == Vec3{-1.0, 0.0, 0.0});

        clear(contacts);

        //        const real_t angle = geometry::deg2rad(30.0);
        //        const real_t in_plane_angle = geometry::deg2rad(80.0);
        //        const Vec3 radial_direction{std::sin(angle) *
        //        std::cos(in_plane_angle),
        //                                    std::sin(angle) *
        //                                    std::sin(in_plane_angle),
        //                                    std::cos(angle)};
        //        // perp tp radial direction
        //        //        const Vec3 cylinder_axis = blaze::column<1UL>(
        //        //
        //        blaze::trans(make_orthogonal_bases_from_tangent(radial_direction)));
        //        const Rot3 tiltedR =
        //            make_orthogonal_bases_from_normal(radial_direction);
        //        const Vec3 pos2 = position + (radius + 0.6 * radius) *
        //        radial_direction;
        //
        //        second_cylinder.transfer(pos2, tiltedR, velocity, omega);
        //
        //        TestMaxContacts::do_collide(first_sphere, second_cylinder,
        //        contacts);
        //
        //        CHECK_FALSE(contacts.empty());
        //        // negative distances are interpenetrating so stick with the
        //        sign
        //        // convention
        //        CHECK(contacts[0].get_first_body() == &first_sphere);
        //        CHECK(contacts[0].get_second_body() == &second_cylinder);
        //        CHECK(contacts[0].get_distance() == approx(-0.4 * radius));
        //        CHECK(contacts[0].get_position() ==
        //              position + (radius - 0.4 / 2 * radius) *
        //              radial_direction);
        //        CHECK(contacts[0].get_normal() == -radial_direction);
        //
        //        clear(contacts);
      }
    }
#endif
  }

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
          TestMaxContacts::do_collide(first_capsule, second_plane, contacts);
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
          TestMaxContacts::do_collide(first_capsule, another_plane, contacts);
          CHECK(contacts.empty());
        }  // infinite

        SECTION("when plane is finite") {
          PlanePrimitive second_plane(1UL, 1UL, true, plane_position,
                                      plane_normal, {2.0, 1.0}, iron);

          TestMaxContacts::do_collide(first_capsule, second_plane, contacts);
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
          TestMaxContacts::do_collide(first_capsule, another_plane, contacts);
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
          TestMaxContacts::do_collide(first_capsule, second_plane, contacts);
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
          TestMaxContacts::do_collide(first_capsule, another_plane, contacts);
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

          TestMaxContacts::do_collide(first_capsule, second_plane, contacts);
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
          TestMaxContacts::do_collide(first_capsule, another_plane, contacts);
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
                    R * cpp17::as_const(first_capsule).get_rotation_matrix();

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
                  TestMaxContacts::do_collide(first_capsule, second_plane,
                                              contacts);

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
                      R * cpp17::as_const(first_capsule).get_rotation_matrix();

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

                      TestMaxContacts::do_collide(first_capsule, another_plane,
                                                  contacts);

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
                          cpp17::as_const(second_plane).get_rotation_matrix();
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

            TestMaxContacts::do_collide(first_capsule, second_plane, contacts);

            CHECK_FALSE(contacts.empty());
            REQUIRE(cpp17::size(contacts) == 2UL);

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
              cpp17::as_const(first_capsule).get_rotation_matrix();

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

  void test_capsule_capsule_penetration(real_t expected_penetration_depth) {
    CV contacts;
    REQUIRE(contacts.empty());
    auto for_each_capsule_primitive = [](auto f) {
      using CPs = tmpl::list<CapsulePrimitive, CapsuleFromRodPrimitive>;
      return tmpl::for_each<CPs>(f);
    };

    const real_t capsule_radius(4.0);
    const real_t capsule_length(2.0 * capsule_radius);
    // 2.0, 1.0, 0.0
    const Vec3 capsule_axis = normalize(Vec3{0.0, 0.0, 1.0});
    const Vec3 capsule_position{5.0, 3.0, 6.0};

    // to check for rod rod
    CapsulePrimitive first_capsule(0UL, 0UL, true, capsule_position,
                                   capsule_axis, capsule_radius, capsule_length,
                                   iron);

    Vec3 velocity{1.0, 0.0, 0.0};
    Vec3 omega{0.0, 0.0, 0.0};
    first_capsule.transfer(first_capsule.get_position(),
                           first_capsule.get_rotation_matrix(), velocity,
                           omega);

    const real_t second_capsule_radius = 2.0;
    const real_t second_capsule_length = 4.0 * second_capsule_radius;

    SECTION("Capsule interacts with spherical region") {
      SECTION("with another capsule's spherical region") {
        SECTION("Contact detected for regular capsules") {
          SECTION("Capsules are straight atop each other") {
            for_each_capsule_primitive([&](auto cp) {
              using CP = tmpl::type_from<decltype(cp)>;

              // straight on top of each other
              //              const real_t expected_penetration_depth =
              //                  0.8 * contact_threshold;
              const Vec3 second_capsule_position{
                  capsule_position +
                  capsule_axis *
                      (0.5 * capsule_length + capsule_radius +
                       0.5 * second_capsule_length + second_capsule_radius -
                       expected_penetration_depth)};

              CP second_capsule(1UL, 1UL, true, second_capsule_position,
                                first_capsule.get_rotation_matrix(),
                                second_capsule_radius, second_capsule_length,
                                iron);
              // always fill in before colliding because we need to make sure
              // that relative velocities are defined
              second_capsule.transfer_impl(second_capsule.get_position(),
                                           second_capsule.get_rotation_matrix(),
                                           velocity, omega);

              TestMaxContacts::do_collide(first_capsule, second_capsule,
                                          contacts);

              CHECK_FALSE(contacts.empty());
              // negative distances are interpenetrating so stick with the
              // sign convention
              CHECK(contacts[0].get_first_body() == &first_capsule);
              CHECK(contacts[0].get_second_body() == &second_capsule);
              CHECK(contacts[0].get_distance() ==
                    approx(-expected_penetration_depth));
              CHECK(contacts[0].get_position() ==
                    capsule_position +
                        capsule_axis * (0.5 * capsule_length + capsule_radius -
                                        0.5 * expected_penetration_depth));
              // from 2 to 1
              CHECK(contacts[0].get_normal() == -capsule_axis);

              clear(contacts);
            });
          }  // straight on top

          SECTION("Capsules are placed at an angle") {
            for_each_capsule_primitive([&](auto cp) {
              using CP = tmpl::type_from<decltype(cp)>;

              // at an angle

              // angle with axis
              const real_t angle = geometry::deg2rad(30.0);
              // this doesnt matter
              const real_t in_plane_angle = geometry::deg2rad(80.0);
              // should not overlap with the first capsule axis
              const Vec3 second_capsule_axis{
                  std::sin(angle) * std::cos(in_plane_angle),
                  std::sin(angle) * std::sin(in_plane_angle), std::cos(angle)};
              CHECK_FALSE(second_capsule_axis == capsule_axis);

              //                const real_t contact_angle =
              //                geometry::deg2rad(60.0); const Vec3
              //                contact_axis{
              //                    std::sin(contact_angle) *
              //                    std::cos(in_plane_angle),
              //                    std::sin(contact_angle) *
              //                    std::sin(in_plane_angle),
              //                    std::cos(contact_angle)};
              //              const real_t expected_penetration_depth =
              //                  0.8 * contact_threshold;

              const Vec3 anchor_point =
                  capsule_position + capsule_axis * (0.5 * capsule_length) +
                  (second_capsule_radius + capsule_radius -
                   expected_penetration_depth) *
                      second_capsule_axis;

              //                const Vec3 expected_contact_point{
              //                    capsule_position + capsule_axis * (0.5 *
              //                    capsule_length) + (capsule_radius - 0.5 *
              //                    expected_penetration_depth) *
              //                        contact_axis};

              const Vec3 second_capsule_position =
                  anchor_point +
                  (0.5 * second_capsule_length) * second_capsule_axis;

              CP second_capsule(1UL, 1UL, true, second_capsule_position,
                                second_capsule_axis, second_capsule_radius,
                                second_capsule_length, iron);

              second_capsule.transfer_impl(second_capsule.get_position(),
                                           second_capsule.get_rotation_matrix(),
                                           velocity, omega);
              TestMaxContacts::do_collide(first_capsule, second_capsule,
                                          contacts);

              CHECK_FALSE(contacts.empty());
              // negative distances are interpenetrating so stick with the
              // sign
              // convention
              CHECK(contacts[0].get_first_body() == &first_capsule);
              CHECK(contacts[0].get_second_body() == &second_capsule);
              CHECK(contacts[0].get_distance() ==
                    approx(-expected_penetration_depth));
              // CHECK(contacts[0].get_position() == expected_contact_point);
              CHECK(contacts[0].get_normal() == -second_capsule_axis);
              clear(contacts);
            });
          }  // not straight
        }
        SECTION("Contact when exactly overlapping") {
          // This need not be tested because this is a very extreme case that
          // it can't occur SECTION("Contact between non-rod capsules
          // detected") {}
          SECTION("Contact between rod capsules are not detected") {
            auto run_test_for = [&](real_t in_plane_angle) {
              using CP = CapsuleFromRodPrimitive;
              // to check for rod rod
              CP another_first_capsule(0UL, 0UL, true, capsule_position,
                                       capsule_axis, capsule_radius,
                                       capsule_length, iron);

              Vec3 velocity{1.0, 0.0, 0.0};
              Vec3 omega{0.0, 0.0, 0.0};
              another_first_capsule.transfer_impl(
                  another_first_capsule.get_position(),
                  another_first_capsule.get_rotation_matrix(), velocity, omega);

              const Vec3 anchor_point =
                  capsule_position + 0.5 * capsule_length * capsule_axis;

              // angle with axis
              const real_t angle = geometry::deg2rad(30.0);
              // this doesnt matter
              const real_t planar_angle = geometry::deg2rad(in_plane_angle);
              // should not overlap with the first capsule axis
              const Vec3 second_capsule_axis{
                  std::sin(angle) * std::cos(planar_angle),
                  std::sin(angle) * std::sin(planar_angle), std::cos(angle)};

              const Vec3 second_capsule_position =
                  anchor_point +
                  0.5 * second_capsule_length * second_capsule_axis;

              CP second_capsule(1UL, 1UL, true, second_capsule_position,
                                second_capsule_axis, second_capsule_radius,
                                second_capsule_length, iron);

              second_capsule.transfer_impl(second_capsule.get_position(),
                                           second_capsule.get_rotation_matrix(),
                                           velocity, omega);

              TestMaxContacts::do_collide(another_first_capsule, second_capsule,
                                          contacts);

              CHECK(contacts.empty());
            };
            for (auto inplane_angle : {15.0, 30.0, 45.0, 60.0, 75.0, 90.0}) {
              run_test_for(inplane_angle);
              run_test_for(-inplane_angle);
            }
          }
        }
      }  // another spherical

      SECTION("with another capsule's cylindrical region") {
        SECTION("when intersection is exactly perpendicular and symmetric") {
          for_each_capsule_primitive([&](auto cp) {
            using CP = tmpl::type_from<decltype(cp)>;

            // on top of each other
            // const real_t expected_penetration_depth = 0.25 *
            // capsule_radius;
            const Vec3 second_capsule_position{
                capsule_position +
                capsule_axis *
                    (0.5 * capsule_length + capsule_radius +
                     second_capsule_radius - expected_penetration_depth)};
            const Rot3 tiltedR =
                make_orthogonal_bases_from_normal(capsule_axis);

            CP second_capsule(1UL, 1UL, true, second_capsule_position, tiltedR,
                              second_capsule_radius, second_capsule_length,
                              iron);
            // always fill in before colliding because we need to make sure
            // that relative velocities are defined
            second_capsule.transfer_impl(second_capsule.get_position(),
                                         second_capsule.get_rotation_matrix(),
                                         velocity, omega);

            TestMaxContacts::do_collide(first_capsule, second_capsule,
                                        contacts);

            CHECK_FALSE(contacts.empty());
            // negative distances are interpenetrating so stick with the sign
            // convention
            CHECK(contacts[0].get_first_body() == &first_capsule);
            CHECK(contacts[0].get_second_body() == &second_capsule);
            CHECK(contacts[0].get_distance() ==
                  approx(-expected_penetration_depth));
            CHECK(contacts[0].get_position() ==
                  capsule_position +
                      capsule_axis * (0.5 * capsule_length + capsule_radius -
                                      0.5 * expected_penetration_depth));
            // from 2 to 1
            CHECK(contacts[0].get_normal() == -capsule_axis);

            clear(contacts);
          });
        }

        SECTION(
            "when intersection is exactly perpendicular, but on a rotated "
            "plane, and non symmetric") {
          for_each_capsule_primitive([&](auto cp) {
            using CP = tmpl::type_from<decltype(cp)>;

            auto run_for = [&](real_t angle, real_t displacement_along_axis) {
              // on top of each other
              //              const real_t expected_penetration_depth =
              //                  0.25 * capsule_radius;

              const Rot3 tiltedR =
                  geometry::rotate_via_axis_angle(
                      get_unit_vector_along(elastica::LagrangianFrame::Tangent),
                      angle, false) *
                  cpp17::as_const(first_capsule).get_rotation_matrix();

              // convert the binormal to the tangent of the second axis
              // (could have been normal as well)
              // mathematically, this is a permute operation or rotation with
              // of 90 along another axis
              const Vec3 second_capsule_axis{
                  tiltedR(cast_along(LagrangianFrame::Binormal),
                          cast_along(EulerianFrame::X)),
                  tiltedR(cast_along(LagrangianFrame::Binormal),
                          cast_along(EulerianFrame::Y)),
                  tiltedR(cast_along(LagrangianFrame::Binormal),
                          cast_along(EulerianFrame::Z))};

              const Vec3 expected_contact_point{
                  capsule_position +
                  capsule_axis * (0.5 * capsule_length + capsule_radius -
                                  0.5 * expected_penetration_depth)};
              const Vec3 second_capsule_position{
                  capsule_position +
                  capsule_axis *
                      (0.5 * capsule_length + capsule_radius +
                       second_capsule_radius - expected_penetration_depth) +
                  displacement_along_axis * second_capsule_axis};

              CP second_capsule(1UL, 1UL, true, second_capsule_position,
                                second_capsule_axis, second_capsule_radius,
                                second_capsule_length, iron);
              // always fill in before colliding because we need to make sure
              // that relative velocities are defined
              second_capsule.transfer_impl(second_capsule.get_position(),
                                           second_capsule.get_rotation_matrix(),
                                           velocity, omega);

              TestMaxContacts::do_collide(first_capsule, second_capsule,
                                          contacts);
              CAPTURE(angle);
              CAPTURE(displacement_along_axis);

              CHECK_FALSE(contacts.empty());
              // negative distances are interpenetrating so stick with the
              // sign convention
              CHECK(contacts[0].get_first_body() == &first_capsule);
              CHECK(contacts[0].get_second_body() == &second_capsule);
              CHECK(contacts[0].get_distance() ==
                    approx(-expected_penetration_depth));
              CHECK(contacts[0].get_position() == expected_contact_point);
              // from 2 to 1
              CHECK(contacts[0].get_normal() == -capsule_axis);

              clear(contacts);
            };

            for (auto angle : {0., 15., 30., 45., 60., 75.}) {
              for (auto disp : {0.2, 0.4}) {
                run_for(angle, disp * second_capsule_length);
                run_for(-angle, disp * second_capsule_length);
                run_for(angle, -disp * second_capsule_length);
                run_for(-angle, -disp * second_capsule_length);
              }
            }
          });
        }

        SECTION(
            "when intersection is not perpendicular, but inclined at an "
            "angle)") {
          for_each_capsule_primitive([&](auto cp) {
            using CP = tmpl::type_from<decltype(cp)>;

            auto run_test_for = [&](const real_t out_of_plane_angle) {
              const Rot3 tempR =
                  // tilt along normal...
                  geometry::rotate_via_axis_angle(
                      get_unit_vector_along(
                          ::elastica::LagrangianFrame::Normal),
                      out_of_plane_angle, false) *
                  cpp17::as_const(first_capsule.get_rotation_matrix());

              // permute the matrix so that
              // tangent -> normal
              // normal -> binormal
              // binormal -> tangent
              // -120 instead of 120 who knows why :/
              const Rot3 permutation_matrix = geometry::rotate_via_axis_angle(
                  normalize(Vec3{1.0, 1.0, 1.0}), -120, false);
              const Rot3 tiltedR = permutation_matrix * tempR;

              // ... and take normal component
              const Vec3 contact_normal_dir{
                  tiltedR(cast_along(LagrangianFrame::Normal),
                          cast_along(EulerianFrame::X)),
                  tiltedR(cast_along(LagrangianFrame::Normal),
                          cast_along(EulerianFrame::Y)),
                  tiltedR(cast_along(LagrangianFrame::Normal),
                          cast_along(EulerianFrame::Z))};

              //              const real_t expected_penetration_depth =
              //                  0.25 * capsule_radius;
              const Vec3 expected_contact_point{
                  capsule_position + capsule_axis * (0.5 * capsule_length) +
                  (capsule_radius - 0.5 * expected_penetration_depth) *
                      contact_normal_dir};
              const Vec3 second_capsule_position{
                  expected_contact_point +
                  (second_capsule_radius - 0.5 * expected_penetration_depth) *
                      contact_normal_dir};

              CP second_capsule(1UL, 1UL, true, second_capsule_position,
                                // This will reform the rotation matrix, but as
                                // long as the tangent is preserved we are good
                                tiltedR, second_capsule_radius,
                                second_capsule_length, iron);
              // always fill in before colliding because we need to make sure
              // that relative velocities are defined
              second_capsule.transfer_impl(second_capsule.get_position(),
                                           second_capsule.get_rotation_matrix(),
                                           velocity, omega);

              TestMaxContacts::do_collide(first_capsule, second_capsule,
                                          contacts);

              CAPTURE(out_of_plane_angle);
              CHECK_FALSE(contacts.empty());
              // negative distances are interpenetrating so stick with the
              // sign convention
              CHECK(contacts[0].get_first_body() == &first_capsule);
              CHECK(contacts[0].get_second_body() == &second_capsule);
              CHECK(contacts[0].get_distance() ==
                    approx(-expected_penetration_depth));
              CHECK(contacts[0].get_position() == expected_contact_point);
              // from 2 to 1
              CHECK(contacts[0].get_normal() == -contact_normal_dir);

              clear(contacts);
            };
            for (auto out_of_plane_angle : {15.0, 30.0, 45.0, 60.0, 75.0}) {
              run_test_for(out_of_plane_angle);
              run_test_for(-out_of_plane_angle);
            }
          });
        }

      }  // cylinder collision

    }  // spherical collision

    SECTION("Capsule interacts with its cylindrical region") {
      const real_t angle = geometry::deg2rad(15.0);
      const real_t in_plane_angle = geometry::deg2rad(30.0);
      const Vec3 perp_direction{std::sin(angle) * std::cos(in_plane_angle),
                                std::sin(angle) * std::sin(in_plane_angle),
                                std::cos(angle)};
      const Vec3 radial_direction = normalize(capsule_axis % perp_direction);

      SECTION("Perpendicular centerlines") {
        for_each_capsule_primitive([&](auto cp) {
          using CP = tmpl::type_from<decltype(cp)>;

          auto run_test_for = [&](real_t normal_angle) {
            // const real_t expected_penetration_depth = 0.25 *
            // capsule_radius;

            // define a radial direction perpendicular to the tangent of
            // the first capsule
            const Vec3 second_capsule_position =
                capsule_position + (capsule_radius + second_capsule_radius -
                                    expected_penetration_depth) *
                                       radial_direction;
            const Vec3 expected_contact_point =
                capsule_position +
                (capsule_radius - 0.5 * expected_penetration_depth) *
                    radial_direction;

            // make it parallel to the cylinder first
            const Rot3 tempR = make_orthogonal_bases_from_normal_and_tangent(
                radial_direction, capsule_axis);

            // and then rotate it by some degree about its normal direction
            const Rot3 second_capsule_R =
                ::elastica::geometry::rotate_via_axis_angle(
                    get_unit_vector_along(::elastica::LagrangianFrame::Normal),
                    normal_angle, false) *
                tempR;

            CP second_capsule(1UL, 1UL, true, second_capsule_position,
                              second_capsule_R, second_capsule_radius,
                              second_capsule_length, iron);
            second_capsule.transfer_impl(second_capsule.get_position(),
                                         second_capsule.get_rotation_matrix(),
                                         velocity, omega);

            CHECK(contacts.empty());
            TestMaxContacts::do_collide(first_capsule, second_capsule,
                                        contacts);

            CHECK_FALSE(contacts.empty());
            REQUIRE(cpp17::size(contacts) == 1UL);
            // negative distances are interpenetrating so stick with the
            // sign convention
            CHECK(contacts[0].get_first_body() == &first_capsule);
            CHECK(contacts[0].get_second_body() == &second_capsule);
            CHECK(contacts[0].get_distance() ==
                  approx(-expected_penetration_depth));
            CHECK(contacts[0].get_position() == expected_contact_point);
            // from 2 to 1
            CHECK(contacts[0].get_normal() == -radial_direction);

            clear(contacts);
          };

          for (auto normal_angle : {15.0, 30.0, 45.0, 60.0, 75.0, 90.0}) {
            run_test_for(normal_angle);
            run_test_for(-normal_angle);
          }
        });
      }  // perpendicular centerline

      SECTION("Parallel centerlines creates two contact points") {
        for_each_capsule_primitive([&](auto cp) {
          using CP = tmpl::type_from<decltype(cp)>;

          auto run_test_for = [&](real_t normal_angle) {
            // const real_t expected_penetration_depth = 0.25 *
            // capsule_radius;

            // define a radial direction perpendicular to the tangent of
            // the first capsule
            const Vec3 second_capsule_position =
                capsule_position + (capsule_radius + second_capsule_radius -
                                    expected_penetration_depth) *
                                       radial_direction;

            // make it parallel to the cylinder first
            const Rot3 tempR = make_orthogonal_bases_from_normal_and_tangent(
                radial_direction, capsule_axis);

            // and then rotate it by some degree about its normal direction
            // in this case it is 0.0
            const Rot3 second_capsule_R =
                ::elastica::geometry::rotate_via_axis_angle(
                    get_unit_vector_along(::elastica::LagrangianFrame::Normal),
                    normal_angle, false) *
                tempR;

            CP second_capsule(1UL, 1UL, true, second_capsule_position,
                              second_capsule_R, second_capsule_radius,
                              second_capsule_length, iron);
            second_capsule.transfer_impl(second_capsule.get_position(),
                                         second_capsule.get_rotation_matrix(),
                                         velocity, omega);

            CHECK(contacts.empty());
            TestMaxContacts::do_collide(first_capsule, second_capsule,
                                        contacts);

            CHECK_FALSE(contacts.empty());
            REQUIRE(cpp17::size(contacts) == 2UL);
            // negative distances are interpenetrating so stick with the
            // sign convention
            CHECK(contacts[0].get_first_body() == &first_capsule);
            CHECK(contacts[0].get_second_body() == &second_capsule);
            CHECK(contacts[0].get_distance() ==
                  approx(-expected_penetration_depth));
            // from 2 to 1
            CHECK(contacts[0].get_normal() == -radial_direction);

            CHECK(contacts[1].get_first_body() == &first_capsule);
            CHECK(contacts[1].get_second_body() == &second_capsule);
            CHECK(contacts[1].get_distance() ==
                  approx(-expected_penetration_depth));
            // from 2 to 1
            CHECK(contacts[1].get_normal() == -radial_direction);

            const Vec3 contact_anchor_point =
                capsule_position +
                (capsule_radius - 0.5 * expected_penetration_depth) *
                    radial_direction;

            const Vec3 expected_first_contact_point =
                contact_anchor_point -
                0.5 * std::min(capsule_length, second_capsule_length) *
                    capsule_axis;
            const Vec3 expected_second_contact_point =
                contact_anchor_point +
                0.5 * std::min(capsule_length, second_capsule_length) *
                    capsule_axis;

            auto comp = [](Vec3 const& contact, Vec3 const& expected_point) {
              // check with approx
              return elastica::length(contact) <
                     elastica::length(expected_point);
            };
            using Set = std::set<Vec3, decltype(comp)>;

            Set expected_contact_points{
                {expected_first_contact_point, expected_second_contact_point},
                comp};
            Set obtained_contact_points{
                {contacts[0].get_position(), contacts[1].get_position()}, comp};

            // this can be interchanged, we only need to check for both
            // so put them in a list
            std::vector<Vec3> difference;

            std::set_difference(
                expected_contact_points.begin(), expected_contact_points.end(),
                obtained_contact_points.begin(), obtained_contact_points.end(),
                std::back_inserter(difference), comp);

            CHECK(difference.empty());

            clear(contacts);
          };

          run_test_for(0.0);
        });
      }  // parallel centerline
    }    // cylinder test
  };

  ELASTICA_TEST_CASE(
      "Unit.Environments.FineDetection.MaxContacts.collide_capsule_capsule",
      "[Unit][Environments]") {
    CV contacts;
    REQUIRE(contacts.empty());
    auto for_each_capsule_primitive = [](auto f) {
      using CPs = tmpl::list<CapsulePrimitive, CapsuleFromRodPrimitive>;
      return tmpl::for_each<CPs>(f);
    };

    const real_t capsule_radius(4.0);
    const real_t capsule_length(2.0 * capsule_radius);
    // 2.0, 1.0, 0.0
    const Vec3 capsule_axis = normalize(Vec3{0.0, 0.0, 1.0});
    const Vec3 capsule_position{5.0, 3.0, 6.0};

    // to check for rod rod
    CapsulePrimitive first_capsule(0UL, 0UL, true, capsule_position,
                                   capsule_axis, capsule_radius, capsule_length,
                                   iron);

    Vec3 velocity{1.0, 0.0, 0.0};
    Vec3 omega{0.0, 0.0, 0.0};
    first_capsule.transfer(first_capsule.get_position(),
                           first_capsule.get_rotation_matrix(), velocity,
                           omega);

    // const auto R1(make_orthogonal_bases_from_tangent(Vec3{1.0, 0.0, 0.0}));
    // const auto R2(make_orthogonal_bases_from_tangent(Vec3{0.0, 1.0, 0.0}));
    // const auto R3(make_orthogonal_bases_from_tangent(Vec3{0.0, 0.0, 1.0}));

    SECTION("Bodies do not contact") {
      const real_t second_capsule_radius = 2.0;
      const real_t second_capsule_length = 4.0 * second_capsule_radius;
      SECTION("When placed far away") {
        const real_t angle = geometry::deg2rad(45.0);
        const real_t in_plane_angle = geometry::deg2rad(10.0);
        const Vec3 radial_direction{std::sin(angle) * std::cos(in_plane_angle),
                                    std::sin(angle) * std::sin(in_plane_angle),
                                    std::cos(angle)};

        SECTION("Radial centerline") {
          for_each_capsule_primitive([&](auto cp) {
            using CP = tmpl::type_from<decltype(cp)>;
            const Vec3 second_capsule_position =
                capsule_position +
                (capsule_radius +
                 10 * (second_capsule_radius + second_capsule_length)) *
                    radial_direction;
            const Vec3 second_capsule_axis = radial_direction;

            CP second_capsule(1UL, 1UL, true, second_capsule_position,
                              second_capsule_axis, second_capsule_radius,
                              second_capsule_length, iron);
            second_capsule.transfer_impl(second_capsule.get_position(),
                                         second_capsule.get_rotation_matrix(),
                                         velocity, omega);

            TestMaxContacts::do_collide(first_capsule, second_capsule,
                                        contacts);
            CHECK(contacts.empty());
          });
        }
        SECTION("Parallel centerlines") {
          for_each_capsule_primitive([&](auto cp) {
            using CP = tmpl::type_from<decltype(cp)>;
            const Vec3 second_capsule_position =
                capsule_position +
                (capsule_radius +
                 10 * (second_capsule_radius + second_capsule_length)) *
                    radial_direction;

            CP second_capsule(1UL, 1UL, true, second_capsule_position,
                              first_capsule.get_rotation_matrix(),
                              second_capsule_radius, second_capsule_length,
                              iron);
            second_capsule.transfer_impl(second_capsule.get_position(),
                                         second_capsule.get_rotation_matrix(),
                                         velocity, omega);

            TestMaxContacts::do_collide(first_capsule, second_capsule,
                                        contacts);
            CHECK(contacts.empty());
          });
        }
        SECTION("Perpendicular centerlines") {
          for_each_capsule_primitive([&](auto cp) {
            using CP = tmpl::type_from<decltype(cp)>;
            const Vec3 second_capsule_position =
                capsule_position +
                (capsule_radius +
                 10 * (second_capsule_radius + second_capsule_length)) *
                    radial_direction;
            const Rot3 second_capsule_R =
                make_orthogonal_bases_from_normal(radial_direction);

            CP second_capsule(1UL, 1UL, true, second_capsule_position,
                              second_capsule_R, second_capsule_radius,
                              second_capsule_length, iron);
            second_capsule.transfer_impl(second_capsule.get_position(),
                                         second_capsule.get_rotation_matrix(),
                                         velocity, omega);

            TestMaxContacts::do_collide(first_capsule, second_capsule,
                                        contacts);
            CHECK(contacts.empty());
          });
        }
      }
      SECTION("When placed close") {
        const real_t angle = geometry::deg2rad(15.0);
        const real_t in_plane_angle = geometry::deg2rad(30.0);
        const Vec3 perp_direction{std::sin(angle) * std::cos(in_plane_angle),
                                  std::sin(angle) * std::sin(in_plane_angle),
                                  std::cos(angle)};
        const Vec3 radial_direction = normalize(capsule_axis % perp_direction);

        SECTION("Radial centerline") {
          for_each_capsule_primitive([&](auto cp) {
            using CP = tmpl::type_from<decltype(cp)>;
            const Vec3 second_capsule_position =
                capsule_position +
                (1.1 * contact_threshold + capsule_radius +
                 1.0 * (second_capsule_radius + 0.5 * second_capsule_length)) *
                    radial_direction;
            const Vec3 second_capsule_axis = radial_direction;

            CP second_capsule(1UL, 1UL, true, second_capsule_position,
                              second_capsule_axis, second_capsule_radius,
                              second_capsule_length, iron);
            second_capsule.transfer_impl(second_capsule.get_position(),
                                         second_capsule.get_rotation_matrix(),
                                         velocity, omega);

            TestMaxContacts::do_collide(first_capsule, second_capsule,
                                        contacts);
            CHECK(contacts.empty());
          });
        }
        SECTION("Parallel centerlines") {
          for_each_capsule_primitive([&](auto cp) {
            using CP = tmpl::type_from<decltype(cp)>;
            const Vec3 second_capsule_position =
                capsule_position + (capsule_radius + 1.1 * contact_threshold +
                                    1.0 * second_capsule_radius) *
                                       radial_direction;

            CP second_capsule(1UL, 1UL, true, second_capsule_position,
                              first_capsule.get_rotation_matrix(),
                              second_capsule_radius, second_capsule_length,
                              iron);
            second_capsule.transfer_impl(second_capsule.get_position(),
                                         second_capsule.get_rotation_matrix(),
                                         velocity, omega);

            TestMaxContacts::do_collide(first_capsule, second_capsule,
                                        contacts);
            CHECK(contacts.empty());
          });

          // along axis

          for_each_capsule_primitive([&](auto cp) {
            using CP = tmpl::type_from<decltype(cp)>;
            const Vec3 second_capsule_position =
                capsule_position +
                (capsule_radius + 0.5 * capsule_length + second_capsule_radius +
                 0.5 * second_capsule_length + 1.1 * contact_threshold) *
                    capsule_axis;

            CP second_capsule(1UL, 1UL, true, second_capsule_position,
                              first_capsule.get_rotation_matrix(),
                              second_capsule_radius, second_capsule_length,
                              iron);
            second_capsule.transfer_impl(second_capsule.get_position(),
                                         second_capsule.get_rotation_matrix(),
                                         velocity, omega);

            TestMaxContacts::do_collide(first_capsule, second_capsule,
                                        contacts);
            CHECK(contacts.empty());
          });
        }
        SECTION("Perpendicular centerlines") {
          for_each_capsule_primitive([&](auto cp) {
            using CP = tmpl::type_from<decltype(cp)>;
            const Vec3 second_capsule_position =
                capsule_position + (capsule_radius + 1.1 * contact_threshold +
                                    second_capsule_radius) *
                                       radial_direction;
            const Rot3 second_capsule_R =
                make_orthogonal_bases_from_normal(radial_direction);

            CP second_capsule(1UL, 1UL, true, second_capsule_position,
                              second_capsule_R, second_capsule_radius,
                              second_capsule_length, iron);
            second_capsule.transfer_impl(second_capsule.get_position(),
                                         second_capsule.get_rotation_matrix(),
                                         velocity, omega);

            TestMaxContacts::do_collide(first_capsule, second_capsule,
                                        contacts);
            CHECK(contacts.empty());
          });
        }
      }
    }
    SECTION("Bodies penetrate each other") {
      test_capsule_capsule_penetration(0.25 * capsule_radius);
    }
    SECTION("Bodies touch each other") {
      SECTION("Contact is recognized at outer threshold") {
        // 1.0 fails
        test_capsule_capsule_penetration(-0.9 * contact_threshold);
      }
      SECTION("Exactly zero") { test_capsule_capsule_penetration(0.0); }
      // test_capsule_capsule_penetration(-contact_threshold);
    }
  }

}  // namespace

// SECTION("Bodies do not contact") {
// SECTION("When placed far away") {}
// SECTION("When placed close") {}
// }
// SECTION("Bodies penetrate each other") {}
// SECTION("Bodies touch each other") {}
