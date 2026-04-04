//==============================================================================
/*!
//  \file
//  \brief Oscillatory magnetic cilia carpet study
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy, Yashraj Bhosale - All Rights
//  Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

#include <algorithm>
#include <tuple>
#include <utility>
#include <vector>
#define MAGNETIC_FIELD_ON 1
#define BACKBONE_CONNECTIONS 1
#define SPHERES_INCLUDED 0
#define COLLISION_ON SPHERES_INCLUDED

#if SPHERES_INCLUDED
#include <elastica/Systems/RigidBody.hpp>
#endif
//
#include "Customization.hpp"
#include "Types.hpp"
//
#include "Extension/Simulator/Environments.hpp"
#include "Extension/Systems/CosseratRods.hpp"
#include "FieldProfiles/OscillatoryMagneticField.hpp"  // customization
#include "FieldProfiles/RampedAdapter.hpp"             // customization
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

// global alias
using String = const char* const;
using Vec3 = ::elastica::Vec3;
using Rot3 = ::elastica::Rot3;
using real_t = ::elastica::real_t;
using MagnetizedCosseratRod =
    ::elastica::cosserat_rod::MagnetizedCosseratRodWithoutDamping;
using ::elastica::cosserat_rod::detail::BackBoneCrissRod;
using ::elastica::cosserat_rod::detail::BackBoneCrossRod;
using MagneticRodInitializer = ::elastica::cosserat_rod::
    StraightUniformMagnetizedCosseratRodWithoutDampingInitializer;
using RodInitializer =
    ::elastica::cosserat_rod::StraightCosseratRodWithoutDampingInitializer;

#if SPHERES_INCLUDED
using Sphere = ::elastica::rigid_body::Sphere;
#endif

using ::elastica::interface::at;
using Traits = ::elastica::cosserat_rod::DefaultCosseratRodTraits;

namespace ec = elastica::configuration;

struct MagneticMillipedesConfiguration {
  static inline auto make_simulator_configuration() {
    auto system_config = ec::make_systems_configuration<
        tmpl::list<BackBoneCrissRod, BackBoneCrossRod, MagnetizedCosseratRod
#if SPHERES_INCLUDED
                   ,
                   Sphere
#endif
                   >>(
        ec::make_blocking_policy(ec::RestrictSizeAcrossBlockTypes{512UL}),
        ec::make_iteration_policy(ec::iterate_across_system_types_in(ec::seq),
                                  ec::iterate_over_systems_in(ec::seq)));

    // Order of fields important here?
    using EnvironmentTags = tmpl::list<
#if MAGNETIC_FIELD_ON
        ::elastica::environments::tags::MagneticFieldTag,
#endif
#if COLLISION_ON
        ::elastica::environments::tags::CollisionSystem,
#endif
        ::elastica::environments::tags::GravityField>;

    auto gravity_config =
        ec::make_gravity_configuration(::elastica::Vec3{0., 0., 0.});

    using ImposedPrimitives =
        tmpl::list<::elastica::collision::ImposedHalfSpacePrimitive>;
    auto collision_config =
        ec::make_collision_configuration(ec::par,   // between each type
                                         ec::seq);  // across each type

    auto magnetic_config =
        ec::make_magnetic_field_configuration(::elastica::make_ramped_field(
            ::elastica::environments::OscillatoryMagneticField(),
            // I initialize some parameter values here, but these can
            // be modified later on
            ::elastica::make_step_rampup(0.0, 200.0)));

    auto env_config =
        ec::make_environments_configuration<EnvironmentTags, ImposedPrimitives>(
            // process environments in sequence
            ec::seq, gravity_config
#if MAGNETIC_FIELD_ON
            ,
            magnetic_config
#endif
#if COLLISION_ON
            ,
            collision_config
#endif
        );

    auto forcing_config = ec::make_forcing_configuration(ec::par);

    return ec::make_simulator_configuration(
        ec::DefaultDomainConfiguration{}, system_config,
        ec::DefaultParallelConfiguration{},
        ec::DefaultConstraintsConfiguration{},
#if BACKBONE_CONNECTIONS
        ec::DefaultConnectionsConfiguration{},
#endif
        env_config, forcing_config);
  }

  static inline auto make_simulator() {
    return elastica::make_simulator<::elastica::modules::Constraints,
#if BACKBONE_CONNECTIONS
                                    ::elastica::modules::Connections,
#endif
                                    ::elastica::modules::Environments,
                                    // Important : forcing contains locomotive
                                    // friction that needs to happen after all
                                    // forces are applied.
                                    ::elastica::modules::Forcing>(
        make_simulator_configuration());
  }

  static inline auto make_time_stepper() {
    using TimeStepper = ::elastica::timestepper::PositionVerlet;
    return TimeStepper{};
  }
};

template <typename T>
inline auto communicate_buffer(
    std::vector<blocks::BlockSliceGenerator<T>> const& ref_generators) {
  // we dont have CTAD :(
  using SliceT =
      decltype(std::declval<blocks::BlockSliceGenerator<T> const&>().get());
  using ReturnT = std::vector<SliceT>;

  ReturnT return_list;
  return_list.reserve(ref_generators.size());
  std::transform(std::cbegin(ref_generators), std::cend(ref_generators),
                 std::back_inserter(return_list),
                 [](blocks::BlockSliceGenerator<T> const& rod_ref) {
                   return rod_ref.get();
                 });

  return return_list;
}

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

  struct radius_magnetic {
    using type = ::elastica::real_t;
    static constexpr String help = {"Radius of the magnetic rod"};
    static type default_value() { return 0.15; }
  };

  struct radius_backbone {
    using type = ::elastica::real_t;
    static constexpr String help = {"Radius of the backbone rod"};
    static type default_value() { return 0.15; }
  };

  struct length_magnetic {
    using type = ::elastica::real_t;
    static constexpr String help = {"Length of the magnetic rod"};
    static type default_value() { return 3.0 / 4.0; }
  };

  struct damping_constant_magnetic {
    using type = ::elastica::real_t;
    static constexpr String help = {
        "Analytical Damping Constant for magnetic rod"};
    static type default_value() { return 1.5; }
  };

  struct number_magnetic_rods_per_backbone {
    // unsigned integer parameter
    using type = std::size_t;
    // documentation
    static constexpr String help = {""};
    static type default_value() { return 6UL; }
  };

  struct number_magnetic_rods_spanwise {
    // unsigned integer parameter
    using type = std::size_t;
    // documentation
    static constexpr String help = {""};
    static type default_value() { return 4UL; }
  };

  struct number_backbone_rods_for_every_magnetic_rod {
    // unsigned integer parameter
    using type = std::size_t;
    // documentation
    static constexpr String help = {""};
    static type default_value() { return 4UL; }
  };

  struct magnetic_rod_spacing_ratio {
    using type = ::elastica::real_t;
    static constexpr String help = {"Spacing to length"};
    static type default_value() { return 2.0; }
  };

  struct damping_constant_backbone {
    using type = ::elastica::real_t;
    static constexpr String help = {
        "Analytical Damping Constant for backbone rod"};
    static type default_value() { return 1.5; }
  };

  struct density_magnetic_to_backbone {
    using type = ::elastica::real_t;
    static constexpr String help = {"Density factor"};
    static type default_value() { return 5.0; }
  };

  struct stiffness_magnetic_to_backbone {
    using type = ::elastica::real_t;
    static constexpr String help = {"Density factor"};
    static type default_value() { return 1.0; }
  };

  struct dt {
    using type = ::elastica::real_t;
    static constexpr String help = {"Time step of simulation"};
    static type default_value() {
      return type(0.01) * (length_magnetic::default_value() /
                           type(n_elements::default_value()));
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

  struct acc_gravity {
    using type = ::elastica::real_t;
    static constexpr String help = {"Gravity acc."};
    static type default_value() { return -9.8065 / 10.0; }
  };

  struct mag_strength {
    using type = ::elastica::real_t;
    static constexpr String help = {"Magnetic field strength."};
    static type default_value() {
      // 80mT
      return 8e-2;
    }
  };

  struct mag_angular_frequency {
    using type = ::elastica::real_t;
    static constexpr String help = {
        "Angular frequency of the rotating magnetic field."};
    static type default_value() { return 5.0 * M_PI / 180.0; }
  };

  struct friction_coeff {
    using type = ::elastica::real_t;
    static constexpr String help = {"Friction coeff."};
    static type default_value() { return 0.2; }
  };

  struct sphere_radius_to_length_ratio {
    using type = ::elastica::real_t;
    static constexpr String help = {"Radius to length ratio."};
    static type default_value() { return 1.0; }
  };

  struct sphere_offset_to_length_ratio {
    using type = ::elastica::real_t;
    static constexpr String help = {"Offset of sphere from front."};
    static type default_value() { return 2.0; }
  };

  struct density_sphere_to_magnetic {
    using type = ::elastica::real_t;
    static constexpr String help = {"Density sphere to magnetic."};
    static type default_value() { return 0.1; }
  };

  struct final_sim_time {
    using type = ::elastica::real_t;
    static constexpr String help = {"Final simulation time"};
    static type default_value() { return type(200.0); }
  };

  using Parameters = tmpl::list<
      n_elements, length_magnetic, radius_magnetic, radius_backbone,
      number_magnetic_rods_per_backbone, number_magnetic_rods_spanwise,
      number_backbone_rods_for_every_magnetic_rod, magnetic_rod_spacing_ratio,
      damping_constant_magnetic, damping_constant_backbone,
      density_magnetic_to_backbone, stiffness_magnetic_to_backbone, dt,
      contact_stiffness, contact_damping_normal, contact_damping_tangential,
      acc_gravity, mag_strength, mag_angular_frequency, friction_coeff,
      sphere_radius_to_length_ratio, sphere_offset_to_length_ratio,
      density_sphere_to_magnetic, final_sim_time>;

  /* Required functions */
  void setup(InputTypes<This> const& input_parameters);
  void step();
  void run();
  inline void progress_bar(float, float&,
                           std::chrono::time_point<std::chrono::system_clock>);
  template <typename RodRefType>
  inline void log_rod_position(std::vector<RodRefType>&, const int,
                               const char*);
  //  inline void log_penetration(const int);

  /* Features */
  template <typename Plugin>
  void add_analytical_damper(
      std::vector<blocks::BlockSliceGenerator<Plugin>> const&, real_t, real_t);
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

  using CrissRodRef = decltype(simulator->get_invalid_ref<BackBoneCrissRod>());
  using CrossRodRef = decltype(simulator->get_invalid_ref<BackBoneCrossRod>());
  using MCRRef = decltype(simulator->get_invalid_ref<MagnetizedCosseratRod>());

  std::vector<CrissRodRef> backbone_criss_rods;
  std::vector<CrossRodRef> backbone_cross_rods;
  std::vector<MCRRef> magnetic_rod_list;

#if SPHERES_INCLUDED
  using SphereRef = decltype(simulator->get_invalid_ref<Sphere>());
  using SphereRefs = std::vector<SphereRef>;
  SphereRefs spheres_list;
#endif

  // Plane (Bottom)
  using ImposedHalfSpace = ::elastica::collision::ImposedPrimitiveAdapter<
      ::elastica::collision::HalfSpacePrimitive>;
  using ImposedHalfPlaneRef =
      decltype(simulator->get_invalid_ref<ImposedHalfSpace>());
  ImposedHalfPlaneRef imposed_half_space_bottom_ref =
      simulator->get_invalid_ref<ImposedHalfSpace>();

  inline auto communicate() {
#if COLLISION_ON
    auto& collision_system =
        simulator
            ->get_environment<elastica::environments::tags::CollisionSystem>();
    auto& contacts = collision_system->contacts();
    ::elastica::real_t penetration_depth =
        contacts.empty() ? 0.0 : -contacts[0].get_distance();
#else
    ::elastica::real_t penetration_depth = 0.0;
#endif

    return std::make_tuple(communicate_buffer(backbone_criss_rods),
                           communicate_buffer(backbone_cross_rods),
                           communicate_buffer(magnetic_rod_list)
#if SPHERES_INCLUDED
                               ,
                           communicate_buffer(spheres_list)
#endif
                               ,
                           penetration_depth);
  }

  MagneticMillipedesCaseStudy() = default;
  MagneticMillipedesCaseStudy(MagneticMillipedesCaseStudy&&) noexcept = default;
};

ELASTICA_REGISTER_CASE_STUDY(MagneticMillipedesCaseStudy);
