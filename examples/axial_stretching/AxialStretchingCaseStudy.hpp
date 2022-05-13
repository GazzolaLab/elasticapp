//==============================================================================
/*!
//  \file
//  \brief Axial stretching case study
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
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
#include <elastica/Simulator/Constraints.hpp>
#include <elastica/Simulator/Forcing.hpp>
#include <elastica/Simulator/Simulator.hpp>
#include <elastica/Systems/CosseratRods.hpp>
#include <elastica/Utilities/DefineTypes.h>
#include <elastica/Utilities/TMPL.hpp>

// global is fine since its a cpp file
using String = const char* const;
namespace ec = elastica::configuration;
using CosseratRod = ::elastica::cosserat_rod::CosseratRod;

struct AxialStretchingConfiguration {
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

struct AxialStretchingCaseStudy
    : public ::elastica::CaseStudyInterface<AxialStretchingCaseStudy> {
  using Configuration = AxialStretchingConfiguration;
  using This = AxialStretchingCaseStudy;

  static constexpr String help = {R"help(
Axial stretching test-case
--------------------------

Assume we have a rod lying aligned in the x-direction, with high internal
damping.

We fix the left end of the rod to a wall. On the right end we apply a force
directed axially pulling the rods tip. Linear theory (assuming small
displacements) analytically predict that the net displacement experienced by the
rod tip as Δx = FL/AE where the symbols carry their usual meaning (the rod is
just a linear spring). We compare our numerical results with the above
analytical result.

We can "improve" the theory by having a better estimate for the rod's
spring constant by assuming that it equilibriates under the new position,
with
Δx = F * (L + Δx)/ (A * E)
which results in Δx = (F*l)/(A*E - F). In practice, we have seen that our
numerical algorithm matches this "improved" estimate in practice.

Note that if the damping is not high, the rod oscillates about its eventual
resting position (which agrees with the theoretical predictions without
any damping), so the results may have mismatch with Δx. Filtering such
oscillations enables comparison with theory.
)help"};

  struct n_elements {
    // unsigned integer parameter
    using type = std::size_t;
    // documentation
    static constexpr String help = {"Number of elements in the rod"};
    static type default_value() { return 49UL; }
  };

  struct youngs_modulus {
    using type = ::elastica::real_t;
    static constexpr String help = {"Youngs modulus of the rod"};
    static type default_value() { return 1e4; }
  };

  struct applied_force {
    using type = ::elastica::real_t;
    static constexpr String help = {"Force applied at the rod tip"};
    static type default_value() { return 1.0; }
  };

  struct radius {
    using type = ::elastica::real_t;
    static constexpr String help = {"Radius of the rod"};
    static type default_value() { return 0.025; }
  };

  struct damping_rate {
    using type = ::elastica::real_t;
    static constexpr String help = {"Rate of damping in the rod"};
    static type default_value() {
      /* We set this value higher than usual because it accelerates convergence
       * to the quasi-steady state, where forces balance out each other. We are
       * only interested in this state, to enable comparison with theory. It is
       * worth noting that in this state, rates (v and omega) are zero : hence
       * the damping contribution to force is zero---so this coefficient does
       * not really matter.
       */
      return 10.0;
    }
  };

  struct length {
    using type = ::elastica::real_t;
    static constexpr String help = {"Length of the rod"};
    static type default_value() { return 1.0; }
  };

  struct dt {
    using type = ::elastica::real_t;
    static constexpr String help = {"Time step of simulation"};
    static type default_value() {
      return type(0.01) *
             (length::default_value() / n_elements::default_value());
    }
  };

  using Parameters = tmpl::list<n_elements, damping_rate, youngs_modulus,
                                applied_force, radius, length, dt>;

  /* Required functions */
  void setup(InputTypes<This> const& input_parameters);
  void step();
  void run();
  inline auto communicate() {
    // TODO : inefficiency : slices at runtime, slice can be stored instead
    return ::elastica::position(rod_ref.get());
  }

  AxialStretchingCaseStudy() = default;
  AxialStretchingCaseStudy(AxialStretchingCaseStudy&&) noexcept = default;

  decltype(Configuration::make_time_stepper()) time_stepper =
      Configuration::make_time_stepper();
  decltype(Configuration::make_simulator()) simulator =
      Configuration::make_simulator();
  using RodRef = decltype(simulator->get_invalid_ref<CosseratRod>());
  RodRef rod_ref = simulator->get_invalid_ref<CosseratRod>();
};

ELASTICA_REGISTER_CASE_STUDY(AxialStretchingCaseStudy);
