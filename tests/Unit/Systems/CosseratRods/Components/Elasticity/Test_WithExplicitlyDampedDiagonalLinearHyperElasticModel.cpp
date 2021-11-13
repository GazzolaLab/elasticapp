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
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/Math/Rot3.hpp"
#include "Utilities/Math/Identity.hpp"

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

    constexpr auto n_initializers = 15UL;
    auto n_times_called = make_array<n_initializers>(0U);
    auto j = 0UL;
    auto initializer = mock::initialize_block<Plugin>(
        blocks::initialize<tags::Position>([&i = n_times_called[++j]](std::size_t index) { ++i; return Vec3({0.0, 1.0*index, 0.0}); }),
        blocks::initialize<tags::Director>([&i = n_times_called[++j]](...) { ++i; return 0.5; }),
        blocks::initialize<tags::ElementDimension>( [&i = n_times_called[++j]](...) { ++i; return 1.0; }),
        blocks::initialize<tags::ElementDilatation>([&i = n_times_called[++j]](...) { ++i; return 1.0; }),
        blocks::initialize<tags::ReferenceCurvature>( [&i = n_times_called[++j]](...) { ++i; return 1.0; }),
        blocks::initialize<tags::ReferenceShearStretchStrain>( [&i = n_times_called[++j]](...) { ++i; return 1.0; }),
        blocks::initialize<tags::ShearStretchRigidityMatrix>( [&i = n_times_called[++j]](...) { ++i; return 4.0; }),
        blocks::initialize<tags::BendingTwistRigidityMatrix>( [&i = n_times_called[++j]](...) { ++i; return 4.0; }),
        blocks::initialize<tags::ForceDampingCoefficient>([&i = n_times_called[++j]](...) { ++i; return 4.0; }),
        blocks::initialize<tags::TorqueDampingCoefficient>([&i = n_times_called[++j]](...) { ++i; return 0.0; }),
        blocks::initialize<tags::Mass>([&i = n_times_called[++j]](...) { ++i; return 1.0; }),
        blocks::initialize<tags::Velocity>([&i = n_times_called[++j]](...) { ++i; return 1.0; }),
        blocks::initialize<tags::AngularVelocity>([&i = n_times_called[++j]](...) { ++i; return 1.0; }),
        blocks::initialize<tags::Acceleration>([&i = n_times_called[++j]](...) { ++i; return 0.0; }),
        blocks::initialize<tags::AngularAcceleration>([&i = n_times_called[++j]](...) { ++i; return 0.0; })
      );

    Model::initialize(model, std::move(initializer));

    // check if initialization is done as expected
    auto expected_call_count = make_array(11U, 10U, 10U, 10U, 9U, 10U, 10U, 9U, 11U, 10U, 11U, 11U, 10U, 11U, 10U);
    CHECK_ITERABLE_APPROX(n_times_called, expected_call_count);

    // 1 + whatever
    auto computed_internal_forces = model.compute_internal_forces();
    auto computed_internal_torques = model.compute_internal_torques();
    auto cif_front = Traits::DataType::Vector::slice(computed_internal_forces, 0UL);
    auto cif_mid = Traits::DataType::Vector::slice(computed_internal_forces, 2UL);
    auto cit_col = Traits::DataType::Vector::slice(computed_internal_torques, 5UL);
    CHECK(cif_front == -2.00);
    CHECK(cif_mid == -4.00);
    CHECK(cit_col == Vec3({-M_PI / 4.0, M_PI / 4.0, 0.0}));
  }

}  // namespace
