//
// Created by Tejaswin Parthasarathy on 6/16/21.
//

#include "Framework/TestingFramework.hpp"
#include "Simulator/Configuration/ModuleConfiguration/Connections.hpp"
#include "Simulator/Configuration/Protocols/Connections.hpp"
#include "Simulator/Configuration/TypeTraits/Connections.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace {

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Configuration.ModuleConfiguration.Connections.make_connections_configuration",
      "[Unit][Simulator][Configuration]") {
    // clang-format on
    using namespace elastica::configuration;

    /// [seq_eg]
    auto sequential_connections = make_connections_configuration(seq);
    // optional parameter ignored
    auto sequential_connections_optional =
        make_connections_configuration(seq, 5UL);
    // can have custom registras
    using CustomRegistrars = tmpl::list<>;
    auto sequential_connections_with_custom_registrars =
        make_connections_configuration<CustomRegistrars>(seq);
    /// [seq_eg]

    static_assert(::elastica::tt::IsValidConnectionsConfiguration<
                      decltype(sequential_connections)>::value,
                  "meta failure!");
    static_assert(
        cpp17::is_same_v<decltype(sequential_connections),
                         ConnectionsConfiguration<
                             sequential_policy,
                             ELASTICA_DEFAULT_REGISTRARS_FOR_CONNECTIONS>>,
        "meta failure");
    static_assert(
        cpp17::is_same_v<
            decltype(sequential_connections),
            ConnectionsConfiguration<sequential_policy, CustomRegistrars>>,
        "meta failure");
    elastica::ignore_unused(sequential_connections,
                            sequential_connections_optional,
                            sequential_connections_with_custom_registrars);

    /// [par_eg]
    auto parallel_connections = make_connections_configuration(par);
    // optional parameter ignored
    auto parallel_connections_optional =
        make_connections_configuration(par, 5UL);
    // can have custom registrars
    auto parallel_connections_with_custom_registrars =
        make_connections_configuration<CustomRegistrars>(par);
    /// [par_eg]

    static_assert(::elastica::tt::IsValidConnectionsConfiguration<
                      decltype(parallel_connections)>::value,
                  "meta failure!");
    static_assert(
        cpp17::is_same_v<
            decltype(parallel_connections),
            ConnectionsConfiguration<
                parallel_policy, ELASTICA_DEFAULT_REGISTRARS_FOR_CONNECTIONS>>,
        "meta failure");
    static_assert(
        cpp17::is_same_v<
            decltype(parallel_connections),
            ConnectionsConfiguration<parallel_policy, CustomRegistrars>>,
        "meta failure");
    elastica::ignore_unused(parallel_connections, parallel_connections_optional,
                            parallel_connections_with_custom_registrars);

    /// [hybrid_eg]
    auto hybrid_connections = make_connections_configuration(hybrid, 5UL);
    // can have custom registrars as well
    auto hybrid_connections_with_custom_registrars =
        make_connections_configuration<CustomRegistrars>(hybrid, 100UL);
    /// [hybrid_eg]

    static_assert(::elastica::tt::IsValidConnectionsConfiguration<
                      decltype(hybrid_connections)>::value,
                  "meta failure!");
    static_assert(
        cpp17::is_same_v<
            decltype(hybrid_connections),
            ConnectionsConfiguration<
                hybrid_policy, ELASTICA_DEFAULT_REGISTRARS_FOR_CONNECTIONS>>,
        "meta failure");
    static_assert(
        cpp17::is_same_v<
            decltype(hybrid_connections),
            ConnectionsConfiguration<hybrid_policy, CustomRegistrars>>,
        "meta failure");

    CHECK(hybrid_connections.connections_parallelism_threshold() == 5UL);
    CHECK(hybrid_connections_with_custom_registrars
              .connections_parallelism_threshold() == 100UL);
  }

}  // namespace
