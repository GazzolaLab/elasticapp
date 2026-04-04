//==============================================================================
/*!
//  \file
//  \brief DynamicCantilever case study
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#include "DynamicCantileverCaseStudy.hpp"

#include <elastica/Simulator/Materials.hpp>
#include <elastica/Utilities/Get.hpp>
#include <elastica/Utilities/Math/Rot3.hpp>
#include <elastica/Utilities/Math/Vec3.hpp>
//
#include <functional>
#include <utility>

void DynamicCantileverCaseStudy::setup(
    InputTypes<This> const& input_parameters) {
  if (simulator->ready()) {
    simulator->reset();
  }

  elastica::MaterialID my_material = elastica::create_material(
      // PyElastica values
      // material density
      ::elastica::get<density>(input_parameters),
      // coefficient of restitution
      0.24,
      // Young's modulus
      ::elastica::get<youngs_modulus>(input_parameters),
      // Shear modulus
      ::elastica::get<shear_modulus>(input_parameters),
      // The below parameters doesn't change the result of this example
      // Contact stiffness
      200,
      // normal contact damping
      1e-10,
      // tangent contact damping
      1e-10,
      // coefficient of static friction
      0.1,
      // cofficient of dynamic friction
      0.1);

  using StraightInit =
      ::elastica::cosserat_rod::StraightCosseratRodWithoutDampingInitializer;

  const ::elastica::Vec3 origin{0.0, 0.0, 0.0};
  const ::elastica::real_t base_length{
      ::elastica::get<length>(input_parameters)};
  const std::size_t n_elem(::elastica::get<n_elements>(input_parameters));

  StraightInit straight_initializer{
      // StraightInitialization Wrapper
      StraightInit::NElement{n_elem},
      StraightInit::Material{my_material},
      StraightInit::Radius{::elastica::get<radius>(input_parameters)},
      StraightInit::Length{base_length},
      StraightInit::Origin{origin},
      StraightInit::Direction{::elastica::Vec3{1.0, 0.0, 0.0}},
      StraightInit::Normal{::elastica::Vec3{0.0, 1.0, 0.0}},
  };

  rod_ref = simulator->emplace_back<CosseratRodWithoutDamping>(
      straight_initializer.initialize<CosseratRodWithoutDamping>());

  namespace eco = ::elastica::constraints;
  const std::size_t my_constraint_index = 0UL;
  auto constraint_generator = eco::make_constraint(
      eco::make_value_constraint(
          [origin, origin_frame = ::elastica::Rot3{::elastica::director(
                       rod_ref.get(), my_constraint_index)}](
              auto /*time*/, auto& rod_system, auto index) {
            ::elastica::position(rod_system, index) = origin;
            ::elastica::director(rod_system, index) = origin_frame;
          }),
      eco::make_rate_constraint(
          [](auto /*time*/, auto& rod_system, std::size_t index) {
            ::elastica::velocity(rod_system, index) = 0.0;
            ::elastica::angular_velocity(rod_system, index) = 0.0;
          }));

  simulator->constrain(rod_ref).at(0UL).with(constraint_generator());

  simulator->finalize();

  // Set the dt value
  set_dt(::elastica::get<dt>(input_parameters));
}

void DynamicCantileverCaseStudy::step() {
  simulator->step(time_stepper, get_current_time(), get_dt(),
                  get_current_iteration());
}

void DynamicCantileverCaseStudy::run() {
  throw std::runtime_error(
      "run() not implemented : launch the case from python!");
}
