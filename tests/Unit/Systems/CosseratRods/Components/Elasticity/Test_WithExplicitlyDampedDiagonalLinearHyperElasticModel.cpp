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

    // Reference Parameters
    const std::size_t dim = 3UL;
    const real_t moduli = 4.0;
    const real_t reference_curvature = 1.0;
    const real_t director_component = 0.5;
    const real_t velocity_component = 1.0;
    const real_t angular_velocity_component = 1.0;
    const real_t damping_rate_for_force = 2.0;
    const real_t damping_rate_for_torque = 3.5;

    constexpr auto n_initializers = 7UL;
    auto n_times_called = make_array<n_initializers>(0U);
    auto initializer = mock::initialize_block<Plugin>(
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
        blocks::initialize<tags::AngularAcceleration>([](...) { return 1.0; }),
        blocks::initialize<tags::ForceDampingRate>(
            [damping_rate_for_force](...) { return damping_rate_for_force; }),
        blocks::initialize<tags::TorqueDampingRate>(
            [damping_rate_for_torque](...) {
              return damping_rate_for_torque;
            }));

    Model::initialize(model, std::move(initializer));

    /* Check the function compute_internal_forces() */
    // clang-format off
    const real_t length(sqrt(3));
    const real_t area(M_PI * sq(4.0));
    const real_t density(::elastica::Iron().get_density());  // iron density
    const real_t I_0 = sq(area) * M_1_PI / 4.;
    const real_t reference_strain = 1.0;
    const real_t element_mass(density * length * area);
    const real_t damped_force = velocity_component * damping_rate_for_force * element_mass; // damping rate * mass * velocity
    const real_t ref_shape_factor(13.5 / 14.0);
    const real_t end_mass_correction_factor(0.5);
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
    auto compute_internal_forces_result = model.compute_internal_forces();
    const Vec3 edge_force =
        Traits::DataType::Vector::slice(compute_internal_forces_result, 0UL);
    const Vec3 mid_force =
        Traits::DataType::Vector::slice(compute_internal_forces_result, 2UL);
    const Vec3 end_force =
        Traits::DataType::Vector::slice(compute_internal_forces_result, 10UL);
    CHECK(edge_force == director * internal_stress -
                            damped_force * end_mass_correction_factor);
    CHECK(mid_force == -damped_force);
    CHECK(end_force == -(director * internal_stress) -
                           damped_force * end_mass_correction_factor);

    /* Check the function compute_internal_torques() */
    // clang-format off
    const Vec3 damped_torque =  // damping rate * mass * angular velocity
        Vec3({damping_rate_for_torque * element_mass,
              damping_rate_for_torque * element_mass,
              damping_rate_for_torque * element_mass});
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

    const auto compute_internal_torques_result =
        model.compute_internal_torques();
    const Vec3 edge_torque =
        Traits::DataType::Vector::slice(compute_internal_torques_result, 0UL);
    const Vec3 mid_torque =
        Traits::DataType::Vector::slice(compute_internal_torques_result, 2UL);
    const Vec3 end_torque =
        Traits::DataType::Vector::slice(compute_internal_torques_result, 9UL);
    // Remove 3./4. end-effector after fixing mass issue.
    /* 
    * We don't implement end-corrections to element mass computation as done in pyelastica 
    * https://github.com/GazzolaLab/PyElastica/blob/9a5928830561b79b58c3e442edbab9a49e51f371/elastica/rod/factory_function.py#L209-L210    
    * And hence need to account for it
    */
    const real_t pyelastica_correction_factor(3.0/4.0);
    CHECK(edge_torque == couple_solution + shear_stretch_couple +
                             lagrangian_transport - (damped_torque * pyelastica_correction_factor));
    CHECK(mid_torque ==
          shear_stretch_couple + lagrangian_transport - damped_torque);
    CHECK(end_torque == -couple_solution + shear_stretch_couple +
                            lagrangian_transport - (damped_torque * pyelastica_correction_factor));

    // check if initialization is done as expected
    auto expected_call_count = make_array(11U, 10U, 10U, 9U, 10U, 10U, 10U);
    CHECK_ITERABLE_APPROX(n_times_called, expected_call_count);
  }

}  // namespace
