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

    // Reference Parameters
    const std::size_t dim = 3UL;
    const real_t moduli = 4.0;
    const real_t reference_curvature = 1.0;
    const real_t director_component = 0.5;
    const real_t velocity_component = 1.0;
    const real_t angular_velocity_component = 1.0;

    constexpr auto n_initializers = 7UL;
    auto n_times_called = make_array<n_initializers>(0U);
    auto initializer = mock::initialize_block<Plugin>(
        // FIXME : Remove 10UL taken from mock::initialize_block
        blocks::initialize<tags::NElement>([]() { return 10UL; }),
        // Geometry Variables
        blocks::initialize<tags::Position>(
            [&i = n_times_called[0]](std::size_t index) {
              ++i;
              return index * 1.0;
            }),
        blocks::initialize<tags::Director>(
            [&i = n_times_called[1], director_component](...) {
              ++i;
              return director_component;
            }),
        blocks::initialize<tags::ElementDimension>(
            [&i = n_times_called[2]](...) {
              ++i;
              return 4.0;
            }),
        blocks::initialize<tags::ReferenceCurvature>(
            [&i = n_times_called[3], reference_curvature](...) {
              ++i;
              return reference_curvature;
            }),
        blocks::initialize<tags::ReferenceShearStretchStrain>(
            [&i = n_times_called[4]](...) {
              ++i;
              return 1.0;
            }),
        blocks::initialize<tags::ShearStretchRigidityMatrix>(
            [&i = n_times_called[5], moduli](...) {
              ++i;
              return moduli;
            }),
        blocks::initialize<tags::BendingTwistRigidityMatrix>(
            [&i = n_times_called[6], moduli](...) {
              ++i;
              return moduli;
            }),
        blocks::initialize<tags::ElementDilatation>([](...) { return 1.0; }),
        blocks::initialize<tags::Material>(
            [](...) { return ::elastica::iron; }),
        blocks::initialize<tags::Velocity>(
            [=](...) { return velocity_component; }),
        blocks::initialize<tags::AngularVelocity>(
            [=](...) { return angular_velocity_component; }),
        blocks::initialize<tags::Acceleration>([](...) { return 3.0; }),
        blocks::initialize<tags::AngularAcceleration>([](...) { return 1.0; }));

    // FIXME : https://github.com/tp5uiuc/elasticapp/issues/460
    // Don't use initialize on model
    Model::initialize(model, std::move(initializer));

    /* Check the function compute_internal_forces() */
    // clang-format off
    const real_t length(sqrt(3));
    const real_t area(M_PI * sq(4.0));
    const real_t density(::elastica::Iron().get_density());  // iron density
    const real_t I_0 = sq(area) * M_1_PI / 4.;
    const real_t reference_strain = 1.0;
    const real_t element_mass(density * length * area);
    const real_t ref_shape_factor(13.5 / 14.0);
    const Vec3 omega_vector({angular_velocity_component, angular_velocity_component, angular_velocity_component});
    const Rot3 shear_stretch_rigidity_matrix({ // G=E=4
      {area * moduli * ref_shape_factor, 0.0                             , 0.0},
      {0.0                             , area * moduli * ref_shape_factor, 0.0},
      {0.0                             , 0.0                             , area * moduli}});
    const Rot3 director(0.5);
    const Vec3 shear_solution({dim * (director_component / length), // director * tangent - z
                               dim * (director_component / length),
                               dim * (director_component / length) - 1.0});
    // clang-format on
    const Vec3 internal_stress =
        shear_stretch_rigidity_matrix * (shear_solution - reference_strain);
    compute_internal_forces(model);
    auto compute_internal_forces_result =
        blocks::get<::elastica::tags::InternalLoads>(model);
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
    const Rot3 BendTwistRigidityMatrix({{moduli*I_0, 0.0       , 0.0},
                                        {0.0       , moduli*I_0, 0.0},
                                        {0.0       , 0.0       , 2.0*moduli*I_0}});
    const Vec3 couple_solution = // Internal_couple solution
      BendTwistRigidityMatrix * (-Vec3({reference_curvature, reference_curvature, reference_curvature}));
    const Vec3 q_into_t({dim * (director_component / length),
                         dim * (director_component / length),
                         dim * (director_component / length)});
    const Vec3 shear_stretch_couple = sqrt(3) * (q_into_t % internal_stress);
    const Rot3 mass_second_moment_of_inertia({{density*length*I_0, 0.0, 0.0},
                                              {0.0, density*length*I_0, 0.0},
                                              {0.0, 0.0, 2.0*density*length*I_0}});
    const Vec3 j_omega_upon_e(mass_second_moment_of_inertia * omega_vector);
    const Vec3 lagrangian_transport(j_omega_upon_e % omega_vector);
    // clang-format on

    compute_internal_torques(model);
    const auto compute_internal_torques_result =
        blocks::get<::elastica::tags::InternalTorques>(model);
    const Vec3 edge_torque =
        Traits::DataType::Vector::slice(compute_internal_torques_result, 0UL);
    const Vec3 mid_torque =
        Traits::DataType::Vector::slice(compute_internal_torques_result, 2UL);
    const Vec3 end_torque =
        Traits::DataType::Vector::slice(compute_internal_torques_result, 9UL);
    // Remove 3./4. end-effector after fixing mass issue.
    CHECK(edge_torque ==
          couple_solution + shear_stretch_couple + lagrangian_transport);
    CHECK(mid_torque == shear_stretch_couple + lagrangian_transport);
    CHECK(end_torque ==
          -couple_solution + shear_stretch_couple + lagrangian_transport);

    // check if initialization is done as expected
    auto expected_call_count = make_array(11U, 10U, 10U, 9U, 10U, 10U, 10U);
    CHECK_ITERABLE_APPROX(n_times_called, expected_call_count);

    // Check RigidityMatrix
    // [0] element : stiff  * I_0

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
