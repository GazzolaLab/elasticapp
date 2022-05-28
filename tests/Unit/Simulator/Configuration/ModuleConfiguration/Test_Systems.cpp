//
// Created by Tejaswin Parthasarathy on 6/17/21.
//

#include "Framework/TestingFramework.hpp"
#include "Simulator/Configuration/ModuleConfiguration/BlockingPolicy.hpp"
#include "Simulator/Configuration/ModuleConfiguration/IterationPolicy.hpp"
#include "Simulator/Configuration/ModuleConfiguration/Systems.hpp"
#include "Simulator/Configuration/Protocols/Systems.hpp"
#include "Simulator/Configuration/TypeTraits/Systems.hpp"
#include "Utilities/ProtocolHelpers.hpp"

/* We need to include the definition of a CosseratRod here for
 * defaults to work, since the protocol check requires the definition, as
 * opposed to just declaration of the CosseratRod kernel.
 */
#include "Systems/CosseratRods/CosseratRods.hpp"

namespace {

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Configuration.ModuleConfiguration.Systems.make_systems_configuration",
      "[Unit][Simulator][Configuration]") {
    // clang-format on
    using namespace elastica::configuration;

    /// [seq_eg]
    auto sequential_systems_configuration = make_systems_configuration(
        make_blocking_policy(RestrictSizeAcrossBlockTypes{20UL}),
        make_iteration_policy(iterate_across_system_types_in(seq),
                              iterate_over_systems_in(seq)));
    // can have custom tags for systems
    using CustomSystemTags = tmpl::list<>;
    auto sequential_systems_configuration_with_custom_tags =
        make_systems_configuration<CustomSystemTags>(
            make_blocking_policy(RestrictSizeAcrossBlockTypes{20UL}),
            make_iteration_policy(iterate_across_system_types_in(seq),
                                  iterate_over_systems_in(hybrid),
                                  AllSerialBelow{40UL}));
    /// [seq_eg]

    static_assert(
        cpp17::is_same_v<
            decltype(sequential_systems_configuration),
            SystemsConfiguration<
                ELASTICA_DEFAULT_ADMISSIBLE_PLUGINS_FOR_SYSTEMS,
                BlockingPolicy<RestrictSizeAcrossBlockTypes>,
                IterationPolicy<sequential_policy, sequential_policy, void>>>,
        "meta failure");
    static_assert(
        cpp17::is_same_v<
            decltype(sequential_systems_configuration_with_custom_tags),
            SystemsConfiguration<
                CustomSystemTags, BlockingPolicy<RestrictSizeAcrossBlockTypes>,
                IterationPolicy<sequential_policy, hybrid_policy,
                                AllSerialBelow>>>,
        "meta failure");

    static_assert(::elastica::tt::IsValidSystemsConfiguration<
                      decltype(sequential_systems_configuration)>::value,
                  "meta failure!");

    CHECK(
        sequential_systems_configuration.blocking_policy().criterion_.value() ==
        20UL);

    CHECK(sequential_systems_configuration_with_custom_tags.blocking_policy()
              .criterion_.value() == 20UL);
    CHECK(sequential_systems_configuration_with_custom_tags.iteration_policy()
              .value() == 40UL);
  }

}  // namespace
