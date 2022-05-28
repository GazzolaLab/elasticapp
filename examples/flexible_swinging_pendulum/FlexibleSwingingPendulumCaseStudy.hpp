//==============================================================================
/*!
//  \file
//  \brief Flexible swinging pendulum case study
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy, Yashraj Bhosale - All Rights
//  Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once
//
#include <elastica/CaseStudy.hpp>
//
#include <elastica/Simulator/Constraints.hpp>
#include <elastica/Simulator/Environments.hpp>
#include <elastica/Simulator/Forcing.hpp>
#include <elastica/Simulator/Simulator.hpp>
#include <elastica/Systems/CosseratRods.hpp>
#include <elastica/Utilities/DefineTypes.h>
#include <elastica/Utilities/TMPL.hpp>

// global is fine since its a cpp file
using String = const char* const;
namespace ec = elastica::configuration;
using CosseratRod = ::elastica::cosserat_rod::CosseratRod;

struct FlexibleSwingingPendulumConfiguration {
  static inline auto make_simulator_configuration() {
    auto env_config = ec::make_environments_configuration<
        tmpl::list<::elastica::environments::tags::GravityField>>(
        // process environments in sequence
        ec::seq,
        // set global gravity field to 0., can be modified later
        ec::make_gravity_configuration(::elastica::Vec3{0., 0., 0.}));

    return ec::make_simulator_configuration(
        ec::DefaultDomainConfiguration{}, ec::DefaultSystemsConfiguration{},
        ec::DefaultParallelConfiguration{},
        ec::DefaultConstraintsConfiguration{}, env_config);
  }

  static inline auto make_simulator() {
    return elastica::make_simulator<::elastica::modules::Constraints,
                                    ::elastica::modules::Environments>(
        make_simulator_configuration());
  }

  static inline auto make_time_stepper() {
    using TimeStepper = ::elastica::timestepper::PositionVerlet;
    return TimeStepper{};
  }
};

struct FlexibleSwingingPendulumCaseStudy
    : public ::elastica::CaseStudyInterface<FlexibleSwingingPendulumCaseStudy> {
  using Configuration = FlexibleSwingingPendulumConfiguration;
  using This = FlexibleSwingingPendulumCaseStudy;

  static constexpr String help = {R"help(
Flexible swinging pendulum test-case
--------------------------

Here, we consider a beam hinged at one end and initially oriented along the
Z axis. A gravitational body force, constant in space and time, then acts on
the body along the negative X axis. Because of this body force, the beam
rotates about the hinged end towards the X axis, oscillating like a pendulum.
Owing to its flexibility, the rod undergoes complex oscillatory dynamics,
thus rendering this case a significant validation step for the solver. Here, we
test the validity of our solver by comparing the temporal motion of the
pendulum (specifically coordinates of the moving tip) against previous published
results:
(Lang, H., Linn, J., & Arnold, M. (2011). Multi-body dynamics simulation of
geometrically exact Cosserat rods. Multibody System Dynamics, 25(3), 285-312)

)help"};

  struct n_elements {
    // unsigned integer parameter
    using type = std::size_t;
    // documentation
    static constexpr String help = {"Number of elements in the rod"};
    static type default_value() { return 50UL; }
  };

  struct youngs_modulus {
    using type = ::elastica::real_t;
    static constexpr String help = {"Youngs modulus of the rod"};
    static type default_value() { return 5e6; }
  };

  struct shear_modulus {
    using type = ::elastica::real_t;
    static constexpr String help = {"Shear modulus of the rod"};
    static type default_value() { return (5e6 / 1.5); }
  };

  // replace by gravity
  struct gravitational_acc {
    using type = ::elastica::real_t;
    static constexpr String help = {"Gravitational acceleration"};
    static type default_value() { return -9.80665; }
  };

  struct radius {
    using type = ::elastica::real_t;
    static constexpr String help = {"Radius of the rod"};
    static type default_value() { return 0.005; }
  };

  struct damping_rate {
    using type = ::elastica::real_t;
    static constexpr String help = {"Rate of damping in the rod"};
    static type default_value() { return 1e-5; }
  };

  struct length {
    using type = ::elastica::real_t;
    static constexpr String help = {"Length of the rod"};
    static type default_value() { return 1.0; }
  };

  struct dt {
    using type = ::elastica::real_t;
    static constexpr String help = {"Time step of simulation"};
    static type default_value() {
      return type(0.002) *
             (length::default_value() / n_elements::default_value());
    }
  };

  using Parameters =
      tmpl::list<n_elements, damping_rate, youngs_modulus, shear_modulus,
                 gravitational_acc, radius, length, dt>;

  /* Required functions */
  void setup(InputTypes<This> const& input_parameters);
  void step();
  void run();
  inline auto communicate() {
    // TODO : inefficiency : slices at runtime, slice can be stored instead
    return ::elastica::position(rod_ref.get());
  }

  FlexibleSwingingPendulumCaseStudy() = default;
  FlexibleSwingingPendulumCaseStudy(
      FlexibleSwingingPendulumCaseStudy&&) noexcept = default;

  decltype(Configuration::make_time_stepper()) time_stepper =
      Configuration::make_time_stepper();
  decltype(Configuration::make_simulator()) simulator =
      Configuration::make_simulator();
  using RodRef = decltype(simulator->get_invalid_ref<CosseratRod>());
  RodRef rod_ref = simulator->get_invalid_ref<CosseratRod>();
};

ELASTICA_REGISTER_CASE_STUDY(FlexibleSwingingPendulumCaseStudy);
