//
// Created by Tejaswin Parthasarathy on 8/8/22.
//
#include <cmath>

#include "Framework/TestingFramework.hpp"
//
#include "Simulator/Environments/CollisionSystem/Detection/Fine/Kernels/SpherePlane.hpp"
//
#include "Simulator/Environments/CollisionSystem/Contact/Contact.hpp"
#include "Simulator/Environments/CollisionSystem/Contact/ContactVectorAdapter.hpp"
#include "Simulator/Materials.hpp"
#include "Utilities/Geometry/Angles.hpp"
#include "Utilities/Math/Vec3.hpp"

namespace {

  using namespace ::elastica;
  using namespace ::elastica::collision;
  using ::elastica::collision::detection::fine::do_collide;

  using CV = ContactVectorAdapter<Contact>;

  void clear(CV& contacts_vector) { contacts_vector.clear(); }

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
          do_collide(first_sphere, second_plane, contacts);
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
          do_collide(first_sphere, another_plane, contacts);
          CHECK(contacts.empty());
        }

        SECTION("Finite plane") {
          PlanePrimitive second_plane(1UL, 1UL, true, plane_position,
                                      plane_normal, {2.0, 1.0}, iron);

          do_collide(first_sphere, second_plane, contacts);
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
          do_collide(first_sphere, another_plane, contacts);
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
          do_collide(first_sphere, second_plane, contacts);
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
          do_collide(first_sphere, another_plane, contacts);
          CHECK(contacts.empty());
        }
        SECTION("Finite plane") {
          // if 1 put 1.0 * contact threshold here it fails the test as it goes
          // just less than the contact threshold
          Vec3 plane_position =
              position - plane_normal * (radius + 1.1 * contact_threshold);
          PlanePrimitive second_plane(1UL, 1UL, true, plane_position,
                                      plane_normal, {2.0, 1.0}, iron);

          do_collide(first_sphere, second_plane, contacts);
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
          do_collide(first_sphere, another_plane, contacts);
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
        do_collide(first_sphere, second_plane, contacts);

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
        do_collide(first_sphere, another_plane, contacts);

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
        do_collide(first_sphere, second_plane, contacts);

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
        do_collide(first_sphere, another_plane, contacts);

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
        do_collide(first_sphere, second_plane, contacts);

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
        do_collide(first_sphere, another_plane, contacts);

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

        do_collide(first_sphere, second_plane, contacts);

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
        do_collide(first_sphere, another_plane, contacts);

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
}  // namespace
