#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <type_traits>

#include "ModuleSettings/Domain/Protocols.hpp"
#include "Simulator/Domain/Types.hpp"
// HEALTH WARNING : We need a complete implementation of Domain here to let
// the compiler know that it conforms to the protocol (the types header only
// forward declares it without conformance)
#include "Simulator/Domain/Domain.hpp"
// This is an implementation detail and hence isn't exposed in Types
#include "Simulator/Domain/Policies/Directions.hpp"
// Complete implementation of this type is needed to do manipulations
#include "Simulator/Domain/BoundaryTypes.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits.hpp"

//==============================================================================
//
//  DOMAIN CONFIGURATION
//
//==============================================================================

#include "Configuration/Domain.hpp"

namespace elastica {

  namespace defaults {

    namespace detail {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Helper struct for accessing boundary types along different
      // directions.
      // \ingroup domain
      //
      // The BoundaryTypeAlongHelper class template offers the ability to obtain
      // the boundary type along the X, Y and Z directions in the current
      // simulation. It defines the nested type \a type, which
      // represents the boundary type for X, Y and Z directions. In case the
      // direction is not one of X, Y and Z, there is no nested type \a type.
      // Note that \c const and \c volatile qualifiers and reference modifiers
      // are generally ignored.
      */
      template <::elastica::detail::Directions::DirectionType>
      struct BoundaryTypeAlongHelper;
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Specialization of the BoundaryTypeAlongHelper class template for
      // the X direction.
      // \ingroup domain
      */
      template <>
      struct BoundaryTypeAlongHelper<::elastica::detail::Directions::X> {
        using type =
            cpp20::remove_cvref_t<ELASTICA_DEFAULT_BOUNDARY_TYPE_ALONG_X>;
      };
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Specialization of the BoundaryTypeAlongHelper class template for
      // the Y direction.
      // \ingroup domain
      */
      template <>
      struct BoundaryTypeAlongHelper<::elastica::detail::Directions::Y> {
        using type =
            cpp20::remove_cvref_t<ELASTICA_DEFAULT_BOUNDARY_TYPE_ALONG_Y>;
      };
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Specialization of the BoundaryTypeAlongHelper class template for
      // the Z direction.
      // \ingroup domain
      */
      template <>
      struct BoundaryTypeAlongHelper<::elastica::detail::Directions::Z> {
        using type =
            cpp20::remove_cvref_t<ELASTICA_DEFAULT_BOUNDARY_TYPE_ALONG_Z>;
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace detail

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    /*!\brief Default configuration of domain in the simulation
    // \ingroup config
    //
    // The DomainConfiguration struct is the customization point of domain
    // related properties in a simulation involving the Elastica++ library. It
    // offers the ability to customize the simulation with different boundary
    // conditions. It defines the nested type \a DomainType, which indicates the
    // overall boundary conditions for the simulation and a nested template type
    // \a BoundaryTypeAlong which represents the boundary type for X, Y and Z
    // directions.
    */
    struct DomainConfiguration
        : public ::tt::ConformsTo<protocols::DomainConfiguration> {
     public:
      //**Type definitions******************************************************

      //! Alias template for boundary type along direction D
      template <::elastica::detail::Directions::DirectionType D>  // Direction
      using BoundaryTypeAlong =
          typename detail::BoundaryTypeAlongHelper<D>::type;

      // Why not lazy?
      // if at least one (boundary_types) == periodic, then periodic domain
      // periodic is true_type and unbounded is false_type

      //! Type of simulation domain
      using DomainType = std::conditional_t<
          tmpl::or_<
              BoundaryTypeAlong<::elastica::detail::Directions::X>,
              BoundaryTypeAlong<::elastica::detail::Directions::Y>,
              BoundaryTypeAlong<::elastica::detail::Directions::Z>>::value,
          PeriodicDomain, UnboundedDomain>;
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace defaults

}  // namespace elastica
