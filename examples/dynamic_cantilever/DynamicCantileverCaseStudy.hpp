//==============================================================================
/*!
//  \file
//  \brief Dynamic cantilever case study
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#ifndef ELASTICA_DYNAMICCANTILEVERCASESTUDY_HPP
#define ELASTICA_DYNAMICCANTILEVERCASESTUDY_HPP

//
#include <elastica/CaseStudy.hpp>
//
#include <elastica/Simulator/Constraints.hpp>
#include <elastica/Simulator/EnableReset.hpp>
#include <elastica/Simulator/Simulator.hpp>
#include <elastica/Systems/CosseratRods.hpp>
#include <elastica/Time/Time.hpp>
#include <elastica/Utilities/DefineTypes.h>
#include <elastica/Utilities/TMPL.hpp>
//
// global is fine since its a cpp file
using String = const char* const;
namespace ec = elastica::configuration;
using ::elastica::cosserat_rod::CosseratRodWithoutDamping;

struct DynamicCantileverConfiguration {
  static inline auto make_simulator_configuration() {
    auto system_config =
        ec::make_systems_configuration<tmpl::list<CosseratRodWithoutDamping>>(
            /*
             * We use the ec::make_blocking_policy to idiomatically construct
             * blocking policies to determine how memory will be arranged. For
             * more details see the tutorial_blocking
             */
            ec::make_blocking_policy(ec::RestrictSizeAcrossBlockTypes{50UL}),
            /*
             * We use the ec::make_iteration_policy to idiomatically construct
             * iteration policies to determine how parallel kernels will operate
             * over the memory above. For more details see the tutorial_blocking
             */
            ec::make_iteration_policy(
                ec::iterate_across_system_types_in(ec::seq),
                ec::iterate_over_systems_in(ec::seq)));

    // We don't need forces or constraints, so we dont provide those
    // configurations
    return ec::make_simulator_configuration(
        ec::DefaultDomainConfiguration{}, system_config,
        ec::DefaultParallelConfiguration{},
        ec::DefaultConstraintsConfiguration{});
  }

  static inline auto make_simulator() {
    return elastica::make_simulator<::elastica::modules::Constraints,
                                    ::elastica::modules::EnableReset>(
        make_simulator_configuration());
  }

  static inline auto make_time_stepper() {
    using TimeStepper = ::elastica::timestepper::PositionVerlet;
    return TimeStepper{};
  }
};

struct DynamicCantileverCaseStudy
    : public ::elastica::CaseStudyInterface<DynamicCantileverCaseStudy> {
  using Configuration = DynamicCantileverConfiguration;
  using This = DynamicCantileverCaseStudy;

  static constexpr String help = {R"help(
DynamicCantilever test-case
---------------------------

The example simulates the response of cantilever CosseratRod,
initialized by modal oscillations at natural frequencies, to
Euler-Bernoulli beam theory.

Here, we consider a beam clamped at one end and subject to an initial oscillation
at natural frequency. These oscillations setup a harmonic response in the beam.
The dynamic solution for the displacement along the vertical
direction of the rod can then be analytically expressed by the Euler-Bernoulli
theory. Comparison between simulations and theory can then be made.
Here, the effects of shear are prominent, and is be controlled by the shear
elasticity modulus of the material.
)help"};

  struct n_elements {
    using type = std::size_t;
    static constexpr String help = {"Number of elements in the rod"};
    static type default_value() { return 100UL; }
  };

  struct youngs_modulus {
    using type = ::elastica::real_t;
    static constexpr String help = {"Youngs modulus of the rod"};
    static type default_value() { return 1e5; }
  };

  struct shear_modulus {
    using type = ::elastica::real_t;
    static constexpr String help = {"Shear modulus of the rod"};
    static type default_value() {
      // poisson ratio 0.5
      return youngs_modulus::default_value() / 3.0;
    }
  };

  struct density {
    using type = ::elastica::real_t;
    static constexpr String help = {"Density of the rod"};
    static type default_value() { return 2000.0; }
  };

  struct radius {
    using type = ::elastica::real_t;
    static constexpr String help = {"Radius of the rod"};
    static type default_value() { return 0.02; }
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
      return type(0.05) *
             (length::default_value() / n_elements::default_value());
    }
  };

  using Parameters = tmpl::list<n_elements, youngs_modulus, shear_modulus,
                                density, radius, length, dt>;

  /* Required functions */
  void setup(InputTypes<This> const& input_parameters);
  void step();
  void run();
  inline auto communicate() {
    // TODO : inefficiency : slices at runtime, slice can be stored instead
    // slices at runtime, slice can be eliminated
    return rod_ref.get();
  }

  DynamicCantileverCaseStudy() = default;
  DynamicCantileverCaseStudy(DynamicCantileverCaseStudy&&) noexcept = default;

  decltype(Configuration::make_time_stepper()) time_stepper =
      Configuration::make_time_stepper();
  decltype(Configuration::make_simulator()) simulator =
      Configuration::make_simulator();
  using RodRef =
      decltype(simulator->get_invalid_ref<CosseratRodWithoutDamping>());
  RodRef rod_ref = simulator->get_invalid_ref<CosseratRodWithoutDamping>();
};

ELASTICA_REGISTER_CASE_STUDY(DynamicCantileverCaseStudy);

#endif  // ELASTICA_DYNAMICCANTILEVERCASESTUDY_HPP
