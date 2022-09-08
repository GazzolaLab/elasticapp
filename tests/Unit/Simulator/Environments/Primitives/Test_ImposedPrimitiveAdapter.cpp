//
// Created by Tejaswin Parthasarathy on 5/22/22.
//
#include "Framework/TestingFramework.hpp"
//
#include "Simulator/Environments/Kinematics.hpp"
#include "Simulator/Environments/Primitives/HalfSpacePrimitive.hpp"
#include "Simulator/Environments/Primitives/ImposedPrimitiveAdapter.hpp"
#include "Simulator/Environments/Primitives/SpherePrimitive.hpp"
#include "Simulator/Frames.hpp"
#include "Simulator/Materials.hpp"
//
#include "Time/SimulationTime.hpp"
//
#include "Utilities/AsConst.hpp"
#include "Utilities/Math/Rot3.hpp"
//
#include "Simulator/Environments/Primitives/Test_AccessAPI.hpp"

namespace {
  using App = Approx;

  using namespace elastica;
  using namespace elastica::collision;
  using ImposedSpherePrimitive = ImposedPrimitiveAdapter<SpherePrimitive>;
  using ImposedHalfSpacePrimitive = ImposedPrimitiveAdapter<HalfSpacePrimitive>;
  using ISP = ImposedSpherePrimitive;
  using IHP = ImposedHalfSpacePrimitive;

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Environments.Primitives.ImposedPrimitiveAdapter",
      "[Unit][Simulator][Environments]") {
    // clang-format on
    EMPTY_TEST();

    Vec3 position{5.0, 3.0, 6.0};
    real_t radius(4.0);

    // no kinematic constructor
    ISP imposed_sphere(SpherePrimitive(0UL, 0UL, true, position, radius, iron));

    auto p = [](elastica::real_t) { return 2; };
    auto v = [](elastica::real_t) { return 0; };

    auto translations_buffer = kinematics::make_translations(
        kinematics::translate_along<EulerianFrame::X>(
            kinematics::mark_position(p), kinematics::mark_velocity(v)),
        kinematics::translate_along<EulerianFrame::Y>(
            kinematics::mark_position(p), kinematics::mark_velocity(v)),
        kinematics::translate_along<EulerianFrame::Z>(
            kinematics::mark_position(p), kinematics::mark_velocity(v)));

    // kinematic constructor
    ISP another_imposed_sphere(
        SpherePrimitive(0UL, 0UL, true, position, radius, iron),
        std::move(translations_buffer));
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Environments.Primitives.ImposedPrimitiveAdapter.access_API",
      "[Unit][Simulator][Environments]") {
    // clang-format on
    EMPTY_TEST();

    Vec3 position{5.0, 3.0, 6.0};
    real_t radius(4.0);
    ISP imposed_sphere(SpherePrimitive(0UL, 0UL, true, position, radius, iron));
    ::testing::check_access_API(imposed_sphere);  // only const
    ::testing::check_access_API(cpp17::as_const(imposed_sphere));
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Environments.Primitives.ImposedPrimitiveAdapter.update",
      "[Unit][Simulator][Environments]") {
    // clang-format on

    Vec3 position{5.0, 3.0, 6.0};
    real_t radius(4.0);
    ISP imposed_sphere(SpherePrimitive(0UL, 0UL, true, position, radius, iron));

    auto wrap = [](elastica::real_t inp) noexcept {
      return [=](elastica::real_t) noexcept -> elastica::real_t { return inp; };
    };

    auto pX = wrap(1.0);
    auto pY = wrap(2.0);
    auto pZ = wrap(3.0);

    auto vX = wrap(11.0);
    auto vY = wrap(12.0);
    auto vZ = wrap(13.0);

    auto translations_buffer = kinematics::make_translations(
        kinematics::translate_along<EulerianFrame::X>(
            kinematics::mark_position(pX), kinematics::mark_velocity(vX)),
        kinematics::translate_along<EulerianFrame::Y>(
            kinematics::mark_position(pY), kinematics::mark_velocity(vY)),
        kinematics::translate_along<EulerianFrame::Z>(
            kinematics::mark_position(pZ), kinematics::mark_velocity(vZ)));

    // kinematic constructor
    ISP another_imposed_sphere(
        SpherePrimitive(0UL, 0UL, true, position, radius, iron),
        std::move(translations_buffer));

    SECTION("primitives") {
      auto pre_update_primitive_check = [=](auto& p) {
        CHECK(p.get_position()[0] == approx(5.0));
        CHECK(p.get_position()[1] == approx(3.0));
        CHECK(p.get_position()[2] == approx(6.0));

        // CHECK AABB positions as well
        CHECK(p.get_AABB()[0] == approx(1.0 - contact_threshold));
        CHECK(p.get_AABB()[1] == approx(-1.0 - contact_threshold));
        CHECK(p.get_AABB()[2] == approx(2.0 - contact_threshold));
        CHECK(p.get_AABB()[3] == approx(9.0 + contact_threshold));
        CHECK(p.get_AABB()[4] == approx(7.0 + contact_threshold));
        CHECK(p.get_AABB()[5] == approx(10.0 + contact_threshold));
      };

      Time time_v{0.0};
      pre_update_primitive_check(imposed_sphere);
      imposed_sphere.update_primitives(time_v);
      // post should be same as pref
      pre_update_primitive_check(imposed_sphere);

      auto post_update_primitive_check = [=](auto& p) {
        CHECK(p.get_position()[0] == approx(1.0));
        CHECK(p.get_position()[1] == approx(2.0));
        CHECK(p.get_position()[2] == approx(3.0));

        CHECK(p.get_AABB()[0] == approx(-3.0 - contact_threshold));
        CHECK(p.get_AABB()[1] == approx(-2.0 - contact_threshold));
        CHECK(p.get_AABB()[2] == approx(-1.0 - contact_threshold));
        CHECK(p.get_AABB()[3] == approx(5.0 + contact_threshold));
        CHECK(p.get_AABB()[4] == approx(6.0 + contact_threshold));
        CHECK(p.get_AABB()[5] == approx(7.0 + contact_threshold));
      };

      pre_update_primitive_check(another_imposed_sphere);
      another_imposed_sphere.update_primitives(time_v);
      post_update_primitive_check(another_imposed_sphere);
    }
    SECTION("rates") {
      Time time_v{0.0};
      auto pre_update_rates_check = [=](auto& p) {
        CHECK(p.get_velocity()[0] == approx(0.0));
        CHECK(p.get_velocity()[1] == approx(0.0));
        CHECK(p.get_velocity()[2] == approx(0.0));
      };

      pre_update_rates_check(imposed_sphere);
      imposed_sphere.update_rates(time_v);  //
      pre_update_rates_check(imposed_sphere);

      auto post_update_rates_check = [=](auto& p) {
        CHECK(p.get_velocity()[0] == approx(11.0));
        CHECK(p.get_velocity()[1] == approx(12.0));
        CHECK(p.get_velocity()[2] == approx(13.0));
      };
      pre_update_rates_check(another_imposed_sphere);
      another_imposed_sphere.update_rates(time_v);
      post_update_rates_check(another_imposed_sphere);
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Environments.Primitives.ImposedPrimitiveAdapter.infinite",
      "[Unit][Simulator][Environments]") {
    // clang-format on
    const Vec3 half_space_anchor{0.0, 0.0, 0.0};
    auto half_space_normal = {0.0, 0.0, 1.0};
    auto half_space_binormal = {0.0, 1.0, 0.0};
    auto half_space_tangent = {1.0, 0.0, 0.0};
    // implicit convention
    ::elastica::Rot3 Q = {half_space_normal, half_space_binormal,
                          half_space_tangent};

    auto const pre_update_primitive_check = [=](auto& p) {
      CHECK(p.get_position()[0] == approx(0.0));
      CHECK(p.get_position()[1] == approx(0.0));
      CHECK(p.get_position()[2] == (-0.5 * HalfSpacePrimitive::get_span()));

      // plane is infinite, so only guarantee precision till contact_threshold
      auto contact_approx = App::custom().epsilon(0.1 * contact_threshold);

      CHECK(p.get_AABB()[0] ==
            contact_approx(-0.5 * HalfSpacePrimitive::get_span() -
                           contact_threshold));
      CHECK(p.get_AABB()[1] ==
            contact_approx(-0.5 * HalfSpacePrimitive::get_span() -
                           contact_threshold));
      CHECK(p.get_AABB()[2] == contact_approx(-HalfSpacePrimitive::get_span() -
                                              contact_threshold));
      CHECK(p.get_AABB()[3] ==
            contact_approx(0.5 * HalfSpacePrimitive::get_span() +
                           contact_threshold));
      CHECK(p.get_AABB()[4] ==
            contact_approx(0.5 * HalfSpacePrimitive::get_span() +
                           contact_threshold));
      // can only guarantee precision till contact threshold
      // CHECK(p.get_AABB()[5] == contact_approx(contact_threshold));
    };
    auto const pre_update_rate_check = [=](auto& p) {
      CHECK(p.get_velocity()[0] == approx(0.0));
      CHECK(p.get_velocity()[1] == approx(0.0));
      CHECK(p.get_velocity()[2] == approx(0.0));
    };
    auto const& post_update_primitive_check = pre_update_primitive_check;
    auto const& post_update_rate_check = pre_update_rate_check;

    IHP imposed_halfspace(
        HalfSpacePrimitive(0UL, 0UL, true, half_space_anchor, Q, iron));
    Time time_v{0.0};
    pre_update_primitive_check(imposed_halfspace);
    pre_update_rate_check(imposed_halfspace);
    imposed_halfspace.update_primitives(time_v);
    post_update_primitive_check(imposed_halfspace);
    post_update_rate_check(imposed_halfspace);
  }

}  // namespace
