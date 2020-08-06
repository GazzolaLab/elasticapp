#pragma once

#include <type_traits>

#include "elastica/Simulator/Domain/Policies/PeriodicDomainPolicy.hpp"
#include "elastica/Simulator/Domain/Policies/UnboundedDomainPolicy.hpp"
#include "elastica/Simulator/Domain/Protocols.hpp"
#include "elastica/Utilities/DefineTypes.h"
#include "elastica/Utilities/ProtocolHelpers.hpp"
#include "elastica/Utilities/Requires.hpp"

namespace elastica {

  namespace detail {

    template <typename Real, template <typename> class DomainPolicy,
              Requires<std::is_floating_point<Real>::value> = nullptr>
    class Domain : public DomainPolicy<Real>,
                   tt::ConformsTo<protocols::ComputationalDomain> {
      using bound_type = Real;
    };

  }  // namespace detail

  using PeriodicDomain = typename detail::Domain<real_t, PeriodicDomainPolicy>;
  using UnboundedDomain =
      typename detail::Domain<real_t, UnboundedDomainPolicy>;

  //  class PeriodicDomain : public NonCopyable,
  //  tt::ConformsTo<protocols::Domain> {
  //    PeriodicDomain() = default;
  //    ~PeriodicDomain() = default;
  //    using DomainStorageType = ;
  //  };

}  // namespace elastica