//==============================================================================
/*!
//  \file
//  \brief ParallelUpdateDynamics case study
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#ifndef ELASTICA_PARALLEL_BUTTERFLYCASESTUDY_HPP
#define ELASTICA_PARALLEL_BUTTERFLYCASESTUDY_HPP

//
#include <elastica/CaseStudy.hpp>
//
#include <elastica/Simulator/Simulator.hpp>
#include <elastica/Systems/CosseratRods.hpp>
#include <elastica/Utilities/DefineTypes.h>
#include <elastica/Utilities/Geometry/Angles.hpp>
#include <elastica/Utilities/TMPL.hpp>
#include <tuple>

// global is fine since its a cpp file
using String = const char* const;
namespace ec = elastica::configuration;
using CosseratRodWithoutDamping =
    ::elastica::cosserat_rod::CosseratRodWithoutDamping;

struct ParallelUpdateDynamicsConfiguration {
  static inline auto make_simulator_configuration() {
    auto system_config =
        ec::make_systems_configuration<tmpl::list<CosseratRodWithoutDamping>>(
            ec::make_blocking_policy(ec::RestrictSizeAcrossBlockTypes{128UL}),
            ec::make_iteration_policy(
                ec::iterate_across_system_types_in(ec::seq),
                ec::iterate_over_systems_in(ec::par)));
    auto parallel_config = ec::make_parallel_configuration(256);

    return ec::make_simulator_configuration(ec::DefaultDomainConfiguration{},
                                            system_config, parallel_config);
  }

  static inline auto make_simulator() {
    return elastica::make_simulator<>(make_simulator_configuration());
  }

  static inline auto make_time_stepper() {
    using TimeStepper = ::elastica::timestepper::PositionVerlet;
    return TimeStepper{};
  }
};

struct ParallelUpdateDynamicsCaseStudy
    : public ::elastica::CaseStudyInterface<ParallelUpdateDynamicsCaseStudy> {
  using Configuration = ParallelUpdateDynamicsConfiguration;
  using This = ParallelUpdateDynamicsCaseStudy;

  static constexpr String help = {R"help(
ParallelUpdateDynamics test-case
-------------------
)help"};

  struct n_elements {
    using type = std::size_t;
    static constexpr String help = {"Number of elements in the rod"};
    static type default_value() { return 20UL; }
  };

  struct angle_of_inclination {
    using type = ::elastica::real_t;
    static constexpr String help = {
        "Angle which the (half)-rod makes with the horizontal axis"};
    static type default_value() { return ::elastica::geometry::deg2rad(45.0); }
  };

  struct length {
    using type = ::elastica::real_t;
    static constexpr String help = {"Length of the rod"};
    static type default_value() { return 3.0; }
  };

  struct dt {
    using type = ::elastica::real_t;
    static constexpr String help = {"Time step of simulation"};
    static type default_value() {
      return type(0.01) *
             (length::default_value() / n_elements::default_value());
    }
  };

  struct n_rods {
    using type = std::size_t;
    static constexpr String help = {"Number of rods"};
    static type default_value() { return 50UL; }
  };

  struct n_threads {
    using type = std::size_t;
    static constexpr String help = {"Number of threads"};
    static type default_value() { return 2UL; }
  };

  // Keep it caps for consistency
  struct BlockingPolicy {
    using element_type = ::elastica::configuration::detail::BlockingPolicyBase<
        ::elastica::configuration::detail::BlockingPolicyRegistrars>;
    using type = std::unique_ptr<element_type>;
    static constexpr Options::String help = {"The type of BlockingPolicy"};
  };

  using Parameters = tmpl::list<n_rods, n_elements, angle_of_inclination,
                                length, dt, n_threads, BlockingPolicy>;

  /* Required functions */
  void setup(InputTypes<This> const& input_parameters);
  void step();
  void run();

  auto compute_elastic_energy() const noexcept {
    auto slice = rod_ref.get();
    return std::make_tuple(::elastica::compute_bend_energy(slice),
                           ::elastica::compute_shear_energy(slice));
  }

  auto compute_kinetic_energy() const noexcept {
    auto slice = rod_ref.get();
    return std::make_tuple(::elastica::compute_translational_energy(slice),
                           ::elastica::compute_rotational_energy(slice));
  }

  inline auto communicate() {
    return std::tuple_cat(std::make_tuple(::elastica::position(rod_ref.get())),
                          compute_elastic_energy(), compute_kinetic_energy());
  }

  ParallelUpdateDynamicsCaseStudy() = default;
  ParallelUpdateDynamicsCaseStudy(ParallelUpdateDynamicsCaseStudy const&) =
      default;

  decltype(Configuration::make_time_stepper()) time_stepper =
      Configuration::make_time_stepper();
  decltype(Configuration::make_simulator()) simulator =
      Configuration::make_simulator();
  using RodRef =
      decltype(simulator->get_invalid_ref<CosseratRodWithoutDamping>());
  RodRef rod_ref = simulator->get_invalid_ref<CosseratRodWithoutDamping>();
};

ELASTICA_REGISTER_CASE_STUDY(ParallelUpdateDynamicsCaseStudy);

#endif  // ELASTICA_PARALLEL_BUTTERFLYCASESTUDY_HPP
