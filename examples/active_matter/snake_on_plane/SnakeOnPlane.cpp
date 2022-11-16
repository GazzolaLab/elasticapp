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

template <typename Traits, template <typename> class ComputationalBlock,
          template <typename /*CRT*/, typename /*InitializedBlock*/>
          class... Components>
auto make_damping_constraint(
    elastica::cosserat_rod::CosseratRodPlugin<Traits, ComputationalBlock,
                                              Components...> const& rod_like,
    elastica::real_t damping_coeff_dt) {
  // setting analytical damper via constraint
  namespace eco = ::elastica::constraints;
  using elastica::real_t;
  auto const& rod = rod_like.self();

  const real_t translational_mode_damping_coeff = std::exp(-damping_coeff_dt);
  constexpr std::size_t dim{3UL};
  const auto n_elem(cpp17::size(rod));

  auto&& mass = ::elastica::get<elastica::tags::Mass>(rod);

  blaze::DynamicMatrix<real_t> rotational_mode_damping_coeff(dim, n_elem);

  for (std::size_t row_idx = 0; row_idx < dim; row_idx++) {
    for (std::size_t column_idx = 0; column_idx < n_elem; column_idx++) {
      real_t elemental_mass = 0.5 * (mass[column_idx] + mass[column_idx + 1]);
      if (column_idx == 0)
        elemental_mass += 0.5 * mass[column_idx];
      if (column_idx == (n_elem - 1))
        elemental_mass += 0.5 * mass[column_idx + 1];

      rotational_mode_damping_coeff(row_idx, column_idx) = std::exp(
          -damping_coeff_dt * elemental_mass *
          ::elastica::get<::elastica::tags::InvMassSecondMomentOfInertia>(rod)(
              row_idx, column_idx));
    }
  }

  return eco::make_constraint(
      eco::make_value_constraint(
          [](auto /*time*/, auto& /*rod_system*/, std::size_t /*index*/) {}),
      eco::make_rate_constraint(
          [translational_mode_damping_coeff, rotational_mode_damping_coeff](
              auto /*time*/, auto& rod_system, std::size_t) {
            ::elastica::velocity(rod_system) *=
                translational_mode_damping_coeff;
            // Easier to encode as powers
            auto const n_elem(cpp17::size(rod_system));
            auto&& omega = ::elastica::angular_velocity(rod_system);
            auto&& dil =
                ::blocks::get<elastica::tags::ElementDilatation>(rod_system);

            for (auto idx_elem = 0UL; idx_elem < n_elem; ++idx_elem) {
              omega(0UL, idx_elem) *= std::pow(
                  rotational_mode_damping_coeff(0UL, idx_elem), dil[idx_elem]);
              omega(1UL, idx_elem) *= std::pow(
                  rotational_mode_damping_coeff(1UL, idx_elem), dil[idx_elem]);
              omega(2UL, idx_elem) *= std::pow(
                  rotational_mode_damping_coeff(2UL, idx_elem), dil[idx_elem]);
            }
          }))();
}

template <class ViewOrSliceGenerator>
decltype(auto) make_damping_constraint(
    blocks::BlockGenerator<ViewOrSliceGenerator> const& generator,
    elastica::real_t damping_coeff_dt) {
  return make_damping_constraint(blocks::generate_from(generator),
                                 damping_coeff_dt);
}

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
      ::elastica::cosserat_rod::StraightCosseratRodWithoutDampingInitializer;
  RodInitializer straight_initializer{
      RodInitializer::NElement{n_elem},
      RodInitializer::Material{my_material},
      RodInitializer::Radius{::elastica::get<radius>(input_parameters)},
      RodInitializer::Length{base_length},
      RodInitializer::Origin{anchor},
      RodInitializer::Direction{direction},
      RodInitializer::Normal{normal},
  };

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

  // Damp them
  // maybe needs to be scaled later
  add_analytical_damper(cosserat_rod_refs, ::elastica::get<damping_rate>(input_parameters),
                        ::elastica::get<dt>(input_parameters));

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

template <typename Plugin>
void SnakeOnPlane::add_analytical_damper(
    std::vector<blocks::BlockSliceGenerator<Plugin>> const& generators,
    elastica::real_t damping_rate, elastica::real_t dt) {
  // setting analytical damper via constraint
  namespace eco = ::elastica::constraints;

  auto const coeff = damping_rate * dt;

  std::set<blocks::BlockRefGenerator<Plugin>> block_refs;
  std::transform(std::cbegin(generators), std::cend(generators),
                 std::inserter(block_refs, std::begin(block_refs)),
                 [](blocks::BlockSliceGenerator<Plugin> const& ref) {
                   return ref.parent();
                 });

  // Do it view-level rather than slice level.
  for (auto& block_ref : block_refs) {
    auto block_ref_view = blocks::slice(block_ref);
    simulator->constrain(block_ref_view)
        .at(0UL)
        .with(make_damping_constraint(block_ref_view, coeff));
  }
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
