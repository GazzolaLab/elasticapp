//
// Created by Tejaswin Parthasarathy on 3/30/22.
//

#include "Framework/TestingFramework.hpp"
//
#include "Simulator/Environments/CollisionSystem/Contact/Contact.hpp"
#include "Simulator/Environments/CollisionSystem/Contact/ContactVectorAdapter.hpp"
//
#include "Simulator/Environments/CollisionSystem/Detection/Fine/MaxContacts.hpp"
#include "Simulator/Environments/CollisionSystem/Response/DEMForces/LinearSpringDashpot.hpp"
#include "Simulator/Environments/CollisionSystem/Response/DEMForces/Protocols.hpp"
//
#include "Simulator/Environments/Primitives.hpp"
#include "Simulator/Materials.hpp"
#include "Utilities/DefineTypes.h"
#include "Utilities/Math/Identity.hpp"
#include "Utilities/Math/Vec3.hpp"

namespace {
  using namespace elastica::collision;
  using elastica::identity3;
  using elastica::iron;
  using elastica::real_t;
  using elastica::Vec3;

  using ImposedSpherePrimitive = ImposedPrimitiveAdapter<SpherePrimitive>;
  using ImposedCapsulePrimitive = ImposedPrimitiveAdapter<CapsulePrimitive>;

  using Model = elastica::collision::response::LinearSpringDashpot;
  static_assert(::tt::assert_conforms_to<
                    Model, ::elastica::collision::protocols::DEMForce>,
                "Meta failure");

  ELASTICA_TEST_CASE("Unit.Environments.CollisionResponse.LinearSpringDashpot",
                     "[Unit][Environments]") {
    using Detector = detection::fine::MaxContacts;
    Model model;

    Vec3 position{5.0, 3.0, 6.0};
    real_t radius(4.0);

    using P = ImposedSpherePrimitive;
    P first_sphere(0UL, 0UL, true, position, radius, iron);

    Vec3 velocity{1.0, 0.0, 0.0};
    Vec3 omega{0.0, 0.0, 0.0};
    first_sphere.transfer(position, velocity, omega);

    using Q = CapsulePrimitive;
    const real_t capsule_radius = radius;
    const real_t capsule_length = 2.0 * radius;
    const Vec3 capsule_position{position[0], position[1],
                                position[2] + radius + 1.75 * radius};

    Q capsule(1UL, 1UL, true, capsule_position, identity3(), capsule_radius,
              capsule_length, iron);
    capsule.transfer(capsule_position, identity3(), velocity, omega);

    using CV = ContactVectorAdapter<Contact>;
    CV contacts;

    REQUIRE(contacts.empty());
    Detector::collide(first_sphere, capsule, contacts);
    REQUIRE_FALSE(contacts.empty());

    // these can be implemented later on. Now we have a template.
    CHECK(::elastica::length(first_sphere.get_force()) == approx(0.0));
    CHECK(::elastica::length(capsule.get_force()) == approx(0.0));
    CHECK(::elastica::length(first_sphere.get_torque()) == approx(0.0));
    CHECK(::elastica::length(capsule.get_torque()) == approx(0.0));

    auto depth = model(contacts.at(0));

    CHECK(depth > 0);
    // these can be implemented later on. Now we have a template.
    CHECK(::elastica::length(first_sphere.get_force()) != approx(0.0));
    CHECK(::elastica::length(capsule.get_force()) != approx(0.0));
    CHECK(::elastica::length(capsule.get_force() + first_sphere.get_force()) ==
          approx(0.0));

    CHECK(::elastica::length(first_sphere.get_torque()) != approx(0.0));
    CHECK(::elastica::length(capsule.get_torque()) != approx(0.0));

    // This need not be satisfied always
    // CHECK(::elastica::length(capsule.get_torque() +
    //                          first_sphere.get_torque()) == approx(0.0));
  }

}  // namespace
