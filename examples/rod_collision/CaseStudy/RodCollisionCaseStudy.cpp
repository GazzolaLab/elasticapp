//==============================================================================
/*!
//  \file
//  \brief Rigid body elastic-collision case study between two cosserat_rods
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#include "RodCollisionCaseStudy.hpp"

#include <elastica/ErrorHandling/FloatingPointExceptions.hpp>
#include <elastica/Simulator/Materials.hpp>
#include <elastica/Utilities/Get.hpp>
#include <elastica/Utilities/Math/Identity.hpp>
#include <elastica/Utilities/Math/Vec3.hpp>
//
#include <cmath>

void RodCollisionCaseStudy::setup(InputTypes<This> const& input_parameters) {
  elastica::MaterialID my_material = elastica::create_material(
      // material density
      1750,  // pyelastica values
      // coefficient of restitution
      0.24,
      // Young's modulus
      3e5,
      // Shear modulus
      2e5,
      // Contact stiffness
      ::elastica::get<contact_stiffness>(input_parameters),
      // normal contact damping
      ::elastica::get<contact_damping>(input_parameters),
      // tangent contact damping
      ::elastica::get<contact_damping>(input_parameters),
      // coefficient of static friction
      1e-10,
      // coefficient of dynamic friction
      1e-10);

  const ::elastica::Vec3 anchor{0., 0., 0.};
  const auto inclination(
      ::elastica::get<angle_of_inclination>(input_parameters));
  const ::elastica::Vec3 direction{0.0, std::cos(inclination),
                                   std::sin(inclination)};
  const ::elastica::Vec3 normal{0.0, -std::sin(inclination),
                                std::cos(inclination)};

  using RodInitializer =
      ::elastica::cosserat_rod::StraightCosseratRodInitializer;
  RodInitializer straight_initializer{
      RodInitializer::NElement{10}, RodInitializer::Material{my_material},
      RodInitializer::Radius{0.01}, RodInitializer::Length{0.5},
      RodInitializer::Origin{anchor + normal * 0.2},
      RodInitializer::Direction{direction}, RodInitializer::Normal{normal},
      // maybe needs to be scaled later
      RodInitializer::ForceDampingRate{0.001},
      RodInitializer::TorqueDampingRate{0.001}};

  cosserat_rod_refs[0] = simulator->template emplace_back<CosseratRod>(
      straight_initializer.initialize<CosseratRod>(
          ::blocks::initialize<::elastica::tags::Velocity>(
              [=](std::size_t) -> ::elastica::Vec3 {
                return -0.05 * normal;  // already normalized
              })));

  const auto second_inclination(0.0);
  const ::elastica::Vec3 second_direction{0.0, std::cos(second_inclination),
                                          std::sin(second_inclination)};
  const ::elastica::Vec3 second_normal{0.0, -std::sin(second_inclination),
                                       std::cos(second_inclination)};
  cosserat_rod_refs[1] = simulator->template emplace_back<CosseratRod>(
      straight_initializer
          .but_with(RodInitializer::Origin{anchor},
                    RodInitializer::Direction{second_direction},
                    RodInitializer::Normal{second_normal})
          .initialize<CosseratRod>());

  simulator->finalize();

  // Set the dt value
  set_dt(::elastica::get<dt>(input_parameters));
}

void RodCollisionCaseStudy::step() {
  simulator->step(time_stepper, get_current_time(), get_dt(),
                  get_current_iteration());
}

void RodCollisionCaseStudy::run() {
  /* After a call to this function, the code will terminate with a floating
   point exception on overflow, divide-by-zero, and invalid operations.
   NOTE : This won't get called if used from the python side!
  */
  enable_floating_point_exceptions();
  const ::elastica::real_t final_time(20.0);
  const std::size_t n_steps = (final_time / current_dt());

  for (std::size_t i = 0UL; i < n_steps; ++i) {
    step();
  }
}
