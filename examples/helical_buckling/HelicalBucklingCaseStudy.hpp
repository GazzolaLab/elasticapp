//==============================================================================
/*!
//  \file
//  \brief HelicalBuckling case study
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#ifndef ELASTICA_HELICALBUCKLINGCASESTUDY_HPP
#define ELASTICA_HELICALBUCKLINGCASESTUDY_HPP

//
#include <elastica/CaseStudy.hpp>
//
#include <elastica/Simulator/Constraints.hpp>
#include <elastica/Simulator/Simulator.hpp>
#include <elastica/Systems/CosseratRods.hpp>
#include <elastica/Utilities/DefineTypes.h>
#include <elastica/Utilities/TMPL.hpp>
#include <tuple>

// global is fine since its a cpp file
using String = const char* const;
namespace ec = elastica::configuration;
using CosseratRod = ::elastica::cosserat_rod::CosseratRod;

struct HelicalBucklingConfiguration {
  static inline auto make_simulator_configuration() {
    auto system_config =
        ec::make_systems_configuration<tmpl::list<CosseratRod>>(
            ec::make_blocking_policy(ec::RestrictSizeAcrossBlockTypes{50UL}),
            ec::make_iteration_policy(
                ec::iterate_across_system_types_in(ec::seq),
                ec::iterate_over_systems_in(ec::seq)));

    return ec::make_simulator_configuration(
        ec::DefaultDomainConfiguration{}, system_config,
        ec::DefaultParallelConfiguration{},
        ec::DefaultConstraintsConfiguration{});
  }

  static inline auto make_simulator() {
    return elastica::make_simulator<::elastica::modules::Constraints>(
        make_simulator_configuration());
  }

  static inline auto make_time_stepper() {
    using TimeStepper = ::elastica::timestepper::PositionVerlet;
    return TimeStepper{};
  }
};

struct HelicalBucklingCaseStudy
    : public ::elastica::CaseStudyInterface<HelicalBucklingCaseStudy> {
  using Configuration = HelicalBucklingConfiguration;
  using This = HelicalBucklingCaseStudy;

  static constexpr String help = {R"help(
HelicalBuckling case-study
--------------------------
Helical buckling validation case, for detailed explanation refer to
Gazzola et. al. R. Soc. 2018  section 3.4.1
)help"};

  struct n_elements {
    using type = std::size_t;
    static constexpr String help = {"Number of elements in the rod"};
  };

  // struct length {
  //   using type = ::elastica::real_t;
  //   static constexpr String help = {"Length of the rod"};
  // };

  //  struct radius {
  //    using type = ::elastica::real_t;
  //    static constexpr String help = {"Radius of the rod"};
  //  };

  struct damping {
    using type = ::elastica::real_t;
    static constexpr String help = {"Internal damping in the rod"};
  };

  struct dt {
    using type = ::elastica::real_t;
    static constexpr String help = {"Time step of simulation"};
  };

  using Parameters = tmpl::list<n_elements, damping, dt>;

  /* Required functions */
  void setup(InputTypes<This> const& input_parameters);
  void step();
  void run();

  inline auto communicate() {
    return std::make_tuple(
        ::elastica::position(*rod_it), ::elastica::director(*rod_it),
        ::elastica::velocity(*rod_it), ::elastica::angular_velocity(*rod_it));
  }

  HelicalBucklingCaseStudy() = default;
  HelicalBucklingCaseStudy(HelicalBucklingCaseStudy&&) noexcept = default;

  decltype(Configuration::make_time_stepper()) time_stepper =
      Configuration::make_time_stepper();
  decltype(Configuration::make_simulator()) simulator =
      Configuration::make_simulator();
  using It = decltype(simulator->get_invalid_iterator<CosseratRod>());
  It rod_it = simulator->get_invalid_iterator<CosseratRod>();
};

ELASTICA_REGISTER_CASE_STUDY(HelicalBucklingCaseStudy);

#endif  // ELASTICA_HELICALBUCKLINGCASESTUDY_HPP