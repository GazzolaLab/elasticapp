//==============================================================================
/*!
//  \file
//  \brief Oscillatory magnetic field case study
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy, Yashraj Bhosale- All Rights
//  Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#include "OscillatoryMagneticFieldCaseStudy.hpp"

#include <cmath>
#include <elastica/ErrorHandling/FloatingPointExceptions.hpp>
#include <elastica/Simulator/Materials.hpp>
#include <elastica/Utilities/Get.hpp>

void OscillatoryMagneticFieldCaseStudy::setup(
    InputTypes<This> const& input_parameters) {
  if (simulator->ready()) {
    simulator->reset();
  }

  elastica::MaterialID my_material = elastica::create_material(
      // material density
      5e3,  // pyelastica values
      // coefficient of restitution
      0.24,
      // Young's modulus
      1e6,
      // Shear modulus
      5e5,
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

  const ::elastica::Vec3 origin{0.0, 0.0, 0.0};
  const ::elastica::real_t base_length{
      ::elastica::get<length>(input_parameters)};
  const std::size_t n_elem = ::elastica::get<n_elements>(input_parameters);

  using RodInitializer =
      ::elastica::cosserat_rod::StraightUniformMagnetizedCosseratRodInitializer;
  RodInitializer rod_initializer{
      // RodInitializerialization Wrapper
      RodInitializer::NElement{n_elem},
      RodInitializer::Material{my_material},
      RodInitializer::Radius{::elastica::get<radius>(input_parameters)},
      RodInitializer::Length{base_length},
      RodInitializer::Origin{origin},
      RodInitializer::Direction{::elastica::Vec3{1.0, 0.0, 0.0}},
      RodInitializer::Normal{::elastica::Vec3{0.0, 1.0, 0.0}},
      RodInitializer::ForceDampingCoefficient{10},
      RodInitializer::TorqueDampingCoefficient{10},
      RodInitializer::MagnetizationDensity{
          ::elastica::get<magnetization_density>(input_parameters)},
      RodInitializer::UniformMagnetizationDirectionInWorldFrame{
          ::elastica::Vec3{1.0, 0.0, 0.0}}};

  // This is to store
  rod_refs[0UL] = simulator->emplace_back<MagnetizedCosseratRod>(
      rod_initializer.initialize<MagnetizedCosseratRod>());
  auto rod_ref = rod_refs[0UL];

  namespace eco = ::elastica::constraints;
  const std::size_t my_constraint_index = 0UL;
  auto constraint_generator = eco::make_constraint(
      eco::make_value_constraint(
          [origin, initial_director = ::elastica::Rot3{::elastica::director(
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

  auto& magnetic_field =
      simulator
          ->get_environment<::elastica::environments::tags::MagneticFieldTag>();

  // The field profile was set in the hpp file, can be modified if needed here
  const auto amp = ::elastica::get<magnetic_field_amplitude>(input_parameters);
  const auto ang = ::elastica::get<magnetic_field_angle>(input_parameters);

  simulator->get_environment<::elastica::environments::tags::MagneticFieldTag>()
      ->profile()
      .field_vector() =
      ::elastica::Vec3{amp * std::cos(ang), amp * std::sin(ang), 0.0};

  // can be parametrized if needed
  magnetic_field->profile().rampup_profile().start_time() = 0.0;
  magnetic_field->profile().rampup_profile().interval() =
      ::elastica::get<rampup_interval>(input_parameters);

#ifdef ELASTICA_DEBUG
  std::cout << magnetic_field->profile().field_vector() << std::endl;
  std::cout << magnetic_field->profile().rampup_profile().start_time()
            << std::endl;
  std::cout << magnetic_field->profile().rampup_profile().interval()
            << std::endl;
#endif ELASTICA_DEBUG

  simulator->finalize();

  // Set the dt value
  set_dt(::elastica::get<dt>(input_parameters));
}

void OscillatoryMagneticFieldCaseStudy::step() {
  simulator->step(time_stepper, get_current_time(), get_dt(),
                  get_current_iteration());
}

void OscillatoryMagneticFieldCaseStudy::run() {
  /* After a call to this function, the code will terminate with a floating
   point exception on overflow, divide-by-zero, and invalid operations.
   NOTE : This won't get called if used from the python side!
  */
  enable_floating_point_exceptions();

  auto& magnetic_field =
      simulator
          ->get_environment<::elastica::environments::tags::MagneticFieldTag>();

  // TODO : replace with event and triggers
  const ::elastica::real_t final_time(500.0);
  const std::size_t n_steps = (final_time / current_dt());

  for (std::size_t i = 0UL; i < n_steps; ++i) {
    step();
  }
}
