//==============================================================================
/*!
//  \file
//  \brief Nest with wall case study
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy, Yashraj Bhosale, Seung Hyun
Kim
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#if ELASTICA_TBB_PARALLEL_MODE
#include <tbb/global_control.h>
#endif

#include <elastica/CaseStudy.hpp>
#include <elastica/Simulator/Constraints.hpp>
#include <elastica/Simulator/Environments.hpp>
#include <elastica/Simulator/Forcing.hpp>
#include <elastica/Simulator/Simulator.hpp>
#include <elastica/Systems/CosseratRods.hpp>
#include <elastica/Systems/CosseratRods/Utility.hpp>
#include <elastica/Utilities/DefineTypes.h>
#include <elastica/Utilities/TMPL.hpp>

// global is fine since its a cpp file
using String = const char* const;

namespace ec = elastica::configuration;

using CosseratRod = ::elastica::cosserat_rod::CosseratRodWithoutDamping;
using StraightInit = ::elastica::cosserat_rod::StraightCosseratRodWithoutDampingInitializer;
using ImposedHalfSpace = ::elastica::collision::ImposedHalfSpacePrimitive;

struct SimulationConfiguration {
  static inline auto make_simulator_configuration() {
    auto system_config =
        ec::make_systems_configuration<tmpl::list<CosseratRod>>(
            ec::make_blocking_policy(ec::RestrictSizeAcrossBlockTypes{16UL}),
            ec::make_iteration_policy(
                ec::iterate_across_system_types_in(ec::seq),
                ec::iterate_over_systems_in(ec::par)));

    auto gravity_config =
        ec::make_gravity_configuration(::elastica::Vec3{0., 0., 0.});

    using EnvironmentTags =
        tmpl::list<::elastica::environments::tags::GravityField,
                   ::elastica::environments::tags::CollisionSystem>;

    //     Tag for bottom plane
    using ImposedPrimitives = tmpl::list<ImposedHalfSpace>;
    auto collision_config =
        ec::make_collision_configuration(ec::par,   // between each type
                                         ec::seq);  // across each type

    auto env_config =
        ec::make_environments_configuration<EnvironmentTags, ImposedPrimitives>(
            ec::seq, gravity_config, collision_config);

    return ec::make_simulator_configuration(
        ec::DefaultDomainConfiguration{}, system_config,
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

struct MovingWallStudy
    : public ::elastica::CaseStudyInterface<MovingWallStudy> {
  using Configuration = SimulationConfiguration;
  using This = MovingWallStudy;
  using ImposedHalfSpace = ::elastica::collision::ImposedPrimitiveAdapter<
      ::elastica::collision::HalfSpacePrimitive>;


  static constexpr String help = {R"help(
Nest with sidewall case study
--------------------------

This case considers a random packing of sticks initialised in a region
specified by the user. The sticks once initialised fall on the ground in a
walled container due to the influence of gravity, while undergoing collisions
with the ground, the walls and each other.
TODO: Check stress strain curves!

)help"};

  struct n_elements {
    // unsigned integer parameter
    using type = std::size_t;
    // documentation
    static constexpr String help = {"Number of elements in the rod"};
    static type default_value() { return 8UL; }
  };

  struct damping {
    using type = ::elastica::real_t;
    static constexpr String help = {"Internal damping in the rod"};
    static type default_value() { return 50.0; }
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
    static type default_value() { return -981.0; }
  };

  struct radius {
    using type = ::elastica::real_t;
    static constexpr String help = {"Radius of the rod"};
    static type default_value() { return 1.21; }
  };

  struct length {
    using type = ::elastica::real_t;
    static constexpr String help = {"Length of the rod"};
    static type default_value() { return 75.74; }
  };

  struct packing_initial_vertical_span {
    using type = ::elastica::real_t;
    static constexpr String help = {"Vertical span of the initialised packing"};
    static type default_value() { return 75.74*1.5; }
  };

  struct packing_initial_radial_span {
    using type = ::elastica::real_t;
    static constexpr String help = {"Radial span of the initialised packing"};
    static type default_value() { return 10.0; }
  };

  struct dt {
    using type = ::elastica::real_t;
    static constexpr String help = {"Time step of simulation"};
    static type default_value() { return 5e-4; }
  };

  struct contact_stiffness {
    using type = ::elastica::real_t;
    static constexpr String help = {"Contact stiffness"};
    static type default_value() { return 1e6; }
  };

  struct contact_damping_normal {
    using type = ::elastica::real_t;
    static constexpr String help = {"Contact damping in normal direction"};
    static type default_value() { return 1e1; }
  };

  struct contact_damping_tangential {
    using type = ::elastica::real_t;
    static constexpr String help = {"Contact damping in tangential direction"};
    static type default_value() { return 1e1; }
  };

  struct final_sim_time {
    using type = ::elastica::real_t;
    static constexpr String help = {"Final simulation time"};
    static type default_value() { return 1.0; }
  };

  using Parameters =
      tmpl::list<n_elements, damping, youngs_modulus, shear_modulus,
                 gravitational_acc, radius, length, dt, contact_stiffness,
                 contact_damping_normal, contact_damping_tangential,
                 final_sim_time, packing_initial_vertical_span,
                 packing_initial_radial_span>;

  /* Required functions */
  void setup(InputTypes<This> const& input_parameters);
  void step();
  void run();

  MovingWallStudy() = default;
  MovingWallStudy(MovingWallStudy&&) noexcept = default;

  elastica::MaterialID wall_material;

  decltype(Configuration::make_time_stepper()) time_stepper =
      Configuration::make_time_stepper();
  decltype(Configuration::make_simulator()) simulator =
      Configuration::make_simulator();

  // Rod
  using RodRef = decltype(simulator->get_invalid_ref<CosseratRod>());
  using RodRefs = std::vector<RodRef>;
  RodRefs rod_refs;

  // Plane (Bottom)
  using ImposedHalfPlaneRef =
      decltype(simulator->get_invalid_ref<ImposedHalfSpace>());
  ImposedHalfPlaneRef imposed_half_space_bottom_ref =
      simulator->get_invalid_ref<ImposedHalfSpace>();
  ImposedHalfPlaneRef imposed_half_space_top_ref =
      simulator->get_invalid_ref<ImposedHalfSpace>();

  ::elastica::real_t final_time;

  inline auto communicate_from(RodRef& ref) { return ref.get(); }
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

    // return also the interpenetration here
    auto& collision_system =
        simulator
            ->get_environment<elastica::environments::tags::CollisionSystem>();
    auto& contacts = collision_system->contacts();
    using Penetrations = std::vector<elastica::real_t>;
    Penetrations penetrations;
    penetrations.reserve(contacts.size());
    std::transform(std::cbegin(contacts), std::cend(contacts),
                   std::back_inserter(penetrations),
                   [](auto& c) { return -c.get_distance(); });

    return std::make_tuple(return_list, std::move(penetrations));
  }
};

ELASTICA_REGISTER_CASE_STUDY(MovingWallStudy);
