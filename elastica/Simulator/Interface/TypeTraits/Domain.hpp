#pragma once

#include <type_traits>

#include "elastica/Simulator/Domain/Policies/Directions.hpp"
#include "elastica/Simulator/Domain/Protocols.hpp"
#include "elastica/Utilities/ProtocolHelpers.hpp"
#include "elastica/Utilities/TMPL.hpp"

namespace elastica {

  namespace interface {

    namespace tt {

      // Can be made to assert, but for infinite domains we really don't need
      // the interface, as long as it just conforms
      template <typename C, typename = void>
      struct IsDomainConfiguration : std::false_type {};

      // Check for
      // 1. Existence of members
      template <typename C>
      struct IsDomainConfiguration<
          C,
          cpp17::void_t<
              typename C::DomainType,
              typename C::template BoundaryTypeAlong<detail::Directions::X>,
              typename C::template BoundaryTypeAlong<detail::Directions::Y>,
              typename C::template BoundaryTypeAlong<detail::Directions::Z> > >
          : ::tt::conforms_to<typename C::DomainType,
                              ::elastica::protocols::ComputationalDomain> {};

      // Or we can explicitly constraint to Periodic and UnboundedDomains at
      // this point too, seems restrictive

    }  // namespace tt

  }  // namespace interface

}  // namespace elastica
