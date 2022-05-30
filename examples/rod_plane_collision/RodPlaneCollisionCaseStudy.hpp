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

#pragma once
//
#include <elastica/CaseStudy.hpp>
//
#include <elastica/Simulator/Environments.hpp>
#include <elastica/Simulator/Forcing.hpp>
#include <elastica/Simulator/Simulator.hpp>
#include <elastica/Systems/CosseratRods.hpp>
#include <elastica/Utilities/DefineTypes.h>
#include <elastica/Utilities/Math/Vec3.hpp>
#include <elastica/Utilities/TMPL.hpp>
//
#include <algorithm>
#include <tuple>
#include <utility>
#include <vector>

// global is fine since its a cpp file
using String = const char* const;
namespace ec = elastica::configuration;
using CosseratRod = ::elastica::cosserat_rod::CosseratRod;
using ImposedPlane = ::elastica::collision::ImposedPlanePrimitive;

struct RodPlaneCollisionConfiguration {
  static inline auto make_simulator_configuration() {
    auto system_config =
        ec::make_systems_configuration<tmpl::list<CosseratRod>>(
            ec::make_blocking_policy(ec::RestrictSizeAcrossBlockTypes{50UL}),
            ec::make_iteration_policy(
                ec::iterate_across_system_types_in(ec::seq),
                ec::iterate_over_systems_in(ec::seq)));

    using EnvironmentTags =
        tmpl::list<::elastica::environments::tags::GravityField,
                   ::elastica::environments::tags::CollisionSystem>;
    // Tag for bottom plane
    using ImposedPrimitives = tmpl::list<ImposedPlane>;
    auto collision_config =
        ec::make_collision_configuration(ec::seq,   // between each type
                                         ec::seq);  // across each type
    auto gravity_config =
        ec::make_gravity_configuration(::elastica::Vec3{0., 0., 0.});

    auto env_config =
        ec::make_environments_configuration<EnvironmentTags, ImposedPrimitives>(
            ec::seq, gravity_config, collision_config);

    return ec::make_simulator_configuration(
        ec::DefaultDomainConfiguration{}, system_config,
        ec::DefaultParallelConfiguration{}, env_config);
  }

  static inline auto make_simulator() {
    return elastica::make_simulator<::elastica::modules::Environments>(
        make_simulator_configuration());
  }

  static inline auto make_time_stepper() {
    using TimeStepper = ::elastica::timestepper::PositionVerlet;
    return TimeStepper{};
  }
};

struct RodPlaneCollisionCaseStudy
    : public ::elastica::CaseStudyInterface<RodPlaneCollisionCaseStudy> {
  using Configuration = RodPlaneCollisionConfiguration;
  using This = RodPlaneCollisionCaseStudy;
  using ImposedPlane = ::elastica::collision::ImposedPrimitiveAdapter<
      ::elastica::collision::PlanePrimitive>;

  static constexpr String help = {R"help(
Rod-plane collision study
-------------------------

Runs an example of a plane at origin, with a rod above the plane and
initially inclined wrt to the plane, at rest. The rod falls due to gravity
towards the plane, eventually meeting the plane. This results in forces being
generated due to the collision.

)help"};

  struct n_elements {
    // unsigned integer parameter
    using type = std::size_t;
    // documentation
    static constexpr String help = {"Number of elements in the rod"};
    static type default_value() { return 8UL; }
  };

  struct damping_rate {
    using type = ::elastica::real_t;
    static constexpr String help = {"Internal damping in the rod"};
    static type default_value() { return 1e-10; }
  };

  struct youngs_modulus {
    using type = ::elastica::real_t;
    static constexpr String help = {"Youngs modulus of the rod"};
    static type default_value() { return 12e9; }
  };

  struct shear_modulus {
    using type = ::elastica::real_t;
    static constexpr String help = {"Shear modulus of the rod"};
    static type default_value() { return (12e9 / 1.5); }
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
    static type default_value() { return 1.21e-3; }
  };

  struct length {
    using type = ::elastica::real_t;
    static constexpr String help = {"Length of the rod"};
    static type default_value() { return 0.07574; }
  };

  struct initial_min_distance_from_plane {
    using type = ::elastica::real_t;
    static constexpr String help = {"Initial minimum distance from the plane"};
    static type default_value() { return 5.0 * radius::default_value(); }
  };

  struct angle_of_inclination {
    using type = ::elastica::real_t;
    static constexpr String help = {
        "Angle of inclination between the rod and plane"};
    static type default_value() { return 0.0; }
  };

  struct dt {
    using type = ::elastica::real_t;
    static constexpr String help = {"Time step of simulation"};
    static type default_value() {
      return type(0.002) *
             (length::default_value() / n_elements::default_value());
    }
  };

  struct contact_stiffness {
    using type = ::elastica::real_t;
    static constexpr String help = {"Contact stiffness"};
    static type default_value() { return 1e3; }
  };

  struct contact_damping {
    using type = ::elastica::real_t;
    static constexpr String help = {"Contact damping"};
    static type default_value() { return 1e-10; }
  };

  struct contact_detection_time_interval {
    using type = ::elastica::real_t;
    static constexpr String help = {"Time interval between contact detection"};
    static type default_value() { return 1e-5; }
  };

  struct final_sim_time {
    using type = ::elastica::real_t;
    static constexpr String help = {"Final simulation time"};
    static type default_value() { return 1.0; }
  };

  using Parameters =
      tmpl::list<n_elements, damping_rate, youngs_modulus, shear_modulus,
                 gravitational_acc, radius, length, dt, contact_stiffness,
                 contact_damping, final_sim_time,
                 initial_min_distance_from_plane, angle_of_inclination,
                 contact_detection_time_interval>;

  /* Required functions */
  void setup(InputTypes<This> const& input_parameters);
  void step();
  void run();

  RodPlaneCollisionCaseStudy() = default;
  RodPlaneCollisionCaseStudy(RodPlaneCollisionCaseStudy&&) noexcept = default;

  decltype(Configuration::make_time_stepper()) time_stepper =
      Configuration::make_time_stepper();
  decltype(Configuration::make_simulator()) simulator =
      Configuration::make_simulator();
  using RodRef = decltype(simulator->get_invalid_ref<CosseratRod>());
  using RodRefs = std::vector<RodRef>;
  RodRefs rod_refs;

  using ImposedPlaneRef = decltype(simulator->get_invalid_ref<ImposedPlane>());
  ImposedPlaneRef imposed_plane_ref =
      simulator->get_invalid_ref<ImposedPlane>();
  ::elastica::real_t final_time;

  inline auto communicate_from(RodRef& ref) {
    auto rod = ref.get();
    return std::make_tuple(::elastica::position(rod),
                           ::elastica::velocity(rod));
  }
  inline auto communicate() {
    // we dont have CTAD :(
    using CT = decltype(communicate_from(rod_refs[0UL]));
    using RT = std::vector<CT>;
    RT return_list;
    return_list.reserve(rod_refs.size());
    std::transform(std::begin(rod_refs), std::end(rod_refs),
                   std::back_inserter(return_list), [this](RodRef& rod_ref) {
                     return this->communicate_from(rod_ref);
                   });
    return return_list;
  }
};

ELASTICA_REGISTER_CASE_STUDY(RodPlaneCollisionCaseStudy);
