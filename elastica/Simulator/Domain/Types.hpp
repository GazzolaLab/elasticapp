#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <type_traits>

#include "Utilities/DefineTypes.h"
#include "Utilities/Requires.hpp"

namespace elastica {

  //////////////////////////////////////////////////////////////////////////////
  //
  // Forward declarations of domain types
  //
  //////////////////////////////////////////////////////////////////////////////

  // Boundary Types
  struct UnboundedBoundary;
  struct PeriodicBoundary;

  // Span
  template <typename Real>
  struct Span;

  // Policy types
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