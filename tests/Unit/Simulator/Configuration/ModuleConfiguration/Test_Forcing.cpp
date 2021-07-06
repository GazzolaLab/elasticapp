//
// Created by Tejaswin Parthasarathy on 6/16/21.
//
#include "Framework/TestingFramework.hpp"
#include "Simulator/Configuration/ModuleConfiguration/Forcing.hpp"
#include "Simulator/Configuration/Protocols/Forcing.hpp"
#include "Simulator/Configuration/TypeTraits/Forcing.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace {

  // clang-format off
  ELASTICA_TEST_CASE(
  "Unit.Simulator.Configuration.ModuleConfiguration.Forcing.make_forcing_configuration",
  "[Unit][Simulator][Configuration]") {
    // clang-format on
    using namespace elastica::configuration;

    /// [seq_eg]
    auto sequential_forcing = make_forcing_configuration(seq);
    // optional parameter ignored
    auto sequential_forcing_optional = make_forcing_configuration(seq, 5UL);
    // can have custom registras
    using CustomRegistrars = tmpl::list<>;
    auto sequential_forcing_with_custom_registrars =
        make_forcing_configuration<CustomRegistrars>(seq);
    /// [seq_eg]

    static_assert(::elastica::tt::IsValidForcingConfiguration<
                      decltype(sequential_forcing)>::value,
                  "meta failure!");
    static_assert(
        cpp17::is_same_v<
            decltype(sequential_forcing),
            ForcingConfiguration<sequential_policy,
                                 ELASTICA_DEFAULT_REGISTRARS_FOR_FORCING>>,
        "meta failure");
    static_assert(
        cpp17::is_same_v<
            decltype(sequential_forcing),
            ForcingConfiguration<sequential_policy, CustomRegistrars>>,
        "meta failure");
    elastica::ignore_unused(sequential_forcing, sequential_forcing_optional,
                            sequential_forcing_with_custom_registrars);

    /// [par_eg]
    auto parallel_forcing = make_forcing_configuration(par);
    // optional parameter ignored
    auto parallel_forcing_optional = make_forcing_configuration(par, 5UL);
    // can have custom registrars
    auto parallel_forcing_with_custom_registrars =
        make_forcing_configuration<CustomRegistrars>(par);
    /// [par_eg]

    static_assert(::elastica::tt::IsValidForcingConfiguration<
                      decltype(parallel_forcing)>::value,
                  "meta failure!");
    static_assert(
        cpp17::is_same_v<
            decltype(parallel_forcing),
            ForcingConfiguration<parallel_policy,
                                 ELASTICA_DEFAULT_REGISTRARS_FOR_FORCING>>,
        "meta failure");
    static_assert(cpp17::is_same_v<
                      decltype(parallel_forcing),
                      ForcingConfiguration<parallel_policy, CustomRegistrars>>,
                  "meta failure");
    elastica::ignore_unused(parallel_forcing, parallel_forcing_optional,
                            parallel_forcing_with_custom_registrars);

    /// [hybrid_eg]
    auto hybrid_forcing = make_forcing_configuration(hybrid, 5UL);
    // can have custom registrars as well
    auto hybrid_forcing_with_custom_registrars =
        make_forcing_configuration<CustomRegistrars>(hybrid, 100UL);
    /// [hybrid_eg]

    static_assert(::elastica::tt::IsValidForcingConfiguration<
                      decltype(hybrid_forcing)>::value,
                  "meta failure!");

    static_assert(
        cpp17::is_same_v<
            decltype(hybrid_forcing),
            ForcingConfiguration<hybrid_policy,
                                 ELASTICA_DEFAULT_REGISTRARS_FOR_FORCING>>,
        "meta failure");
    static_assert(
        cpp17::is_same_v<decltype(hybrid_forcing),
                         ForcingConfiguration<hybrid_policy, CustomRegistrars>>,
        "meta failure");

    CHECK(hybrid_forcing.forcing_parallelism_threshold() == 5UL);
    CHECK(
        hybrid_forcing_with_custom_registrars.forcing_parallelism_threshold() ==
        100UL);
  }

}  // namespace
