//
// Created by Tejaswin Parthasarathy on 6/17/21.
//

#include "Simulator/Configuration/ModuleConfiguration/Constraints.hpp"
#include "Simulator/Configuration/TypeTraits/Constraints.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace {

  /// [is_valid_constraints_config_eg]
  using SequentialConstraintsConfig =
  decltype(elastica::configuration::make_constraints_configuration(
      elastica::configuration::seq));
  // rigorously checks conformance
  static_assert(elastica::tt::IsValidConstraintsConfiguration<
                    SequentialConstraintsConfig>::value,
                "Domain meta-failure!");

  struct IGNORE_UNUSED NotAConstraintsConfigButIndicatesConformance
      : public ::tt::ConformsTo<
          elastica::configuration::protocols::ConstraintsConfiguration> {};

  // so the following call fails
  //  static_assert(elastica::tt::IsValidConstraintsConfiguration<
  //                   NotAConstraintsConfigButIndicatesConformance>::value,
  //                "Domain meta-failure!");
  /// [is_valid_constraints_config_eg]

}  // namespace
