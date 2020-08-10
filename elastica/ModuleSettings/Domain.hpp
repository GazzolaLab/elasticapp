#pragma once

#include <type_traits>

#include "elastica/Simulator/Domain/Types.hpp"
// HEALTH WARNING : We need a complete implementatino of Domain here to let
// the compiler know that it conforms to the protocol (the types header only
// forward declares it without conformance)
#include "elastica/Simulator/Domain/Domain.hpp"
// This is an implementation detail and hence isnt exposed in Types
#include "elastica/Simulator/Domain/Policies/Directions.hpp"
// Complete implementation of this type is needed to do manipulations
#include "elastica/Simulator/Domain/BoundaryTypes.hpp"
#include "elastica/Utilities/TMPL.hpp"

// The configuration file
#include "elastica/Configuration/Domain.hpp"

namespace elastica {

  namespace defaults {

    namespace detail {

      template <std::size_t>
      struct BoundaryTypeAlongHelper;

      // Explicitly instantiate
      template <>
      struct BoundaryTypeAlongHelper<::elastica::detail::Directions::X> {
        using type = ELASTICA_DEFAULT_BOUNDARY_TYPE_ALONG_X;
      };

      template <>
      struct BoundaryTypeAlongHelper<::elastica::detail::Directions::Y> {
        using type = ELASTICA_DEFAULT_BOUNDARY_TYPE_ALONG_Y;
      };

      template <>
      struct BoundaryTypeAlongHelper<::elastica::detail::Directions::Z> {
        using type = ELASTICA_DEFAULT_BOUNDARY_TYPE_ALONG_Z;
      };

    }  // namespace detail

    struct DomainConfiguration {
      template <std::size_t N>
      using BoundaryTypeAlong =
          typename detail::BoundaryTypeAlongHelper<N>::type;

      // if at least one (boundary_types) == periodic, then
      // periodic domain
      // periodic is true_type and unbounded is false_type
      using DomainType = std::conditional_t<
          tmpl::or_<
              BoundaryTypeAlong<::elastica::detail::Directions::X>,
              BoundaryTypeAlong<::elastica::detail::Directions::Y>,
              BoundaryTypeAlong<::elastica::detail::Directions::Z>>::value,
          PeriodicDomain, UnboundedDomain>;
    };

  }  // namespace defaults

}  // namespace elastica
