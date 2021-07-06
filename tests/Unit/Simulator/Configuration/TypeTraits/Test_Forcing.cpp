//
// Created by Tejaswin Parthasarathy on 6/17/21.
//

#include "Simulator/Configuration/ModuleConfiguration/Forcing.hpp"
#include "Simulator/Configuration/TypeTraits/Forcing.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace {

  /// [is_valid_forcing_config_eg]
  using SequentialForcingConfig =
  decltype(elastica::configuration::make_forcing_configuration(
      elastica::configuration::seq));
  // rigorously checks conformance
  static_assert(elastica::tt::IsValidForcingConfiguration<
                    SequentialForcingConfig>::value,
                "Domain meta-failure!");

  struct IGNORE_UNUSED NotAForcingConfigButIndicatesConformance
      : public ::tt::ConformsTo<
          elastica::configuration::protocols::ForcingConfiguration> {};

  // so the following call fails
  //  static_assert(elastica::tt::IsValidForcingConfiguration<
  //                   NotAForcingConfigButIndicatesConformance>::value,
  //                "Domain meta-failure!");
  /// [is_valid_forcing_config_eg]

}  // namespace
