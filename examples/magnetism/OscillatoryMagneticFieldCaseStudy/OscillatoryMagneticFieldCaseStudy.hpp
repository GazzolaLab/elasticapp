//==============================================================================
/*!
//  \file
//  \brief Oscillatory magnetic field case study
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
#include <elastica/Simulator/EnableReset.hpp>  // for convergence studies
#include <elastica/Simulator/Simulator.hpp>
#include <elastica/Utilities/DefineTypes.h>
#include <elastica/Utilities/Geometry/Angles.hpp>
#include <elastica/Utilities/TMPL.hpp>
//
#include "Extension/Simulator/Environments.hpp"
#include "Extension/Systems/CosseratRods.hpp"
#include "FieldProfiles/ConstantMagneticField.hpp"  // customization
#include "FieldProfiles/RampedAdapter.hpp"          // customization

// global is fine since its a cpp file
using String = const char* const;
namespace ec = elastica::configuration;
using MagnetizedCosseratRod = ::elastica::cosserat_rod::MagnetizedCosseratRod;

struct OscillatoryMagneticFieldConfiguration {
  static inline auto make_simulator_configuration() {
    auto system_config =
        ec::make_systems_configuration<tmpl::list<MagnetizedCosseratRod>>(
            ec::make_blocking_policy(ec::RestrictSizeAcrossBlockTypes{50UL}),
            ec::make_iteration_policy(
                ec::iterate_across_system_types_in(ec::seq),
                ec::iterate_over_systems_in(ec::seq)));

    using EnvironmentTags =
        tmpl::list<::elastica::environments::tags::MagneticFieldTag>;
    auto env_config = ec::make_environments_configuration<EnvironmentTags>(
        // process environments in sequence
        ec::seq,
        // you can pass in a lambda function here as well, but I explicitly
        // make a struct so that the example is clear
        ec::make_magnetic_field_configuration(
            // I initialize some parameter values here, but these can be
            // modified later on
            ::elastica::make_ramped_field(
                ::elastica::environments::ConstantMagneticField{
                    ::elastica::Vec3{0.0, 0.8, 2.0}},
                ::elastica::make_linear_rampup(0.0, 200.0))));

    return ec::make_simulator_configuration(
        ec::DefaultDomainConfiguration{}, system_config,
        ec::DefaultParallelConfiguration{},
        ec::DefaultConstraintsConfiguration{}, env_config);
  }

  static inline auto make_simulator() {
    return elastica::make_simulator<::elastica::modules::Constraints,
                                    ::elastica::modules::Environments,
                                    ::elastica::modules::EnableReset>(
        make_simulator_configuration());
  }

  static inline auto make_time_stepper() {
    using TimeStepper = ::elastica::timestepper::PositionVerlet;
    return TimeStepper{};
  }
};

struct OscillatoryMagneticFieldCaseStudy
    : public ::elastica::CaseStudyInterface<OscillatoryMagneticFieldCaseStudy> {
  using Configuration = OscillatoryMagneticFieldConfiguration;
  using This = OscillatoryMagneticFieldCaseStudy;

  static constexpr String help = {R"help(
Oscillatory Magnetic field study
-----------------------------

Demonstrates rod deformations under a oscillatory magnetic field.

// TODO : Arman fill

)help"};

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

  struct magnetization_density {
    using type = ::elastica::real_t;
    static constexpr String help = {"Magnetization density of the rod"};
    static type default_value() { return 6.0; }
  };

  struct magnetic_field_angle {
    using type = ::elastica::real_t;
    static constexpr String help = {
        "In-plane angle of the external magnetic field"};
    static type default_value() { return elastica::geometry::deg2rad(120.0); }
  };

  struct magnetic_field_amplitude {
    using type = ::elastica::real_t;
    static constexpr String help = {"Amplitude of the external magnetic field"};
    static type default_value() { return 1e-2; }
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

  using Parameters = tmpl::list<n_elements, length, radius,
                                magnetization_density, magnetic_field_amplitude,
                                magnetic_field_angle, rampup_interval, dt>;

  /* Required functions */
  void setup(InputTypes<This> const& input_parameters);
  void step();
  void run();

  decltype(Configuration::make_time_stepper()) time_stepper =
      Configuration::make_time_stepper();
  decltype(Configuration::make_simulator()) simulator =
      Configuration::make_simulator();
  using RodRef = decltype(simulator->get_invalid_ref<MagnetizedCosseratRod>());
  using RodRefs = std::vector<RodRef>;
  RodRefs rod_refs{1UL, simulator->get_invalid_ref<MagnetizedCosseratRod>()};

  inline auto communicate_from(RodRef& ref) {
    auto rod = ref.get();
    return std::make_tuple(::elastica::position(rod),
                           ::blocks::get<::elastica::tags::Tangent>(rod),
                           ::elastica::velocity(rod));
  }
  inline auto communicate() {
    // we dont have CTAD :(
    using CT = decltype(communicate_from(std::declval<RodRef&>()));
    using RT = std::vector<CT>;
    RT return_list;
    return_list.reserve(1UL);
    std::transform(std::begin(rod_refs), std::end(rod_refs),
                   std::back_inserter(return_list), [this](RodRef& rod_ref) {
                     return this->communicate_from(rod_ref);
                   });
    // return also the interpenetration here
    return return_list;
  }

  OscillatoryMagneticFieldCaseStudy() = default;
  OscillatoryMagneticFieldCaseStudy(
      OscillatoryMagneticFieldCaseStudy&&) noexcept = default;
};

ELASTICA_REGISTER_CASE_STUDY(OscillatoryMagneticFieldCaseStudy);
