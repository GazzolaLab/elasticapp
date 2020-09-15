//==============================================================================
/*!
//  \file
//  \brief Type traits for the Domain configuration and interfaces
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include "ModuleSettings/Domain/Protocols.hpp"
#include "Simulator/Domain/Protocols.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace elastica {

  namespace interface {

    namespace tt {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Compile time check whether the given type is a Domain type.
      // \ingroup type_traits
      //
      // This type trait class tests whether or not the given type \a D is a
      // Elastica++ Domain type. In order to qualify as a valid Domain type,
      // the given type has to derive publicly from the ComputationalDomain
      // protocol class. In case the given type is a valid Domain type, the \a
      // value member constant is set to \a true, the nested type definition \a
      // type is \a std::true_type, and the class derives from
      // \a std::true_type. Otherwise \a value is set to \a false, \a type is
      // \a std::false_type, and the class derives from \a std::false_type.
      */
      template <typename D>
      struct IsDomain : public ::tt::detail::AssertConformsToImpl<
                            D, ::elastica::protocols::ComputationalDomain> {};
      //************************************************************************

      //************************************************************************
      /*!\brief Auxiliary variable template for the IsDomain type trait.
      // \ingroup type_traits
      //
      // The is_domain_v variable template provides a convenient shortcut to
      // access the nested \a value of the IsDomain class template. For
      // instance, given the type \a T the following two statements are
      // identical:

         \code
         using namespace elastica::interface::tt;
         constexpr bool value1 = IsDomain<T>::value;
         constexpr bool value2 = is_domain_v<T>;
         \endcode
      */
      template <typename D>
      constexpr bool is_domain_v = IsDomain<D>::value;
      //************************************************************************

      //************************************************************************
      /*!\brief Compile time check for conformance to Domain configuration
      // requirements.
      // \ingroup type_traits
      //
      // This type trait class tests whether or not the given type \a DC is a
      // Elastica++ DomainConfiguration. In order to qualify as a valid Domain
      // Configuration the given type has to derive publicly from the
      // DomainConfiguration protocol class. In case the given type is a valid
      // DomainConfiguration type, the \a value member constant is set to \a
      // true, the nested type definition \a type is \a std::true_type, and
      // the class derives from \a std::true_type. Otherwise \a value is set to
      // \a false, \a type is \a std::false_type, and the class derives from
      // \a std::false_type.
      */
      template <typename DC>
      struct IsDomainConfiguration
          : ::tt::conforms_to<
                DC, ::elastica::defaults::protocols::DomainConfiguration> {};
      //************************************************************************

      //************************************************************************
      /*!\brief Auxiliary variable template for the IsDomainConfiguration type
      // trait.
      // \ingroup type_traits
      //
      // The is_domain_configuration_v variable template provides a convenient
      // shortcut to access the nested \a value of the IsDomainConfiguration
      // class template. For instance, given the type \a T the following two
      // statements are identical:

         \code
         using namespace elastica::interface::tt;
         constexpr bool value1 = IsDomainConfiguration<T>::value;
         constexpr bool value2 = is_domain_configuration_v<T>;
         \endcode
      */
      template <typename DC>
      constexpr bool is_domain_configuration_v =
          IsDomainConfiguration<DC>::value;
      //************************************************************************

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Compile time check for strict conformance to Domain
      // requirements.
      // \ingroup type_traits
      //
      // This type trait class asserts whether or not the given type \a DC can
      // be used as an Elastica++ DomainConfiguration in the current
      // simulation. In order to be safely used as a valid Domain
      // Configuration the given type has to derive publicly from
      // the DomainConfiguration protocol class and satisfy all its
      // requirements. Furthermore, the nested alias \c DomainType needs to
      // satisfy the requirements of \c IsDomain type trait for \DC to qualify
      // as a safe DomainConfiguration to be used in the current simulation. In
      // case the given  type is a safe DomainConfiguration type, the \a value
      // member constant is set to \a true, the nested type definition \a type
      // is \a std::true_type , and the class derives from \a std::true_type.
      // Otherwise, a compiler- error is thrown indicating the mode of failure
      // and remedial measures.
      */
      template <typename DC>
      struct AssertValidDomainConfiguration
          // 1. strict check for conformance
          : public ::tt::detail::AssertConformsToImpl<
                DC, ::elastica::defaults::protocols::DomainConfiguration> {
        using ConstraintPassed = IsDomain<::elastica::tt::domain_type_t<DC>>;
      };
      //************************************************************************

    }  // namespace tt

  }  // namespace interface

}  // namespace elastica
