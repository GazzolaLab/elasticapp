//==============================================================================
/*!
//  \file
//  \brief Imposed primitive case study
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#ifndef ELASTICA_IMPOSEDPRIMITIVECASESTUDY_HPP
#define ELASTICA_IMPOSEDPRIMITIVECASESTUDY_HPP

//
#include <elastica/CaseStudy.hpp>
//
#include <elastica/Simulator/Environments.hpp>
#include <elastica/Simulator/Simulator.hpp>
#include <elastica/Systems/RigidBody.hpp>
#include <elastica/Utilities/DefineTypes.h>
#include <elastica/Utilities/Math/Vec3.hpp>
#include <elastica/Utilities/TMPL.hpp>
//
#include <tuple>
#include <utility>
#include <vector>

// global is fine since its a cpp file
using String = const char* const;
namespace ec = elastica::configuration;
using Sphere = ::elastica::rigid_body::Sphere;
#define USE_PLANE 1

using ImposedGround =
#if USE_PLANE
    ::elastica::collision::ImposedPlanePrimitive;
#else
    ::elastica::collision::ImposedHalfSpacePrimitive;
#endif

struct ImposedPrimitiveConfiguration {
  static inline auto make_simulator_configuration() {
    auto system_config = ec::make_systems_configuration<tmpl::list<Sphere>>(
        ec::make_blocking_policy(ec::RestrictSizeAcrossBlockTypes{50UL}),
        ec::make_iteration_policy(ec::iterate_across_system_types_in(ec::seq),
                                  ec::iterate_over_systems_in(ec::seq)));

    using EnvironmentTags =
        tmpl::list<::elastica::environments::tags::GravityField,
                   ::elastica::environments::tags::CollisionSystem>;
    // Tag for bottom plane
    using ImposedPrimitives = tmpl::list<ImposedGround>;

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

struct ImposedPrimitiveCaseStudy
    : public ::elastica::CaseStudyInterface<ImposedPrimitiveCaseStudy> {
  using Configuration = ImposedPrimitiveConfiguration;
  using This = ImposedPrimitiveCaseStudy;

  static constexpr String help = {R"help(
Imposed primitive test-case
---------------------------

Runs an example of a plane oscillating about the origin, with a sphere initially
held above the plane at rest. The sphere falls due to gravity towards the plane,
eventually meeting the plane. The forces generated due to the collision leads to
chaotic dynamics of the sphere. Entire papers have been written about this topic
but perhaps the most cited one is

Holmes, P., 1982, “The Dynamics of Repeated Impacts with a Sinusoidally
Vibrating Table,” J. Sound Vib., 64(2), pp. 173–189.
)help"};

  // replace by gravity
  struct gravitational_acc {
    using type = ::elastica::real_t;
    static constexpr String help = {"Gravitational acceleration"};
    static type default_value() { return -9.80665; }
  };

  struct radius {
    using type = ::elastica::real_t;
    static constexpr String help = {"Radius of the sphere"};
    static type default_value() { return 1.21; }
  };

  struct initial_height_from_plane {
    using type = ::elastica::real_t;
    static constexpr String help = {
        "Initial distance of the COM from the plane"};
    static type default_value() { return 5.0; }
  };

  struct dt {
    using type = ::elastica::real_t;
    static constexpr String help = {"Time step of simulation"};
    static type default_value() { return 2e-4; }
  };

  struct contact_stiffness {
    using type = ::elastica::real_t;
    static constexpr String help = {"Contact stiffness"};
    static type default_value() { return 1e5; }
  };

  struct contact_damping {
    using type = ::elastica::real_t;
    static constexpr String help = {"Contact damping"};
    static type default_value() { return 0.1; }
  };

  using Parameters =
      tmpl::list<gravitational_acc, radius, dt, contact_stiffness,
                 contact_damping, initial_height_from_plane>;

  /* Required functions */
  void setup(InputTypes<This> const& input_parameters);
  void step();
  void run();

  ImposedPrimitiveCaseStudy() = default;
  ImposedPrimitiveCaseStudy(ImposedPrimitiveCaseStudy&&) noexcept = default;

  decltype(Configuration::make_time_stepper()) time_stepper =
      Configuration::make_time_stepper();
  decltype(Configuration::make_simulator()) simulator =
      Configuration::make_simulator();
  using SphereRef = decltype(simulator->get_invalid_ref<Sphere>());
  using SphereRefs = std::vector<SphereRef>;
  SphereRefs sphere_refs;

  using ImposedGroundRef =
      decltype(simulator->get_invalid_ref<ImposedGround>());
  ImposedGroundRef imposed_ground_ref =
      simulator->get_invalid_ref<ImposedGround>();

  inline auto communicate_from(SphereRef const& ref) { return ref.get(); }

  inline auto communicate() {
    // return also the interpenetration here
    auto& collision_system =
        simulator
            ->get_environment<elastica::environments::tags::CollisionSystem>();
    auto& contacts = collision_system->contacts();
    ::elastica::real_t penetration_depth =
        contacts.empty() ? 0.0 : -contacts[0].get_distance();

    using V = blaze::DynamicVector<::elastica::real_t>;
    V pp{::elastica::position(imposed_ground_ref.get())};
    V pv{::elastica::velocity(imposed_ground_ref.get())};
    return std::make_tuple(penetration_depth,
                           communicate_from(sphere_refs[0UL]),
                           std::make_tuple(std::move(pp), std::move(pv)));
  }
};

ELASTICA_REGISTER_CASE_STUDY(ImposedPrimitiveCaseStudy);

#endif  // ELASTICA_IMPOSEDPRIMITIVECASESTUDY_HPP
