//==============================================================================
/*!
//  \file
//  \brief Type traits for the Systems configuration
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

#include "Simulator/Configuration/Protocols/Systems.hpp"
#include "Utilities/ProtocolHelpers.hpp"

// DONE Should be in a traits namespace somewhere

namespace elastica {

  namespace tt {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Check whether a given type `SC` is a SystemsConfiguration
     * \ingroup config_type_traits
     *
     * \details
     * Inherits from std::true_type if `SC` conforms to
     * ::elastica::configuration::protocols::SystemsConfiguration, otherwise
     * inherits from std::false_type.
     *
     * \usage
     * For any type `SC`,
     * \code
     * using result = IsSystemsConfiguration<T>;
     * \endcode
     *
     * \metareturns
     * cpp17::bool_constant
     *
     * \semantics
     * If the type `SC` publicly inherits from
     * ::elastica::configuration::protocols::SystemsConfiguration
     * \code
     * typename result::type = std::true_type;
     * \endcode
     * otherwise
     * \code
     * typename result::type = std::false_type;
     * \endcode
     *
     * \example
     * \snippet TypeTraits/Test_Systems.cpp is_systems_config_eg
     *
     * \tparam SC the type to check
     *
     * \see ::elastica::configuration::protocols::SystemsConfiguration
     */
    template <typename SC>
    struct IsSystemsConfiguration
        : public ::tt::conforms_to<
              SC, ::elastica::configuration::protocols::SystemsConfiguration> {
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Auxiliary variable template for tt::IsSystemsConfiguration
     * \ingroup config_type_traits
     *
     * The is_systems_configuration_v variable template provides a
     * convenient shortcut to access the nested value of the
     * tt::IsSystemConfiguration class template. For instance, given the type
     * \a T the following two statements are identical:
     * \code
     * constexpr bool value1 = elastica::tt::IsSystemsConfiguration<T>::value;
     * constexpr bool value2 = elastica::tt::is_systems_configuration_v<T>;
     * \endcode
     */
    template <typename EC>
    constexpr bool is_systems_configuration_v =
        IsSystemsConfiguration<EC>::value;
    //**************************************************************************

    //**************************************************************************
    /*!\brief Check whether a given type `SC` is a valid SystemsConfiguration
     * \ingroup config_type_traits
     *
     * \details
     * Inherits from std::true_type if `SC` strictly conforms to
     * ::elastica::configuration::protocols::SystemsConfiguration, otherwise
     * inherits from std::false_type.
     *
     * \usage
     * For any type `SC`,
     * \code
     * using result = IsValidSystemsConfiguration<T>;
     * \endcode
     *
     * \metareturns
     * cpp17::bool_constant
     *
     * \semantics
     * If the type `SC` strictly conforms to
     * ::elastica::configuration::protocols::SystemsConfiguration, that is it
     * publicly inherits from the protocol and passes all the requirements
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
     * \snippet TypeTraits/Test_Systems.cpp is_valid_systems_config_eg
     *
     * \tparam SC the type to check
     *
     * \see ::elastica::configuration::protocols::SystemsConfiguration
     */
    template <typename SC>
    struct IsValidSystemsConfiguration
        : public ::tt::detail::AssertConformsToImpl<
              SC, ::elastica::configuration::protocols::SystemsConfiguration> {
    };
    //**************************************************************************

  }  // namespace tt

}  // namespace elastica

//    namespace {
//
//      //========================================================================
//      //
//      //  CLASS DEFINITION
//      //
//      //========================================================================
//
//      //************************************************************************
//      /*! \cond ELASTICA_INTERNAL */
//      /*!\brief Compile time check for System Tag
//      // \ingroup config_type_traits
//      //
//      // This type trait class tests whether or not the given type \a Tag is a
//      // valid \elastica system. In order to qualify as a valid
//      // system the given type should have two nested typedefs called
//      // \a PrimitiveType and \a SystemType. In case the given type is a valid
//      // system type, the \a value member constant is set to \a true, the
//      // nested type definition \a type is \a std::true_type, and the class
//      // derives from \a std::true_type. Otherwise \a value is set to \a false
//      // \a type is \a std::false_type, and the class derives from \a
//      // std::false_type. Note that cv qualifiers and references are not
//      removed
//      // by design, the input tags need to be non-reference and non-cv
//      qualified
//      */
//      template <typename SystemTag>
//      struct IsSystemTag
//          : public cpp17::conjunction<
//                ::tt::is_detected<primitive_type_t, SystemTag>,
//                ::tt::is_detected<system_type_t, SystemTag>,
//                cpp17::negation<::tt::is_cvref_t<system_type_t<SystemTag>>>,
//                cpp17::negation<::tt::is_cvref_t<primitive_type_t<SystemTag>>>>
//
//      {};
//      /*! \endcond */
//      //************************************************************************
//
//    }  // namespace
//
//    //==========================================================================
//    //
//    //  CLASS DEFINITION
//    //
//    //==========================================================================
//
//    //**************************************************************************
//    /*!\brief Compile time check for strict conformance to
//    SystemsConfiguration
//    // requirements.
//    // \ingroup config_type_traits
//    //
//    // This type trait class asserts whether or not the given type \a EC can
//    // be used as an \elastica SystemsConfiguration in the current
//    // simulation. In order to be safely used as a valid Systems
//    // Configuration the given type has to derive publicly from
//    // the SystemsConfiguration protocol class and satisfy all its
//    // requirements. Furthermore, all nested tags need to satisfy the
//    // \c IsSystemsTag predicate for \a EC to qualify as safe for use in
//    // the current simulation. In case the given type is a safe
//    // SystemsConfiguration type, the \a value member constant is set to
//    // \a true, the nested type definition \a type is \a std::true_type
//    // , and the class derives from \a std::true_type. Otherwise, a compiler-
//    // error is thrown indicating the mode of failure and remedial measures.
//    */
//    template <typename SC>
//    struct AssertValidSystemConfiguration
//        // 1. strict check for conformance
//        : public ::tt::detail::AssertConformsToImpl<
//              SC, ::elastica::configuration::protocols::SystemsConfiguration>
//              {
//      using AdmissibleSystemCollectionTags =
//          ::elastica::tt::admissible_system_plugins_t<SC>;
//      using Constraint = ListRequires<IsSystemTag>;
//      // The system tag should be valid ()
//      using ConstraintPassed =
//          tmpl::transform<AdmissibleSystemCollectionTags,
//                          Constraint::template apply<tmpl::_1>>;
//    };
//    //**************************************************************************
