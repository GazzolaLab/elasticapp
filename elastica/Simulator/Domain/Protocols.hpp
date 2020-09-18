//==============================================================================
/*!
//  \file
//  \brief Domain protocols
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

#include <utility>

#include "Utilities/TypeTraits.hpp"

namespace elastica {

  namespace protocols {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Domain protocol.
    // \ingroup protocols
    //
    // Class to enforce adherence to Domain protocol. Any valid Domain within
    // the Elastica++ library should (publicly) inherit from this class to
    // indicate it qualifies as a Domain. Only in case a class is derived
    // publicly from this base class, the \c tt::conforms_to and
    // \c tt::assert_conforms_to type traits recognizes the class as valid
    // Domains.
    //
    // Requires that conforming class has these nested types:
    // - \a bound_type : Type of upper/lower bound
    //
    // Requires that the conforming class has these templated member functions,
    // templated on the directions
    // - \c get_lower_bound : Returns the lower bound of the domain
    // - \c get_upper_bound : Returns the upper bound of the domain
    // - \c get_span : Returns the span of the domain
    */
    struct ComputationalDomain {
      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Auxiliary helper struct for enforcing protocols.
      // \ingroup protocols
      */
      template <typename ConformingType>
      struct test {
       public:
        //**Type definitions****************************************************
        // Try calling the `ConformingType::get_lower_bound` templated member
        // function
        using lower_domain_return_type = typename std::common_type<
            decltype(std::declval<ConformingType&>()
                         .template get_lower_bound<ConformingType::X>()),
            decltype(std::declval<ConformingType&>()
                         .template get_lower_bound<ConformingType::Y>()),
            decltype(std::declval<ConformingType&>()
                         .template get_lower_bound<ConformingType::Z>())>::type;

        // Try calling the `ConformingType::get_lower_bound` templated member
        // function
        using upper_domain_return_type = typename std::common_type<
            decltype(std::declval<ConformingType&>()
                         .template get_upper_bound<ConformingType::X>()),
            decltype(std::declval<ConformingType&>()
                         .template get_upper_bound<ConformingType::Y>()),
            decltype(std::declval<ConformingType&>()
                         .template get_upper_bound<ConformingType::Z>())>::type;

        // Try calling the `ConformingType::get_span` templated member function
        using span_return_type = typename std::common_type<
            decltype(std::declval<ConformingType&>()
                         .template get_span<ConformingType::X>()),
            decltype(std::declval<ConformingType&>()
                         .template get_span<ConformingType::Y>()),
            decltype(std::declval<ConformingType&>()
                         .template get_span<ConformingType::Z>())>::type;

        using bound_type = typename ConformingType::bound_type;
        //**********************************************************************

        // Check the return type of the member functions
        static_assert(cpp17::is_same_v<lower_domain_return_type, bound_type>,
                      "Not a conforming domain, doesn't properly implement "
                      "`get_lower_bound`");
        static_assert(cpp17::is_same_v<upper_domain_return_type, bound_type>,
                      "Not a conforming domain, doesn't properly implement "
                      "`get_upper_bound`");
        static_assert(
            cpp17::is_same_v<span_return_type, bound_type>,
            "Not a conforming domain, doesn't properly implement `get_span`");
      };
      /*! \endcond */
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace protocols

}  // namespace elastica
