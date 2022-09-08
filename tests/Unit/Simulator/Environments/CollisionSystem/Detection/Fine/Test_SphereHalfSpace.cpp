//
// Created by Tejaswin Parthasarathy on 8/8/22.
//
#include <cmath>

#include "Framework/TestingFramework.hpp"
//
#include "Simulator/Environments/CollisionSystem/Detection/Fine/Kernels/SphereHalfSpace.hpp"
//
#include "Simulator/Environments/CollisionSystem/Contact/Contact.hpp"
#include "Simulator/Environments/CollisionSystem/Contact/ContactVectorAdapter.hpp"
#include "Simulator/Materials.hpp"
#include "Utilities/Geometry/Angles.hpp"
#include "Utilities/Math/Vec3.hpp"

namespace {
  using App = Approx;

  using namespace ::elastica;
  using namespace ::elastica::collision;
  using ::elastica::collision::detection::fine::do_collide;

  using CV = ContactVectorAdapter<Contact>;

  void clear(CV& contacts_vector) { contacts_vector.clear(); }

  template <typename T>
  int sgn(T val) {
    return (T(0) < val) - (val < T(0));
  }

  ELASTICA_TEST_CASE(
      "Unit.Environments.FineDetection.MaxContacts.collide_sphere_halfspace",
      "[Unit][Environments]") {
    CV contacts;
    REQUIRE(contacts.empty());

    Vec3 position{5.0, 3.0, 6.0};
    real_t radius(4.0);
    SpherePrimitive first_sphere(0UL, 0UL, true, position, radius, iron);

    Vec3 velocity{1.0, 0.0, 0.0};
    Vec3 omega{0.0, 0.0, 0.0};
    first_sphere.transfer(position, velocity, omega);

    // plane is infinite, so only guarantee precision till contact_threshold
    auto contact_approx = App::custom().margin(0.1 * contact_threshold);

    SECTION("Bodies do not contact") {
      SECTION("When placed far away") {
        Vec3 half_space_position = {0.0, 0.0, 0.0};
        Vec3 half_space_normal = {0.0, 0.0, 1.0};

        HalfSpacePrimitive second_half_space(1UL, 1UL, half_space_position,
                                             half_space_normal, iron);
        do_collide(first_sphere, second_half_space, contacts);
        CHECK(contacts.empty());

        const real_t angle = geometry::deg2rad(45.0);
        const real_t in_half_space_angle = geometry::deg2rad(10.0);
        const Vec3 radial_direction{
            std::sin(angle) * std::cos(in_half_space_angle),
            std::sin(angle) * std::sin(in_half_space_angle), std::cos(angle)};
        const Vec3 another_half_space_position =
            position + radial_direction * 2 * radius;
        HalfSpacePrimitive another_half_space(
            2UL, 2UL, another_half_space_position, -radial_direction, iron);
        do_collide(first_sphere, another_half_space, contacts);
        CHECK(contacts.empty());
      }
      SECTION("When placed close") {
        Vec3 half_space_normal = {0.0, 0.0, 1.0};

        // if 1 put 1.0 * contact threshold here it fails the test as it goes
        // just less than the contact threshold
        Vec3 half_space_position =
            position - half_space_normal * (radius + 1.1 * contact_threshold);
        HalfSpacePrimitive second_half_space(1UL, 1UL, half_space_position,
                                             half_space_normal, iron);
        do_collide(first_sphere, second_half_space, contacts);
        CHECK(contacts.empty());

        const real_t angle = geometry::deg2rad(45.0);
        const real_t in_half_space_angle = geometry::deg2rad(10.0);
        const Vec3 radial_direction{
            std::sin(angle) * std::cos(in_half_space_angle),
            std::sin(angle) * std::sin(in_half_space_angle), std::cos(angle)};
        const Vec3 another_half_space_position =
            position + radial_direction * (radius + 1.1 * contact_threshold);
        HalfSpacePrimitive another_half_space(
            2UL, 2UL, another_half_space_position, -radial_direction, iron);
        do_collide(first_sphere, another_half_space, contacts);
        CHECK(contacts.empty());
      }
    }  // no contact
    SECTION("Bodies penetrate each other") {
      SECTION("Sphere COM outside half-plane") {
        // if 1 put 1.0 * contact threshold here it fails the test as it goes
        // just less than the contact threshold
        Vec3 half_space_normal = {0.0, 0.0, 1.0};
        Vec3 half_space_position =
            position + half_space_normal * (0.6 * radius);
        HalfSpacePrimitive second_half_space(1UL, 1UL, half_space_position,
                                             -half_space_normal, iron);
        do_collide(first_sphere, second_half_space, contacts);

        REQUIRE_FALSE(contacts.empty());

        // negative distances are interpenetrating so stick with the sign
        // convention
        CHECK(contacts[0].get_first_body() == &first_sphere);
        CHECK(contacts[0].get_second_body() == &second_half_space);
        CHECK(contacts[0].get_distance() ==
              contact_approx(-0.4 * radius).epsilon(contact_threshold));
        CHECK(contacts[0].get_position() ==
              position + (0.8 * radius) * half_space_normal);
        CHECK(contacts[0].get_normal() == -half_space_normal);

        clear(contacts);

        const real_t angle = geometry::deg2rad(45.0);
        const real_t in_plane_angle = geometry::deg2rad(10.0);
        const Vec3 radial_direction{std::sin(angle) * std::cos(in_plane_angle),
                                    std::sin(angle) * std::sin(in_plane_angle),
                                    std::cos(angle)};
        // can be something quite different
        half_space_normal = {0.0, 1.0, 0.0};
        const Vec3 another_half_space_position =
            position + radial_direction * (0.8 * radius);
        const real_t projected((0.8 * radius) *
                               dot(radial_direction, half_space_normal));

        auto constexpr is_outside = true;
        // Change sign so that sphere lies on the outside of half-space
        half_space_normal *= (is_outside ? -1 : 1) *
                             sgn(dot(radial_direction, half_space_normal));

        HalfSpacePrimitive another_half_space(
            2UL, 2UL, another_half_space_position, half_space_normal, iron);
        do_collide(first_sphere, another_half_space, contacts);

        REQUIRE_FALSE(contacts.empty());
        // negative distances are interpenetrating so stick with the sign
        // convention
        CHECK(contacts[0].get_first_body() == &first_sphere);
        CHECK(contacts[0].get_second_body() == &another_half_space);
        CHECK(contacts[0].get_distance() ==
              contact_approx(-radius + projected));
        CHECK(contacts[0].get_position() ==
              position + 0.5 * (projected + radius) * -half_space_normal);
        CHECK(contacts[0].get_normal() == half_space_normal);

        clear(contacts);
      }  // sphere COM outside
      SECTION("Sphere COM inside half-plane") {
        // if 1 put 1.0 * contact threshold here it fails the test as it goes
        // just less than the contact threshold

        SECTION("Partially immersed") {
          Vec3 half_space_normal = {0.0, 0.0, 1.0};
          Vec3 half_space_position =
              position + half_space_normal * (0.6 * radius);
          HalfSpacePrimitive second_half_space(1UL, 1UL, half_space_position,
                                               half_space_normal, iron);
          do_collide(first_sphere, second_half_space, contacts);

          REQUIRE_FALSE(contacts.empty());

          // negative distances are interpenetrating so stick with the sign
          // convention
          CHECK(contacts[0].get_first_body() == &first_sphere);
          CHECK(contacts[0].get_second_body() == &second_half_space);
          CHECK(contacts[0].get_distance() ==
                contact_approx(-1.6 * radius).epsilon(contact_threshold));
          CHECK(contacts[0].get_position() ==
                position + (0.2 * radius * -half_space_normal));
          CHECK(contacts[0].get_normal() == half_space_normal);

          clear(contacts);

          const real_t angle = geometry::deg2rad(45.0);
          const real_t in_plane_angle = geometry::deg2rad(10.0);
          const Vec3 radial_direction{
              std::sin(angle) * std::cos(in_plane_angle),
              std::sin(angle) * std::sin(in_plane_angle), std::cos(angle)};
          // can be something quite different
          half_space_normal = {0.0, 1.0, 0.0};
          const Vec3 another_half_space_position =
              position + radial_direction * (0.8 * radius);
          const real_t projected((0.8 * radius) *
                                 dot(radial_direction, half_space_normal));

          auto constexpr is_outside = false;
          // Change sign so that sphere lies on the inside of half-space
          half_space_normal *= (is_outside ? -1 : 1) *
                               sgn(dot(radial_direction, half_space_normal));
          HalfSpacePrimitive another_half_space(
              2UL, 2UL, another_half_space_position, half_space_normal, iron);
          do_collide(first_sphere, another_half_space, contacts);

          CHECK_FALSE(contacts.empty());
          // negative distances are interpenetrating so stick with the sign
          // convention
          CHECK(contacts[0].get_first_body() == &first_sphere);
          CHECK(contacts[0].get_second_body() == &another_half_space);
          CHECK(contacts[0].get_distance() ==
                contact_approx(-radius - projected));
          CHECK(contacts[0].get_position() ==
                (position + 0.5 * (radius - projected) * -half_space_normal));
          CHECK(contacts[0].get_normal() == half_space_normal);

          clear(contacts);
        }  // partially immersed

        SECTION("Fully immersed") {
          Vec3 half_space_normal = {0.0, 0.0, 1.0};
          Vec3 half_space_position =
              position + half_space_normal * (1.6 * radius);
          HalfSpacePrimitive second_half_space(1UL, 1UL, half_space_position,
                                               half_space_normal, iron);
          do_collide(first_sphere, second_half_space, contacts);

          REQUIRE_FALSE(contacts.empty());

          // negative distances are interpenetrating so stick with the sign
          // convention
          CHECK(contacts[0].get_first_body() == &first_sphere);
          CHECK(contacts[0].get_second_body() == &second_half_space);
          CHECK(contacts[0].get_distance() ==
                contact_approx(-2.6 * radius).epsilon(contact_threshold));
          CHECK(contacts[0].get_position() == position);
          CHECK(contacts[0].get_normal() == half_space_normal);

          clear(contacts);

          const real_t angle = geometry::deg2rad(45.0);
          const real_t in_plane_angle = geometry::deg2rad(10.0);
          const Vec3 radial_direction{
              std::sin(angle) * std::cos(in_plane_angle),
              std::sin(angle) * std::sin(in_plane_angle), std::cos(angle)};
          // can be something quite different
          half_space_normal = {0.0, 1.0, 0.0};
          const Vec3 another_half_space_position =
              position + radial_direction * (10.0 * radius);
          // 10.0 is roughly 1/in_plane_angle
          const real_t projected((10.0 * radius) *
                                 dot(radial_direction, half_space_normal));

          auto constexpr is_outside = false;
          // Change sign so that sphere lies on the inside of half-space
          half_space_normal *= (is_outside ? -1 : 1) *
                               sgn(dot(radial_direction, half_space_normal));
          HalfSpacePrimitive another_half_space(
              2UL, 2UL, another_half_space_position, half_space_normal, iron);
          do_collide(first_sphere, another_half_space, contacts);

          CHECK_FALSE(contacts.empty());
          // negative distances are interpenetrating so stick with the sign
          // convention
          CHECK(contacts[0].get_first_body() == &first_sphere);
          CHECK(contacts[0].get_second_body() == &another_half_space);
          CHECK(contacts[0].get_distance() ==
                contact_approx(-radius - projected));
          CHECK(contacts[0].get_position() == position);
          CHECK(contacts[0].get_normal() == half_space_normal);

          clear(contacts);
        }  // fully immersed
      }    // sphere COM inside
    }
    SECTION("Bodies touch each other") {
      Vec3 plane_normal = {0.0, 0.0, 1.0};

      // if 1 put 1.0 * contact threshold here it fails the test as it
      // goes just less than the contact threshold
      Vec3 plane_position = position - plane_normal * (radius);
      HalfSpacePrimitive second_halfspace(1UL, 1UL, plane_position,
                                          plane_normal, iron);
      do_collide(first_sphere, second_halfspace, contacts);

      CHECK_FALSE(contacts.empty());
      // negative distances are interpenetrating so stick with the sign
      // convention
      CHECK(contacts[0].get_first_body() == &first_sphere);
      CHECK(contacts[0].get_second_body() == &second_halfspace);
      CHECK(contacts[0].get_distance() == contact_approx(0.0));
      CHECK(contacts[0].get_position() == plane_position);
      CHECK(contacts[0].get_normal() == plane_normal);

      clear(contacts);

      const real_t angle = geometry::deg2rad(32.0);
      const real_t in_plane_angle = geometry::deg2rad(15.0);
      const Vec3 radial_direction{std::sin(angle) * std::cos(in_plane_angle),
                                  std::sin(angle) * std::sin(in_plane_angle),
                                  std::cos(angle)};
      const Vec3 another_halfspace_position =
          position + radial_direction * (radius);
      HalfSpacePrimitive another_halfspace(2UL, 2UL, another_halfspace_position,
                                           -radial_direction, iron);
      do_collide(first_sphere, another_halfspace, contacts);

      CHECK_FALSE(contacts.empty());
      // negative distances are interpenetrating so stick with the sign
      // convention
      CHECK(contacts[0].get_first_body() == &first_sphere);
      CHECK(contacts[0].get_second_body() == &another_halfspace);
      CHECK(contacts[0].get_distance() == contact_approx(0.0));
      CHECK(contacts[0].get_position() == another_halfspace_position);
      CHECK(contacts[0].get_normal() == -radial_direction);

      clear(contacts);
    }
  }

}  // namespace
