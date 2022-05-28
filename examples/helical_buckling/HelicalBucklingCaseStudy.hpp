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
  static inline auto make_simulator() {
    return elastica::make_simulator<::elastica::modules::Constraints>();
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
    static type default_value() { return 100UL; }
  };

  // struct length {
  //   using type = ::elastica::real_t;
  //   static constexpr String help = {"Length of the rod"};
  // };

  //  struct radius {
  //    using type = ::elastica::real_t;
  //    static constexpr String help = {"Radius of the rod"};
  //  };

  struct damping_rate {
    using type = ::elastica::real_t;
    static constexpr String help = {"Rate of damping in the rod"};
    static type default_value() { return 0.01; }
  };

  struct dt {
    using type = ::elastica::real_t;
    static constexpr String help = {"Time step of simulation"};
    static type default_value() { return 0.001; }
  };

  using Parameters = tmpl::list<n_elements, damping_rate, dt>;

  /* Required functions */
  void setup(InputTypes<This> const& input_parameters);
  void step();
  void run();

  inline auto communicate() {
    auto rod_slice = rod_ref.get();
    return std::make_tuple(::elastica::position(rod_slice),
                           ::elastica::director(rod_slice),
                           ::elastica::velocity(rod_slice),
                           ::elastica::angular_velocity(rod_slice));
  }

  HelicalBucklingCaseStudy() = default;
  HelicalBucklingCaseStudy(HelicalBucklingCaseStudy&&) noexcept = default;

  decltype(Configuration::make_time_stepper()) time_stepper =
      Configuration::make_time_stepper();
  decltype(Configuration::make_simulator()) simulator =
      Configuration::make_simulator();
  using RodRef = decltype(simulator->get_invalid_ref<CosseratRod>());
  RodRef rod_ref = simulator->get_invalid_ref<CosseratRod>();
};

ELASTICA_REGISTER_CASE_STUDY(HelicalBucklingCaseStudy);

#endif  // ELASTICA_HELICALBUCKLINGCASESTUDY_HPP