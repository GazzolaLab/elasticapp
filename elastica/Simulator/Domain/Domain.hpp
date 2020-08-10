#pragma once

#include <type_traits>

#include "elastica/Simulator/Domain/Policies/PeriodicDomainPolicy.hpp"
#include "elastica/Simulator/Domain/Policies/UnboundedDomainPolicy.hpp"
#include "elastica/Simulator/Domain/Protocols.hpp"
#include "elastica/Simulator/Domain/Types.hpp"
#include "elastica/Utilities/ProtocolHelpers.hpp"
#include "elastica/Utilities/Requires.hpp"

namespace elastica {

  namespace detail {

    // Default argument specfied in forward declaration
    template <typename Real, template <typename> class DomainPolicy,
              Requires<std::is_floating_point<Real>::value>>
    class Domain : public ::tt::ConformsTo<
                       ::elastica::protocols::ComputationalDomain>,
                   public DomainPolicy<Real> {
      using bound_type = Real;
    };

  }  // namespace detail

  //  class PeriodicDomain : public NonCopyable,
  //  tt::ConformsTo<protocols::Domain> {
  //    PeriodicDomain() = default;
  //    ~PeriodicDomain() = default;
  //    using DomainStorageType = ;
  //  };

}  // namespace elastica