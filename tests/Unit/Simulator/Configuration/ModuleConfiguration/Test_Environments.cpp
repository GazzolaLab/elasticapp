//
// Created by Tejaswin Parthasarathy on 6/17/21.
//

#include "Framework/TestingFramework.hpp"
#include "Simulator/Configuration/ModuleConfiguration/Environments.hpp"
#include "Simulator/Configuration/Protocols/Environments.hpp"
#include "Simulator/Configuration/TypeTraits/Environments.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace {

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Configuration.ModuleConfiguration.Environments.make_environments_configuration",
      "[Unit][Simulator][Configuration][Environments]") {
    // clang-format on
    using namespace elastica::configuration;

    /// [seq_eg]
    auto sequential_environments_config = make_environments_configuration(seq);
    // can have custom tags for environments
    using CustomEnvironmentTags = tmpl::list<>;
    auto sequential_environments_config_with_custom_environments =
        make_environments_configuration<CustomEnvironmentTags>(seq);
    // can also have custom imposed primitives for environments
    using CustomImposedPrimitives = tmpl::list<>;
    auto sequential_environments_config_with_custom_environments_and_imposed =
        make_environments_configuration<CustomEnvironmentTags,
                                        CustomImposedPrimitives>(seq);
    /// [seq_eg]

    static_assert(
        cpp17::is_same_v<decltype(sequential_environments_config),
                         EnvironmentsConfiguration<
                             ELASTICA_DEFAULT_ADMISSIBLE_ENVIRONMENT_TAGS,
                             ELASTICA_DEFAULT_ADMISSIBLE_IMPOSED_PRIMITIVES,
                             sequential_policy>>,
        "meta failure");
    static_assert(
        cpp17::is_same_v<
            decltype(sequential_environments_config_with_custom_environments),
            EnvironmentsConfiguration<
                CustomEnvironmentTags,
                ELASTICA_DEFAULT_ADMISSIBLE_IMPOSED_PRIMITIVES,
                sequential_policy>>,
        "meta failure");
    static_assert(
        cpp17::is_same_v<
            decltype(sequential_environments_config_with_custom_environments_and_imposed),
            EnvironmentsConfiguration<CustomEnvironmentTags,
                                      CustomImposedPrimitives,
                                      sequential_policy>>,
        "meta failure");

    static_assert(::elastica::tt::IsValidEnvironmentsConfiguration<
                      decltype(sequential_environments_config)>::value,
                  "meta failure!");

    elastica::ignore_unused(
        sequential_environments_config,
        sequential_environments_config_with_custom_environments,
        sequential_environments_config_with_custom_environments_and_imposed);

    /// [par_eg]
    auto parallel_environments_config = make_environments_configuration(par);
    // can have custom tags for environments
    using CustomEnvironmentTags = tmpl::list<>;
    auto parallel_environments_config_with_custom_environments =
        make_environments_configuration<CustomEnvironmentTags>(par);
    // can also have custom imposed primitives for environments
    using CustomImposedPrimitives = tmpl::list<>;
    auto parallel_environments_config_with_custom_environments_and_imposed =
        make_environments_configuration<CustomEnvironmentTags,
                                        CustomImposedPrimitives>(par);
    /// [par_eg]

    static_assert(
        cpp17::is_same_v<decltype(parallel_environments_config),
                         EnvironmentsConfiguration<
                             ELASTICA_DEFAULT_ADMISSIBLE_ENVIRONMENT_TAGS,
                             ELASTICA_DEFAULT_ADMISSIBLE_IMPOSED_PRIMITIVES,
                             parallel_policy>>,
        "meta failure");
    static_assert(
        cpp17::is_same_v<
            decltype(parallel_environments_config_with_custom_environments),
            EnvironmentsConfiguration<
                CustomEnvironmentTags,
                ELASTICA_DEFAULT_ADMISSIBLE_IMPOSED_PRIMITIVES,
                parallel_policy>>,
        "meta failure");
    static_assert(
        cpp17::is_same_v<
            decltype(parallel_environments_config_with_custom_environments_and_imposed),
            EnvironmentsConfiguration<CustomEnvironmentTags,
                                      CustomImposedPrimitives,
                                      parallel_policy>>,
        "meta failure");

    static_assert(::elastica::tt::IsValidEnvironmentsConfiguration<
                      decltype(parallel_environments_config)>::value,
                  "meta failure!");

    auto parallel_environments_config_with_gravity =
        make_environments_configuration(
            par, make_gravity_configuration(1.0, 2.0, 3.0));
    CHECK(parallel_environments_config_with_gravity.gravity() ==
          ::elastica::Vec3{1.0, 2.0, 3.0});

    elastica::ignore_unused(
        parallel_environments_config,
        parallel_environments_config_with_custom_environments,
        parallel_environments_config_with_custom_environments_and_imposed);
  }

}  // namespace
