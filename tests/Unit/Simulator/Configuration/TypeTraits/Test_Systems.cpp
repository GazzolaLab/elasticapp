//
// Created by Tejaswin Parthasarathy on 6/17/21.
//
#include "Simulator/Configuration/ModuleConfiguration/BlockingPolicy.hpp"
#include "Simulator/Configuration/ModuleConfiguration/IterationPolicy.hpp"
#include "Simulator/Configuration/ModuleConfiguration/Systems.hpp"
#include "Simulator/Configuration/Protocols/Systems.hpp"
#include "Simulator/Configuration/TypeTraits/Systems.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace {

  /// [is_systems_config_eg]
  using ValidSystemsConfig =
      decltype(elastica::configuration::make_systems_configuration(
          elastica::configuration::make_blocking_policy(
              elastica::configuration::RestrictSizeAcrossBlockTypes{20UL}),
          elastica::configuration::make_iteration_policy(
              elastica::configuration::iterate_across_system_types_in(
                  elastica::configuration::seq),
              elastica::configuration::iterate_over_systems_in(
                  elastica::configuration::seq))));
  static_assert(elastica::tt::IsSystemsConfiguration<ValidSystemsConfig>::value,
                "Systems config meta-failure!");

  struct NotASystemsConfig {};
  static_assert(not elastica::tt::is_systems_configuration_v<NotASystemsConfig>,
                "Systems config meta-failure!");

  struct IGNORE_UNUSED NotASystemsConfigButIndicatesConformance
      : public ::tt::ConformsTo<
            elastica::configuration::protocols::SystemsConfiguration> {};
  static_assert(elastica::tt::IsSystemsConfiguration<
                    NotASystemsConfigButIndicatesConformance>::value,
                "Systems config meta-failure!");
  /// [is_systems_config_eg]

  /// [is_valid_systems_config_eg]
  static_assert(
      elastica::tt::IsValidSystemsConfiguration<ValidSystemsConfig>::value,
      "Systems config meta-failure!");

  struct IGNORE_UNUSED AnotherNotASystemsConfigButIndicatesConformance
      : public ::tt::ConformsTo<
            elastica::configuration::protocols::SystemsConfiguration> {};
  // so the following call fails
  //  static_assert(elastica::tt::IsValidSystemsConfiguration<
  //                   AnotherNotASystemsConfigButIndicatesConformance>::value,
  //                "Systems meta-failure!");

  /// [is_valid_systems_config_eg]

}  // namespace