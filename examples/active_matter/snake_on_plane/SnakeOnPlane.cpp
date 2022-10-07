//==============================================================================
/*!
//  \file
//  \brief Snake on plane
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#include "SnakeOnPlane.hpp"

#include <elastica/ErrorHandling/FloatingPointExceptions.hpp>
#include <elastica/Simulator/Forcing/InternalTorquesAdapter.hpp>
#include <elastica/Simulator/Materials.hpp>
#include <elastica/Utilities/Get.hpp>
#include <elastica/Utilities/Math/Identity.hpp>
#include <elastica/Utilities/Math/Vec3.hpp>
#include <elastica/Utilities/Rampup.hpp>

#include "Activation/Funcs.hpp"
//
#include <cmath>

void SnakeOnPlane::setup(InputTypes<This> const& input_parameters) {
  elastica::MaterialID my_material = elastica::create_material(
      // material density
      ::elastica::get<density>(input_parameters),
      // coefficient of restitution
      0.24,
      // Young's modulus
      ::elastica::get<youngs_modulus>(input_parameters),
      // Shear modulus
      ::elastica::get<shear_modulus>(input_parameters),
      // Contact stiffness
      ::elastica::get<contact_stiffness>(input_parameters),
      // normal contact damping
      ::elastica::get<contact_damping>(input_parameters),
      // tangent contact damping
      ::elastica::get<contact_damping>(input_parameters),
      // coefficient of static friction
      1e-10,
      // coefficient of dynamic friction
      1e-10);

  const ::elastica::Vec3 anchor{0., 0., 0.};
  const ::elastica::Vec3 rod_plane_offset{
      3.0 * ::elastica::get<radius>(input_parameters), 0., 0.};

  const auto inclination(0.0);
  // Normal along y, Binormal along -x, Tangent along z
  const ::elastica::Vec3 direction{0.0, std::sin(inclination),
                                   std::cos(inclination)};
  const ::elastica::Vec3 normal{0.0, std::cos(inclination),
                                -std::sin(inclination)};

  // const ::elastica::Vec3 origin{0.0, 0.0, 0.0};
  const ::elastica::real_t base_length{
      ::elastica::get<length>(input_parameters)};
  const auto number_of_snakes = ::elastica::get<n_snakes>(input_parameters);
  const std::size_t n_elem(::elastica::get<n_elements>(input_parameters));

  using RodInitializer =
      ::elastica::cosserat_rod::StraightCosseratRodInitializer;
  RodInitializer straight_initializer{
      RodInitializer::NElement{n_elem}, RodInitializer::Material{my_material},
      RodInitializer::Radius{::elastica::get<radius>(input_parameters)},
      RodInitializer::Length{base_length}, RodInitializer::Origin{anchor},
      RodInitializer::Direction{direction}, RodInitializer::Normal{normal},
      // maybe needs to be scaled later
      RodInitializer::ForceDampingRate{0.1},
      RodInitializer::TorqueDampingRate{0.1}};

  cosserat_rod_refs.resize(number_of_snakes,
                           simulator->get_invalid_ref<CosseratRod>());

  for (auto snake_idx = 0UL; snake_idx < number_of_snakes; ++snake_idx) {
    cosserat_rod_refs[snake_idx] =
        simulator->template emplace_back<CosseratRod>(
            straight_initializer
                .but_with(RodInitializer::Origin{
                    anchor + ::elastica::real_t(snake_idx) * rod_plane_offset})
                .initialize<CosseratRod>());
  }

  const auto T = ::elastica::get<time_period>(input_parameters);
  const auto lambda = ::elastica::get<wave_length>(input_parameters);

  // Numbers from pyelastica snake.
  std::vector<double> X = {0.050000, 0.130000, 0.210000, 0.290000,
                           0.370000, 0.450000, 0.530000, 0.610000,
                           0.690000, 0.770000, 0.850000, 0.930000};
  std::vector<double> Y = {0.003394, 0.003474, 0.003604, 0.003697,
                           0.003673, 0.003525, 0.003330, 0.003170,
                           0.003126, 0.003219, 0.003371, 0.003489};

  // Spatio-temporal actuation
  const auto omega = elastica::real_t(2.0 * M_PI) / T;
  const auto wavenumber = elastica::real_t(2.0 * M_PI) / lambda;
  auto const phase_shift(0.0);

  namespace activation = ::elastica::forcing::activation;

  activation::Sinusoid wave{omega, wavenumber, phase_shift};
  auto spline_activation(activation::mark_spatial_activation(
      activation::InterpolatingSpline(std::move(X), std::move(Y))));
  auto ramp(activation::mark_temporal_activation(
      elastica::make_linear_rampup(0.0, T)));

  // copy it here (why not?)
  auto forcing_amplitude(activation::multiply(wave, spline_activation, ramp));

  auto forcing_func(
      [=, forcing_direction = normal](elastica::real_t s,
                                      elastica::real_t t) -> elastica::Vec3 {
        return forcing_amplitude(s, t) * forcing_direction;
      });

  namespace efo = ::elastica::forcing;
  auto snake_forcing_gen = [&]() {
    return efo::as_internal_torques(forcing_func);
  };

  // Force them
  for (auto snake_idx = 0UL; snake_idx < number_of_snakes; ++snake_idx) {
    simulator->force(cosserat_rod_refs[snake_idx])
        .at(::elastica::interface::range(0UL, n_elem))
        .with(snake_forcing_gen());
  }

  const ::elastica::Vec3 gravity_acc_vector =
      ::elastica::get<gravitational_acc>(input_parameters) * normal;
  auto& gravity_field =
      simulator->get_environment<elastica::environments::tags::GravityField>();
  gravity_field->gravity() = gravity_acc_vector;

  // half-space
  const ::elastica::Vec3 half_space_anchor{0.0, 0.0, 0.0};
  auto half_space_normal = normal;
  auto half_space_tangent = direction;
  auto half_space_binormal = cross(half_space_tangent, half_space_normal);

  // implicit convention
  ::elastica::Rot3 Q = {
      {half_space_normal[0], half_space_normal[1], half_space_normal[2]},
      {half_space_binormal[0], half_space_binormal[1], half_space_binormal[2]},
      {half_space_tangent[0], half_space_tangent[1], half_space_tangent[2]}};

  namespace ek = elastica::kinematics;

  auto always_zero_ = [](::elastica::real_t) noexcept -> ::elastica::real_t {
    return 0.0;
  };
  auto displaced_by_radius =
      [r = ::elastica::get<radius>(input_parameters)](
          ::elastica::real_t) noexcept -> ::elastica::real_t { return -r; };

  //  elastica::MaterialID plane_material = elastica::create_material(
  //      // material density
  //      ::elastica::get<density>(input_parameters),
  //      // coefficient of restitution
  //      0.24,
  //      // Young's modulus
  //      ::elastica::get<youngs_modulus>(input_parameters),
  //      // Shear modulus
  //      ::elastica::get<shear_modulus>(input_parameters),
  //      // Contact stiffness
  //      1e6, /
  //      // normal contact damping
  //      ::elastica::get<contact_damping>(input_parameters),
  //      // tangent contact damping
  //      ::elastica::get<contact_damping>(input_parameters),
  //      // coefficient of static friction
  //      1e-10,
  //      // coefficient of dynamic friction
  //      1e-10);

  auto translations_buffer = ek::make_translations(
      ek::translate_along<elastica::EulerianFrame::X>(
          ek::mark_position(always_zero_), ek::mark_velocity(always_zero_)),
      ek::translate_along<elastica::EulerianFrame::Y>(
          // The COM along z axis is not at 0 (the anchor position), but rather
          // inside the halfspace, so we displace it by anchor_to_com.
          ek::mark_position(
              ImposedHalfSpace::anchor_to_com_n(displaced_by_radius)),
          ek::mark_velocity(always_zero_)),
      ek::translate_along<elastica::EulerianFrame::Z>(
          ek::mark_position(always_zero_), ek::mark_velocity(always_zero_)));
  // Use same material for now.
  imposed_half_space_ref = simulator->emplace_back<ImposedHalfSpace>(
      ::elastica::collision::HalfSpacePrimitive(
          0UL, 0UL, true, half_space_anchor, Q, my_material),
      std::move(translations_buffer));

  simulator->finalize();

  // Set the dt value
  set_dt(::elastica::get<dt>(input_parameters));
}

void SnakeOnPlane::step() {
  simulator->step(time_stepper, get_current_time(), get_dt(),
                  get_current_iteration());
}

void SnakeOnPlane::run() {
  /* After a call to this function, the code will terminate with a floating
   point exception on overflow, divide-by-zero, and invalid operations.
   NOTE : This won't get called if used from the python side!
  */
  enable_floating_point_exceptions();
  const ::elastica::real_t final_time(20.0);
  const std::size_t n_steps = (final_time / current_dt());

  for (std::size_t i = 0UL; i < n_steps; ++i) {
    step();
  }
}
