//
// Created by Tejaswin Parthasarathy on 6/17/21.
//

#include "Simulator/Configuration/ModuleConfiguration/Environments.hpp"
#include "Simulator/Configuration/TypeTraits/Environments.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace {

  /// [is_valid_environments_config_eg]
  using SequentialEnvironmentsConfig =
      decltype(elastica::configuration::make_environments_configuration(
          elastica::configuration::seq));
  // rigorously checks conformance
  static_assert(elastica::tt::IsValidEnvironmentsConfiguration<
                    SequentialEnvironmentsConfig>::value,
                "Domain meta-failure!");

  struct IGNORE_UNUSED NotAEnvironmentsConfigButIndicatesConformance
      : public ::tt::ConformsTo<
            elastica::configuration::protocols::EnvironmentsConfiguration> {};

  // so the following call fails
  //  static_assert(elastica::tt::IsValidEnvironmentsConfiguration<
  //                   NotAEnvironmentsConfigButIndicatesConformance>::value,
  //                "Domain meta-failure!");
  /// [is_valid_environments_config_eg]

}  // namespace
