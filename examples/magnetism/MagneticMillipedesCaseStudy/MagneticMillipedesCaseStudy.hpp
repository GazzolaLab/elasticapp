//==============================================================================
/*!
//  \file
//  \brief Oscillatory magnetic cilia carpet study
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

#include <algorithm>
#include <tuple>
#include <utility>
#include <vector>
//
#include <elastica/CaseStudy.hpp>
//
#include <elastica/Simulator/Connections.hpp>
#include <elastica/Simulator/Constraints.hpp>
#include <elastica/Simulator/Environments.hpp>
#include <elastica/Simulator/Forcing.hpp>
#include <elastica/Simulator/Simulator.hpp>
#include <elastica/Systems/CosseratRods.hpp>
#include <elastica/Utilities/DefineTypes.h>
#include <elastica/Utilities/Geometry/Angles.hpp>
#include <elastica/Utilities/TMPL.hpp>
//
#include "Extension/Simulator/Environments.hpp"
#include "Extension/Systems/CosseratRods.hpp"
#include "FieldProfiles/OscillatoryMagneticField.hpp"  //customization
#include "FieldProfiles/RampedAdapter.hpp"             // customization

// global alias
using String = const char* const;
using Vec3 = ::elastica::Vec3;
using Rot3 = ::elastica::Rot3;
using real_t = ::elastica::real_t;
using MagnetizedCosseratRod =
    ::elastica::cosserat_rod::MagnetizedCosseratRodWithoutDamping;
using CosseratRod = ::elastica::cosserat_rod::CosseratRodWithoutDamping;
using MagneticRodInitializer =
    ::elastica::cosserat_rod::StraightUniformMagnetizedCosseratRodInitializer;
using RodInitializer =
    ::elastica::cosserat_rod::StraightCosseratRodWithoutDampingInitializer;

using ::elastica::interface::at;
using Traits = ::elastica::cosserat_rod::DefaultCosseratRodTraits;

namespace ec = elastica::configuration;
struct MagneticMillipedesConfiguration {
  static inline auto make_simulator_configuration() {
    auto system_config = ec::make_systems_configuration<
        tmpl::list<CosseratRod, MagnetizedCosseratRod>>(
        ec::make_blocking_policy(ec::RestrictSizeAcrossBlockTypes{256UL}),
        ec::make_iteration_policy(ec::iterate_across_system_types_in(ec::seq),
                                  ec::iterate_over_systems_in(ec::seq)));

    using EnvironmentTags =
        tmpl::list<::elastica::environments::tags::MagneticFieldTag,
                   ::elastica::environments::tags::GravityField,
                   ::elastica::environments::tags::CollisionSystem>;

    auto gravity_config =
        ec::make_gravity_configuration(::elastica::Vec3{0., 0., 0.});

    using ImposedPrimitives = tmpl::list<::elastica::collision::ImposedHalfSpacePrimitive>;
    auto collision_config =
        ec::make_collision_configuration(ec::par,   // between each type
                                         ec::seq);  // across each type

    auto magnetic_config =
        ec::make_magnetic_field_configuration(::elastica::make_ramped_field(
            ::elastica::environments::OscillatoryMagneticField(),
            // I initialize some parameter values here, but these can
            // be modified later on
            ::elastica::make_linear_rampup(0.0, 200.0)));

    auto env_config = ec::make_environments_configuration<EnvironmentTags, ImposedPrimitives>(
        // process environments in sequence
        ec::seq, magnetic_config, gravity_config, collision_config);

    return ec::make_simulator_configuration(
        ec::DefaultDomainConfiguration{}, system_config,
        ec::DefaultParallelConfiguration{},
        ec::DefaultConstraintsConfiguration{},
        ec::DefaultConnectionsConfiguration{}, env_config);
  }

  static inline auto make_simulator() {
    return elastica::make_simulator<::elastica::modules::Constraints,
                                    ::elastica::modules::Connections,
                                    ::elastica::modules::Environments>(
        make_simulator_configuration());
  }

  static inline auto make_time_stepper() {
    using TimeStepper = ::elastica::timestepper::PositionVerlet;
    return TimeStepper{};
  }
};

struct MagneticMillipedesCaseStudy
    : public ::elastica::CaseStudyInterface<MagneticMillipedesCaseStudy> {
  using Configuration = MagneticMillipedesConfiguration;
  using This = MagneticMillipedesCaseStudy;

  static constexpr String help = {R"help(
Magnetic millipedes case study
------------------------------
Demonstrates an magnetic millipedes simulation.
)help"};

  struct n_elements {
    // unsigned integer parameter
    using type = std::size_t;
    // documentation
    static constexpr String help = {"Number of elements in the rod"};
    static type default_value() { return 10UL; }
  };

  struct radius {
    using type = ::elastica::real_t;
    static constexpr String help = {"Radius of the rod"};
    static type default_value() { return 0.15; }
  };

  struct damping_constant {
    using type = ::elastica::real_t;
    static constexpr String help = {"Analytical Damping Constant"};
    static type default_value() { return 1.5; }
  };

  struct length {
    using type = ::elastica::real_t;
    static constexpr String help = {"Length of the rod"};
    static type default_value() { return 6.0; }
  };

  struct dt {
    using type = ::elastica::real_t;
    static constexpr String help = {"Time step of simulation"};
    static type default_value() {
      return type(0.01) *
             (length::default_value() / type(n_elements::default_value()));
    }
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
    static type default_value() { return type(200.0); }
  };

  using Parameters = tmpl::list<n_elements, length, radius, damping_constant,
                                dt, contact_stiffness, contact_damping_normal,
                                contact_damping_tangential, final_sim_time>;

  /* Required functions */
  void setup(InputTypes<This> const& input_parameters);
  void step();
  void run();

  /* Features */
  template <typename RodType>
  void add_analytical_damper(std::vector<RodType>&, InputTypes<This> const&,
                             const real_t&);
  template <typename RodType>
  void group_parallel_connection(std::vector<RodType>&);
  template <typename RodTypeOne, typename RodTypeTwo>
  void crisscross_connection(std::vector<RodTypeOne>&,
                             std::vector<RodTypeTwo>&);
  template <typename RodTypeOne, typename RodTypeTwo>
  void orthogonal_connection(std::vector<RodTypeOne>&,
                             std::vector<RodTypeTwo>&);
  ::elastica::real_t final_time;

  // Rods
  decltype(Configuration::make_time_stepper()) time_stepper =
      Configuration::make_time_stepper();
  decltype(Configuration::make_simulator()) simulator =
      Configuration::make_simulator();
  using CRRef = decltype(simulator->get_invalid_ref<CosseratRod>());
  using MCRRef = decltype(simulator->get_invalid_ref<MagnetizedCosseratRod>());
  std::vector<CRRef> backbone_rod_list, backbone_rod_second_layer_list;
  std::vector<MCRRef> magnetic_rod_list;

  // Plane (Bottom)
  using ImposedHalfSpace = ::elastica::collision::ImposedPrimitiveAdapter<
      ::elastica::collision::HalfSpacePrimitive>;
  using ImposedHalfPlaneRef =
      decltype(simulator->get_invalid_ref<ImposedHalfSpace>());
  ImposedHalfPlaneRef imposed_half_space_bottom_ref =
      simulator->get_invalid_ref<ImposedHalfSpace>();

  inline auto communicate_from(CRRef& ref) {
    auto rod = ref.get();
    return std::make_tuple(::elastica::position(rod),
                           ::elastica::velocity(rod));
  }
  inline auto communicate() {
    // we dont have CTAD :(
    //    using CT = decltype(communicate_from(std::declval<CRRef&>()));
    //    using RT = std::vector<CT>;
    //    RT return_list;
    //    return_list.reserve(rod_refs.size());
    //    std::transform(std::begin(rod_refs), std::end(rod_refs),
    //                   std::back_inserter(return_list), [this](CRRef& rod_ref)
    //                   {
    //                     return this->communicate_from(rod_ref);
    //                   });
    //    // return also the interpenetration here
    //    return return_list;
  }

  MagneticMillipedesCaseStudy() = default;
  MagneticMillipedesCaseStudy(MagneticMillipedesCaseStudy&&) noexcept = default;
};

ELASTICA_REGISTER_CASE_STUDY(MagneticMillipedesCaseStudy);
