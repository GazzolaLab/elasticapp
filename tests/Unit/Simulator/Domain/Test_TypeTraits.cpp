//
// Created by Tejaswin Parthasarathy on 6/17/21.
//
// needs definition for asserting conformation
#include "Simulator/Domain/Domain.hpp"
///
#include "Simulator/Domain/BoundaryTypes.hpp"
///
#include "Simulator/Domain/TypeTraits.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"

namespace {

  /// [is_domain_example]
  using elastica::PeriodicDomain;
  static_assert(elastica::tt::IsDomain<PeriodicDomain>::value,
                "Domain meta-failure!");

  struct NotADomain {};
  static_assert(not elastica::tt::is_domain_v<NotADomain>,
                "Domain meta-failure!");

  struct NotADomainButIndicatesConformance
      : public ::tt::ConformsTo<elastica::protocols::ComputationalDomain> {};
  static_assert(
      elastica::tt::IsDomain<NotADomainButIndicatesConformance>::value,
      "Domain meta-failure!");
  /// [is_domain_example]

  /// [is_valid_domain_example]
  using elastica::PeriodicDomain;
  // rigorously checks conformance
  static_assert(elastica::tt::IsValidDomain<PeriodicDomain>::value,
                "Domain meta-failure!");

  struct IGNORE_UNUSED AnotherNotADomainButIndicatesConformance
      : public ::tt::ConformsTo<elastica::protocols::ComputationalDomain> {};

  // so the following call fails
  //  static_assert(elastica::tt::IsValidDomain<
  //                    AnotherNotADomainButIndicatesConformance>::value,
  //                "Domain meta-failure!");
  /// [is_valid_domain_example]

  /// [SimulationDomainType_eg]
  using P = elastica::IsPeriodic;
  using U = elastica::IsUnbounded;

  using PPP = elastica::tt::SimulationDomainType<P, P, P>;
  static_assert(std::is_same_v<PPP, elastica::PeriodicDomain>,
                "Domain meta failure");

  using PUP = elastica::tt::SimulationDomainType<P, U, P>;
  static_assert(std::is_same_v<PUP, elastica::PeriodicDomain>,
                "Domain meta failure");

  using UUU = elastica::tt::SimulationDomainType<U, U, U>;
  static_assert(std::is_same_v<UUU, elastica::UnboundedDomain>,
                "Domain meta failure");
  /// [SimulationDomainType_eg]

}  // namespace