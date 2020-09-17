//==============================================================================
/*!
//  \file
//  \brief Type traits for the Systems configuration and interfaces
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

#include "Simulator/Interface/Protocols/Systems.hpp"
#include "Simulator/Interface/TypeTraits/Aliases/Systems.hpp"
#include "Utilities/ListRequires.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/TypeTraits.hpp"
#include "Utilities/TypeTraits/IsDetected.hpp"

// DONE Should be in a traits namespace somewhere

namespace elastica {

  namespace tt {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Compile time check for conformance to Systems configuration
    // requirements.
    // \ingroup type_traits
    //
    // This type trait class tests whether or not the given type \a SC is a
    // Elastica++ SystemsConfiguration. In order to qualify as a valid
    // SystemsConfiguration the given type has to derive publicly from
    // the SystemsConfiguration protocol class. In case the given type is
    // a valid SystemsConfiguration type, the \a value member constant is
    // set to \a true, the nested type definition \a type is \a std::true_type
    // ,and the class derives from \a std::true_type. Otherwise \a value is
    // set to \a false, \a type is \a std::false_type, and the class derives
    // from \a std::false_type.
    */
    template <typename SC>
    struct IsSystemsConfiguration
        : public ::tt::conforms_to<
              SC, ::elastica::configuration::protocols::SystemsConfiguration> {
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Auxiliary variable template for the IsSystemsConfiguration
    // type trait.
    // \ingroup type_traits
    //
    // The is_systems_configuration_v variable template provides a
    // convenient shortcut to access the nested \a value of the
    // IsSystemConfiguration class template. For instance, given the type
    // \a T the following two statements are identical:

       \code
       using namespace elastica::interface::tt;
       constexpr bool value1 = IsSystemsConfiguration<T>::value;
       constexpr bool value2 = is_systems_configuration_v<T>;
       \endcode
    */
    template <typename EC>
    constexpr bool is_systems_configuration_v =
        IsSystemsConfiguration<EC>::value;
    //**************************************************************************

    namespace {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Compile time check for System Tag
      // \ingroup type_traits
      //
      // This type trait class tests whether or not the given type \a Tag is a
      // valid Elastica++ system. In order to qualify as a valid
      // system the given type should have two nested typedefs called
      // \a PrimitiveType and \a SystemType. In case the given type is a valid
      // environment type, the \a value member constant is set to \a true, the
      // nested type definition \a type is \a std::true_type, and the class
      // derives from \a std::true_type. Otherwise \a value is set to \a false
      // \a type is \a std::false_type, and the class derives from \a
      // std::false_type.
      */
      template <typename SystemTag>
      struct IsSystemTag : public cpp17::conjunction<
                               ::tt::is_detected<primitive_type_t, SystemTag>,
                               ::tt::is_detected<system_type_t, SystemTag>> {};
      /*! \endcond */
      //************************************************************************

    }  // namespace

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //************************************************************************
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
    template <typename SC>
    struct AssertValidSystemConfiguration
        // 1. strict check for conformance
        : public ::tt::detail::AssertConformsToImpl<
              SC, ::elastica::configuration::protocols::SystemsConfiguration> {
      using AdmissibleSystemCollectionTags =
          ::elastica::tt::admissible_system_tags_t<SC>;
      using Constraint = ListRequires<IsSystemTag>;
      // The system tag should be valid ()
      using ConstraintPassed =
          tmpl::transform<AdmissibleSystemCollectionTags,
                          Constraint::template apply<tmpl::_1>>;
    };
    //**************************************************************************

  }  // namespace tt

}  // namespace elastica
