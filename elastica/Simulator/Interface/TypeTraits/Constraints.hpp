//==============================================================================
/*!
//  \file
//  \brief Constraint interface type traits
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
#include <utility>

#include "Utilities/TypeTraits.hpp"
#include "Simulator/Interface/Protocols/Constraints.hpp"
#include "Utilities/ProtocolHelpers.hpp"


namespace elastica {

  namespace tt {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Compile time check for conformance to Constraint interface
    // requirements.
    // \ingroup type_traits
    //
    // This type trait class tests whether or not the given type \c CM has a
    // \elastica \constraint interface. In order to qualify as a valid
    // \constraint the given type has to derive publicly from the
    // elastica::interface::protocols::Constraints protocol class. In case the
    // given type is a valid, Constraints type, the \a value member constant is
    // set to \a true, the nested type definition \a type is \a std::true_type,
    // and the class derives from \a std::true_type. Otherwise \a value is set
    // to \a false, \a type is \a std::false_type, and the class derives from
    // \a std::false_type.
    //
    // \usage
    // For any \constraint type \c CM,
       \code
       using result = IsConstraintsModule<CM>;
       \endcode
    //
    // \metareturns
    // cpp17::bool_constant
    //
    // \semantics
    // If the type \c CM conforms to protocols::Constraints, then
       \code
       typename result::type = std::true_type;
       \endcode
    // otherwise
       \code
       typename result::type = std::false_type;
       \endcode
    //
    // \see protocols::Constraints
    //
    // \tparam CM : the \constraint type to check
    */
    template <typename CM> // Type to check
    struct IsConstraintsModule
        : ::tt::conforms_to<
            CM, ::elastica::interface::protocols::Constraints> {};
    //**************************************************************************

    //**************************************************************************
    /*!\brief Auxiliary variable template for the IsConstraintsModule type
    // trait.
    // \ingroup type_traits
    //
    // The is_constraints_module_v variable template provides a convenient
    // shortcut to access the nested \a value of the IsConstraintsModule
    // class template. For instance, given the type \a T the following two
    // statements are identical:
    // \example
       \code
         using namespace elastica::interface::tt;
         constexpr bool value1 = IsConstraintsModule<T>::value;
         constexpr bool value2 = is_constraints_module_v<T>;
       \endcode
    */
    template <typename CM>
    constexpr bool is_constraints_module_v = IsConstraintsModule<CM>::value;
    //**************************************************************************

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Compile time check for strict conformance to Constraint interface
    // requirements.
    // \ingroup type_traits
    //
    // This type trait class asserts whether or not the given type \c CM can
    // be used as an \elastica \constraints module in the current
    // simulation. In order to be safely used as a valid \constraints module
    // the given type \c CM has to derive publicly from
    // the elastica::interface::protocols::Constraints protocol class and
    // satisfy all its requirements. In case the given type is a safe
    // ConstraintsModule type, the \a value member constant is set to \a true,
    // the nested type definition \a type is \a std::true_type, and the class
    // derives from \a std::true_type. Otherwise, a compiler-error is thrown
    // indicating the mode of failure and remedial measures.
    */
    template <typename CM>
    struct AssertValidConstraintsModule
      // 1. strict check for conformance
        : public ::tt::detail::AssertConformsToImpl<
            CM, ::elastica::interface::protocols::Constraints> {
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Helper trait for finding and validating an Constraint interface
    // \ingroup type_traits
    //
    // This type trait offers a modular way to both find an Constraint type and
    // assert strict conformance to the expected interface, using two nested
    // template type aliases \c predicate and \c check respectively.
    //
    // \see IsConstraintsModule, AssertValidConstraintsModule
    */
    struct ConstraintsTrait{
      //! The predicate template alias
      template <typename T>
      using predicate = IsConstraintsModule<T>;

      //! The check template alias
      template <typename T>
      using check = AssertValidConstraintsModule<T>;
    };
    //**************************************************************************

  }  // namespace tt

}  // namespace elastica
