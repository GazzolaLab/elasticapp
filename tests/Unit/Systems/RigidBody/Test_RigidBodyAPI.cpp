//
// Created by Tejaswin Parthasarathy on 9/9/22.
//
#include "Framework/TestingFramework.hpp"
#include "Systems/Sphere.hpp"
#include "Utilities/Math/Identity.hpp"

namespace {

  template <class RB>
  void check_member_API(RB& rigid_body) {
    /// [rigid_body_member_api]
    rigid_body.get_position();
    rigid_body.get_director();
    //
    rigid_body.get_velocity();
    rigid_body.get_angular_velocity();
    //
    //
    rigid_body.get_material();
    rigid_body.get_mass();
    rigid_body.get_external_loads();
    rigid_body.get_external_torques();
    rigid_body.get_inverse_mass_second_moment_of_inertia();
    //
    rigid_body.get_acceleration();
    rigid_body.get_angular_acceleration();
    /// [rigid_body_member_api]

    /// [rigid_body_member_index_api]
    const std::size_t index = 0UL;  // For slice as well.
    rigid_body.get_position(index);
    rigid_body.get_director(index);

    rigid_body.get_velocity(index);
    rigid_body.get_angular_velocity(index);

    rigid_body.get_acceleration(index);
    rigid_body.get_angular_acceleration(index);

    rigid_body.get_material(index);
    rigid_body.get_mass(index);

    rigid_body.get_external_loads(index);
    rigid_body.get_external_torques(index);

    rigid_body.get_inverse_mass_second_moment_of_inertia(index);
    /// [rigid_body_member_index_api]
  }

  ELASTICA_TEST_CASE("Unit.Systems.RigidBody.API.member_functions",
                     "[Systems][Unit]") {
    using Plugin = ::elastica::rigid_body::Sphere;
    using Model = ::blocks::Block<Plugin>;

    Model rigid_body{};

    auto add_a_body = [&rigid_body]() {
      auto initializer = ::blocks::initialize_block<Plugin>(
          ::blocks::initialize<::elastica::tags::Position>(
              []() { return ::elastica::Vec3{}; }),
          ::blocks::initialize<::elastica::tags::Director>(
              []() { return ::elastica::identity3(); }),
          ::blocks::initialize<::elastica::tags::Radius>([]() { return 0.3; }),
          ::blocks::initialize<::elastica::tags::Velocity>(
              []() { return ::elastica::Vec3{}; }),
          ::blocks::initialize<::elastica::tags::AngularVelocity>(
              []() { return ::elastica::Vec3{}; }),
          ::blocks::initialize<::elastica::tags::Acceleration>(
              []() { return ::elastica::Vec3{}; }),
          ::blocks::initialize<::elastica::tags::AngularAcceleration>(
              []() { return ::elastica::Vec3{}; }),
          ::blocks::initialize<::elastica::tags::Material>(
              []() { return ::elastica::iron; }));
      return rigid_body.emplace(std::move(initializer));
    };
    auto first_it = add_a_body();
    auto second_it = add_a_body();

    SECTION("Block API") {
      EMPTY_TEST();

      check_member_API(rigid_body);
      check_member_API(cpp17::as_const(rigid_body));
    }
    SECTION("Slice API") {
      EMPTY_TEST();

      check_member_API(first_it);
      check_member_API(cpp17::as_const(first_it));
      check_member_API(second_it);
      check_member_API(cpp17::as_const(second_it));
    }
  }

  template <class RB>
  void check_free_API(RB& rigid_body) {
    /// [rigid_body_free_api]
    ::elastica::position(rigid_body);
    ::elastica::director(rigid_body);
    ::elastica::velocity(rigid_body);
    ::elastica::angular_velocity(rigid_body);
    ::elastica::external_loads(rigid_body);
    ::elastica::external_torques(rigid_body);
    /// [rigid_body_free_api]

    /// [rigid_body_free_index_api]
    const std::size_t index = 0UL;
    ::elastica::position(rigid_body, index);
    ::elastica::director(rigid_body, index);
    ::elastica::velocity(rigid_body, index);
    ::elastica::angular_velocity(rigid_body, index);
    ::elastica::external_loads(rigid_body, index);
    ::elastica::external_torques(rigid_body, index);
    /// [rigid_body_free_index_api]
  }

  ELASTICA_TEST_CASE("Unit.Systems.RigidBody.API.free_functions",
                     "[Systems][Unit]") {
    using Plugin = ::elastica::rigid_body::Sphere;
    using Model = ::blocks::Block<Plugin>;

    Model rigid_body{};

    auto add_a_body = [&rigid_body]() {
      auto initializer = ::blocks::initialize_block<Plugin>(
          ::blocks::initialize<::elastica::tags::Position>(
              []() { return ::elastica::Vec3{}; }),
          ::blocks::initialize<::elastica::tags::Director>(
              []() { return ::elastica::identity3(); }),
          ::blocks::initialize<::elastica::tags::Radius>([]() { return 0.3; }),
          ::blocks::initialize<::elastica::tags::Velocity>(
              []() { return ::elastica::Vec3{}; }),
          ::blocks::initialize<::elastica::tags::AngularVelocity>(
              []() { return ::elastica::Vec3{}; }),
          ::blocks::initialize<::elastica::tags::Acceleration>(
              []() { return ::elastica::Vec3{}; }),
          ::blocks::initialize<::elastica::tags::AngularAcceleration>(
              []() { return ::elastica::Vec3{}; }),
          ::blocks::initialize<::elastica::tags::Material>(
              []() { return ::elastica::iron; }));
      return rigid_body.emplace(std::move(initializer));
    };
    auto first_it = add_a_body();
    auto second_it = add_a_body();

    SECTION("Block API") {
      EMPTY_TEST();

      check_free_API(rigid_body);
      check_free_API(cpp17::as_const(rigid_body));
    }
    SECTION("Slice API") {
      EMPTY_TEST();

      check_free_API(first_it);
      check_free_API(cpp17::as_const(first_it));
      check_free_API(second_it);
      check_free_API(cpp17::as_const(second_it));
    }
  }

}  // namespace