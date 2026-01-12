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
     * \ingroup domain
     *
     * Class to enforce adherence to Domain protocol. Any valid Domain within
     * the \elastica library should (publicly) inherit from this class to
     * indicate it qualifies as a Domain. Only in case a class is derived
     * publicly from this base class, the  tt::conforms_to and
     * tt::assert_conforms_to type traits recognizes the class as valid
     * Domains.
     *
     * Requires that a conforming type `ConformingType` has these nested types
     * \snippet this expected_types
     * and these member functions,
     * \snippet this expected_functions
     *
     * \example
     * The following shows an example of minimal conformance to this protocol.
     * \snippet Domain/Test_Protocols.cpp protocols_eg
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
        /// [expected_types]
        using bound_type = typename ConformingType::bound_type;
        /// [expected_types]
        //**********************************************************************

        /// [expected_functions]
        // Try calling the `ConformingType::get_lower_bound` templated member
        // function
        using lower_domain_return_type = typename std::common_type<
            decltype(std::declval<ConformingType const&>()
                         .template get_lower_bound<ConformingType::X>()),
            decltype(std::declval<ConformingType const&>()
                         .template get_lower_bound<ConformingType::Y>()),
            decltype(std::declval<ConformingType const&>()
                         .template get_lower_bound<ConformingType::Z>())>::type;
        static_assert(std::is_same_v<lower_domain_return_type, bound_type>,
                      R"error(
Not a conforming Domain, doesn't properly implement function
`template <DirectionType Direction>
bound_type get_lower_bound() const`)error");

        // Try calling the `ConformingType::get_lower_bound` templated member
        // function
        using upper_domain_return_type = typename std::common_type<
            decltype(std::declval<ConformingType const&>()
                         .template get_upper_bound<ConformingType::X>()),
            decltype(std::declval<ConformingType const&>()
                         .template get_upper_bound<ConformingType::Y>()),
            decltype(std::declval<ConformingType const&>()
                         .template get_upper_bound<ConformingType::Z>())>::type;
        static_assert(std::is_same_v<upper_domain_return_type, bound_type>,
                      R"error(
Not a conforming Domain, doesn't properly implement function
`template <DirectionType Direction>
bound_type get_upper_bound() const`)error");

        // Try calling the `ConformingType::get_span` templated member function
        using span_return_type = typename std::common_type<
            decltype(std::declval<ConformingType const&>()
                         .template get_span<ConformingType::X>()),
            decltype(std::declval<ConformingType const&>()
                         .template get_span<ConformingType::Y>()),
            decltype(std::declval<ConformingType const&>()
                         .template get_span<ConformingType::Z>())>::type;
        static_assert(std::is_same_v<span_return_type, bound_type>,
                      R"error(
Not a conforming Domain, doesn't properly implement function
`template <DirectionType Direction>
bound_type get_span() const`)error");
        /// [expected_functions]
      };
      /*! \endcond */
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace protocols

}  // namespace elastica
