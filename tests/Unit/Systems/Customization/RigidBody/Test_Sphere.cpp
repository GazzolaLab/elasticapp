//
// Created by Tejaswin Parthasarathy on 4/25/22.
//
#include <algorithm>

#include "Framework/TestingFramework.hpp"
#include "Systems/Customization/RigidBody/Sphere.hpp"
// init
#include "Systems/Sphere.hpp"
#include "Utilities/CountingIterator.hpp"
#include "Utilities/Math/Identity.hpp"

//
#include "Simulator/Environments/Transfer.hpp"

namespace {

  using namespace elastica::rigid_body;
  using ::elastica::collision::tt::PrimitiveTrait_t;

  static_assert(std::is_same_v<
                    ::elastica::collision::tt::PrimitiveTrait_t<detail::Sphere>,
                    ::elastica::collision::SpherePrimitive>,
                "Meta-failure");
  static_assert(
      std::is_same_v<
          ::elastica::collision::tt::PrimitiveTrait_t<detail::SphereBlock>,
          ::elastica::collision::SpherePrimitive>,
      "Meta-failure");
  static_assert(
      std::is_same_v<
          ::elastica::collision::tt::PrimitiveTrait_t<detail::SphereBlockSlice>,
          ::elastica::collision::SpherePrimitive>,
      "Meta-failure");
  static_assert(std::is_same_v<::elastica::collision::tt::PrimitiveTrait_t<
                                     detail::SphereBlockConstSlice>,
                                 ::elastica::collision::SpherePrimitive>,
                "Meta-failure");

  ELASTICA_TEST_CASE("Unit.Systems.Customization.Sphere.emplacement",
                     "[Systems][Unit]") {
    using Primitive = ::elastica::collision::SpherePrimitive;
    using Primitives = std::vector<Primitive>;

    using Plugin = ::elastica::rigid_body::Sphere;
    using Model = ::blocks::Block<Plugin>;

    const std::size_t n_slices = 20UL;

    Model model{};
    // random values
    Primitives primitives{};

    std::for_each(
        elastica::make_counting_iterator(0UL),
        elastica::make_counting_iterator(n_slices), [&](std::size_t) {
          auto initializer = ::blocks::initialize_block<Plugin>(
              ::blocks::initialize<::elastica::tags::Position>([]() {
                return ::elastica::Vec3{1., 0., 0.};
              }),
              ::blocks::initialize<::elastica::tags::Director>(
                  []() -> ::elastica::Rot3 {
                    return 2.0 * ::elastica::identity3();
                  }),
              ::blocks::initialize<::elastica::tags::Radius>(
                  []() { return 0.3; }),
              ::blocks::initialize<::elastica::tags::Velocity>([]() {
                return ::elastica::Vec3{1., 0., 0.};
              }),
              ::blocks::initialize<::elastica::tags::AngularVelocity>([]() {
                return ::elastica::Vec3{1., 0., 0.};
              }),
              ::blocks::initialize<::elastica::tags::Acceleration>([]() {
                return ::elastica::Vec3{1., 0., 0.};
              }),
              ::blocks::initialize<::elastica::tags::AngularAcceleration>([]() {
                return ::elastica::Vec3{1., 0., 0.};
              }),
              ::blocks::initialize<::elastica::tags::Material>(
                  []() { return ::elastica::copper; }));

          model.emplace(std::move(initializer));
        });

    std::for_each(
        elastica::make_counting_iterator(0UL),
        elastica::make_counting_iterator(n_slices), [&](std::size_t i) {
          auto slice = ::blocks::slice(std::as_const(model), i);
          primitives.emplace_back(i, 0UL, true, slice.get_material(0UL));
        });

    std::for_each(std::cbegin(primitives), std::cend(primitives),
                  [](Primitive const& p) {
                    CHECK(p.get_material() == ::elastica::copper);
                  });
  }

  ELASTICA_TEST_CASE("Unit.Systems.Customization.Sphere.transfer",
                     "[Systems][Unit]") {
    using Primitive = ::elastica::collision::SpherePrimitive;
    using Primitives = std::vector<Primitive>;

    using Plugin = ::elastica::rigid_body::Sphere;
    using Model = ::blocks::Block<Plugin>;

    const std::size_t n_slices = 20UL;

    Model model{};
    // random values
    Primitives primitives{};

    std::for_each(elastica::make_counting_iterator(0UL),
                  elastica::make_counting_iterator(n_slices),
                  [&](std::size_t i) {
                    primitives.emplace_back(i, 0UL, true, ::elastica::copper);
                  });

    std::for_each(
        elastica::make_counting_iterator(0UL),
        elastica::make_counting_iterator(n_slices), [&](std::size_t) {
          auto initializer = ::blocks::initialize_block<Plugin>(
              ::blocks::initialize<::elastica::tags::Position>([]() {
                return ::elastica::Vec3{1., 0., 0.};
              }),
              ::blocks::initialize<::elastica::tags::Director>(
                  []() -> ::elastica::Rot3 {
                    return 2.0 * ::elastica::identity3();
                  }),
              ::blocks::initialize<::elastica::tags::Radius>(
                  []() { return 0.3; }),
              ::blocks::initialize<::elastica::tags::Velocity>([]() {
                return ::elastica::Vec3{1., 0., 0.};
              }),
              ::blocks::initialize<::elastica::tags::AngularVelocity>([]() {
                return ::elastica::Vec3{1., 0., 0.};
              }),
              ::blocks::initialize<::elastica::tags::Acceleration>([]() {
                return ::elastica::Vec3{1., 0., 0.};
              }),
              ::blocks::initialize<::elastica::tags::AngularAcceleration>([]() {
                return ::elastica::Vec3{1., 0., 0.};
              }),
              ::blocks::initialize<::elastica::tags::Material>(
                  []() { return ::elastica::iron; }));

          model.emplace(std::move(initializer));
        });

    auto pre_check = [](Primitive const& p) {
      CHECK_FALSE(p.get_radius() == approx(0.3));
      CHECK_FALSE(p.get_position() == ::elastica::Vec3{1., 0., 0.});
      CHECK_FALSE(p.get_rotation_matrix() == (2.0 * ::elastica::identity3()));
      CHECK_FALSE(p.get_velocity() == ::elastica::Vec3{1., 0., 0.});
      CHECK_FALSE(p.get_angular_velocity() == ::elastica::Vec3{1., 0., 0.});
    };

    auto post_check = [](Primitive const& p) {
      CHECK(p.get_radius() == approx(0.3));
      CHECK(p.get_position() == ::elastica::Vec3{1., 0., 0.});
      CHECK(p.get_rotation_matrix() == (2.0 * ::elastica::identity3()));
      CHECK(p.get_velocity() == ::elastica::Vec3{1., 0., 0.});
      CHECK(p.get_angular_velocity() == ::elastica::Vec3{1., 0., 0.});
    };

    // are NaNs and usually throw
    // std::for_each(std::begin(primitives), std::end(primitives), pre_check);

    std::for_each(
        elastica::make_counting_iterator(0UL),
        elastica::make_counting_iterator(n_slices),
        [&, beg = std::begin(primitives)](std::size_t i) {
          auto slice = ::blocks::slice(std::as_const(model), i);
          auto begit =
              beg + static_cast<typename decltype(beg)::difference_type>(i);
          auto endit =
              beg +
              static_cast<typename decltype(beg)::difference_type>(i + 1UL);
          using namespace ::elastica::collision;
          Transfer<decltype(slice)>::template do_transfer(
              ::elastica::collision::from, slice, ::elastica::collision::to,
              begit, endit);
        });

    std::for_each(std::begin(primitives), std::end(primitives), post_check);

    std::for_each(std::begin(primitives), std::end(primitives),
                  [](Primitive& p) {
                    p.add_force_at_position(::elastica::Vec3{1.0, 2.0, 3.0},
                                            ::elastica::Vec3{1.0, 2.0, 3.0});
                  });

    auto pre_transfer_check_block = [](Model& m) {
      CHECK(::blocks::get<::elastica::tags::ExternalLoads>(m) == 0.0);
      CHECK(::blocks::get<::elastica::tags::ExternalTorques>(m) == 0.0);
    };
    auto post_transfer_check_block = [](Model& m) {
      CHECK_FALSE(::blocks::get<::elastica::tags::ExternalLoads>(m) == 0.0);
      CHECK_FALSE(::blocks::get<::elastica::tags::ExternalTorques>(m) == 0.0);
    };

    pre_transfer_check_block(model);

    std::for_each(
        elastica::make_counting_iterator(0UL),
        elastica::make_counting_iterator(n_slices),
        [&, beg = std::cbegin(primitives)](std::size_t i) {
          auto begit =
              beg + static_cast<typename decltype(beg)::difference_type>(i);
          auto endit =
              beg +
              static_cast<typename decltype(beg)::difference_type>(i + 1UL);

          auto slice = ::blocks::slice(model, i);
          using namespace ::elastica::collision;
          Transfer<decltype(slice)>::template do_transfer(
              ::elastica::collision::to, slice, ::elastica::collision::from,
              begit, endit);
        });

    post_transfer_check_block(model);
  }

}  // namespace
