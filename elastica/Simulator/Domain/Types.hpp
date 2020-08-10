#pragma once

#include <type_traits>

#include "elastica/Utilities/Requires.hpp"

//#include "elastica/Simulator/Domain/BoundaryType.hpp"
#include "elastica/Utilities/DefineTypes.h"

namespace elastica {

  //////////////////////////////////////////////////////////////////////////////
  // Forward declarations of domain types
  //////////////////////////////////////////////////////////////////////////////

  // Boundary Types
  struct UnboundedBoundary;
  struct PeriodicBoundary;

  template <typename Real>
  struct Span;

  template <typename Real>
  class PeriodicDomainPolicy;
  template <typename Real>
  class UnboundedDomainPolicy;

  namespace detail {
    template <typename Real, template <typename> class DomainPolicy,
              Requires<std::is_floating_point<Real>::value> = nullptr>
    class Domain;
  }

  using PeriodicDomain = typename detail::Domain<real_t, PeriodicDomainPolicy>;
  using UnboundedDomain =
      typename detail::Domain<real_t, UnboundedDomainPolicy>;
}  // namespace elastica