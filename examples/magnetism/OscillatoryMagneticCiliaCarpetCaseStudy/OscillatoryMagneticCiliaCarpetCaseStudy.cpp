//==============================================================================
/*!
//  \file
//  \brief Oscillatory magnetic cilia carpet study
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy, Yashraj Bhosale- All Rights
//  Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#include "OscillatoryMagneticCiliaCarpetCaseStudy.hpp"

#include <cmath>
#include <iterator>
#include <elastica/ErrorHandling/FloatingPointExceptions.hpp>
#include <elastica/Simulator/Materials.hpp>
#include <elastica/Utilities/Get.hpp>
#include <set>
#include <tuple>

template <template <typename> class ViewOrSliceGenerator>
auto make_damping_constraint(
    blocks::BlockGenerator<ViewOrSliceGenerator<
        elastica::cosserat_rod::MagnetizedCosseratRodWithoutDamping>> const&
        generator,
    elastica::real_t damping_coeff_dt) {
  // setting analytical damper via constraint
  namespace eco = ::elastica::constraints;
  using elastica::real_t;

  auto rod = blocks::generate_from(generator);
  const real_t translational_mode_damping_coeff = std::exp(-damping_coeff_dt);
  constexpr std::size_t dim{3UL};
  const auto n_elem(std::size(rod));

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
            auto const n_elem(std::size(rod_system));
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

auto make_fixed_constraint(
    blocks::BlockSliceGenerator<
        elastica::cosserat_rod::MagnetizedCosseratRodWithoutDamping>
        rod_ref,
    std::size_t const constraint_index = 0UL) {
  const elastica::Vec3 rod_origin(
      ::elastica::position(rod_ref.get(), constraint_index));

  namespace eco = ::elastica::constraints;
  return eco::make_constraint(
      eco::make_value_constraint(
          [origin = rod_origin,
           initial_director = ::elastica::Rot3{::elastica::director(
               rod_ref.get(), constraint_index)}](
              auto /*time*/, auto& rod_system, std::size_t index) {
            ::elastica::position(rod_system, index) = origin;
            ::elastica::director(rod_system, index) = initial_director;
          }),
      eco::make_rate_constraint(
          [](auto /*time*/, auto& rod_system, std::size_t index) {
            ::elastica::velocity(rod_system, index) = 0.0;
            ::elastica::angular_velocity(rod_system, index) = 0.0;
          }))();
}

void OscillatoryMagneticCiliaCarpetCaseStudy::setup(
    InputTypes<This> const& input_parameters) {
  elastica::MaterialID my_material = elastica::create_material(
      // material density
      2.39e3,  // pyelastica values
      // coefficient of restitution
      0.24,
      // Young's modulus
      ::elastica::get<youngs_modulus>(input_parameters),
      // Shear modulus
      ::elastica::get<shear_modulus>(input_parameters),
      // Properties below don't matter
      // Contact stiffness
      200,
      // normal contact damping
      1e-10,
      // tangent contact damping
      1e-10,
      // coefficient of static friction
      0.1,
      // coefficient of dynamic friction
      0.1);

  const ::elastica::real_t base_length{
      ::elastica::get<length>(input_parameters)};
  const std::size_t n_elem = ::elastica::get<n_elements>(input_parameters);
  const std::size_t num_rods_along_x =
      ::elastica::get<n_rods_along_x>(input_parameters);
  const std::size_t num_rods_along_y =
      ::elastica::get<n_rods_along_y>(input_parameters);
  const std::size_t num_rods = num_rods_along_x * num_rods_along_y;
  const ::elastica::real_t cilia_carpet_spacing =
      ::elastica::get<carpet_spacing>(input_parameters);
  const ::elastica::real_t
      cilia_carpet_magnetization_spatial_wavelength_along_x =
          ::elastica::get<carpet_magnetization_spatial_wavelength_along_x>(
              input_parameters);
  const ::elastica::real_t
      cilia_carpet_magnetization_spatial_wavelength_along_y =
          ::elastica::get<carpet_magnetization_spatial_wavelength_along_y>(
              input_parameters);
  const ::elastica::real_t cilia_carpet_magnetization_spatial_phase_difference =
      ::elastica::get<carpet_magnetization_spatial_phase_difference>(
          input_parameters);
  constexpr auto x_axis = 0UL;
  constexpr auto y_axis = 1UL;
  const ::elastica::real_t
      cilia_carpet_magnetization_spatial_wavenumber_along_x =
          2 * M_PI / cilia_carpet_magnetization_spatial_wavelength_along_x;
  const ::elastica::real_t
      cilia_carpet_magnetization_spatial_wavenumber_along_y =
          2 * M_PI / cilia_carpet_magnetization_spatial_wavelength_along_y;

  for (std::size_t i = 0; i < num_rods; ++i) {
    ::elastica::Vec3 rod_origin{0.0, 0.0, 0.0};
    rod_origin[x_axis] += (i % num_rods_along_x) * cilia_carpet_spacing;
    rod_origin[y_axis] += (i / num_rods_along_x) * cilia_carpet_spacing;
    const ::elastica::real_t rod_magnetization_angle_along_x =
        cilia_carpet_magnetization_spatial_phase_difference +
        rod_origin[x_axis] *
            cilia_carpet_magnetization_spatial_wavenumber_along_x;
    const ::elastica::real_t rod_magnetization_angle_along_y =
        cilia_carpet_magnetization_spatial_phase_difference +
        rod_origin[y_axis] *
            cilia_carpet_magnetization_spatial_wavenumber_along_y;
    const ::elastica::Vec3 magnetization_vector{
        std::sin(rod_magnetization_angle_along_x),
        std::sin(rod_magnetization_angle_along_y),
        std::cos(rod_magnetization_angle_along_x) +
            std::cos(rod_magnetization_angle_along_y)};
    const ::elastica::Vec3 magnetization_direction =
        blaze::normalize(magnetization_vector);
    using RodInitializer = ::elastica::cosserat_rod::
        StraightUniformMagnetizedCosseratRodWithoutDampingInitializer;
    RodInitializer rod_initializer{
        // Rod Initializer Wrapper
        RodInitializer::NElement{n_elem},
        RodInitializer::Material{my_material},
        RodInitializer::Radius{::elastica::get<radius>(input_parameters)},
        RodInitializer::Length{base_length},
        RodInitializer::Origin{rod_origin},
        RodInitializer::Direction{::elastica::Vec3{0.0, 0.0, 1.0}},
        RodInitializer::Normal{::elastica::Vec3{1.0, 0.0, 0.0}},
        RodInitializer::MagnetizationDensity{
            ::elastica::get<magnetization_density>(input_parameters)},
        RodInitializer::UniformMagnetizationDirectionInWorldFrame{
            magnetization_direction}};

    // This is to store
    RodRef rod_ref = simulator->emplace_back<MagnetizedCosseratRod>(
        rod_initializer.initialize<MagnetizedCosseratRod>());
    rod_refs.push_back(rod_ref);
  }

  // Constraints
  {
    for (auto const& rod_ref : rod_refs) {
      const auto constraint_index(0UL);
      simulator->constrain(rod_ref)
          .at(constraint_index)
          .with(make_fixed_constraint(rod_ref, constraint_index));
    }

    auto const damping{::elastica::get<damping_rate>(input_parameters)};
    auto const dt(::elastica::get<dt>(input_parameters));
    // More painful than usual because the interface depends on having a view
    // generator and not a view itself.
    std::set<blocks::BlockRefGenerator<MagnetizedCosseratRod>> block_refs;
    std::transform(
        std::cbegin(rod_refs), std::cend(rod_refs),
        std::inserter(block_refs, std::begin(block_refs)),
        [](blocks::BlockSliceGenerator<MagnetizedCosseratRod> const& ref) {
          return ref.parent();
        });

    // Do it view-level rather than slice level.
    for (auto& block_ref : block_refs) {
      auto coeff = damping * dt;
      auto block_ref_view = blocks::slice(block_ref);
      simulator->constrain(block_ref_view)
          .at(0UL)
          .with(make_damping_constraint(block_ref_view, coeff));
    }

    auto& container = simulator->get_system_collection_container();
    auto const block_count = std::apply(
        [](auto&... cont) {
          std::size_t count = 0UL;
          EXPAND_PACK_LEFT_TO_RIGHT(count += cont.size());
          return count;
        },
        container);
    // std::cout << "Block count from Set: " << block_refs.size() << std::endl;
    // std::cout << "Block Count from Sim: " << block_count << '\n';
  }

  // Environments
  {
    auto& magnetic_field = simulator->get_environment<
        ::elastica::environments::tags::MagneticFieldTag>();

    // The field profile was set in the hpp file, can be modified if needed here
    const auto in_plane_amplitude =
        ::elastica::get<magnetic_field_in_plane_amplitude>(input_parameters);
    const auto out_of_plane_amplitude =
        ::elastica::get<magnetic_field_out_of_plane_amplitude>(
            input_parameters);
    const auto frequency =
        ::elastica::get<magnetic_field_angular_frequency>(input_parameters);

    magnetic_field->profile().field_amplitude() = ::elastica::Vec3{
        in_plane_amplitude, out_of_plane_amplitude, in_plane_amplitude};
    magnetic_field->profile().field_angular_frequency() =
        ::elastica::Vec3{frequency, 0.0, frequency};
    magnetic_field->profile().field_phase_difference() =
        ::elastica::Vec3{0.0, M_PI_2, M_PI_2};

    // can be parametrized if needed
    magnetic_field->profile().rampup_profile().start_time() = 0.0;
    magnetic_field->profile().rampup_profile().interval() =
        ::elastica::get<rampup_interval>(input_parameters);
  }

  final_time = ::elastica::get<final_sim_time>(input_parameters);
  simulator->finalize();

  // Set the dt value
  set_dt(::elastica::get<dt>(input_parameters));
}

void OscillatoryMagneticCiliaCarpetCaseStudy::step() {
  simulator->step(time_stepper, get_current_time(), get_dt(),
                  get_current_iteration());
}

void OscillatoryMagneticCiliaCarpetCaseStudy::run() {
  /* After a call to this function, the code will terminate with a floating
   point exception on overflow, divide-by-zero, and invalid operations.
   NOTE : This won't get called if used from the python side!
  */
  enable_floating_point_exceptions();

  const std::size_t n_steps = (final_time / current_dt());

  for (std::size_t i = 0UL; i < n_steps; ++i) {
    step();
  }
}
