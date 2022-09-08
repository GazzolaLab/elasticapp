//==============================================================================
/*!
//  \file
//  \brief Rod-Plane collision case study
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy, Yashraj Bhosale - All Rights
//  Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#include "RodPlaneCollisionCaseStudy.hpp"

#include "ErrorHandling/FloatingPointExceptions.hpp"
#include "Simulator/Environments/Kinematics.hpp"
#include "Simulator/Frames.hpp"
#include "Simulator/Materials.hpp"
#include "Utilities/Get.hpp"
#include "Utilities/Math/Rot3.hpp"
#include "Utilities/Math/Vec3.hpp"
//
#include <utility>  // move

void RodPlaneCollisionCaseStudy::setup(
    InputTypes<This> const& input_parameters) {
  elastica::MaterialID my_material = elastica::create_material(
      // material density
      894.0,  // pyelastica values
      // coefficient of restitution
      0.24,
      // Young's modulus
      ::elastica::get<youngs_modulus>(input_parameters),
      // Shear modulus
      ::elastica::get<shear_modulus>(input_parameters),
      // Properties below don't matter
      // Contact stiffness
      ::elastica::get<contact_stiffness>(input_parameters),
      // normal contact damping
      ::elastica::get<contact_damping>(input_parameters),
      // tangent contact damping
      ::elastica::get<contact_damping>(input_parameters),
      // coefficient of static friction
      0.4,
      // coefficient of dynamic friction
      0.2);

  using StraightInit = ::elastica::cosserat_rod::StraightCosseratRodInitializer;
  const ::elastica::real_t min_distance_from_ground =
      ::elastica::get<initial_min_distance_from_plane>(input_parameters);
  const ::elastica::Vec3 origin{0.0, 0.0, min_distance_from_ground};
  const ::elastica::real_t inclination(
      ::elastica::get<angle_of_inclination>(input_parameters));
  const ::elastica::Vec3 direction{0.0, std::cos(inclination),
                                   std::sin(inclination)};
  const ::elastica::Vec3 normal{0.0, -std::sin(inclination),
                                std::cos(inclination)};

  StraightInit straight_initializer{
      // StraightInitialization Wrapper
      StraightInit::NElement{::elastica::get<n_elements>(input_parameters)},
      StraightInit::Material{my_material},
      StraightInit::Radius{::elastica::get<radius>(input_parameters)},
      StraightInit::Length{::elastica::get<length>(input_parameters)},
      StraightInit::Origin{origin},
      StraightInit::Direction{direction},
      StraightInit::Normal{normal},
      StraightInit::ForceDampingRate{
          ::elastica::get<damping_rate>(input_parameters)},
      StraightInit::TorqueDampingRate{
          ::elastica::get<damping_rate>(input_parameters)},
  };
  RodRef rod_ref = simulator->emplace_back<CosseratRod>(
      straight_initializer.initialize<CosseratRod>());
  rod_refs.push_back(rod_ref);

  namespace ek = elastica::kinematics;
  auto always_zero_ = [](::elastica::real_t) noexcept -> ::elastica::real_t {
    return 0.0;
  };

  auto sinusoidal_p = [r = ::elastica::get<radius>(input_parameters)](
      ::elastica::real_t t) noexcept -> ::elastica::real_t {
    return 2.0 * r * std::sin(5.0 * M_PI * t);
  };

  auto sinusoidal_v = [r = ::elastica::get<radius>(input_parameters)](
      ::elastica::real_t t) noexcept -> ::elastica::real_t {
    return 5.0 * M_PI * 2.0 * r * std::cos(5.0 * M_PI * t);
  };

  auto iequals = [](std::string const& a, std::string const& b) -> bool {
    return std::equal(a.begin(), a.end(), b.begin(), b.end(),
                      [](unsigned char a, unsigned char b) {
                        return std::tolower(a) == std::tolower(b);
                      });
  };

  // Set plane geometry
  // Set the bottom plane at (0, 0, 0) and with normal along Z
  const ::elastica::Vec3 plane_position{0.0, 0.0, 0.0};
  auto plane_normal = {0.0, 0.0, 1.0};
  auto plane_binormal = {0.0, 1.0, 0.0};
  auto plane_tangent = {1.0, 0.0, 0.0};
  // implicit convention
  ::elastica::Rot3 Q = {plane_normal, plane_binormal, plane_tangent};
  using Spans = typename ImposedPlane::Spans;

  // Setup motion
  if (iequals(::elastica::get<imposed_motion>(input_parameters),
              imposed_motion::default_value())) {
    auto translations_buffer = ek::make_translations(
        ek::translate_along<elastica::EulerianFrame::X>(
            ek::mark_position(always_zero_), ek::mark_velocity(always_zero_)),
        ek::translate_along<elastica::EulerianFrame::Y>(
            ek::mark_position(always_zero_), ek::mark_velocity(always_zero_)),
        ek::translate_along<elastica::EulerianFrame::Z>(
            ek::mark_position(always_zero_), ek::mark_velocity(always_zero_)));

    imposed_plane_ref = simulator->emplace_back<ImposedPlane>(
        ::elastica::collision::PlanePrimitive(0UL, 0UL, true, plane_position, Q,
                                              Spans{1e4, 1e4},  // plane extent
                                              my_material),
        std::move(translations_buffer));
  } else {
    // Oscillatory case.
    auto translations_buffer = ek::make_translations(
        ek::translate_along<elastica::EulerianFrame::X>(
            ek::mark_position(always_zero_), ek::mark_velocity(always_zero_)),
        ek::translate_along<elastica::EulerianFrame::Y>(
            ek::mark_position(always_zero_), ek::mark_velocity(always_zero_)),
        ek::translate_along<elastica::EulerianFrame::Z>(
            // The COM along z axis is not at 0 (the anchor position), but rather
            // inside the halfspace, so we displace it by anchor_to_com.
            ek::mark_position(sinusoidal_p),
            ek::mark_velocity(sinusoidal_v)));

    imposed_plane_ref = simulator->emplace_back<ImposedPlane>(
        ::elastica::collision::PlanePrimitive(0UL, 0UL, true, plane_position, Q,
                                              Spans{1e4, 1e4},  // plane extent
                                              my_material),
        std::move(translations_buffer));
  }

  // set near-list update time
  auto& collision_solver =
      simulator
          ->get_environment<elastica::environments::tags::CollisionSystem>();
  collision_solver->time_interval_between_detection() =
      ::elastica::get<contact_detection_time_interval>(
          input_parameters);  // anything > 0.0

  const ::elastica::Vec3 gravity_acc_vector{
      0.0, 0.0, ::elastica::get<gravitational_acc>(input_parameters)};
  auto& gravity_field =
      simulator->get_environment<elastica::environments::tags::GravityField>();
  gravity_field->gravity() = gravity_acc_vector;

  simulator->finalize();
  // Set the dt value
  final_time = ::elastica::get<final_sim_time>(input_parameters);
  set_dt(::elastica::get<dt>(input_parameters));
}

void RodPlaneCollisionCaseStudy::step() {
  simulator->step(time_stepper, get_current_time(), get_dt(),
                  get_current_iteration());
}

void RodPlaneCollisionCaseStudy::run() {
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
