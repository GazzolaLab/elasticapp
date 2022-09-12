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

#include <vector>
//
#include <elastica/CaseStudy.hpp>
//
#include <elastica/Simulator/Constraints.hpp>
#include <elastica/Simulator/Simulator.hpp>
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
using MagnetizedCosseratRod = ::elastica::cosserat_rod::MagnetizedCosseratRodWithoutDamping;
using CosseratRod = ::elastica::cosserat_rod::CosseratRodWithoutDamping;
using MagneticRodInitializer = ::elastica::cosserat_rod::StraightUniformMagnetizedCosseratRodInitializer;
using RodInitializer = ::elastica::cosserat_rod::StraightCosseratRodWithoutDampingInitializer;
using Vec3 = ::elastica::Vec3;
using Rot3 = ::elastica::Rot3;
using real_t = ::elastica::real_t;
using Traits = ::elastica::cosserat_rod::DefaultCosseratRodTraits;

namespace ec = elastica::configuration;
struct MagneticMillipedesConfiguration {
  static inline auto make_simulator_configuration() {
    auto system_config =
        ec::make_systems_configuration<tmpl::list<CosseratRod, MagnetizedCosseratRod>>(
            ec::make_blocking_policy(ec::RestrictSizeAcrossBlockTypes{256UL}),
            ec::make_iteration_policy(
                ec::iterate_across_system_types_in(ec::seq),
                ec::iterate_over_systems_in(ec::seq)));

    using EnvironmentTags =
        tmpl::list<::elastica::environments::tags::MagneticFieldTag>;
    auto env_config = ec::make_environments_configuration<EnvironmentTags>(
        // process environments in sequence
        ec::seq,
        ec::make_magnetic_field_configuration(::elastica::make_ramped_field(
            ::elastica::environments::OscillatoryMagneticField(),
            // I initialize some parameter values here, but these can
            // be modified later on
            ::elastica::make_linear_rampup(0.0, 200.0))));

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

struct MagneticMillipedesCaseStudy
    : public ::elastica::CaseStudyInterface<MagneticMillipedesCaseStudy> {
  using Configuration = MagneticMillipedesConfiguration;
  using This = MagneticMillipedesCaseStudy;

  static constexpr String help = {R"help(
Magnetic millipedes case study
------------------------------
Demonstrates an magnetic millipedes simulation.
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
    static type default_value() { return 10UL; }
  };

  struct radius {
    using type = ::elastica::real_t;
    static constexpr String help = {"Radius of the rod"};
    static type default_value() { return 0.15; }
  };

  struct damping_constant{
    using type = ::elastica::real_t;
    static constexpr String help = {"Analytical Damping Constant"};
    static type default_value() { return 1.5; }
  };

  struct length {
    using type = ::elastica::real_t;
    static constexpr String help = {"Length of the rod"};
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

  using Parameters = tmpl::list<
      n_rods_along_x, n_rods_along_y, n_elements, length, radius,
      damping_constant, carpet_spacing,
      carpet_magnetization_spatial_wavelength_along_x,
      carpet_magnetization_spatial_wavelength_along_y,
      carpet_magnetization_spatial_phase_difference,
      magnetic_field_in_plane_amplitude, magnetic_field_out_of_plane_amplitude,
      magnetic_field_angular_frequency, rampup_interval, dt, final_sim_time>;

  /* Required functions */
  void setup(InputTypes<This> const& input_parameters);
  void step();
  void run();

  /* Features */
  void add_analytical_damper(InputTypes<This> const&, const real_t&);
  ::elastica::real_t final_time;

  decltype(Configuration::make_time_stepper()) time_stepper =
      Configuration::make_time_stepper();
  decltype(Configuration::make_simulator()) simulator =
      Configuration::make_simulator();
  using CRRef = decltype(simulator->get_invalid_ref<CosseratRod>());
  using MCRRef = decltype(simulator->get_invalid_ref<MagnetizedCosseratRod>());
  std::vector<CRRef> backbone_rod_list, backbone_rod_second_layer_list;
  std::vector<MCRRef> magnetic_rod_list;

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
//                   std::back_inserter(return_list), [this](CRRef& rod_ref) {
//                     return this->communicate_from(rod_ref);
//                   });
//    // return also the interpenetration here
//    return return_list;
  }

  MagneticMillipedesCaseStudy() = default;
  MagneticMillipedesCaseStudy(MagneticMillipedesCaseStudy&&) noexcept = default;
};

ELASTICA_REGISTER_CASE_STUDY(MagneticMillipedesCaseStudy);
