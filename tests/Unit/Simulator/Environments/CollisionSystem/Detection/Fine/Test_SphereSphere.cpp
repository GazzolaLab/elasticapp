//
// Created by Tejaswin Parthasarathy on 8/8/22.
//
#include <algorithm>
#include <cmath>
#include <memory>

#include "Framework/TestingFramework.hpp"
//
#include "Simulator/Environments/CollisionSystem/Detection/Fine/Kernels/SphereSphere.hpp"
//
#include "Simulator/Environments/CollisionSystem/Contact/Contact.hpp"
#include "Simulator/Environments/CollisionSystem/Contact/ContactVectorAdapter.hpp"
#include "Simulator/Frames.hpp"
#include "Simulator/Materials.hpp"
#include "Utilities/Geometry/Angles.hpp"
#include "Utilities/Math/Length.hpp"
#include "Utilities/Math/Orthogonal.hpp"
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/TMPL.hpp"

namespace {

  using namespace ::elastica;
  using namespace ::elastica::collision;
  using ::elastica::collision::detection::fine::do_collide;

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
        do_collide(first_sphere, second_sphere, contacts);
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
        do_collide(first_sphere, second_sphere, contacts);
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

        do_collide(first_sphere, second_sphere, contacts);

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

        do_collide(first_sphere, second_sphere, contacts);

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

        do_collide(first_sphere, second_sphere, contacts);

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

        do_collide(first_sphere, second_sphere, contacts);

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

        do_collide(first_sphere, second_sphere, contacts);

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

}  // namespace