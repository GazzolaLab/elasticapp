//
// Created by Tejaswin Parthasarathy on 5/3/21.
//
#include "Framework/TestingFramework.hpp"
#include "Systems/CosseratRods/Components/Elasticity/WithExplicitlyDampedDiagonalLinearHyperElasticModel.hpp"
#include "Systems/CosseratRods/Components/Geometry.hpp"
#include "Systems/CosseratRods/Components/Kinematics.hpp"
#include "Systems/CosseratRods/CosseratRodTraits.hpp"
#include "Systems/CosseratRods/Tags.hpp"
///
///
#include "Systems/CosseratRods/Mocks.hpp"
#include "Utilities/MakeArray.hpp"
#include "Utilities/Math/Identity.hpp"
#include "Utilities/Math/Rot3.hpp"
#include "Utilities/Math/Square.hpp"
#include "Utilities/Math/Vec3.hpp"

namespace {
  using namespace elastica;
  using namespace elastica::cosserat_rod;
  using namespace elastica::cosserat_rod::component;
  using Traits = typename elastica::cosserat_rod::DefaultCosseratRodTraits;

  template <typename B>
  struct TestGeometry : WithCircularCosseratRod<Traits, B> {};

  template <typename B>
  struct TestKinematics : WithRodKinematics<Traits, B> {};

  template <typename B>
  struct TestElasticity
      : WithExplicitlyDampedDiagonalLinearHyperElasticModel<Traits, B> {};

  using Plugin =
      mock::MockPlugin<Traits, TestGeometry, TestElasticity, TestKinematics>;
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
  ELASTICA_TEST_CASE(
      "Unit.Systems.CosseratRods.WithExplicitlyDampedDiagonalLinearHyperElasticModel.data",
      "[Systems][Unit]") {
    // clang-format on
    Model model{};

    constexpr auto n_initializers = 7UL;
    auto n_times_called = make_array<n_initializers>(0U);
    auto initializer = mock::initialize_block<Plugin>(
        // Geometry Variables
        blocks::initialize<tags::Position>(
            [&i = n_times_called[0]](std::size_t index) {
              ++i;
              return index * 1.0;
            }),
        blocks::initialize<tags::Director>([&i = n_times_called[1]](...) {
          ++i;
          return 0.5;
        }),
        blocks::initialize<tags::ElementDimension>(
            [&i = n_times_called[2]](...) {
              ++i;
              return 4.0;
            }),
        blocks::initialize<tags::ReferenceCurvature>(
            [&i = n_times_called[3]](...) {
              ++i;
              return 1.0;
            }),
        blocks::initialize<tags::ReferenceShearStretchStrain>(
            [&i = n_times_called[4]](...) {
              ++i;
              return 1.0;
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
        blocks::initialize<tags::ElementDilatation>([](...) { return 1.0; }),
        blocks::initialize<tags::Mass>([](...) { return 1.0; }),
        blocks::initialize<tags::Velocity>([](...) { return 1.0; }),
        blocks::initialize<tags::AngularVelocity>([](...) { return 1.0; }),
        blocks::initialize<tags::Acceleration>([](...) { return 3.0; }),
        blocks::initialize<tags::AngularAcceleration>([](...) { return 1.0; }),
        blocks::initialize<tags::ForceDampingCoefficient>(
            [](...) { return 2.0; }),
        blocks::initialize<tags::TorqueDampingCoefficient>(
            [](...) { return 2.0; }));

    Model::initialize(model, std::move(initializer));

    /* Check the function compute_internal_forces() */
    // clang-format off
    const Rot3 ssrm({
                        {16 * 4 * M_PI * 4 / 3, 0.0                  , 0.0},
                        {0.0                  , 16 * 4 * M_PI * 4 / 3, 0.0},
                        {0.0                  , 0.0                  , 16 * 4 * M_PI}});
    const Rot3 dir(0.5);
    const Vec3 shear_solution(
        {3 * (0.5 / sqrt(3)),
         3 * (0.5 / sqrt(3)),
         3 * (0.5 / sqrt(3)) - 1.0});
    const auto is = ssrm * (shear_solution-1.0);
    // clang-format on
    auto compute_internal_forces_result = model.compute_internal_forces();
    const Vec3 edge_force =
        Traits::DataType::Vector::slice(compute_internal_forces_result, 0UL);
    const Vec3 mid_force =
        Traits::DataType::Vector::slice(compute_internal_forces_result, 2UL);
    const Vec3 end_force =
        Traits::DataType::Vector::slice(compute_internal_forces_result, 10UL);
    CHECK(edge_force == dir * is - sqrt(3));
    CHECK(mid_force == -sqrt(3) * 2);
    CHECK(end_force == -(dir * is) - sqrt(3));

    /* Check the function compute_internal_torques() */
    // clang-format off
    const Rot3 btrm({
                        {4.0*(0.25*M_1_PI)*sq(4*4*M_PI), 0.0, 0.0},
                        {0.0, 4.0*(0.25*M_1_PI)*sq(4*4*M_PI), 0.0},
                        {0.0, 0.0, 2.0*4.0*(0.25*M_1_PI)*sq(4*4*M_PI)}});
    const auto couple_solution = btrm * Vec3({-1.0,-1.0,-1.0});
    const Vec3 q_into_t(
        {3 * (0.5 / sqrt(3)),
         3 * (0.5 / sqrt(3)),
         3 * (0.5 / sqrt(3))});
    const Vec3 ssc = sqrt(3) * (q_into_t % is) - (2*sqrt(3));
    const Rot3 msmoi({
                        {sqrt(3)*(0.25*M_1_PI)*sq(4*4*M_PI), 0.0, 0.0},
                        {0.0, sqrt(3)*(0.25*M_1_PI)*sq(4*4*M_PI), 0.0},
                        {0.0, 0.0, 2.0*sqrt(3)*(0.25*M_1_PI)*sq(4*4*M_PI)}});
    const auto joueb(msmoi * Vec3({1.0, 1.0, 1.0}));
    const auto ltb(joueb % Vec3({1.0, 1.0, 1.0}));
    // clang-format on

    auto compute_internal_torques_result = model.compute_internal_torques();
    const Vec3 edge_torque =
        Traits::DataType::Vector::slice(compute_internal_torques_result, 0UL);
    const Vec3 mid_torque =
        Traits::DataType::Vector::slice(compute_internal_torques_result, 2UL);
    const Vec3 end_torque =
        Traits::DataType::Vector::slice(compute_internal_torques_result, 9UL);
    CHECK(edge_torque == couple_solution + ssc + ltb);
    CHECK(mid_torque == ssc + ltb);
    CHECK(end_torque == -couple_solution + ssc + ltb);

    // check if initialization is done as expected
    auto expected_call_count = make_array(11U, 10U, 10U, 9U, 10U, 10U, 9U);
    CHECK_ITERABLE_APPROX(n_times_called, expected_call_count);

    // Check RigidityMatrix
    // [0] element : stiff  * I_0
    const double area = M_PI * 4.0 * 4.0;
    const double I_0 = area * area * 0.25 * M_1_PI;
  }

}  // namespace
