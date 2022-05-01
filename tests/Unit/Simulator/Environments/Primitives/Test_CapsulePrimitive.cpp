//
// Created by Tejaswin Parthasarathy on 9/26/21.
//
#include <cmath>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Environments/Primitives/CapsulePrimitive.hpp"
#include "Simulator/Environments/Primitives/Protocols.hpp"
#include "Simulator/Materials.hpp"
#include "Utilities/Math/Identity.hpp"
#include "Utilities/Math/Rot3.hpp"
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace {

  using namespace elastica;
  using namespace elastica::collision;

  static_assert(
      ::tt::assert_conforms_to<CapsulePrimitive, protocols::ConvexPrimitive>,
      "Does not conform to convex protocol!");

  ELASTICA_TEST_CASE("Unit.Environments.Primitives.CapsulePrimitive.transfer",
                     "[Unit][Simulator][Environments]") {
    Vec3 position{5.0, 3.0, 6.0};
    real_t radius{4.0};
    real_t length{12.0};

    CapsulePrimitive capsule(0UL, 0UL, true, position, identity3(), radius,
                             length, iron);

    Vec3 new_position = 2.0 * position;
    Rot3 new_rotation = {{0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 0.0, 0.0}};
    Vec3 velocity = Vec3{1.0, 0.0, 1.0};
    Vec3 omega = Vec3{1.0, 2.0, 1.0};
    capsule.transfer(new_position, new_rotation, velocity, omega);

    CHECK(capsule.get_position() == (new_position));
    CHECK(capsule.get_rotation_matrix() == (new_rotation));
    CHECK(capsule.get_velocity() == (velocity));
    CHECK(capsule.get_angular_velocity() == (omega));

    // check bounding box updated
    // [10, 6, 12] +/- whatever
    CHECK(capsule.get_AABB() ==
          typename CapsulePrimitive::AABB(
              10.0 - 6.0 - 4.0 - contact_threshold,
              6.0 - 4.0 - contact_threshold, 12.0 - 4.0 - contact_threshold,
              10.0 + 6.0 + 4.0 + contact_threshold,
              6.0 + 4.0 + contact_threshold, 12.0 + 4.0 + contact_threshold));
  }

}  // namespace