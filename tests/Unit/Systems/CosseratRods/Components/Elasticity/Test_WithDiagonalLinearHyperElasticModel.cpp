//
// Created by Tejaswin Parthasarathy on 5/3/21.
//
#include "Framework/TestingFramework.hpp"
#include "Systems/CosseratRods/Components/Elasticity/WithDiagonalLinearHyperElasticModel.hpp"
#include "Systems/CosseratRods/Components/Geometry.hpp"
#include "Systems/CosseratRods/Components/Kinematics.hpp"
#include "Systems/CosseratRods/Components/Tags.hpp"
#include "Systems/CosseratRods/CosseratRodTraits.hpp"
///
///
#include "Systems/CosseratRods/Mocks.hpp"
#include "Utilities/MakeArray.hpp"
#include "Utilities/Math/Rot3.hpp"
#include "Utilities/Math/Sqrt.hpp"
#include "Utilities/Math/Square.hpp"
#include "Utilities/Math/Vec3.hpp"

namespace {
  using namespace elastica;
  using namespace elastica::cosserat_rod;

  using Traits = typename elastica::cosserat_rod::DefaultCosseratRodTraits;

  template <typename B>
  struct TestGeometry : public component::WithCircularCosseratRod<Traits, B> {};

  template <typename B>
  struct TestKinematics : public component::WithRodKinematics<Traits, B> {};

  template <typename B>
  struct TestElasticity
      : public component::WithDiagonalLinearHyperElasticModel<Traits, B> {};

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

  ELASTICA_TEST_CASE(
      "Unit.Systems.CosseratRods.WithDiagonalLinearHyperElasticModel.EBO",
      "[Systems][Unit]") {
    EMPTY_TEST();

    CHECK_EBO(Model);
  }

  ELASTICA_TEST_CASE(
      "Unit.Systems.CosseratRods.WithDiagonalLinearHyperElasticModel.data",
      "[Systems][Unit]") {
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
        // blocks::initialize<tags::Mass>([](...) { return 1.0; }),
        blocks::initialize<tags::Material>(
            [](...) { return ::elastica::iron; }),
        blocks::initialize<tags::Velocity>([](...) { return 0.0; }),
        blocks::initialize<tags::AngularVelocity>([](...) { return 0.0; }),
        blocks::initialize<tags::Acceleration>([](...) { return 0.0; }),
        blocks::initialize<tags::AngularAcceleration>([](...) { return 1.0; }));

    Model::initialize(model, std::move(initializer));

    /* Check the function compute_internal_forces() */
    // clang-format off
    const double ref_shape_factor = (13.5 / 14.);
    const Rot3 shear_stretch_rigidity_matrix({
      {16 * 4 * M_PI * ref_shape_factor, 0.0                              , 0.0},
      {0.0                             , 16 * 4 * M_PI * ref_shape_factor},
      {0.0                             , 0.0                              , 16 * 4 * M_PI}});
    const Rot3 director(0.5);
    const Vec3 shear_solution(
      {3 * (0.5 / sqrt(3)),
       3 * (0.5 / sqrt(3)),
       3 * (0.5 / sqrt(3)) - 1.0});
    const Vec3 internal_stress = shear_stretch_rigidity_matrix * (shear_solution-1.0);
    // clang-format on
    auto compute_internal_forces_result = model.compute_internal_forces();
    const Vec3 edge_force =
        Traits::DataType::Vector::slice(compute_internal_forces_result, 0UL);
    const Vec3 mid_force =
        Traits::DataType::Vector::slice(compute_internal_forces_result, 2UL);
    const Vec3 end_force =
        Traits::DataType::Vector::slice(compute_internal_forces_result, 10UL);
    CHECK(edge_force == director * internal_stress);
    CHECK(mid_force == 0.0);
    CHECK(end_force == -(director * internal_stress));

    /* Check the function compute_internal_torques() */
    // clang-format off
    const Rot3 BendTwistRigidityMatrix({
      {4.0*(0.25*M_1_PI)*sq(4*4*M_PI), 0.0, 0.0},
      {0.0, 4.0*(0.25*M_1_PI)*sq(4*4*M_PI), 0.0},
      {0.0, 0.0, 2.0*4.0*(0.25*M_1_PI)*sq(4*4*M_PI)}});
    const Vec3 couple_solution = BendTwistRigidityMatrix * Vec3({-1.0,-1.0,-1.0});
    const Vec3 q_into_t(
        {3 * (0.5 / sqrt(3)),
         3 * (0.5 / sqrt(3)),
         3 * (0.5 / sqrt(3))});
    const Vec3 shear_stretch_couple = sqrt(3) * (q_into_t % internal_stress);
    // clang-format on

    const auto compute_internal_torques_result =
        model.compute_internal_torques();
    const Vec3 edge_torque =
        Traits::DataType::Vector::slice(compute_internal_torques_result, 0UL);
    const Vec3 mid_torque =
        Traits::DataType::Vector::slice(compute_internal_torques_result, 2UL);
    const Vec3 end_torque =
        Traits::DataType::Vector::slice(compute_internal_torques_result, 9UL);
    CHECK(edge_torque == couple_solution + shear_stretch_couple);
    CHECK(mid_torque == shear_stretch_couple);
    CHECK(end_torque == -couple_solution + shear_stretch_couple);

    // check if initialization is done as expected
    auto expected_call_count = make_array(11U, 10U, 10U, 9U, 10U, 10U, 10U);
    CHECK_ITERABLE_APPROX(n_times_called, expected_call_count);

    // Check RigidityMatrix
    // [0] element : stiff  * I_0
    const double area = M_PI * 4.0 * 4.0;
    const double I_0 = area * area * 0.25 * M_1_PI;

    auto ssr_matrix = blocks::get<tags::ShearStretchRigidityMatrix>(model);
    auto ssr_I0 = Traits::DataType::Matrix::slice(ssr_matrix, 2UL)(0UL, 0UL);
    auto ssr_I1 = Traits::DataType::Matrix::slice(ssr_matrix, 2UL)(1UL, 1UL);
    auto ssr_I2 = Traits::DataType::Matrix::slice(ssr_matrix, 2UL)(2UL, 2UL);
    auto ssr_Id0 = Traits::DataType::Matrix::slice(ssr_matrix, 3UL)(0UL, 1UL);
    auto ssr_Id1 = Traits::DataType::Matrix::slice(ssr_matrix, 3UL)(0UL, 2UL);
    auto ssr_Id2 = Traits::DataType::Matrix::slice(ssr_matrix, 3UL)(1UL, 2UL);
    auto ssr_Id3 = Traits::DataType::Matrix::slice(ssr_matrix, 4UL)(1UL, 0UL);
    auto ssr_Id4 = Traits::DataType::Matrix::slice(ssr_matrix, 4UL)(2UL, 0UL);
    auto ssr_Id5 = Traits::DataType::Matrix::slice(ssr_matrix, 4UL)(2UL, 1UL);
    CHECK(ssr_I0 == 4.0 * area * ref_shape_factor);
    CHECK(ssr_I1 == 4.0 * area * ref_shape_factor);
    CHECK(ssr_I2 == 4.0 * area);
    CHECK(ssr_Id0 == 0.0);
    CHECK(ssr_Id1 == 0.0);
    CHECK(ssr_Id2 == 0.0);
    CHECK(ssr_Id3 == 0.0);
    CHECK(ssr_Id4 == 0.0);
    CHECK(ssr_Id5 == 0.0);

    auto btr_matrix = blocks::get<tags::BendingTwistRigidityMatrix>(model);
    auto btr_I0 = Traits::DataType::Matrix::slice(btr_matrix, 2UL)(0UL, 0UL);
    auto btr_I1 = Traits::DataType::Matrix::slice(btr_matrix, 2UL)(1UL, 1UL);
    auto btr_I2 = Traits::DataType::Matrix::slice(btr_matrix, 2UL)(2UL, 2UL);
    auto btr_Id0 = Traits::DataType::Matrix::slice(btr_matrix, 3UL)(0UL, 1UL);
    auto btr_Id1 = Traits::DataType::Matrix::slice(btr_matrix, 3UL)(0UL, 2UL);
    auto btr_Id2 = Traits::DataType::Matrix::slice(btr_matrix, 3UL)(1UL, 2UL);
    auto btr_Id3 = Traits::DataType::Matrix::slice(btr_matrix, 4UL)(1UL, 0UL);
    auto btr_Id4 = Traits::DataType::Matrix::slice(btr_matrix, 4UL)(2UL, 0UL);
    auto btr_Id5 = Traits::DataType::Matrix::slice(btr_matrix, 4UL)(2UL, 1UL);
    CHECK(btr_I0 == approx(4.0 * I_0));
    CHECK(btr_I1 == approx(4.0 * I_0));
    CHECK(btr_I2 == approx(2.0 * 4.0 * I_0));
    CHECK(btr_Id0 == 0.0);
    CHECK(btr_Id1 == 0.0);
    CHECK(btr_Id2 == 0.0);
    CHECK(btr_Id3 == 0.0);
    CHECK(btr_Id4 == 0.0);
    CHECK(btr_Id5 == 0.0);
  }

}  // namespace
