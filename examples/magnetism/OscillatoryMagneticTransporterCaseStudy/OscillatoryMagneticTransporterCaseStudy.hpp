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

#include <vector>
//
#include <elastica/CaseStudy.hpp>
//
#include <elastica/Simulator/Constraints.hpp>
#include <elastica/Simulator/Environments.hpp>
#include <elastica/Simulator/Simulator.hpp>
#include <elastica/Systems/RigidBody.hpp>
#include <elastica/Utilities/DefineTypes.h>
#include <elastica/Utilities/Geometry/Angles.hpp>
#include <elastica/Utilities/TMPL.hpp>
//
#include "Extension/Simulator/Environments.hpp"
#include "Extension/Systems/CosseratRods.hpp"
#include "FieldProfiles/OscillatoryMagneticField.hpp"  //customization
#include "FieldProfiles/RampedAdapter.hpp"             // customization

// global is fine since its a cpp file
using String = const char* const;
namespace ec = elastica::configuration;
using MagnetizedCosseratRod =
    ::elastica::cosserat_rod::MagnetizedCosseratRodWithoutDamping;
using Sphere = ::elastica::rigid_body::Sphere;

struct OscillatoryMagneticTransporterConfiguration {
  static inline auto make_simulator_configuration() {
    auto system_config =
        ec::make_systems_configuration<tmpl::list<MagnetizedCosseratRod, Sphere>>(
            ec::make_blocking_policy(ec::RestrictSizeAcrossBlockTypes{50UL}),
            ec::make_iteration_policy(
                ec::iterate_across_system_types_in(ec::seq),
                ec::iterate_over_systems_in(ec::seq)));

    using EnvironmentTags =
        tmpl::list<::elastica::environments::tags::MagneticFieldTag,
                   ::elastica::environments::tags::GravityField,
                   ::elastica::environments::tags::CollisionSystem>;

    auto collision_config =
        ec::make_collision_configuration(ec::par,   // between each type
                                         ec::seq);  // across each type

    auto magnetic_config =
        ec::make_magnetic_field_configuration(::elastica::make_ramped_field(
            ::elastica::environments::OscillatoryMagneticField(),
            // I initialize some parameter values here, but these can
            // be modified later on
            ::elastica::make_linear_rampup(0.0, 200.0)));
    auto gravity_config =
        ec::make_gravity_configuration(::elastica::Vec3{0., 0., 0.});

    auto env_config = ec::make_environments_configuration<EnvironmentTags>(
        // process environments in sequence
        ec::seq, magnetic_config, gravity_config, collision_config);

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

struct OscillatoryMagneticTransporterCaseStudy
    : public ::elastica::CaseStudyInterface<
          OscillatoryMagneticTransporterCaseStudy> {
  using Configuration = OscillatoryMagneticTransporterConfiguration;
  using This = OscillatoryMagneticTransporterCaseStudy;

  static constexpr String help = {R"help(
Oscillatory magnetic cilia carpet study
-----------------------------
Demonstrates an oscillating cilia carpet under an oscillatory magnetic field.

Here, we consider a cilia carpet, composed of equidistant magnetic Cosserat rods
clamped at one end, and initially inclined along the Z axis. The rods are
magnetised with a constant magnetisation density, but with spatially varying
magnetization angle, prescribed as a spatial sine wave (with wavelength and
phase difference prescribed by the user). We then place the rods under the
influence of a spatially invariant (ramped in time for stability) temporally
varying magnetic field, rotating in the XZ plane. The rods then bend as result
of the magnetic torques, trying to align with the time varying magnetic field,
but now with varying degrees owing to their different mangnetization angles.
resulting in a metachronal wave . To mimic the experimental
perturbations present in the original setup (Gu 2020, detailed citation below),
a small spatio-temporally invariant magnetic field is applied along the Y axis.
For additional details on the setup, refer to:
Gu, H., Boehler, Q., Cui, H., Secchi, E., Savorana, G., De Marco, C., ... &
Nelson, B. J. (2020). Magnetic cilia carpets with programmable metachronal
waves. Nature communications, 11(1), 1-10

This case serves as an illustration of extending the core elasticapp library
for custom environments (magnetic field) for systems with multiple
magnetic rods.

)help"};

  struct n_rods_along_x {
    // unsigned integer parameter
    using type = std::size_t;
    // documentation
    static constexpr String help = {"Number of rods along X"};
    static type default_value() { return 8UL; }
  };

  struct n_rods_along_y {
    // unsigned integer parameter
    using type = std::size_t;
    // documentation
    static constexpr String help = {"Number of rods along Y"};
    static type default_value() { return 8UL; }
  };

  struct n_elements {
    // unsigned integer parameter
    using type = std::size_t;
    // documentation
    static constexpr String help = {"Number of elements in the rod"};
    static type default_value() { return 100UL; }
  };

  struct radius {
    using type = ::elastica::real_t;
    static constexpr String help = {"Radius of the rod"};
    static type default_value() { return 0.15; }
  };

  struct length {
    using type = ::elastica::real_t;
    static constexpr String help = {"Length of the rod"};
    static type default_value() { return 6.0; }
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

  struct damping_rate {
    using type = ::elastica::real_t;
    static constexpr String help = {"Internal damping rate of the rod"};
    static type default_value() { return 1.0; }
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

  struct friction_coeff {
    using type = ::elastica::real_t;
    static constexpr String help = {"Friction coeff."};
    static type default_value() { return 0.2; }
  };

  struct magnetization_density {
    using type = ::elastica::real_t;
    static constexpr String help = {"Magnetization density of the rod"};
    static type default_value() { return 6.0; }
  };

  struct carpet_spacing {
    using type = ::elastica::real_t;
    static constexpr String help = {"Spacing between cilia in the carpet"};
    static type default_value() { return 6.0; }
  };

  struct carpet_magnetization_spatial_wavelength_along_x {
    using type = ::elastica::real_t;
    static constexpr String help = {
        "Wavelength of the spatial magnetization wave of the carpet along X"};
    static type default_value() { return (8 - 1) * 6.0; }
  };

  struct carpet_magnetization_spatial_wavelength_along_y {
    using type = ::elastica::real_t;
    static constexpr String help = {
        "Wavelength of the spatial magnetization wave of the carpet along Y"};
    static type default_value() { return (8 - 1) * 6.0; }
  };

  struct carpet_magnetization_spatial_phase_difference {
    using type = ::elastica::real_t;
    static constexpr String help = {
        "Phase difference of the spatial magnetization wave of the carpet"};
    static type default_value() { return 0.0; }
  };

  struct magnetic_field_in_plane_amplitude {
    using type = ::elastica::real_t;
    static constexpr String help = {
        "Amplitude of the in-plane external oscillating magnetic field"};
    static type default_value() { return 1.0; }
  };

  struct magnetic_field_out_of_plane_amplitude {
    using type = ::elastica::real_t;
    static constexpr String help = {
        "Amplitude of the out-of-plane invariant external magnetic field"};
    static type default_value() { return 0.01; }
  };

  struct magnetic_field_angular_frequency {
    using type = ::elastica::real_t;
    static constexpr String help = {
        "Angular frequency the external oscillating magnetic field"};
    static type default_value() { return 1.0; }
  };

  struct rampup_interval {
    using type = ::elastica::real_t;
    static constexpr String help = {
        "Time interval for which we rampup the mangetic field"};
    static type default_value() { return type(100.0); }
  };

  struct acc_gravity {
    using type = ::elastica::real_t;
    static constexpr String help = {"Gravity acc."};
    static type default_value() { return -9.8065 / 10.0; }
  };

  struct sphere_radius_to_rod_radius {
    using type = ::elastica::real_t;
    static constexpr String help = {"Sphere radius"};
    static type default_value() { return 2.0; }
  };

  struct sphere_density_to_rod_density {
    using type = ::elastica::real_t;
    static constexpr String help = {"Sphere density"};
    static type default_value() { return 1.0; }
  };

  struct top_layer_nondimensional_width {
    using type = ::elastica::real_t;
    static constexpr String help = {"Top layer"};
    static type default_value() { return 2.0; }
  };

  struct dt {
    using type = ::elastica::real_t;
    static constexpr String help = {"Time step of simulation"};
    static type default_value() {
      return type(0.01) *
             (length::default_value() / type(n_elements::default_value()));
    }
  };

  struct final_sim_time {
    using type = ::elastica::real_t;
    static constexpr String help = {"Final simulation time"};
    static type default_value() { return type(200.0); }
  };

  using Parameters =
      tmpl::list<n_rods_along_x, n_rods_along_y, n_elements, length, radius,
                 youngs_modulus, shear_modulus, damping_rate, contact_stiffness,
                 contact_damping_normal, contact_damping_tangential,
                 friction_coeff, magnetization_density, carpet_spacing,
                 carpet_magnetization_spatial_wavelength_along_x,
                 carpet_magnetization_spatial_wavelength_along_y,
                 carpet_magnetization_spatial_phase_difference,
                 magnetic_field_in_plane_amplitude,
                 magnetic_field_out_of_plane_amplitude,
                 magnetic_field_angular_frequency, rampup_interval, acc_gravity,
                 sphere_radius_to_rod_radius, sphere_density_to_rod_density,
                 top_layer_nondimensional_width,
                 dt, final_sim_time>;

  /* Required functions */
  void setup(InputTypes<This> const& input_parameters);
  void step();
  void run();
  ::elastica::real_t final_time;

  decltype(Configuration::make_time_stepper()) time_stepper =
      Configuration::make_time_stepper();
  decltype(Configuration::make_simulator()) simulator =
      Configuration::make_simulator();
  using RodRef = decltype(simulator->get_invalid_ref<MagnetizedCosseratRod>());
  using RodRefs = std::vector<RodRef>;

  using SphereRef = decltype(simulator->get_invalid_ref<Sphere>());
  using SphereRefs = std::vector<SphereRef>;

  RodRefs rod_refs;
  SphereRefs sphere_refs;

  inline auto communicate() {
    // return also the interpenetration here
    return std::make_tuple(communicate_buffer(rod_refs), communicate_buffer(sphere_refs));
  }

  OscillatoryMagneticTransporterCaseStudy() = default;
  OscillatoryMagneticTransporterCaseStudy(
      OscillatoryMagneticTransporterCaseStudy&&) noexcept = default;
};

ELASTICA_REGISTER_CASE_STUDY(OscillatoryMagneticTransporterCaseStudy);
