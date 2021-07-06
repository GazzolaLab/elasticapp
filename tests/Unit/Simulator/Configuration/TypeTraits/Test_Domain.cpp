//
// Created by Tejaswin Parthasarathy on 6/17/21.
//
#include "Simulator/Configuration/ModuleConfiguration/Domain.hpp"
#include "Simulator/Configuration/TypeTraits/Domain.hpp"
#include "Simulator/Domain.hpp"
#include "Simulator/Frames.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace {

  /// [is_valid_domain_config_eg]
  using UnboundedDomainConfig =
      decltype(elastica::configuration::make_domain_configuration(
          elastica::boundary_along<elastica::EulerianFrame::X,
                                   elastica::IsUnbounded>(),
          elastica::boundary_along<elastica::EulerianFrame::Y,
                                   elastica::IsUnbounded>(),
          elastica::boundary_along<elastica::EulerianFrame::Z,
                                   elastica::IsUnbounded>()));
  // rigorously checks conformance
  static_assert(
      elastica::tt::IsValidDomainConfiguration<UnboundedDomainConfig>::value,
      "Domain meta-failure!");

  struct IGNORE_UNUSED NotADomainConfigButIndicatesConformance
      : public ::tt::ConformsTo<
            elastica::configuration::protocols::DomainConfiguration> {};

  // so the following call fails
  //  static_assert(elastica::tt::IsValidDomainConfiguration<
  //                   NotADomainConfigButIndicatesConformance>::value,
  //                "Domain meta-failure!");
  /// [is_valid_domain_config_eg]

}  // namespace
