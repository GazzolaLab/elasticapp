//
// Created by Tejaswin Parthasarathy on 8/8/22.
//
#include <algorithm>
#include <cmath>
#include <memory>

#include "Framework/TestingFramework.hpp"
//
#include "Simulator/Environments/CollisionSystem/Detection/Fine/Kernels/ConvexConvex.hpp"
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

  //  #define TEST_GJK_EPA_ALGORITHM

#if defined(TEST_GJK_EPA_ALGORITHM)
  using namespace ::elastica;
  using namespace ::elastica::collision;
  using ::elastica::collision::detection::fine::do_collide;

  using CV = ContactVectorAdapter<Contact>;

  void clear(CV& contacts_vector) { contacts_vector.clear(); }
#endif

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
      //        do_collide(first_sphere, second_cylinder,
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
      //        do_collide(first_sphere, second_cylinder,
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

        //        do_collide(first_cylinder, second_cylinder,
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

        do_collide(first_sphere, second_cylinder, contacts);

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
        //        do_collide(first_sphere, second_cylinder,
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

}  // namespace
