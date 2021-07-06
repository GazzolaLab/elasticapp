//
// Created by Tejaswin Parthasarathy on 6/17/21.
//
#include <utility>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Configuration/Defaults.hpp"
#include "Simulator/Configuration/ModuleConfiguration.hpp"
#include "Simulator/Configuration/Protocols.hpp"
#include "Simulator/Configuration/SimulatorConfiguration.hpp"
#include "Simulator/Configuration/TypeTraits.hpp"
#include "Simulator/Frames.hpp"

namespace {

  ELASTICA_TEST_CASE("Unit.Simulator.Configuration.SimulatorConfiguration",
                     "[Unit][Simulator][Configuration]") {
    {
      /// [basic_eg]
      auto&& blocking_policy = elastica::configuration::make_blocking_policy(
          elastica::configuration::RestrictSizeAcrossBlockTypes{20UL});
      auto&& iteration_policy = elastica::configuration::make_iteration_policy(
          elastica::configuration::iterate_across_system_types_in(
              elastica::configuration::seq),
          elastica::configuration::iterate_over_systems_in(
              elastica::configuration::seq));
      auto&& unbounded_domain_configuration =
          elastica::configuration::make_domain_configuration(
              elastica::boundary_along<elastica::EulerianFrame::X,
                                       elastica::IsUnbounded>(),
              elastica::boundary_along<elastica::EulerianFrame::Y,
                                       elastica::IsUnbounded>(),
              elastica::boundary_along<elastica::EulerianFrame::Z,
                                       elastica::IsUnbounded>());

      auto&& basic_sim_config =
          elastica::configuration::make_simulator_configuration(
              // systems configuration
              elastica::configuration::make_systems_configuration(
                  std::move(blocking_policy), std::move(iteration_policy)),
              // domain configuration
              std::move(unbounded_domain_configuration));
      /// [basic_eg]

      static_assert(
          ::elastica::tt::IsValidDomainConfiguration<
              cpp20::remove_cvref_t<decltype(basic_sim_config)>>::value,
          "meta failure!");
      CHECK(basic_sim_config.domain().get_upper_bound(
                elastica::EulerianFrame::Y) ==
            (elastica::real_t(elastica::inf)));
      CHECK(basic_sim_config.domain().get_lower_bound(
                elastica::EulerianFrame::X) ==
            (elastica::real_t(-elastica::inf)));

      static_assert(
          ::elastica::tt::IsValidSystemsConfiguration<
              cpp20::remove_cvref_t<decltype(basic_sim_config)>>::value,
          "meta failure!");
      CHECK(basic_sim_config.blocking_policy().criterion_.value() == 20UL);
      (void)basic_sim_config.iteration_policy();
    }

    {
      /// [basic_shorthand_eg]
      using namespace elastica;
      using namespace elastica::configuration;
      auto&& basic_sim_config = make_simulator_configuration(
          // domain is unbounded in all directions
          make_domain_configuration(
              boundary_along<EulerianFrame::X, IsUnbounded>(),
              boundary_along<EulerianFrame::Y, IsUnbounded>(),
              boundary_along<EulerianFrame::Z, IsUnbounded>()),
          // iterate across system types in sequence, but parallelize operation
          // over systems
          make_systems_configuration(
              make_blocking_policy(RestrictSizeAcrossBlockTypes{20UL}),
              make_iteration_policy(iterate_across_system_types_in(par),
                                    iterate_over_systems_in(seq))));
      /// [basic_shorthand_eg]
      static_assert(
          ::elastica::tt::IsValidDomainConfiguration<
              cpp20::remove_cvref_t<decltype(basic_sim_config)>>::value,
          "meta failure!");
      CHECK(basic_sim_config.domain().get_upper_bound(
                elastica::EulerianFrame::Y) ==
            (elastica::real_t(elastica::inf)));
      CHECK(basic_sim_config.domain().get_lower_bound(
                elastica::EulerianFrame::X) ==
            (elastica::real_t(-elastica::inf)));

      static_assert(
          ::elastica::tt::IsValidSystemsConfiguration<
              cpp20::remove_cvref_t<decltype(basic_sim_config)>>::value,
          "meta failure!");
      CHECK(basic_sim_config.blocking_policy().criterion_.value() == 20UL);
      (void)basic_sim_config.iteration_policy();
    }

    {
      /// [with_connections_eg]
      using namespace elastica;
      using namespace elastica::configuration;
      auto&& with_connections_sim_config = make_simulator_configuration(
          // domain is unbounded still
          make_domain_configuration(
              boundary_along<EulerianFrame::X, IsUnbounded>(),
              boundary_along<EulerianFrame::Y, IsUnbounded>(),
              boundary_along<EulerianFrame::Z, IsUnbounded>()),
          // iterate across system types in parallel, but serialize operation
          // over systems
          make_systems_configuration(
              make_blocking_policy(RestrictSizeAcrossBlockTypes{20UL}),
              make_iteration_policy(iterate_across_system_types_in(seq),
                                    iterate_over_systems_in(par))),
          // process connections in a hybrid mode
          make_connections_configuration(hybrid, 50UL));
      /// [with_connections_eg]
      static_assert(
          ::elastica::tt::IsValidDomainConfiguration<cpp20::remove_cvref_t<
              decltype(with_connections_sim_config)>>::value,
          "meta failure!");
      CHECK(with_connections_sim_config.domain().get_upper_bound(
                elastica::EulerianFrame::Y) ==
            (elastica::real_t(elastica::inf)));
      CHECK(with_connections_sim_config.domain().get_lower_bound(
                elastica::EulerianFrame::X) ==
            (elastica::real_t(-elastica::inf)));

      static_assert(
          ::elastica::tt::IsValidSystemsConfiguration<cpp20::remove_cvref_t<
              decltype(with_connections_sim_config)>>::value,
          "meta failure!");
      CHECK(with_connections_sim_config.blocking_policy().criterion_.value() ==
            20UL);
      (void)with_connections_sim_config.iteration_policy();

      static_assert(
          ::elastica::tt::IsValidConnectionsConfiguration<cpp20::remove_cvref_t<
              decltype(with_connections_sim_config)>>::value,
          "meta failure!");
      CHECK(with_connections_sim_config.connections_parallelism_threshold() ==
            50UL);
    }

    {
      /// [all_eg]
      using namespace elastica;
      using namespace elastica::configuration;

      // Remember, order does not matter!
      auto&& sim_config = make_simulator_configuration(
          // domain is periodic in the Y direction now from [-20.0m, 20.0m]
          make_domain_configuration(
              boundary_along<EulerianFrame::X, IsUnbounded>(),
              boundary_along<EulerianFrame::Y, IsPeriodic>(-20.0, 20.0),
              boundary_along<EulerianFrame::Z, IsUnbounded>()),
          // iterate across system types in sequence, but parallelize operation
          // over systems
          make_systems_configuration(
              make_blocking_policy(RestrictSizeAcrossBlockTypes{20UL}),
              make_iteration_policy(iterate_across_system_types_in(par),
                                    iterate_over_systems_in(seq))),
          // process connections in hybrid mode
          make_connections_configuration(hybrid, 50UL),
          // process constraints in sequence
          make_constraints_configuration(seq),
          // process forcing in sequence
          make_forcing_configuration(seq),
          // process environments in parallel
          make_environments_configuration(par));
      /// [all_eg]

      static_assert(::elastica::tt::IsValidDomainConfiguration<
                        cpp20::remove_cvref_t<decltype(sim_config)>>::value,
                    "meta failure!");
      CHECK(sim_config.domain().get_upper_bound(elastica::EulerianFrame::Y) ==
            approx(20.0));
      CHECK(sim_config.domain().get_lower_bound(elastica::EulerianFrame::X) ==
            (elastica::real_t(-elastica::inf)));

      static_assert(::elastica::tt::IsValidSystemsConfiguration<
                        cpp20::remove_cvref_t<decltype(sim_config)>>::value,
                    "meta failure!");
      CHECK(sim_config.blocking_policy().criterion_.value() == 20UL);
      (void)sim_config.iteration_policy();

      static_assert(::elastica::tt::IsValidConnectionsConfiguration<
                        cpp20::remove_cvref_t<decltype(sim_config)>>::value,
                    "meta failure!");
      CHECK(sim_config.connections_parallelism_threshold() == 50UL);

      static_assert(::elastica::tt::IsValidConstraintsConfiguration<
                        cpp20::remove_cvref_t<decltype(sim_config)>>::value,
                    "meta failure!");

      static_assert(::elastica::tt::IsValidForcingConfiguration<
                        cpp20::remove_cvref_t<decltype(sim_config)>>::value,
                    "meta failure!");

      static_assert(::elastica::tt::IsValidEnvironmentsConfiguration<
                        cpp20::remove_cvref_t<decltype(sim_config)>>::value,
                    "meta failure!");
    }

    {
      /// [defaults_eg]
      using namespace elastica;
      using namespace elastica::configuration;

      // Remember, order does not matter!
      auto&& sim_config = make_simulator_configuration(
          // these are defaults because I don't really care
          // Notice that they are all default constructible
          DefaultSystemsConfiguration{}, DefaultDomainConfiguration{},
          DefaultConstraintsConfiguration{}, DefaultConnectionsConfiguration{},
          // process forcing in parallel, point of customization
          make_forcing_configuration(par));
      /// [defaults_eg]

      static_assert(::elastica::tt::IsValidConstraintsConfiguration<
                        cpp20::remove_cvref_t<decltype(sim_config)>>::value,
                    "meta failure!");

      static_assert(::elastica::tt::IsValidForcingConfiguration<
                        cpp20::remove_cvref_t<decltype(sim_config)>>::value,
                    "meta failure!");

      static_assert(::elastica::tt::IsValidConnectionsConfiguration<
                        cpp20::remove_cvref_t<decltype(sim_config)>>::value,
                    "meta failure!");

      static_assert(::elastica::tt::IsValidSystemsConfiguration<
                        cpp20::remove_cvref_t<decltype(sim_config)>>::value,
                    "meta failure!");

      static_assert(::elastica::tt::IsValidDomainConfiguration<
                        cpp20::remove_cvref_t<decltype(sim_config)>>::value,
                    "meta failure!");
    }
  }

}  // namespace