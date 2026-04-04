//==============================================================================
/*!
//  \file
//  \brief Domain configuration protocols
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <type_traits>
#include <utility>

#include "Simulator/Configuration/TypeTraits/Aliases/Domain.hpp"
#include "Simulator/Domain/TypeTraits.hpp"
#include "Simulator/Frames/EulerianFrame.hpp"
#include "Utilities/ProtocolHelpers.hpp"
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
       * \ingroup configuration_protocols
       *
       * Class to enforce adherence to Domain configuration protocol. Any valid
       * domain configuration within the \elastica library should (publicly)
       * inherit from this class to indicate it qualifies as a
       * DomainConfiguration. Only in case a class is derived publicly from this
       * base class, the tt::conforms_to and tt::assert_conforms_to type traits
       * recognizes the class as valid DomainConfiguration.
       *
       * Requires that a conforming type `ConformingType` has these nested types
       * \snippet this expected_types
       * and these member functions
       * \snippet this expected_functions
       *
       * \example
       * ::elastica::configuration::DomainConfiguration shows a minimal
       * example of conformance to this protocol
       */
      struct DomainConfiguration {
        //**********************************************************************
        /*! \cond ELASTICA_INTERNAL */
        /*!\brief Auxiliary helper struct for enforcing protocols.
        // \ingroup configuration_protocols
        */
        template <typename ConformingType>
        struct test {
         public:
          // Check for nested types of the ConfirmingType
          /// [expected_types]
          // check for the domain type
          static_assert(::tt::is_detected_v<::elastica::tt::domain_type_t,
                                            ConformingType>,
                        R"error(
Not a conforming domain configuration, doesn't have a nested type `DomainType`
)error");
          // if detected, check if its a valid domain type
          using domain_type = ::elastica::tt::domain_type_t<ConformingType>;
          static_assert(::elastica::tt::IsValidDomain<domain_type>::value,
                        R"error(
Configuration does not contain a `DomainType` conforming to protocol requirements
)error");

          //**Type definitions**************************************************
          //! Auxiliary alias template for boundary type along direction D
          template <typename PossibleDomainType>
          using boundary_type_along_X =
              typename PossibleDomainType::template BoundaryTypeAlong<
                  ::elastica::detail::EulerianFrame::X>;
          template <typename PossibleDomainType>
          using boundary_type_along_Y =
              typename PossibleDomainType::template BoundaryTypeAlong<
                  ::elastica::detail::EulerianFrame::Y>;
          template <typename PossibleDomainType>
          using boundary_type_along_Z =
              typename PossibleDomainType::template BoundaryTypeAlong<
                  ::elastica::detail::EulerianFrame::Z>;

          // Check for nested boundary types of the ConfirmingType
          static_assert(
              ::tt::is_detected_v<boundary_type_along_X, ConformingType>,
              R"error(
Not a conforming domain configuration, doesn't have boundary types in the
X direction
)error");
          static_assert(
              ::tt::is_detected_v<boundary_type_along_Y, ConformingType>,
              R"error(
Not a conforming domain configuration, doesn't have boundary types in the
Y direction
)error");
          static_assert(
              ::tt::is_detected_v<boundary_type_along_Z, ConformingType>,
              R"error(
Not a conforming domain configuration, doesn't have boundary types in the
Z direction
)error");
          /// [expected_types]

          /// [expected_functions]
          // Try calling the `ConformingType::domain` templated member
          // function
          using const_lvalue_domain_return_type = std::remove_cvref_t<
              decltype(std::declval<ConformingType const&>().domain())>;
          static_assert(
              std::is_same_v<const_lvalue_domain_return_type, domain_type>,
              R"error(
Not a conforming Domain configuration, doesn't properly implement function
`domain_type const& domain() const&`
)error");

          using rvalue_domain_return_type = std::remove_cvref_t<
              decltype(std::declval<ConformingType&&>().domain())>;
          static_assert(
              std::is_same_v<rvalue_domain_return_type, domain_type>,
              R"error(
Not a conforming Domain configuration, doesn't properly implement function
`domain_type&& domain() &&`
)error");
          /// [expected_functions]
        };
        /*! \endcond */
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace protocols

  }  // namespace configuration

}  // namespace elastica
