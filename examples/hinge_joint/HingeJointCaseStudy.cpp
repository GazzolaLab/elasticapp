//==============================================================================
/*!
//  \file
//  \brief Hinge joint case study
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#include "HingeJointCaseStudy.hpp"

#include <cmath>
#include <elastica/Simulator/Materials.hpp>
#include <elastica/Utilities/Get.hpp>

void HingeJointCaseStudy::setup(InputTypes<This> const& input_parameters) {
  //  void HingeJointCaseStudy::setup(
  //      InputTypes<HingeJointCaseStudy> const& input_parameters) {
  elastica::MaterialID my_material = elastica::create_material(
      // material density
      1750,  // pyelastica values
      // coefficient of restitution
      0.24,
      // Young's modulus
      3e7,
      // Shear modulus
      2e7,
      // Parameters below don't matter
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

  const ::elastica::real_t base_length{
      ::elastica::get<length>(input_parameters)};

  const ::elastica::Vec3 link_direction{0.0, 0.0, 1.0};
  const ::elastica::Vec3 link_normal{0.0, 1.0, 0.0};
  const ::elastica::Vec3 link_one_origin{0.0, 0.0, 0.0};
  const ::elastica::Vec3 link_two_origin =
      link_one_origin + base_length * link_direction;

  const std::size_t n_elem = ::elastica::get<n_elements>(input_parameters);

  using StraightInit = ::elastica::cosserat_rod::StraightCosseratRodInitializer;
  StraightInit straight_initializer{
      // StraightInitialization Wrapper
      StraightInit::NElement{n_elem},
      StraightInit::Material{my_material},
      StraightInit::Radius{::elastica::get<radius>(input_parameters)},
      StraightInit::Length{base_length},
      StraightInit::Origin{link_one_origin},
      StraightInit::Direction{link_direction},
      StraightInit::Normal{link_normal},
      StraightInit::ForceDampingCoefficient{0.1},
      StraightInit::TorqueDampingCoefficient{0.1},
  };

  first_rod_it = simulator->emplace_back<CosseratRod>(
      straight_initializer.initialize<CosseratRod>());
  second_rod_it = simulator->emplace_back<CosseratRod>(
      straight_initializer.but_with(StraightInit::Origin{link_two_origin})
          .initialize<CosseratRod>());

  namespace eco = ::elastica::constraints;
  const std::size_t my_constraint_index = 0UL;
  auto constraint_generator = eco::make_constraint(
      eco::make_value_constraint(
          [origin = link_one_origin, origin_frame = ::elastica::director(
                                         *first_rod_it, my_constraint_index)](
              auto /*time*/, auto& rod_system, auto index) {
            ::elastica::position(rod_system, index) = origin;
            ::elastica::director(rod_system, index) = origin_frame;
          }),
      eco::make_rate_constraint(
          [](auto /*time*/, auto& rod_system, std::size_t index) {
            ::elastica::velocity(rod_system, index) = 0.0;
            ::elastica::angular_velocity(rod_system, index) = 0.0;
          }));

  simulator->constrain(*first_rod_it).at(0UL).with(constraint_generator());

  namespace efo = ::elastica::forcing;

  const ::elastica::real_t end_force_magnitude(5.0);
  const ::elastica::real_t rampup_time(0.2);

  auto force_generator = efo::make_forcing(
      efo::make_force(
          [=, normal_direction = link_normal,
           roll_direction = ::elastica::Vec3(link_normal % link_direction)](
              auto time_v, auto& rod_system, auto index) {
            const auto time_d = static_cast<::elastica::real_t>(time_v);
            ::elastica::Vec3 end_force;
            if (time_d < rampup_time) {
              end_force = -2.0 * end_force_magnitude * normal_direction;
            } else {
              const ::elastica::Vec3 end_roll_forces =
                  end_force_magnitude *
                  std::cos(M_PI_2 * (time_d - rampup_time)) * roll_direction;
              const ::elastica::Vec3 end_normal_forces =
                  end_force_magnitude *
                  std::sin(M_PI_2 * (time_d - rampup_time)) * normal_direction;
              end_force = end_roll_forces + end_normal_forces;
            }
            using Tag = ::elastica::tags::ExternalLoads;
            blaze::column(::elastica::get<Tag>(rod_system), index + 1UL) =
                end_force;
          }),
      efo::make_torque([](auto /*time*/, auto&, std::size_t) {}));

  simulator->force(*second_rod_it)
      .at(::elastica::end - 1UL)
      .with(force_generator());

  simulator->finalize();

  // Set the dt value
  set_dt(::elastica::get<dt>(input_parameters));
}

void HingeJointCaseStudy::step() {
  simulator->step(time_stepper, get_current_time(), get_dt());
}

void HingeJointCaseStudy::run() {
  const ::elastica::real_t final_time(10.0);
  const std::size_t n_steps = (final_time / current_dt());

  for (std::size_t i_step = {0UL}; i_step < n_steps; ++i_step) {
    step();
  }
}
