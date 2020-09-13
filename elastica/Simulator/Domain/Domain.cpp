//******************************************************************************
// Includes
//******************************************************************************

#include "Simulator/Domain/Domain.hpp"

namespace elastica {

  // Explicitly instantiate only the classes we want to be defined.
  template class detail::Domain<real_t, PeriodicDomainPolicy>;
  template class detail::Domain<real_t, UnboundedDomainPolicy>;

}  // namespace elastica