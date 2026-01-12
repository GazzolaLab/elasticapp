//
// Created by Tejaswin Parthasarathy on 9/25/21.
//

#include "Framework/TestingFramework.hpp"
#include "Simulator/Environments/Primitives/RigidBodyPrimitive.hpp"
#include "Utilities/AsConst.hpp"
#include "Utilities/Math/Orthogonal.hpp"
//
#include "Simulator/Environments/Primitives/Test_AccessAPI.hpp"

namespace {

  using namespace elastica;
  using namespace elastica::collision;

  class TestRigidBodyPrimitive : public RigidBodyPrimitive {
   public:
    TestRigidBodyPrimitive(bool finite, ::elastica::id_t sid,
                           ::elastica::id_t uid)
        : RigidBodyPrimitive(this, finite, sid, uid){};
    ~TestRigidBodyPrimitive() = default;
    explicit TestRigidBodyPrimitive(TestRigidBodyPrimitive&& other)
        : RigidBodyPrimitive(this, other.finite_, other.sid_, other.uid_){};

    inline Rot3& get_rotation_matrix() noexcept { return Q_; }
    inline Vec3& get_velocity() noexcept { return v_; }
    inline Vec3& get_angular_velocity() noexcept { return w_; }
    inline Vec3& get_position() noexcept { return gpos_; }
  };

  auto get_rigid_primitive() noexcept -> TestRigidBodyPrimitive {
    return {true, 0, 1};
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Environments.Primitives.RigidBodyPrimitive.access_API",
      "[Unit][Simulator][Environments]") {
    // clang-format on
    EMPTY_TEST();

    auto rb(get_rigid_primitive());
    ::testing::check_access_API(rb);
    ::testing::check_access_API(std::as_const(rb));
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Environments.Primitives.RigidBodyPrimitive.transformations",
     "[Unit][Simulator][Environments]") {
    // clang-format on
    TestRigidBodyPrimitive rp(get_rigid_primitive());
    //    trans(Q_) * Vec3{vx, vy, vz}

    // ( normal | binormal | tangent)^T
    // [0.6, -0.8, 0.0], [0.8, 0.6, 0.0]. [0., 0., 1]
    rp.get_rotation_matrix() =
        ::elastica::make_orthogonal_bases_from_normal_and_tangent(
            Vec3{0.6, -0.8, 0.0}, Vec3{0., 0., 1.0});

    SECTION("Transform and its inverse is an identity") {
      Vec3 expected = {1.0, 2.0, 3.0};
      Vec3 transformed = rp.from_world_FOR_to_body_FOR(
          rp.from_body_FOR_to_world_FOR(expected));
      CHECK(expected == transformed);
      // rp.
    }

    SECTION("Transform from world frame to body frame") {
      Vec3 seed = {1.0, 2.0, 3.0};
      Vec3 expected_result = {-1.0, 2.0, 3.0};
      CHECK(rp.from_world_FOR_to_body_FOR(seed) == expected_result);

      seed = {3.0, 2.0, 1.0};
      expected_result = {0.2, 3.6, 1.0};
      CHECK(rp.from_world_FOR_to_body_FOR(seed) == expected_result);
    }

    SECTION("Transform from body frame to world frame") {
      Vec3 seed = {-1.0, 2.0, 3.0};
      Vec3 expected_result = {1.0, 2.0, 3.0};
      CHECK(rp.from_body_FOR_to_world_FOR(seed) == expected_result);

      seed = {0.2, 3.6, 1.0};
      expected_result = {3.0, 2.0, 1.0};
      CHECK(rp.from_body_FOR_to_world_FOR(seed) == expected_result);
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Environments.Primitives.RigidBodyPrimitive.velocity_from_world_FOR",
      "[Unit][Simulator][Environments]") {
    // clang-format on
    TestRigidBodyPrimitive rp(get_rigid_primitive());
    SECTION("2D test") {
      // we assume this is in local coordinates
      rp.get_angular_velocity() = Vec3{0.0, 0.0, 1.0};
      rp.get_position() = Vec3{2.0, 5.0, 0.0};
      rp.get_velocity() = Vec3{4.0, 6.0, 0.0};

      Vec3 expected_velocity{6.0, 7.0, 0.0};
      CHECK(rp.velocity_from_world_FOR(Vec3{3.0, 3.0, 0.0}) ==
            expected_velocity);
    }
    SECTION("3D test") {
      Vec3 position{3.0, 6.0, 3.0};
      rp.get_position() = Vec3{2.0, 5.0, 1.0};
      rp.get_angular_velocity() = Vec3{1.0, 1.0, -1.0};
      rp.get_velocity() = Vec3{-3.0, 3.0, 0.0};

      Vec3 expected_velocity{0.0, 0.0, 0.0};
      CHECK(rp.velocity_from_world_FOR(position) == expected_velocity);

      rp.get_angular_velocity() = Vec3{0.0, 0.0, 1.0};
      // adjust so that angular velocity is along different axis
      rp.get_rotation_matrix() = {
          {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 0.0, 0.0}};
      expected_velocity = {-3.0, 1.0, 1.0};

      CHECK(rp.velocity_from_world_FOR(position) == expected_velocity);
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Environments.Primitives.RigidBodyPrimitive.add_force_at_position",
      "[Unit][Simulator][Environments]") {
    // clang-format on
    TestRigidBodyPrimitive rp(get_rigid_primitive());
    SECTION("2D test") {
      Vec3 position{3.0, 6.0, 0.0};
      rp.get_position() = Vec3{2.0, 5.0, 0.0};

      Vec3 expected_force{6.0, 6.0, 0.0};
      Vec3 expected_torque{0.0, 0.0, 0.0};

      rp.add_force_at_position(expected_force, position);
      CHECK(rp.get_force() == expected_force);
      CHECK(rp.get_torque() == expected_torque);

      rp.reset_forces_and_torques();

      expected_force = {6.0, -6.0, 0.0};
      expected_torque = {0.0, 0.0, -12.0};
      rp.add_force_at_position(expected_force, position);
      CHECK(rp.get_force() == expected_force);
      CHECK(rp.get_torque() == expected_torque);
    }

    SECTION("3D test") {
      Vec3 position{3.0, 6.0, 2.0};
      rp.get_position() = Vec3{2.0, 5.0, 0.0};

      Vec3 expected_force{3.0, 3.0, 6.0};
      Vec3 expected_torque{0.0, 0.0, 0.0};

      rp.add_force_at_position(expected_force, position);
      CHECK(rp.get_force() == expected_force);
      CHECK(rp.get_torque() == expected_torque);

      rp.reset_forces_and_torques();

      expected_force = {3.0, -3.0, -6.0};
      expected_torque = {0.0, 12.0, -6.0};
      rp.add_force_at_position(expected_force, position);
      CHECK(rp.get_force() == expected_force);
      CHECK(rp.get_torque() == expected_torque);
    }
  }
}  // namespace