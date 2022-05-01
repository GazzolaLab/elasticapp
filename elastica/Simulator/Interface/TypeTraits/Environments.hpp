//==============================================================================
/*!
//  \file
//  \brief Environment interface type traits
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

#include "Simulator/Interface/Protocols/Environments.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/TypeTraits.hpp"

namespace elastica {

  namespace tt {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Compile time check for conformance to Environment interface
     * requirements.
     * \ingroup interface_type_traits
     *
     * This type trait class tests whether or not the given type \c CM has a
     * \elastica \environment interface. In order to qualify as a valid
     * \environment the given type has to derive publicly from the
     * elastica::interface::protocols::Environments protocol class. In case the
     * given type is a valid, Environments type, the \a value member constant is
     * set to \a true, the nested type definition \a type is \a std::true_type,
     * and the class derives from \a std::true_type. Otherwise \a value is set
     * to \a false, \a type is \a std::false_type, and the class derives from
     * \a std::false_type.
     *
     * \usage
     * For any \environment type \c CM,
     * \code
     * using result = IsEnvironmentsModule<CM>;
     * \endcode
     *
     * \metareturns
     * cpp17::bool_constant
     *
     * \semantics
     * If the type \c CM conforms to protocols::Environments, then
     * \code
     * typename result::type = std::true_type;
     * \endcode
     * otherwise
     * \code
     * typename result::type = std::false_type;
     * \endcode
     *
     * \see protocols::Environments
     *
     * \tparam CM : the \environment type to check
     */
    template <typename CM>  // Type to check
    struct IsEnvironmentsModule
        : ::tt::conforms_to<CM,
                            ::elastica::interface::protocols::Environments> {};
    //**************************************************************************

    //**************************************************************************
    /*!\brief Auxiliary variable template for the IsEnvironmentsModule type
     * trait.
     * \ingroup interface_type_traits
     *
     * The is_environments_module_v variable template provides a convenient
     * shortcut to access the nested \a value of the IsEnvironmentsModule
     * class template. For instance, given the type \a T the following two
     * statements are identical:
     * \example
     * \code
     *   using namespace elastica::interface::tt;
     *   constexpr bool value1 = IsEnvironmentsModule<T>::value;
     *   constexpr bool value2 = is_environments_module_v<T>;
     * \endcode
     */
    template <typename CM>
    constexpr bool is_environments_module_v = IsEnvironmentsModule<CM>::value;
    //**************************************************************************

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Compile time check for strict conformance to Environment interface
     * requirements.
     * \ingroup interface_type_traits
     *
     * This type trait class asserts whether or not the given type \c CM can
     * be used as an \elastica \environments module in the current
     * simulation. In order to be safely used as a valid \environments module
     * the given type \c CM has to derive publicly from
     * the elastica::interface::protocols::Environments protocol class and
     * satisfy all its requirements. In case the given type is a safe
     * EnvironmentsModule type, the \a value member constant is set to \a true,
     * the nested type definition \a type is \a std::true_type, and the class
     * derives from \a std::true_type. Otherwise, a compiler-error is thrown
     * indicating the mode of failure and remedial measures.
     */
    template <typename CM>
    struct AssertValidEnvironmentsModule
        // 1. strict check for conformance
        : public ::tt::detail::AssertConformsToImpl<
              CM, ::elastica::interface::protocols::Environments> {};
    //**************************************************************************

    //**************************************************************************
    /*!\brief Helper trait for finding and validating an Environment interface
     * \ingroup interface_type_traits
     *
     * This type trait offers a modular way to both find an Environment type and
     * assert strict conformance to the expected interface, using two nested
     * template type aliases \c predicate and \c check respectively.
     *
     * \see IsEnvironmentsModule, AssertValidEnvironmentsModule
     */
    struct EnvironmentsTrait {
      //! The predicate template alias
      template <typename T>
      using predicate = IsEnvironmentsModule<T>;

      //! The check template alias
      template <typename T>
      using check = AssertValidEnvironmentsModule<T>;
    };
    //**************************************************************************

  }  // namespace tt

}  // namespace elastica
