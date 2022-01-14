//==============================================================================
/*!
//  \file
//  \brief Timoshenko case study
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#ifndef ELASTICA_TIMOSHENKOCASESTUDY_HPP
#define ELASTICA_TIMOSHENKOCASESTUDY_HPP

//
#include <elastica/CaseStudy.hpp>
//
#include <elastica/Simulator/Constraints.hpp>
#include <elastica/Simulator/Forcing.hpp>
#include <elastica/Simulator/Simulator.hpp>
#include <elastica/Systems/CosseratRods.hpp>
#include <elastica/Time/Time.hpp>
#include <elastica/Utilities/DefineTypes.h>
#include <elastica/Utilities/TMPL.hpp>
#include <tuple>

// global is fine since its a cpp file
using String = const char* const;
namespace ec = elastica::configuration;
using CosseratRod = ::elastica::cosserat_rod::CosseratRod;

struct TimoshenkoConfiguration {
  static inline auto make_simulator_configuration() {
    auto system_config =
        ec::make_systems_configuration<tmpl::list<CosseratRod>>(
            ec::make_blocking_policy(ec::RestrictSizeAcrossBlockTypes{50UL}),
            ec::make_iteration_policy(
                ec::iterate_across_system_types_in(ec::seq),
                ec::iterate_over_systems_in(ec::seq)));

    return ec::make_simulator_configuration(
        ec::DefaultDomainConfiguration{}, system_config,
        ec::DefaultParallelConfiguration{}, ec::DefaultForcingConfiguration{},
        ec::DefaultConstraintsConfiguration{});
  }

  static inline auto make_simulator() {
    return elastica::make_simulator<::elastica::modules::Forcing,
                                    ::elastica::modules::Constraints>(
        make_simulator_configuration());
  }

  static inline auto make_time_stepper() {
    using TimeStepper = ::elastica::timestepper::PositionVerlet;
    return TimeStepper{};
  }
};

struct TimoshenkoCaseStudy
    : public ::elastica::CaseStudyInterface<TimoshenkoCaseStudy> {
  using Configuration = TimoshenkoConfiguration;
  using This = TimoshenkoCaseStudy;

  static constexpr String help = {R"help(
Timoshenko beam test-case
-------------------------

Here, we consider a beam clamped at one end and subject to a constant downward
force F at the other free end. Because of this force, the beam deforms and
bends downwards. The static solution for the displacement  along the vertical
direction of the rod can then be analytically expressed by the Timoshenko
theory, valid on the assumption of small deflections. Here, the effects of shear
are prominent, and is be controlled by the shear elasticity modulus of the
material.

For a more detailed explanation, refer to Gazzola et. al. R. Soc. 2018.
Section 3.4.3.
)help"};

  struct n_elements {
    using type = std::size_t;
    static constexpr String help = {"Number of elements in the rod"};
  };

  struct damping {
    using type = ::elastica::real_t;
    static constexpr String help = {"Internal damping in the rod"};
  };

  struct youngs_modulus {
    using type = ::elastica::real_t;
    static constexpr String help = {"Youngs modulus of the rod"};
  };

  struct shear_modulus {
    using type = ::elastica::real_t;
    static constexpr String help = {"Shear modulus of the rod"};
  };

  struct applied_force {
    using type = ::elastica::real_t;
    static constexpr String help = {"Force applied at the rod tip"};
  };

  struct radius {
    using type = ::elastica::real_t;
    static constexpr String help = {"Radius of the rod"};
  };

  struct length {
    using type = ::elastica::real_t;
    static constexpr String help = {"Length of the rod"};
  };

  struct dt {
    using type = ::elastica::real_t;
    static constexpr String help = {"Time step of simulation"};
  };

  using Parameters =
      tmpl::list<n_elements, damping, youngs_modulus, shear_modulus,
                 applied_force, radius, length, dt>;

  /* Required functions */
  void setup(InputTypes<This> const& input_parameters);
  void step();
  void run();
  inline auto communicate() { return ::elastica::position(*rod_it); }

  TimoshenkoCaseStudy() = default;
  TimoshenkoCaseStudy(TimoshenkoCaseStudy&&) noexcept = default;

  decltype(Configuration::make_time_stepper()) time_stepper =
      Configuration::make_time_stepper();
  decltype(Configuration::make_simulator()) simulator =
      Configuration::make_simulator();
  using It = decltype(simulator->get_invalid_iterator<CosseratRod>());
  It rod_it = simulator->get_invalid_iterator<CosseratRod>();
};

ELASTICA_REGISTER_CASE_STUDY(TimoshenkoCaseStudy);

#endif  // ELASTICA_TIMOSHENKOCASESTUDY_HPP
