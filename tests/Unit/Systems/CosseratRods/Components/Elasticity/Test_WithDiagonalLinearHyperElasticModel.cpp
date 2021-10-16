//
// Created by Tejaswin Parthasarathy on 5/3/21.
//
#include "Framework/TestingFramework.hpp"
#include "Systems/CosseratRods/Components/Tags.hpp"
#include "Systems/CosseratRods/Components/Elasticity/WithDiagonalLinearHyperElasticModel.hpp"
#include "Systems/CosseratRods/Components/Geometry.hpp"
#include "Systems/CosseratRods/Components/Kinematics.hpp"
#include "Systems/CosseratRods/CosseratRodTraits.hpp"
///
///
#include "Systems/CosseratRods/Mocks.hpp"
#include "Utilities/MakeArray.hpp"

namespace {
  using namespace elastica;
  using namespace elastica::cosserat_rod;

  using Traits = typename elastica::cosserat_rod::DefaultCosseratRodTraits;

  template <typename B>
  struct TestGeometry:
      public component::WithCircularCosseratRod<Traits, B>{
    using Traits = Traits;
  };

  template <typename B>
  struct TestKinematics:
      public component::WithRodKinematics<Traits, B>{
    using Traits = Traits;
  };

  template <typename B>
  struct TestElasticity:
      public component::WithDiagonalLinearHyperElasticModel<Traits, B> {
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

  ELASTICA_TEST_CASE(
      "Unit.Systems.CosseratRods.WithDiagonalLinearHyperElasticModel:EBO",
      "[Systems][Unit]") {
    CHECK_EBO(Model);
  }

  ELASTICA_TEST_CASE(
      "Unit.Systems.CosseratRods.WithDiagonalLinearHyperElasticModel:data",
      "[Systems][Unit]") {
    Model model{};

    constexpr auto n_initializers = 7UL;
    auto n_times_called = make_array<n_initializers>(0U);
    auto initializer = mock::initialize_block<Plugin>(
        // Geometry Variables
        blocks::initialize<tags::Position>([&i = n_times_called[0]](std::size_t index) {
          ++i;
          return index * 1.0;
        }),
        blocks::initialize<tags::Director>([&i = n_times_called[1]](...) {
          ++i;
          return 0.5;
        }),
        blocks::initialize<tags::ElementDimension>([&i = n_times_called[2]](...) {
          ++i;
          return 4.0;
        }),
        // Elasticity Variables
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
        // Kinematics Variables
        blocks::initialize<tags::Density>([](...) { return 1.0; }),
        blocks::initialize<tags::Velocity>([](...) { return 0.0; }),
        blocks::initialize<tags::AngularVelocity>([](...) { return 0.0; }),
        blocks::initialize<tags::Acceleration>([](...) { return 0.0; }),
        blocks::initialize<tags::AngularAcceleration>([](...) { return 0.0; })
    );

    Model::initialize(model, std::move(initializer));

    CHECK(model.compute_internal_forces() == 1.00);
    CHECK(model.compute_internal_torques() == 2.0);


    // check if initialization is done as expected
    //CHECK_ITERABLE_APPROX(n_times_called, make_array<n_initializers>(1U));

    // [0] element : stiff  * I_0
    const double area = M_PI * 4.0 * 4.0;
    const double I_0 = area * area * 0.25 * M_1_PI;
    CHECK(blocks::get<tags::BendingTwistRigidityMatrix>(model) ==
          4.0 * I_0);
    CHECK(blocks::get<tags::ShearStretchRigidityMatrix>(model) ==
          4.0 * area * 4.0 / 3.0);
  }

}  // namespace
