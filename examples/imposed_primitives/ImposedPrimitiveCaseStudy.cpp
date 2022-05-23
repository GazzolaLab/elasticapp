//==============================================================================
/*!
//  \file
//  \brief Imposed Primitive case-study
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#include "ImposedPrimitiveCaseStudy.hpp"

#include <elastica/ErrorHandling/FloatingPointExceptions.hpp>
#include <elastica/Simulator/Environments/Kinematics.hpp>
#include <elastica/Simulator/Materials.hpp>
#include <elastica/Utilities/Get.hpp>
#include <elastica/Utilities/Math/Identity.hpp>
#include <iomanip>
#include <ostream>
#include <utility>  // pair

void ImposedPrimitiveCaseStudy::setup(
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
      ::elastica::get<contact_stiffness>(input_parameters),
      // normal contact damping
      ::elastica::get<contact_damping>(input_parameters),
      // tangent contact damping
      ::elastica::get<contact_damping>(input_parameters),
      // coefficient of static friction
      0.1,
      // coefficient of dynamic friction
      0.1);

  using Sys = ::elastica::rigid_body::Sphere;
  auto first_sphere_initializer = ::blocks::initialize_block<Sys>(
      ::blocks::initialize<::elastica::tags::Position>([&]() {
        return ::elastica::Vec3{
            0.0, 0.0,
            ::elastica::get<initial_height_from_plane>(input_parameters)};
      }),
      ::blocks::initialize<::elastica::tags::Director>(
          []() { return ::elastica::identity3(); }),
      ::blocks::initialize<::elastica::tags::Radius>(
          [&]() { return ::elastica::get<radius>(input_parameters); }),
      ::blocks::initialize<::elastica::tags::Velocity>([&]() {
        return ::elastica::Vec3{0.0, 0.0, 0.0};
      }),
      ::blocks::initialize<::elastica::tags::AngularVelocity>(
          []() { return ::elastica::Vec3{}; }),
      ::blocks::initialize<::elastica::tags::Acceleration>(
          []() { return ::elastica::Vec3{}; }),
      ::blocks::initialize<::elastica::tags::AngularAcceleration>(
          []() { return ::elastica::Vec3{}; }),
      ::blocks::initialize<::elastica::tags::Material>(
          [=]() { return my_material; }));
  sphere_refs.push_back(simulator->template emplace_back<Sys>(
      std::move(first_sphere_initializer)));

  namespace ek = elastica::kinematics;
  auto always_zero_ = [](::elastica::real_t) noexcept -> ::elastica::real_t {
    return 0.0;
  };

  auto simple_harmonic_oscillation_position =
      [](::elastica::real_t time_v) noexcept -> ::elastica::real_t {
    return 0.01 * std::sin(5.0 * M_PI * time_v);
  };
  auto simple_harmonic_oscillation_velocity =
      [](::elastica::real_t time_v) noexcept -> ::elastica::real_t {
    return 0.05 * M_PI * std::cos(5.0 * M_PI * time_v);
  };

  // can enable a flag switch here, but leave it for later
  auto pz = always_zero_;
  auto vz = always_zero_;

  auto z_translations = ek::translate_along<elastica::EulerianFrame::Z>(
      ek::mark_position(pz), ek::mark_velocity(vz));

  auto translations_buffer = ek::make_translations(
      ek::translate_along<elastica::EulerianFrame::X>(
          ek::mark_position(always_zero_), ek::mark_velocity(always_zero_)),
      ek::translate_along<elastica::EulerianFrame::Y>(
          ek::mark_position(always_zero_), ek::mark_velocity(always_zero_)),
      std::move(z_translations));

  // Set the bottom plane at (0, 0, 0) and with normal along Z
  const ::elastica::Vec3 plane_position{0.0, 0.0, 0.0};
  auto plane_normal = {0.0, 0.0, 1.0};
  auto plane_binormal = {0.0, 1.0, 0.0};
  auto plane_tangent = {1.0, 0.0, 0.0};
  // implicit convention
  ::elastica::Rot3 Q = {plane_normal, plane_binormal, plane_tangent};
  using Spans = typename ImposedPlane::Spans;
  imposed_plane_ref = simulator->emplace_back<ImposedPlane>(
      ::elastica::collision::PlanePrimitive(0UL, 0UL, true, plane_position, Q,
                                            Spans{1e4, 1e4},  // plane extent
                                            my_material),
      std::move(translations_buffer));

  const ::elastica::Vec3 gravity_acc_vector{
      0.0, 0.0, ::elastica::get<gravitational_acc>(input_parameters)};
  auto& gravity_field =
      simulator->get_environment<elastica::environments::tags::GravityField>();
  gravity_field->gravity() = gravity_acc_vector;

  simulator->finalize();

  // Set the dt value
  set_dt(::elastica::get<dt>(input_parameters));
}

void ImposedPrimitiveCaseStudy::step() {
  simulator->step(time_stepper, get_current_time(), get_dt(),
                  get_current_iteration());
}

void ImposedPrimitiveCaseStudy::run() {
  /* After a call to this function, the code will terminate with a floating
   point exception on overflow, divide-by-zero, and invalid operations.
   NOTE : This won't get called if used from the python side!
  */
  enable_floating_point_exceptions();
  // TODO : replace with event and triggers
  using TimePositionVelocity =
      std::tuple<::elastica::real_t, ::elastica::real_t, ::elastica::real_t>;
  std::vector<TimePositionVelocity> time_position_velocity_buffer;

  const ::elastica::real_t final_time(5.0);
  // explicitly get the system
  constexpr auto z_axis = 0UL;

  const std::size_t n_steps = (final_time / current_dt());

  const std::size_t write_interval = 200UL;
  time_position_velocity_buffer.reserve(n_steps / write_interval);

  for (std::size_t i = 0UL; i < n_steps; ++i) {
    step();

    if (not(current_iteration() % write_interval)) {
      time_position_velocity_buffer.emplace_back(
          current_time(),
          ::elastica::position(sphere_refs[0UL].get(), 0UL)[z_axis],
          ::elastica::position(sphere_refs[0UL].get(), 0UL)[z_axis]);
    }
  }

  // TODO : replace with a csv writer
  std::ofstream my_file;
  my_file.open("sphere_positions.csv");
  constexpr auto Time = 0UL;
  constexpr auto Position = 1UL;
  constexpr auto Velocity = 2UL;
  for (auto const& td : time_position_velocity_buffer) {
    my_file << std::setprecision(16) << std::get<Time>(td) << ","
            << std::get<Position>(td) << "," << std::get<Velocity>(td) << "\n";
  }
  my_file << std::endl;
  my_file.close();
}
