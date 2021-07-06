//
// Created by Tejaswin Parthasarathy on 6/17/21.
//
#include "Simulator/Configuration/ModuleConfiguration/Connections.hpp"
#include "Simulator/Configuration/TypeTraits/Connections.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace {

  /// [is_valid_connections_config_eg]
  using SequentialConnectionsConfig =
      decltype(elastica::configuration::make_connections_configuration(
          elastica::configuration::seq));
  // rigorously checks conformance
  static_assert(elastica::tt::IsValidConnectionsConfiguration<
                    SequentialConnectionsConfig>::value,
                "Domain meta-failure!");

  struct IGNORE_UNUSED NotAConnectionsConfigButIndicatesConformance
      : public ::tt::ConformsTo<
            elastica::configuration::protocols::ConnectionsConfiguration> {};

  // so the following call fails
  //  static_assert(elastica::tt::IsValidConnectionsConfiguration<
  //                   NotAConnectionsConfigButIndicatesConformance>::value,
  //                "Domain meta-failure!");
  /// [is_valid_connections_config_eg]

}  // namespace