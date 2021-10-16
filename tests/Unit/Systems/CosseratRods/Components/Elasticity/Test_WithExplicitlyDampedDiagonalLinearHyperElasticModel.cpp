//
// Created by Tejaswin Parthasarathy on 5/3/21.
//
#include "Framework/TestingFramework.hpp"
#include "Systems/CosseratRods/Tags.hpp"
#include "Systems/CosseratRods/CosseratRodTraits.hpp"
#include "Systems/CosseratRods/Components/Elasticity/WithExplicitlyDampedDiagonalLinearHyperElasticModel.hpp"
#include "Systems/CosseratRods/Components/Geometry.hpp"
#include "Systems/CosseratRods/Components/Kinematics.hpp"
///
///
#include "Systems/CosseratRods/Mocks.hpp"
#include "Utilities/MakeArray.hpp"

namespace {
  using namespace elastica;
  using namespace elastica::cosserat_rod;
  using namespace elastica::cosserat_rod::component;
  using Traits = typename elastica::cosserat_rod::DefaultCosseratRodTraits;

  template <typename B>
  struct TestGeometry : WithCircularCosseratRod<Traits, B>{
    using Traits = Traits;
  };

  template <typename B>
  struct TestKinematics : WithRodKinematics<Traits, B>{
    using Traits = Traits;
  };

  template <typename B>
  struct TestElasticity : WithExplicitlyDampedDiagonalLinearHyperElasticModel<Traits, B> {
    using Traits = Traits;
  };

  using Plugin = mock::MockPlugin<TestGeometry, TestElasticity, TestKinematics>;
  using Model = typename Plugin::Model;

  //  static_assert(
  //      tt::assert_conforms_to<Plugin,
  //      ::elastica::cosserat_rod::protocols::Shpae>, "failure!");

  //  static_assert(
  //      tt::assert_conforms_to<Plugin,
  //      ::elastica::cosserat_rod::protocols::Elastic1D>, "failure!");

}  // namespace

namespace {

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.WithExplicitlyDampedDiagonalLinearHyperElasticModel.data",
                     "[Systems][Unit]") {
    // clang-format on
    Model model{};

    CHECK_EBO(Model);

    constexpr auto n_initializers = 9UL;
    auto n_times_called = make_array<n_initializers>(0U);
    auto initializer = mock::initialize_block<Plugin>(
        // from before
        blocks::initialize<tags::Position>([&i = n_times_called[0]](std::size_t index) {
          ++i;
          return index * 1.0;
        }),
        blocks::initialize<tags::Director>([&i = n_times_called[1]](...) {
          ++i;
          return 0.5;
        }),
        blocks::initialize<tags::ElementDimension>(
            [&i = n_times_called[4]](...) {
              ++i;
              return 4.0;
            }),
        blocks::initialize<tags::ShearStretchRigidityMatrix>(
            [&i = n_times_called[5]](...) {
              ++i;
              return 4.0;
            }),
        blocks::initialize<tags::BendingTwistRigidityMatrix>(
            [&i = n_times_called[6]](...) {
              ++i;
              return 4.0;
            }),
        // from before
        blocks::initialize<tags::ForceDampingCoefficient>([&i = n_times_called[7]](...) {
          ++i;
          return 4.0;
        }),
        blocks::initialize<tags::TorqueDampingCoefficient>([&i = n_times_called[8]](...) {
          ++i;
          return 4.0;
        }),
        blocks::initialize<tags::Density>([](...) { return 4.0; }),
        blocks::initialize<tags::Velocity>([](...) { return 4.0; }),
        blocks::initialize<tags::AngularVelocity>([](...) { return 4.0; }),
        blocks::initialize<tags::Acceleration>([](...) { return 4.0; }),
        blocks::initialize<tags::AngularAcceleration>([](...) { return 4.0; })
      );

    Model::initialize(model, std::move(initializer));

    // check if initialization is done as expected
    CHECK_ITERABLE_APPROX(n_times_called, make_array<n_initializers>(1U));

    // [0] element : stiff  * I_0
    const double area = M_PI * 4.0 * 4.0;
    const double I_0 = area * area * 0.25 * M_1_PI;
//    CHECK(double(blocks::get<tags::BendingTwistRigidityMatrix>(model)) ==
//          approx(4.0 * I_0));
//    CHECK(double(blocks::get<tags::ShearStretchRigidityMatrix>(model)) ==
//          approx(4.0 * area * 4.0 / 3.0));

    // 1 + whatever
    CHECK(model.compute_internal_forces() == 2.00);
    CHECK(model.compute_internal_torques() == 3.0);
  }

}  // namespace
