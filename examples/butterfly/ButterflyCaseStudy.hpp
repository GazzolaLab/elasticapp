//==============================================================================
/*!
//  \file
//  \brief Butterfly case study
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#ifndef ELASTICA_BUTTERFLYCASESTUDY_HPP
#define ELASTICA_BUTTERFLYCASESTUDY_HPP

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

struct ButterflyConfiguration {
  static inline auto make_simulator_configuration() {
    auto system_config =
        ec::make_systems_configuration<tmpl::list<CosseratRodWithoutDamping>>(
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
    return elastica::make_simulator<>(make_simulator_configuration());
  }

  static inline auto make_time_stepper() {
    using TimeStepper = ::elastica::timestepper::PositionVerlet;
    return TimeStepper{};
  }
};

struct ButterflyCaseStudy
    : public ::elastica::CaseStudyInterface<ButterflyCaseStudy> {
  using Configuration = ButterflyConfiguration;
  using This = ButterflyCaseStudy;

  static constexpr String help = {R"help(
Butterfly test-case
-------------------
We have a rod (with no internal damping_ that is initially bent into a "\/" shape
and we let it free to deform. Now because the relaxed configuration of the rod
is a "--" shape i.e. (horizontal line), the rod will try and approach it. But
because there is no internal damping, the total energy of the rod is conserved.
This means that the initial elastic (potential) energy stored in the rod as a result of its
deformation is converted more and more to kinetic energy as the rod approaches
its relaxed configuration. Hence as it reaches the "--" shape, the rod has
significant velocity, which makes it overshoot this "--" shape. The rod
overshoots till it reaches a "/\" shape--its exact initial configuration, but
mirrored about the horizontal axis. We can check the validity of our solver by
tracking the configuration of the rod as it deforms, along with the stored
elastic (bending) and kinetic energies.

This test case checks that the bending equations in our Cosserat rod are
implemented correctly.
)help"};

  struct n_elements {
    using type = std::size_t;
    static constexpr String help = {"Number of elements in the rod"};
    static type default_value() { return 2UL; }
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

  using Parameters = tmpl::list<n_elements, angle_of_inclination, length, dt>;

  /* Required functions */
  void setup(InputTypes<This> const& input_parameters);
  void step();
  void run();

  auto compute_elastic_energy() const noexcept {
    return std::make_tuple(::elastica::compute_bend_energy(rod_ref.get()),
                           ::elastica::compute_shear_energy(rod_ref.get()));
  }

  auto compute_kinetic_energy() const noexcept {
    return std::make_tuple(
        ::elastica::compute_translational_energy(rod_ref.get()),
        ::elastica::compute_rotational_energy(rod_ref.get()));
  }

  inline auto communicate() {
    return std::tuple_cat(std::make_tuple(::elastica::position(rod_ref)),
                          compute_elastic_energy(), compute_kinetic_energy());
  }

  ButterflyCaseStudy() = default;
  ButterflyCaseStudy(ButterflyCaseStudy&&) noexcept = default;

  decltype(Configuration::make_time_stepper()) time_stepper =
      Configuration::make_time_stepper();
  decltype(Configuration::make_simulator()) simulator =
      Configuration::make_simulator();
  using RodRef =
      decltype(simulator->get_invalid_ref<CosseratRodWithoutDamping>());
  RodRef rod_ref = simulator->get_invalid_ref<CosseratRodWithoutDamping>();
};

ELASTICA_REGISTER_CASE_STUDY(ButterflyCaseStudy);

#endif  // ELASTICA_BUTTERFLYCASESTUDY_HPP
