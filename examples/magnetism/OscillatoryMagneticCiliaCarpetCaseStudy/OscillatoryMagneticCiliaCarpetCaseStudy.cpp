//==============================================================================
/*!
//  \file
//  \brief Oscillatory magnetic cilia carpet study
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy, Yashraj Bhosale- All Rights
//  Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#include "OscillatoryMagneticCiliaCarpetCaseStudy.hpp"

#include <cmath>
#include <elastica/ErrorHandling/FloatingPointExceptions.hpp>
#include <elastica/Simulator/Materials.hpp>
#include <elastica/Utilities/Get.hpp>

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
  const std::size_t num_rods = ::elastica::get<n_rods>(input_parameters);
  const ::elastica::real_t cilia_carpet_spacing =
      ::elastica::get<carpet_spacing>(input_parameters);
  const ::elastica::real_t cilia_carpet_magnetization_spatial_wavelength =
      ::elastica::get<carpet_magnetization_spatial_wavelength>(
          input_parameters);
  const ::elastica::real_t cilia_carpet_magnetization_spatial_phase_difference =
      ::elastica::get<carpet_magnetization_spatial_phase_difference>(
          input_parameters);
  constexpr auto x_axis = 0UL;
  const ::elastica::real_t cilia_carpet_magnetization_spatial_wavenumber =
      2 * M_PI / cilia_carpet_magnetization_spatial_wavelength;

  for (std::size_t i = 0; i < num_rods; ++i) {
    ::elastica::Vec3 rod_origin{0.0, 0.0, 0.0};
    rod_origin[x_axis] += i * cilia_carpet_spacing;
    const ::elastica::real_t rod_magnetization_angle =
        cilia_carpet_magnetization_spatial_phase_difference +
        rod_origin[x_axis] * cilia_carpet_magnetization_spatial_wavenumber;
    using RodInitializer = ::elastica::cosserat_rod::
        StraightUniformMagnetizedCosseratRodInitializer;
    RodInitializer rod_initializer{
        // RodInitializerialization Wrapper
        RodInitializer::NElement{n_elem},
        RodInitializer::Material{my_material},
        RodInitializer::Radius{::elastica::get<radius>(input_parameters)},
        RodInitializer::Length{base_length},
        RodInitializer::Origin{rod_origin},
        RodInitializer::Direction{::elastica::Vec3{0.0, 0.0, 1.0}},
        RodInitializer::Normal{::elastica::Vec3{1.0, 0.0, 0.0}},
        RodInitializer::ForceDampingRate{
            ::elastica::get<damping_rate>(input_parameters)},
        RodInitializer::TorqueDampingRate{
            ::elastica::get<damping_rate>(input_parameters)},
        RodInitializer::MagnetizationDensity{
            ::elastica::get<magnetization_density>(input_parameters)},
        RodInitializer::UniformMagnetizationDirectionInWorldFrame{
            ::elastica::Vec3{std::sin(rod_magnetization_angle), 0.0,
                             std::cos(rod_magnetization_angle)}}};

    // This is to store
    RodRef rod_ref = simulator->emplace_back<MagnetizedCosseratRod>(
        rod_initializer.initialize<MagnetizedCosseratRod>());
    rod_refs.push_back(rod_ref);

    namespace eco = ::elastica::constraints;
    const std::size_t my_constraint_index = 0UL;
    auto constraint_generator = eco::make_constraint(
        eco::make_value_constraint(
            [origin = rod_origin,
             initial_director = ::elastica::Rot3{::elastica::director(
                 rod_ref.get(), my_constraint_index)}](
                auto /*time*/, auto& rod_system, auto index) {
              ::elastica::position(rod_system, index) = origin;
              ::elastica::director(rod_system, index) = initial_director;
            }),
        eco::make_rate_constraint(
            [](auto /*time*/, auto& rod_system, std::size_t index) {
              ::elastica::velocity(rod_system, index) = 0.0;
              ::elastica::angular_velocity(rod_system, index) = 0.0;
            }));

    simulator->constrain(rod_ref)
        .at(my_constraint_index)
        .with(constraint_generator());
  }

  auto& magnetic_field =
      simulator
          ->get_environment<::elastica::environments::tags::MagneticFieldTag>();

  // The field profile was set in the hpp file, can be modified if needed here
  const auto in_plane_amplitude =
      ::elastica::get<magnetic_field_in_plane_amplitude>(input_parameters);
  const auto out_of_plane_amplitude =
      ::elastica::get<magnetic_field_out_of_plane_amplitude>(input_parameters);
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
