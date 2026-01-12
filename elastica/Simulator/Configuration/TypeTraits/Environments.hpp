//==============================================================================
/*!
//  \file
//  \brief Type traits for the Environments configurations
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
#include "Simulator/Configuration/Protocols/Environments.hpp"
#include "Utilities/ProtocolHelpers.hpp"

// DONE Type trait for valid Environment Configuration
// DONE Should be in a traits namespace somewhere

namespace elastica {

  namespace tt {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Check whether a given type `EC` is a valid
     * EnvironmentsConfiguration \ingroup config_type_traits
     *
     * \details
     * Inherits from std::true_type if `EC` strictly conforms to
     * ::elastica::configuration::protocols::EnvironmentsConfiguration,
     * otherwise inherits from std::false_type.
     *
     * \usage
     * For any type `EC`,
     * \code
     * using result = IsValidEnvironmentsConfiguration<EC>;
     * \endcode
     *
     * \metareturns
     * std::bool_constant
     *
     * \semantics
     * If the type `EC` strictly conforms to
     * ::elastica::configuration::protocols::EnvironmentsConfiguration, that is
     * it publicly inherits from the protocol and passes all the requirements
     * imposed by the protocol, then
     * \code
     * typename result::type = std::true_type;
     * \endcode
     * otherwise
     * \code
     * typename result::type = std::false_type;
     * \endcode
     *
     * \example
     * \snippet TypeTraits/Test_Environments.cpp is_valid_environments_config_eg
     *
     * \tparam EC the type to check
     *
     * \see ::elastica::configuration::protocols::EnvironmentsConfiguration
     */
    template <typename EC>
    struct IsValidEnvironmentsConfiguration
        // 1. strict check for conformance
        : public ::tt::detail::AssertConformsToImpl<
              EC,
              ::elastica::configuration::protocols::EnvironmentsConfiguration> {
    };
    //**************************************************************************

  }  // namespace tt

}  // namespace elastica

// namespace {
//
//   //========================================================================
//   //
//   //  CLASS DEFINITION
//   //
//   //========================================================================
//
//   //************************************************************************
//   /*! \cond ELASTICA_INTERNAL */
//   /*!\brief Compile time check for Environment Tags
//   // \ingroup config_type_traits
//   //
//   // This type trait class tests whether or not the given type \a Tag is a
//   // valid \elastica environment. In order to qualify as a valid
//   // environment the given type should have a nested template alias called
//   // \a type. In case the given type is a valid environment type, the \a
//   // value member constant is set to \a true, the nested type definition
//   // \a type is \a std::true_type, and the class derives from \a
//   // std::true_type. Otherwise \a value is set to \a false, \a type is \a
//   // std::false_type, and the class derives from \a std::false_type.
//   */
//   template <typename Tag, typename = ::tt::templated_void_t<>>
//   struct IsEnvironmentTag : std::false_type {};
//   /*! \endcond */
//   //************************************************************************
//
//   //************************************************************************
//   /*! \cond ELASTICA_INTERNAL */
//   /*!\brief Specialization of the IsEnvironmentTag class template for
//    * Environment Tag types
//   // \ingroup math_traits
//   */
//   template <typename Tag>
//   struct IsEnvironmentTag<Tag, ::tt::templated_void_t<Tag::template type>>
//       : std::true_type {};
//   /*! \endcond */
//   //************************************************************************
//
// }  // namespace
