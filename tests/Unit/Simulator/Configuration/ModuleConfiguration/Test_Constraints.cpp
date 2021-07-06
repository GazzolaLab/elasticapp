//
// Created by Tejaswin Parthasarathy on 6/16/21.
//

#include "Framework/TestingFramework.hpp"
#include "Simulator/Configuration/ModuleConfiguration/Constraints.hpp"
#include "Simulator/Configuration/Protocols/Constraints.hpp"
#include "Simulator/Configuration/TypeTraits/Constraints.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace {

  // clang-format off
  ELASTICA_TEST_CASE(
  "Unit.Simulator.Configuration.ModuleConfiguration.Constraints.make_constraints_configuration",
  "[Unit][Simulator][Configuration]") {
    // clang-format on
    using namespace elastica::configuration;

    /// [seq_eg]
    auto sequential_constraints = make_constraints_configuration(seq);
    // optional parameter ignored
    auto sequential_constraints_optional =
        make_constraints_configuration(seq, 5UL);
    // can have custom registras
    using CustomRegistrars = tmpl::list<>;
    auto sequential_constraints_with_custom_registrars =
        make_constraints_configuration<CustomRegistrars>(seq);
    /// [seq_eg]

    static_assert(::elastica::tt::IsValidConstraintsConfiguration<
                      decltype(sequential_constraints)>::value,
                  "meta failure!");
    static_assert(
        cpp17::is_same_v<decltype(sequential_constraints),
                         ConstraintsConfiguration<
                             sequential_policy,
                             ELASTICA_DEFAULT_REGISTRARS_FOR_CONSTRAINTS>>,
        "meta failure");
    static_assert(
        cpp17::is_same_v<
            decltype(sequential_constraints),
            ConstraintsConfiguration<sequential_policy, CustomRegistrars>>,
        "meta failure");
    elastica::ignore_unused(sequential_constraints,
                            sequential_constraints_optional,
                            sequential_constraints_with_custom_registrars);

    /// [par_eg]
    auto parallel_constraints = make_constraints_configuration(par);
    // optional parameter ignored
    auto parallel_constraints_optional =
        make_constraints_configuration(par, 5UL);
    // can have custom registrars
    auto parallel_constraints_with_custom_registrars =
        make_constraints_configuration<CustomRegistrars>(par);
    /// [par_eg]

    static_assert(::elastica::tt::IsValidConstraintsConfiguration<
                      decltype(parallel_constraints)>::value,
                  "meta failure!");
    static_assert(
        cpp17::is_same_v<
            decltype(parallel_constraints),
            ConstraintsConfiguration<
                parallel_policy, ELASTICA_DEFAULT_REGISTRARS_FOR_CONSTRAINTS>>,
        "meta failure");
    static_assert(
        cpp17::is_same_v<
            decltype(parallel_constraints),
            ConstraintsConfiguration<parallel_policy, CustomRegistrars>>,
        "meta failure");
    elastica::ignore_unused(parallel_constraints, parallel_constraints_optional,
                            parallel_constraints_with_custom_registrars);

    /// [hybrid_eg]
    auto hybrid_constraints = make_constraints_configuration(hybrid, 5UL);
    // can have custom registrars as well
    auto hybrid_constraints_with_custom_registrars =
        make_constraints_configuration<CustomRegistrars>(hybrid, 100UL);
    /// [hybrid_eg]

    static_assert(::elastica::tt::IsValidConstraintsConfiguration<
                      decltype(hybrid_constraints)>::value,
                  "meta failure!");
    static_assert(
        cpp17::is_same_v<
            decltype(hybrid_constraints),
            ConstraintsConfiguration<
                hybrid_policy, ELASTICA_DEFAULT_REGISTRARS_FOR_CONSTRAINTS>>,
        "meta failure");
    static_assert(
        cpp17::is_same_v<
            decltype(hybrid_constraints),
            ConstraintsConfiguration<hybrid_policy, CustomRegistrars>>,
        "meta failure");

    CHECK(hybrid_constraints.constraints_parallelism_threshold() == 5UL);
    CHECK(hybrid_constraints_with_custom_registrars
              .constraints_parallelism_threshold() == 100UL);
  }

}  // namespace
