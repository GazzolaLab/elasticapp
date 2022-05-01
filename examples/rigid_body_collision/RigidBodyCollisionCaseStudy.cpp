//==============================================================================
/*!
//  \file
//  \brief Rigid body elastic-collision case study between two spheres
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#include "RigidBodyCollisionCaseStudy.hpp"

#include <elastica/ErrorHandling/FloatingPointExceptions.hpp>
#include <elastica/Simulator/Materials.hpp>
#include <elastica/Utilities/Get.hpp>
#include <elastica/Utilities/Math/Identity.hpp>
#include <elastica/Utilities/Size.hpp>
#include <iomanip>
#include <ostream>
#include <utility>  // pair

void RigidBodyCollisionCaseStudy::setup(
    InputTypes<This> const& input_parameters) {
  elastica::MaterialID my_material = elastica::create_material(
      // material density
      1100,  // pyelastica values
      // coefficient of restitution
      0.24,
      // Young's modulus
      1e6,
      // Shear modulus
      1e6,
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

  using Sys = ::elastica::rigid_body::Sphere;
  auto first_sphere_initializer = ::blocks::initialize_block<Sys>(
      ::blocks::initialize<::elastica::tags::Position>([]() {
        return ::elastica::Vec3{-0.5, 0.0, 0.0};
      }),
      ::blocks::initialize<::elastica::tags::Director>(
          []() { return ::elastica::identity3(); }),
      ::blocks::initialize<::elastica::tags::Radius>(
          [&]() { return ::elastica::get<radius1>(input_parameters); }),
      ::blocks::initialize<::elastica::tags::Velocity>([&]() {
        return ::elastica::Vec3{::elastica::get<xvelocity1>(input_parameters),
                                0.0, 0.0};
      }),
      ::blocks::initialize<::elastica::tags::AngularVelocity>(
          []() { return ::elastica::Vec3{}; }),
      ::blocks::initialize<::elastica::tags::Acceleration>(
          []() { return ::elastica::Vec3{}; }),
      ::blocks::initialize<::elastica::tags::AngularAcceleration>(
          []() { return ::elastica::Vec3{}; }),
      ::blocks::initialize<::elastica::tags::Material>(
          [=]() { return my_material; }));
  // First sphere intersects one imposed sphere
  sphere_refs[0] = simulator->template emplace_back<Sys>(
      std::move(first_sphere_initializer));

  auto second_sphere_initializer = ::blocks::initialize_block<Sys>(
      ::blocks::initialize<::elastica::tags::Position>([]() {
        return ::elastica::Vec3{0.5, 0.0, 0.0};
      }),
      ::blocks::initialize<::elastica::tags::Director>(
          []() { return ::elastica::identity3(); }),
      ::blocks::initialize<::elastica::tags::Radius>(
          [&]() { return ::elastica::get<radius2>(input_parameters); }),
      ::blocks::initialize<::elastica::tags::Velocity>([&]() {
        return ::elastica::Vec3{::elastica::get<xvelocity2>(input_parameters),
                                0.0, 0.0};
      }),
      ::blocks::initialize<::elastica::tags::AngularVelocity>(
          []() { return ::elastica::Vec3{}; }),
      ::blocks::initialize<::elastica::tags::Acceleration>(
          []() { return ::elastica::Vec3{}; }),
      ::blocks::initialize<::elastica::tags::AngularAcceleration>(
          []() { return ::elastica::Vec3{}; }),
      ::blocks::initialize<::elastica::tags::Material>(
          [=]() { return my_material; }));
  // First sphere intersects one imposed sphere
  sphere_refs[1] = simulator->template emplace_back<Sys>(
      std::move(second_sphere_initializer));

  simulator->finalize();

  // Set the dt value
  set_dt(::elastica::get<dt>(input_parameters));
}

void RigidBodyCollisionCaseStudy::step() {
  simulator->step(time_stepper, get_current_time(), get_dt(),
                  get_current_iteration());
}

void RigidBodyCollisionCaseStudy::run() {
  /* After a call to this function, the code will terminate with a floating
   point exception on overflow, divide-by-zero, and invalid operations.
   NOTE : This won't get called if used from the python side!
  */
  enable_floating_point_exceptions();
  // TODO : replace with event and triggers
  using TimeAndXPositions =
      std::tuple<::elastica::real_t, ::elastica::real_t, ::elastica::real_t>;
  std::vector<TimeAndXPositions> time_and_xpositions;

  const ::elastica::real_t final_time(5.0);
  // explicitly get the system
  constexpr auto x_axis = 0UL;

  const std::size_t n_steps = (final_time / current_dt());

  const std::size_t write_interval = 200UL;
  time_and_xpositions.reserve(n_steps / write_interval);

  for (std::size_t i = 0UL; i < n_steps; ++i) {
    step();

    if (not(current_iteration() % write_interval)) {
      time_and_xpositions.emplace_back(
          current_time(),
          ::elastica::position(sphere_refs[0UL].get(), 0UL)[x_axis],
          ::elastica::position(sphere_refs[1UL].get(), 0UL)[x_axis]);
    }
  }

  // TODO : replace with a csv writer
  std::ofstream my_file;
  my_file.open("sphere_positions.csv");
  constexpr auto Time = 0UL;
  constexpr auto FirstSphereXCoordinate = 1UL;
  constexpr auto SecondSphereXCoordinate = 2UL;
  for (auto const& td : time_and_xpositions) {
    my_file << std::setprecision(16) << std::get<Time>(td) << ","
            << std::get<FirstSphereXCoordinate>(td) << ","
            << std::get<SecondSphereXCoordinate>(td) << "\n";
  }
  my_file << std::endl;
  my_file.close();
}
