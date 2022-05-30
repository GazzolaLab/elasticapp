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

#ifndef ELASTICA_RIGIDBODYCOLLISIONCASESTUDY_HPP
#define ELASTICA_RIGIDBODYCOLLISIONCASESTUDY_HPP

//
#include <elastica/CaseStudy.hpp>
//
#include <elastica/Simulator/Environments.hpp>
#include <elastica/Simulator/Simulator.hpp>
#include <elastica/Systems/Sphere.hpp>
#include <elastica/Utilities/DefineTypes.h>
#include <elastica/Utilities/TMPL.hpp>
//
#include <algorithm>
#include <tuple>
#include <utility>
#include <vector>

// global is fine since its a cpp file
using String = const char* const;
namespace ec = elastica::configuration;
using ::elastica::rigid_body::Sphere;

struct RigidBodyCollisionConfiguration {
  static inline auto make_simulator_configuration() {
    auto system_config = ec::make_systems_configuration<tmpl::list<Sphere>>(
        ec::DefaultBlockingPolicy{},
        // sequential
        ec::make_iteration_policy(ec::iterate_across_system_types_in(ec::seq),
                                  ec::iterate_over_systems_in(ec::seq)));

    auto environment_config = ec::make_environments_configuration<
        tmpl::list<elastica::environments::tags::CollisionSystem>>(
        // iteration, currently unused
        ec::seq,
        // use default force model of a linear spring damper
        ec::make_collision_configuration(ec::seq, ec::seq));

    return ec::make_simulator_configuration(
        ec::DefaultDomainConfiguration{}, system_config,
        ec::DefaultParallelConfiguration{}, environment_config);
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

struct RigidBodyCollisionCaseStudy
    : public ::elastica::CaseStudyInterface<RigidBodyCollisionCaseStudy> {
  using Configuration = RigidBodyCollisionConfiguration;
  using This = RigidBodyCollisionCaseStudy;

  static constexpr String help = {R"help(
Rigid body collision test-case
------------------------------

Runs an example of two spheres separated along x axis with some initial velocity.
These two eventually meet and forces generate because of collision. No other
forces (like gravity) are considered.

              ░░░░░░░░░░░░                              ▒▒▒▒▒▒▒▒▒▒▒▒
          ░░░░░░░░░░░░░░░░░░░░                      ▒▒▒▒░░░░░░░░░░░░▒▒▒▒
        ░░▒▒░░░░░░░░░░░░░░░░░░░░                  ░░▒▒░░░░░░░░░░░░░░░░░░▒▒
      ░░▒▒▒▒░░░░░░░░░░░░░░░░░░░░░░              ▒▒▒▒▒▒░░░░░░░░░░░░░░░░░░░░▒▒
    ░░▒▒▒▒░░░░░░░░░░░░░░    ░░░░░░░░          ▒▒▒▒▒▒░░░░░░░░░░░░      ░░░░░░░░
    ░░▒▒▒▒░░░░░░░░░░░░        ░░░░░░          ▒▒▒▒▒▒░░░░░░░░░░░░        ░░░░▒▒
  ░░▓▓▒▒▒▒░░░░░░░░░░          ░░░░░░░░      ▒▒▓▓▒▒▒▒░░░░░░░░░░░░        ░░░░░░▒▒
  ░░▓▓▒▒▒▒░░░░░░░░░░░░░░    ░░░░░░░░░░      ▒▒▓▓▒▒▒▒░░░░░░░░░░░░░░    ░░░░░░░░▒▒
  ░░▓▓▒▒▒▒░░░░░░░░░░░░░░░░░░░░░░░░░░░░      ▒▒▓▓▒▒▒▒░░░░░░░░░░░░░░░░░░░░░░░░░░▒▒
  ░░▓▓▒▒▒▒▒▒░░░░░░░░░░░░░░░░░░░░░░░░░░      ▒▒▓▓▒▒▒▒▒▒░░░░░░░░░░░░░░░░░░░░░░░░▒▒
  ░░▓▓▓▓▒▒▒▒░░░░░░░░░░░░░░░░░░░░░░░░░░      ▒▒▓▓▓▓▒▒▒▒░░░░░░░░░░░░░░░░░░░░░░░░▒▒
  ░░▓▓▓▓▒▒▒▒▒▒░░░░░░░░░░░░░░░░░░░░░░░░      ▒▒▓▓▓▓▒▒▒▒▒▒░░░░░░░░░░░░░░░░░░░░░░▒▒
    ░░▓▓▓▓▒▒▒▒▒▒░░░░░░░░░░░░░░░░░░░░          ▒▒▓▓▓▓▒▒▒▒▒▒░░░░░░░░░░░░░░░░░░▒▒
    ░░▓▓▓▓▓▓▒▒▒▒▒▒▒▒░░░░░░░░░░▒▒▒▒░░          ▒▒▓▓▓▓▓▓▒▒▒▒▒▒▒▒░░░░░░░░░░▒▒▒▒▒▒
      ░░▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒░░              ▒▒▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒
        ░░▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▒▒░░                  ▒▒▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒
          ░░░░▓▓▓▓▓▓▓▓▓▓▒▒░░░░                      ▒▒▒▒▓▓▓▓▓▓▓▓▓▓▓▓▒▒▒▒
              ░░░░░░░░░░░░                              ▒▒▒▒▒▒▒▒▒▒▒▒

   ^ y
   |
   |
   o — — —> x
  /
 /
z

The collision forces generated are elastic, hence the total energy of the system
(kinetic energies of the sphere + potential energy due to collision) is
conserved.

)help"};

  struct radius1 {
    using type = ::elastica::real_t;
    static constexpr String help = {"Radius of the first sphere"};
    static type default_value() { return 0.05; }
  };

  struct radius2 {
    using type = ::elastica::real_t;
    static constexpr String help = {"Radius of the second sphere"};
    static type default_value() { return 0.05; }
  };

  struct xvelocity1 {
    using type = ::elastica::real_t;
    static constexpr String help = {"x velocity of the first sphere"};
    static type default_value() { return 0.2; }
  };

  struct xvelocity2 {
    using type = ::elastica::real_t;
    static constexpr String help = {"x velocity of the second sphere"};
    static type default_value() { return -0.2; }
  };

  struct dt {
    using type = ::elastica::real_t;
    static constexpr String help = {"Time step of simulation"};
    static type default_value() { return 1e-02; }
  };

  using Parameters = tmpl::list<radius1, radius2, xvelocity1, xvelocity2, dt>;

  /* Required functions */
  void setup(InputTypes<This> const& input_parameters);
  void step();
  void run();

  RigidBodyCollisionCaseStudy() = default;
  RigidBodyCollisionCaseStudy(RigidBodyCollisionCaseStudy&&) noexcept = default;

  decltype(Configuration::make_time_stepper()) time_stepper =
      Configuration::make_time_stepper();
  decltype(Configuration::make_simulator()) simulator =
      Configuration::make_simulator();
  using SphereRef = decltype(simulator->get_invalid_ref<Sphere>());
  using SphereRefs = std::vector<SphereRef>;

  SphereRefs sphere_refs{2UL, simulator->get_invalid_ref<Sphere>()};

  inline auto communicate_from(SphereRef& ref) {
    auto sphere = ref.get();
    return std::make_tuple(::elastica::position(sphere),
                           ::elastica::velocity(sphere));
  }
  inline auto communicate() {
    // we dont have CTAD :(
    using CT = decltype(communicate_from(sphere_refs[0UL]));
    using RT = std::vector<CT>;
    RT return_list;
    return_list.reserve(sphere_refs.size());
    std::transform(std::begin(sphere_refs), std::end(sphere_refs),
                   std::back_inserter(return_list), [this](SphereRef& rod_ref) {
                     return this->communicate_from(rod_ref);
                   });
    // return also the interpenetration here
    auto& collision_system =
        simulator
            ->get_environment<elastica::environments::tags::CollisionSystem>();
    auto& contacts = collision_system->contacts();
    ::elastica::real_t penetration_depth =
        contacts.empty() ? 0.0 : -contacts[0].get_distance();
    return std::make_tuple(penetration_depth, std::move(return_list));
  }
};

ELASTICA_REGISTER_CASE_STUDY(RigidBodyCollisionCaseStudy);

#endif  // ELASTICA_RIGIDBODYCOLLISIONCASESTUDY_HPP
