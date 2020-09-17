#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <type_traits>
#include <utility>

#include "Simulator/Domain/Policies/Directions.hpp"
#include "Simulator/Interface/TypeTraits/Aliases/Domain.hpp"
#include "Utilities/TypeTraits/IsDetected.hpp"

namespace elastica {

  namespace configuration {

    namespace protocols {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Domain configuration protocol.
      // \ingroup protocols
      //
      // Class to enforce adherence to Domain config protocol. Any valid domain
      // configuration within the Elastica++ library should (publicly) inherit
      // from this class to indicate it qualifies as a Domain. Only in case a
      // class is derived publicly from this base class, the
      // \c tt::conforms_to and \c tt::assert_conforms_to type traits recognizes
      // the class as valid Domain.
      //
      // Requires that conforming type has these nested types:
      // - \a DomainType : Type of the domain
      // - \a BoundaryTypeAlong : Type of boundary along one direction
      */
      struct DomainConfiguration {
        //**********************************************************************
        /*! \cond ELASTICA_INTERNAL */
        /*!\brief Auxiliary helper struct for enforcing protocols.
        // \ingroup protocols
        */
        template <typename ConformingType>
        struct test {
         public:
          //**Type definitions**************************************************
          //! Auxiliary alias template for boundary type along direction D
          template <typename PossibleDomainType,
                    ::elastica::detail::Directions::DirectionType D>
          using boundary_type_along_t =
              typename PossibleDomainType::template BoundaryTypeAlong<D>;
          //********************************************************************

          // Check for nested types of the ConfirmingType
          static_assert(::tt::is_detected_v<::elastica::tt::domain_type_t,
                                            ConformingType>,
                        "Not a conforming domain configuration, doesn't have a "
                        "nested DomainType member");

          // Check for nested boundary types of the ConfirmingType
          static_assert(
              ::tt::is_detected_v<boundary_type_along_t, ConformingType,
                                  ::elastica::detail::Directions::X>,
              "Not a conforming domain configuration, doesn't have "
              "boundary types in the X direction");
          static_assert(
              ::tt::is_detected_v<boundary_type_along_t, ConformingType,
                                  ::elastica::detail::Directions::Y>,
              "Not a conforming domain configuration, doesn't have "
              "boundary types in the Y direction");
          static_assert(
              ::tt::is_detected_v<boundary_type_along_t, ConformingType,
                                  ::elastica::detail::Directions::Z>,
              "Not a conforming domain configuration, doesn't have "
              "boundary types in the Z direction");
        };
        /*! \endcond */
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace protocols

  }  // namespace configuration

}  // namespace elastica
