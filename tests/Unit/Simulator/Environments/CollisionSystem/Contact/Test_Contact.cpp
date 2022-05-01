//
// Created by Tejaswin Parthasarathy on 3/5/22.
//

#include "Framework/TestingFramework.hpp"
#include "Simulator/Environments/CollisionSystem/Contact/Contact.hpp"
#include "Simulator/Environments/CollisionSystem/Contact/ContactType.hpp"
#include "Simulator/Environments/Primitives.hpp"
#include "Simulator/Materials.hpp"
#include "Utilities/AsConst.hpp"

namespace {

  using namespace elastica::collision;
  using elastica::Vec3;

#define STAMP_PRIMITIVES                                               \
  using Prim = SpherePrimitive;                                        \
  Vec3 first_position{0., 0., 0.};                                     \
  Prim first(0UL, 0UL, true, first_position, 1.0, elastica::iron);     \
                                                                       \
  Vec3 velocity{0., 0., 0.};                                           \
  Vec3 omega{0., 0., 0.};                                              \
  first.transfer(first_position, velocity, omega);                     \
                                                                       \
  Vec3 second_position{0., 0., 1.};                                    \
  Prim second(0UL, 1UL, true, second_position, 1.0, elastica::copper); \
  second.transfer(second_position, velocity, omega)

  void test_is_penetrating() {
    STAMP_PRIMITIVES;

    Contact cb(&first, &second, Vec3{0., 0., 0.5}, Vec3{0., 0., 1.}, 0.5);
    CHECK_FALSE(cpp17::as_const(cb).is_penetrating());

    Contact other_cb(&first, &second, Vec3{0., 0., 0.5}, Vec3{0., 0., 1.},
                     -0.2);
    CHECK(cpp17::as_const(other_cb).is_penetrating());
  }

  void test_is_contacting() {
    STAMP_PRIMITIVES;

    Vec3 third_position{0., 0., 0.6};
    SpherePrimitive third(0UL, 2UL, true, third_position, 1.0, elastica::fir);
    third.transfer(third_position, velocity, omega);

    Contact cb(&first, &second, Vec3{0., 0., 0.5}, Vec3{0., 0., 1.}, 0.5);
    REQUIRE(cpp17::as_const(cb).is_contacting(&first));
    REQUIRE(cpp17::as_const(cb).is_contacting(&second));
    REQUIRE_FALSE(cpp17::as_const(cb).is_contacting(&third));
  }

  void test_getters() {
    STAMP_PRIMITIVES;

    Contact cb(&first, &second, Vec3{0., 0., 0.5}, Vec3{0., 0., 1.}, 0.5);
    // Because of static tracking
    // Will this be a problem in multithreaded application?
    // DONE : removed
    // CHECK(cpp17::as_const(cb).get_ID() == 5);

    CHECK(cpp17::as_const(cb).get_index() == 0);
    CHECK(cpp17::as_const(cb).get_first_body() == &first);
    CHECK(cpp17::as_const(cb).get_second_body() == &second);
    CHECK(cpp17::as_const(cb).get_position() == Vec3{0., 0., 0.5});
    CHECK(cpp17::as_const(cb).get_normal() == Vec3{0., 0., 1.0});
    CHECK(cpp17::as_const(cb).get_distance() == approx(0.5));
    // how to test?
    CHECK(cpp17::as_const(cb).get_restitution() >= 0.0);
    CHECK(cpp17::as_const(cb).get_stiffness() > 0.0);
    CHECK(cpp17::as_const(cb).get_normal_damping() > 0.0);
    CHECK(cpp17::as_const(cb).get_tangential_damping() > 0.0);
    CHECK(cpp17::as_const(cb).get_friction() > 0.0);
  }

  void test_compute_methods() {
    {
      STAMP_PRIMITIVES;
      Contact cb(&first, &second, Vec3{0., 0., 0.5}, Vec3{0., 0., 1.}, 0.5);

      CHECK(cpp17::as_const(cb).get_relative_velocity() == Vec3{0., 0., 0.});
      CHECK(cpp17::as_const(cb).get_normal_relative_velocity() == 0.0);
      CHECK(cpp17::as_const(cb).get_contact_type() == ContactType::resting);
    }
    {
      using Prim = SpherePrimitive;
      Vec3 first_position{0., 0., 0.};
      Prim first(0UL, 0UL, true, first_position, 1.0, elastica::iron);

      Vec3 velocity{0., 0., 0.};
      Vec3 omega{0., 0., 0.};
      first.transfer(first_position, velocity, omega);

      Vec3 second_position{0., 0., 1.};
      Prim second(0UL, 1UL, true, second_position, 1.0, elastica::copper);
      second.transfer(second_position, {0., -1., 0.}, omega);

      Contact cb(&first, &second, Vec3{0., 0., 0.5}, Vec3{0., 1., 0.}, 0.5);

      // from 2 to 1 always so 1 -2
      CHECK(cpp17::as_const(cb).get_relative_velocity() == Vec3{0., 1., 0.});
      CHECK(cpp17::as_const(cb).get_normal_relative_velocity() == 1.0);
      CHECK(cpp17::as_const(cb).get_contact_type() == ContactType::separating);
    }
    {
      using Prim = SpherePrimitive;
      Vec3 first_position{0., 0., 0.};
      Prim first(0UL, 0UL, true, first_position, 1.0, elastica::iron);

      Vec3 velocity{0., 0., 0.};
      Vec3 omega{0., 0., 0.};
      first.transfer(first_position, velocity, omega);

      Vec3 second_position{0., 0., 1.};
      Prim second(0UL, 1UL, true, second_position, 1.0, elastica::copper);
      second.transfer(second_position, {0., 1., 0.}, omega);

      Contact cb(&first, &second, Vec3{0., 0., 0.5}, Vec3{0., 1., 0.}, 0.5);

      // from 2 to 1 always so 1 -2
      CHECK(cpp17::as_const(cb).get_relative_velocity() == Vec3{0., -1., 0.});
      CHECK(cpp17::as_const(cb).get_normal_relative_velocity() == -1.0);
      CHECK(cpp17::as_const(cb).get_contact_type() == ContactType::colliding);
    }
  }

  void test_apply_force() {
    STAMP_PRIMITIVES;

    Contact cb(&first, &second, Vec3{0., 0., 0.5}, Vec3{0., 0., 1.}, 0.5);
    CHECK(first.get_force() == Vec3{0., 0., 0.});
    CHECK(second.get_force() == Vec3{0., 0., 0.});

    const Vec3 force{1.0, 0.2, 0.4};
    cb.apply_force(force);

    CHECK_FALSE(first.get_force() == Vec3{0., 0., 0.});
    CHECK_FALSE(second.get_force() == Vec3{0., 0., 0.});
  }

  ELASTICA_TEST_CASE("Unit.Environments.CollisionSystem.Contact.Contact",
                     "[Unit][Environments]") {
    SECTION("Constructor") {
      STAMP_PRIMITIVES;

      Contact cb(&first, &second, Vec3{0., 0., 0.5}, Vec3{0., 0., 1.}, 0.5);
      Contact another_cb(&first, &second, Vec3{0., 0., 0.5}, Vec3{0., 0., 1.},
                         Vec3{0., 0., 1.}, Vec3{0., 0., 1.}, 0.5);
    }
    SECTION("is_penetrating") { test_is_penetrating(); }
    SECTION("is_contacting") { test_is_contacting(); }
    SECTION("getters") { test_getters(); }
    SECTION("compute methods") { test_compute_methods(); }
    SECTION("apply_force") { test_apply_force(); }
  }

#undef STAMP_PRIMITIVES

}  // namespace
