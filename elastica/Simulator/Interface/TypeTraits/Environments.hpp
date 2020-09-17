//==============================================================================
/*!
//  \file
//  \brief Type traits for the Environments configuration and interfaces
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

#include <type_traits>

#include "Simulator/Interface/Protocols/Environments.hpp"
#include "Simulator/Interface/TypeTraits/Aliases/Environments.hpp"
#include "Utilities/ListRequires.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/TypeTraits.hpp"

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
    /*!\brief Compile time check for conformance to Environments configuration
    // requirements.
    // \ingroup type_traits
    //
    // This type trait class tests whether or not the given type \a EC is a
    // Elastica++ EnvironmentsConfiguration. In order to qualify as a valid
    // EnvironmentsConfiguration the given type has to derive publicly from
    // the EnvironmentsConfiguration protocol class. In case the given type is
    // a valid EnvironmentsConfiguration type, the \a value member constant is
    // set to \a true, the nested type definition \a type is \a std::true_type
    // ,and the class derives from \a std::true_type. Otherwise \a value is
    // set to \a false, \a type is \a std::false_type, and the class derives
    // from \a std::false_type.
    */
    template <typename EC>
    struct IsEnvironmentsConfiguration
        : ::tt::conforms_to<
              EC,
              ::elastica::configuration::protocols::EnvironmentsConfiguration> {
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Auxiliary variable template for the IsEnvironmentsConfiguration
    // type trait.
    // \ingroup type_traits
    //
    // The is_environments_configuration_v variable template provides a
    // convenient shortcut to access the nested \a value of the
    // IsEnvironmentConfiguration class template. For instance, given the type
    // \a T the following two statements are identical:

       \code
       using namespace elastica::interface::tt;
       constexpr bool value1 = IsEnvironmentsConfiguration<T>::value;
       constexpr bool value2 = is_environments_configuration_v<T>;
       \endcode
    */
    template <typename EC>
    constexpr bool is_environments_configuration_v =
        IsEnvironmentsConfiguration<EC>::value;
    //**************************************************************************

    namespace {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Compile time check for Environment Tags
      // \ingroup type_traits
      //
      // This type trait class tests whether or not the given type \a Tag is a
      // valid Elastica++ environment. In order to qualify as a valid
      // environment the given type should have a nested template alias called
      // \a type. In case the given type is a valid environment type, the \a
      // value member constant is set to \a true, the nested type definition
      // \a type is \a std::true_type, and the class derives from \a
      // std::true_type. Otherwise \a value is set to \a false, \a type is \a
      // std::false_type, and the class derives from \a std::false_type.
      */
      template <typename Tag, typename = ::tt::templated_void_t<>>
      struct IsEnvironmentTag : std::false_type {};
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Specialization of the IsEnvironmentTag class template for
       * Environment Tag types
      // \ingroup math_traits
      */
      template <typename Tag>
      struct IsEnvironmentTag<Tag, ::tt::templated_void_t<Tag::template type>>
          : std::true_type {};
      /*! \endcond */
      //************************************************************************

    }  // namespace

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Compile time check for strict conformance to Environment
    // requirements.
    // \ingroup type_traits
    //
    // This type trait class asserts whether or not the given type \a EC can
    // be used as an Elastica++ EnvironmentConfiguration in the current
    // simulation. In order to be safely used as a valid Environment
    // Configuration the given type has to derive publicly from
    // the EnvironmentConfiguration protocol class and satisfy all its
    // requirements. Furthermore, all nested tags need to satisfy the
    // \c IsEnvironmentTag predicate for \a EC to qualify as safe for use in
    // the current simulation. In case the given type is a safe
    // EnvironmentConfiguration type, the \a value member constant is set to
    // \a true, the nested type definition \a type is \a std::true_type
    // , and the class derives from \a std::true_type. Otherwise, a compiler-
    // error is thrown indicating the mode of failure and remedial measures.
    */
    template <typename EC>
    struct AssertValidEnvironmentConfiguration
        // 1. strict check for conformance
        : public ::tt::detail::AssertConformsToImpl<
              EC,
              ::elastica::configuration::protocols::EnvironmentsConfiguration> {
      using AdmissibleEnvironmentTags =
          ::elastica::tt::admissible_environment_tags_t<EC>;
      using Constraint = ListRequires<IsEnvironmentTag>;
      // 2. The environment tags should be valid ()
      using ConstraintPassed =
          tmpl::transform<AdmissibleEnvironmentTags,
                          Constraint::template apply<tmpl::_1>>;
    };
    //**************************************************************************

  }  // namespace tt

}  // namespace elastica
