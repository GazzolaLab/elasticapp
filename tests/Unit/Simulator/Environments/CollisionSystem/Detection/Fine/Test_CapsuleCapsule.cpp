//
// Created by Tejaswin Parthasarathy on 8/8/22.
//
#include <cmath>

#include "Framework/TestingFramework.hpp"
//
#include "Simulator/Environments/CollisionSystem/Detection/Fine/Kernels/CapsuleCapsule.hpp"
#include "Simulator/Environments/CollisionSystem/Detection/Fine/Kernels/RodCapsuleRodCapsule.hpp"
//
#include "Simulator/Environments/CollisionSystem/Contact/Contact.hpp"
#include "Simulator/Environments/CollisionSystem/Contact/ContactVectorAdapter.hpp"
#include "Simulator/Frames.hpp"
#include "Simulator/Materials.hpp"
#include "Utilities/AsConst.hpp"
#include "Utilities/Geometry/Angles.hpp"
#include "Utilities/Geometry/AxisAngleRotation.hpp"
#include "Utilities/Math/Length.hpp"
#include "Utilities/Math/Orthogonal.hpp"
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/Size.hpp"
#include "Utilities/TMPL.hpp"

namespace {

  using namespace ::elastica;
  using namespace ::elastica::collision;
  using ::elastica::collision::detection::fine::do_collide;

  using CV = ContactVectorAdapter<Contact>;

  void clear(CV& contacts_vector) { contacts_vector.clear(); }

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

              do_collide(first_capsule, second_capsule, contacts);

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
              do_collide(first_capsule, second_capsule, contacts);

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

              do_collide(another_first_capsule, second_capsule, contacts);

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

            do_collide(first_capsule, second_capsule, contacts);

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

              do_collide(first_capsule, second_capsule, contacts);
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

              do_collide(first_capsule, second_capsule, contacts);

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
            do_collide(first_capsule, second_capsule, contacts);

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
            do_collide(first_capsule, second_capsule, contacts);

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
  }

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

            do_collide(first_capsule, second_capsule, contacts);
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

            do_collide(first_capsule, second_capsule, contacts);
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

            do_collide(first_capsule, second_capsule, contacts);
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

            do_collide(first_capsule, second_capsule, contacts);
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

            do_collide(first_capsule, second_capsule, contacts);
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

            do_collide(first_capsule, second_capsule, contacts);
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

            do_collide(first_capsule, second_capsule, contacts);
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
