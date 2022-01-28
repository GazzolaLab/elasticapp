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
#include <elastica/Utilities/Math/Dot.hpp>
#include <elastica/Utilities/Math/Normalize.hpp>
#include <ostream>

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
  const ::elastica::Vec3 link_one_origin{1.0, 1.0, 1.0};
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
      StraightInit::ForceDampingCoefficient{0.001},
      StraightInit::TorqueDampingCoefficient{0.001},
  };

  first_rod_it = simulator->emplace_back<CosseratRod>(
      straight_initializer.initialize<CosseratRod>());
  second_rod_it = simulator->emplace_back<CosseratRod>(
      straight_initializer.but_with(StraightInit::Origin{link_two_origin})
          .initialize<CosseratRod>());
  std::cout << *second_rod_it << std::endl;

  namespace eco = ::elastica::constraints;
  const std::size_t my_constraint_index = 0UL;
  auto constraint_generator = eco::make_constraint(
      eco::make_value_constraint(
          [origin = ::elastica::Vec3{0., 0., 0.},
           origin_frame = ::elastica::Rot3{::elastica::director(
               *first_rod_it, my_constraint_index)}](
              auto /*time*/, auto& rod_system, auto index) {
            ::elastica::position(rod_system, index) = origin;
            ::elastica::director(rod_system, index) = origin_frame;

            std::cout << std::addressof(rod_system) << std::endl;
            // std::cout << ::elastica::position(rod_system, index) <<
            // std::endl;
          }),
      eco::make_rate_constraint(
          [](auto /*time*/, auto& rod_system, std::size_t index) {
            ::elastica::velocity(rod_system, index) = 0.0;
            ::elastica::angular_velocity(rod_system, index) = 0.0;
          }));

  simulator->constrain(*first_rod_it).at(0UL).with(constraint_generator());

  namespace efo = ::elastica::forcing;

  const ::elastica::real_t end_force_magnitude(0.005);
  const ::elastica::real_t rampup_time(0.2);
  const ::elastica::Vec3 roll_direction(link_normal % link_direction);

  auto force_generator = efo::make_forcing(
      efo::make_force([=, normal_direction = link_normal](
                          auto time_v, auto& rod_system, auto index) {
        const auto time_d = static_cast<::elastica::real_t>(time_v);
        ::elastica::Vec3 end_force;
        if (time_d < rampup_time) {
          end_force = -2.0 * end_force_magnitude * normal_direction;
        } else {
          const ::elastica::Vec3 end_roll_forces =
              end_force_magnitude * std::cos(M_PI_2 * (time_d - rampup_time)) *
              roll_direction;
          const ::elastica::Vec3 end_normal_forces =
              end_force_magnitude * std::sin(M_PI_2 * (time_d - rampup_time)) *
              normal_direction;
          end_force = end_roll_forces + end_normal_forces;
        }
        // std::cout << "Force " << end_force << std::endl;

        std::cout << std::addressof(rod_system) << std::endl;
        using Tag = ::elastica::tags::ExternalLoads;
        blaze::column(::elastica::get<Tag>(rod_system), index + 1UL) +=
            end_force;
      }),
      efo::make_torque([](auto /*time*/, auto&, std::size_t) {}));

  simulator->force(*second_rod_it)
      .at(::elastica::end - 1UL)
      .with(force_generator());

  namespace econ = ::elastica::connections;

  const ::elastica::real_t joint_trans_stiffness(1e5);
  const ::elastica::real_t joint_trans_damping(0.0);
  const ::elastica::real_t joint_rot_stiffness(5e3);
  auto connection_generator = econ::make_connection(
      econ::make_force([=](auto /*time*/, auto& first_rod,
                           std::size_t arg_first_index, auto& second_rod,
                           auto arg_second_index) {
        // TODO : fix
        const auto first_index = arg_first_index + 1UL;
        const auto second_index = arg_second_index;

        const ::elastica::Vec3 distance_vector(
            ::elastica::position(second_rod, second_index) -
            ::elastica::position(first_rod, first_index));
        auto elastic_force = joint_trans_stiffness * distance_vector;

        // TODO there should be an utility to project
        const ::elastica::Vec3 normalized_distance_vector =
            ::elastica::normalize(distance_vector);
        auto velocity_vector =
            ::elastica::dot(::elastica::velocity(second_rod, second_index) -
                                ::elastica::velocity(first_rod, first_index),
                            normalized_distance_vector) *
            normalized_distance_vector;
        auto damping_force = -joint_trans_damping * velocity_vector;
        const ::elastica::Vec3 contact_force = elastic_force + damping_force;

        //        std::cout << "Force " << contact_force << std::endl;

        using Tag = ::elastica::tags::ExternalLoads;
        blaze::column(::elastica::get<Tag>(first_rod), first_index) +=
            contact_force;
        blaze::column(::elastica::get<Tag>(second_rod), second_index) -=
            contact_force;
      }),
      econ::make_torque([=, normal_direction = roll_direction](
                            auto /*time*/, auto& first_rod,
                            std::size_t first_index, auto& second_rod,
                            auto second_index) {
        // TODO : fix
        auto link =
            // TODO : refactor this 1UL as a constexpr index
            ::elastica::position(second_rod, second_index + 1UL) -
            ::elastica::position(second_rod, second_index);
        auto force = -joint_rot_stiffness *
                     // projection of link onto the plane direction
                     ::elastica::dot(link, normal_direction) * normal_direction;
        // TODO : provide a cross product operation, don't rely on %
        const ::elastica::Vec3 torque = link % force;
        // std::cout << "Torque" << torque << std::endl;

        using Tag = ::elastica::tags::ExternalTorques;
        // this is not a nodal quantitiy
        blaze::column(::elastica::get<Tag>(first_rod), first_index) -=
            (::elastica::director(first_rod, first_index) * torque);
        blaze::column(::elastica::get<Tag>(second_rod), second_index) +=
            (::elastica::director(second_rod, second_index) * torque);
      }));

  using ::elastica::interface::at;
  //  simulator->connect(*first_rod_it, at(::elastica::end - 1UL))
  //      .to(*second_rod_it, at(0UL))
  //      .with(connection_generator());

  std::cout << "Before" << std::endl;
  std::cout << "First" << std::endl;
  std::cout << (*first_rod_it) << std::endl;

  std::cout << "Second" << std::endl;
  std::cout << (*second_rod_it) << std::endl;

  simulator->finalize();

  std::cout << "AFter" << std::endl;
  std::cout << "First" << std::endl;
  std::cout << (*first_rod_it) << std::endl;

  std::cout << "Second" << std::endl;
  std::cout << (*second_rod_it) << std::endl;

  // Set the dt value
  set_dt(::elastica::get<dt>(input_parameters));
}

void HingeJointCaseStudy::step() {
  simulator->step(time_stepper, get_current_time(), get_dt());
}

void HingeJointCaseStudy::run() {
  const ::elastica::real_t final_time(0.1);
  const std::size_t n_steps = (final_time / current_dt()) - 1UL;

  for (std::size_t i_step = {0UL}; i_step < n_steps; ++i_step) {
    step();
    if (not(i_step % 50UL)) {
      //      std::cout << "First" << std::endl;
      //      std::cout << ::elastica::position(*first_rod_it) << std::endl;
      //
      //      std::cout << "Second" << std::endl;
      //      std::cout << ::elastica::position(*second_rod_it) << std::endl;
    }
  }

  // write out the final positions of the body
  std::ofstream my_file;
  my_file.open("final_first_positions.txt");
  my_file << ::elastica::position(*first_rod_it);
  my_file << std::endl;
  my_file.close();

  my_file.open("final_second_positions.txt");
  my_file << ::elastica::position(*second_rod_it);
  my_file << std::endl;
  my_file.close();
}
