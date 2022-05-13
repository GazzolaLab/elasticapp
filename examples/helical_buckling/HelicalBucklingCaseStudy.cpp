//==============================================================================
/*!
//  \file
//  \brief HelicalBuckling case study
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#include "HelicalBucklingCaseStudy.hpp"

#include <cmath>
#include <elastica/Simulator/Materials.hpp>
#include <elastica/Utilities/Geometry/AxisAngleRotation.hpp>
#include <elastica/Utilities/Get.hpp>
#include <elastica/Utilities/Math/Identity.hpp>
#include <elastica/Utilities/Math/Rot3.hpp>
#include <elastica/Utilities/Math/Vec3.hpp>
#include <elastica/Utilities/Math/Zero.hpp>

void HelicalBucklingCaseStudy::setup(InputTypes<This> const& input_parameters) {
  const ::elastica::real_t radius(0.35);
  const auto initial_area = M_PI * radius * radius;

  elastica::MaterialID my_material = elastica::create_material(
      // PyElastica values
      // material density
      static_cast<::elastica::real_t>(1.0) / initial_area,
      // coefficient of restitution
      0.24,
      // Young's modulus
      1e6,
      // Shear modulus
      1e5,
      // The below parameters doesn't change the result of this example
      // coefficient of static friction
      100.0,  // needs to be > 0.0
      // cofficient of dynamic friction
      20.0,  // needs to be > 0.0
      // Contact stiffness
      200,
      // normal contact damping
      1e-10,
      // tangent contact damping
      1e-10);

  const std::size_t n_elem(elastica::get<n_elements>(input_parameters));

  const ::elastica::Vec3 origin{0.0, 0.0, 0.0};
  const ::elastica::real_t base_length(100.0);
  //  const ::elastica::real_t base_length{
  //      ::elastica::get<length>(input_parameters)};

  const ::elastica::Vec3 rod_direction({0.0, 0.0, 1.0});
  const ::elastica::Vec3 normal_direction({0.0, 1.0, 0.0});

  using StraightInit = ::elastica::cosserat_rod::StraightCosseratRodInitializer;
  StraightInit initializer{
      // StraightInitialization Wrapper
      StraightInit::NElement{n_elem},
      StraightInit::Material{my_material},
      StraightInit::Radius{radius},
      StraightInit::Length{base_length},
      StraightInit::Origin{origin},
      StraightInit::Direction{rod_direction},
      StraightInit::Normal{normal_direction},
      StraightInit::ForceDampingRate{
          ::elastica::get<damping_rate>(input_parameters)},
      StraightInit::TorqueDampingRate{
          ::elastica::get<damping_rate>(input_parameters)},
  };

  rod_ref =
      simulator->emplace_back<CosseratRod>(initializer.initialize<CosseratRod>(
          // velocity
          ::blocks::initialize<::elastica::tags::Velocity>(
              [half_elem = (n_elem / 2UL)](std::size_t index) {
                // put a disturbance initially
                if (index == half_elem)
                  return ::elastica::Vec3{0.0, 1e-6, 0.0};
                else
                  return ::elastica::zero3();
              })));

  // modify explicitly after construction
  {
    // TODO : Highlight good design. Only use auto. Using auto& raises compiler
    // error. Promote value semantics.
    auto shear_matrix =
        ::elastica::get<::elastica::tags::ShearStretchRigidityMatrix>(
            rod_ref.get());
    const auto sm = ::elastica::Material::get_shear_modulus(my_material);
    for (std::size_t i_elem = 0UL; i_elem < n_elem; ++i_elem) {
      // can be refactored but whatever
      shear_matrix(0UL, 0UL, i_elem) = sm;
      shear_matrix(1UL, 1UL, i_elem) = sm;
      shear_matrix(2UL, 2UL, i_elem) = sm;
    }
  }

  {
    auto bend_matrix =
        ::elastica::get<::elastica::tags::BendingTwistRigidityMatrix>(
            rod_ref.get());
    for (std::size_t i_elem = 0UL; i_elem < (n_elem - 1); ++i_elem) {
      // can be refactored but whatever
      bend_matrix(0UL, 0UL, i_elem) = 1.345;
      bend_matrix(1UL, 1UL, i_elem) = 1.345;
      bend_matrix(2UL, 2UL, i_elem) = 0.789;
    }
  }

  namespace eco = ::elastica::constraints;

  // parameters
  const ::elastica::real_t slack(3.0);
  const ::elastica::real_t n_rotations(27.0);  // can be a float
  const ::elastica::real_t twisting_time(500.0);

  // derived
  const ::elastica::real_t angular_vel_scalar =
      (2.0 * n_rotations * M_PI / twisting_time) / 2.0;
  const ::elastica::real_t shrink_vel_scalar = 0.5 * slack / twisting_time;
  const ::elastica::Vec3 ang_vel{angular_vel_scalar * rod_direction};
  const ::elastica::Vec3 shrink_vel{shrink_vel_scalar * rod_direction};
  const ::elastica::real_t theta(n_rotations * M_PI);

  const ::std::size_t left_tip_index = 0UL;
  const ::elastica::Vec3 final_start_position =
      origin + 0.5 * slack * rod_direction;
  const ::elastica::Rot3 final_start_director =
      ::elastica::geometry::rotate_via_axis_angle(rod_direction, theta) *
      ::elastica::director(rod_ref.get(), left_tip_index);

  auto left_constraint_generator = eco::make_constraint(
      eco::make_value_constraint(
          [=](::elastica::real_t time_v, auto& rod_system, auto index) {
            if (time_v > twisting_time) {
              ::elastica::position(rod_system, index) = final_start_position;
              ::elastica::director(rod_system, index) = final_start_director;
            }
          }),
      eco::make_rate_constraint(
          [=](::elastica::real_t time_v, auto& rod_system, std::size_t index) {
            if (time_v > twisting_time) {
              ::elastica::velocity(rod_system, index) = 0.0;
              ::elastica::angular_velocity(rod_system, index) = 0.0;
            } else {
              ::elastica::velocity(rod_system, index) = shrink_vel;
              ::elastica::angular_velocity(rod_system, index) = ang_vel;
            }
          }));
  simulator->constrain(rod_ref)
      .at(left_tip_index)
      .with(left_constraint_generator());

  const ::elastica::Vec3 final_end_position =
      origin + (base_length - 0.5 * slack) * rod_direction;
  const ::elastica::Rot3 final_end_director =
      ::elastica::geometry::rotate_via_axis_angle(rod_direction, -theta) *
      // need end support here my dude
      ::elastica::director(rod_ref.get(), n_elem - 1UL);

  auto right_constraint_generator = eco::make_constraint(
      eco::make_value_constraint([=](::elastica::real_t time_v,
                                     auto& rod_system, auto index) {
        if (time_v > twisting_time) {
          ::elastica::position(rod_system, index + 1UL) = final_end_position;
          ::elastica::director(rod_system, index) = final_end_director;
        }
      }),
      eco::make_rate_constraint(
          [=](::elastica::real_t time_v, auto& rod_system, std::size_t index) {
            if (time_v > twisting_time) {
              ::elastica::velocity(rod_system, index + 1UL) = 0.0;
              ::elastica::angular_velocity(rod_system, index) = 0.0;
            } else {
              ::elastica::velocity(rod_system, index + 1UL) = -shrink_vel;
              ::elastica::angular_velocity(rod_system, index) = -ang_vel;
            }
          }));
  simulator->constrain(rod_ref)
      .at(::elastica::end - 1UL)
      .with(right_constraint_generator());

  [&]() {
    std::cout << final_start_position << std::endl;
    std::cout << final_end_position << std::endl;
    std::cout << final_end_director << std::endl;
    std::cout << final_start_director << std::endl;
    std::cout << ang_vel << std::endl;
    std::cout << shrink_vel << std::endl;
  };

  simulator->finalize();

  // Set the dt value
  set_dt(::elastica::get<dt>(input_parameters));
}

void HelicalBucklingCaseStudy::step() {
  simulator->step(time_stepper, get_current_time(), get_dt(),
                  get_current_iteration());
}

void HelicalBucklingCaseStudy::run() {
  // TODO : replace with event and triggers
  //  using Temporal = ::elastica::real_t;
  //  using BendEnergy = ::elastica::real_t;
  //  using ShearEnergy = ::elastica::real_t;
  //  using TranslationEnergy = ::elastica::real_t;
  //  using RotationEnergy = ::elastica::real_t;
  //  using TimesAndEnergies = std::tuple<Temporal, BendEnergy, ShearEnergy,
  //                                      TranslationEnergy, RotationEnergy>;
  //  std::vector<TimesAndEnergies> stats;

  const ::elastica::real_t final_time(10500.0);

  const auto n_iterations = static_cast<std::size_t>(final_time / current_dt());

  //  constexpr std::size_t write_interval = 10000UL;
  //  stats.reserve(n_iterations / write_interval);
  run_for_n_iterations(n_iterations);

  //  // TODO : replace with a csv writer
  //  std::ofstream my_file;
  //  my_file.open("energies.csv");
  //  constexpr auto temporal = 0UL;
  //  constexpr auto bend_elastic_energy = 1UL;
  //  constexpr auto shear_elastic_energy = 2UL;
  //  constexpr auto translation_energy = 3UL;
  //  constexpr auto rotation_energy = 4UL;
  //  for (auto const& st : stats) {
  //    my_file << std::setprecision(16)  //
  //            << std::get<temporal>(st) << ","
  //            << std::get<bend_elastic_energy>(st) << ","
  //            << std::get<shear_elastic_energy>(st) << ", "
  //            << std::get<translation_energy>(st) << ","
  //            << std::get<rotation_energy>(st) << "\n";
  //  }
  //  my_file << std::endl;
  //  my_file.close();
}
