//==============================================================================
/*!
//  \file
//  \brief Connection interface type traits
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

#include "Simulator/Interface/Protocols/Connections.hpp"
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
    /*!\brief Compile time check for conformance to Connection interface
     * requirements.
     * \ingroup interface_type_traits
     *
     * This type trait class tests whether or not the given type \c CM has a
     * \elastica \connection interface. In order to qualify as a valid
     * \connection the given type has to derive publicly from the
     * elastica::interface::protocols::Connections protocol class. In case the
     * given type is a valid, Connections type, the \a value member constant is
     * set to \a true, the nested type definition \a type is \a std::true_type,
     * and the class derives from \a std::true_type. Otherwise \a value is set
     * to \a false, \a type is \a std::false_type, and the class derives from
     * \a std::false_type.
     *
     * \usage
     * For any \connection type \c CM,
     * \code
     * using result = IsConnectionsModule<CM>;
     * \endcode
     *
     * \metareturns
     * cpp17::bool_constant
     *
     * \semantics
     * If the type \c CM conforms to protocols::Connections, then
     * \code
     * typename result::type = std::true_type;
     * \endcode
     * otherwise
     * \code
     * typename result::type = std::false_type;
     * \endcode
     *
     * \see protocols::Connections
     *
     * \tparam CM : the \connection type to check
     */
    template <typename CM>  // Type to check
    struct IsConnectionsModule
        : ::tt::conforms_to<CM, ::elastica::interface::protocols::Connections> {
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Auxiliary variable template for the IsConnectionsModule type
     * trait.
     * \ingroup interface_type_traits
     *
     * The is_connections_module_v variable template provides a convenient
     * shortcut to access the nested \a value of the IsConnectionsModule
     * class template. For instance, given the type \a T the following two
     * statements are identical:
     * \example
     * \code
     *   using namespace elastica::interface::tt;
     *   constexpr bool value1 = IsConnectionsModule<T>::value;
     *   constexpr bool value2 = is_connections_module_v<T>;
     * \endcode
     */
    template <typename CM>
    constexpr bool is_connections_module_v = IsConnectionsModule<CM>::value;
    //**************************************************************************

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Compile time check for strict conformance to Connection interface
     * requirements.
     * \ingroup interface_type_traits
     *
     * This type trait class asserts whether or not the given type \c CM can
     * be used as an \elastica \connections module in the current
     * simulation. In order to be safely used as a valid \connections module
     * the given type \c CM has to derive publicly from
     * the elastica::interface::protocols::Connections protocol class and
     * satisfy all its requirements. In case the given type is a safe
     * ConnectionsModule type, the \a value member constant is set to \a true,
     * the nested type definition \a type is \a std::true_type, and the class
     * derives from \a std::true_type. Otherwise, a compiler-error is thrown
     * indicating the mode of failure and remedial measures.
     */
    template <typename CM>
    struct AssertValidConnectionsModule
        // 1. strict check for conformance
        : public ::tt::detail::AssertConformsToImpl<
              CM, ::elastica::interface::protocols::Connections> {};
    //**************************************************************************

    //**************************************************************************
    /*!\brief Helper trait for finding and validating an Connection interface
     * \ingroup interface_type_traits
     *
     * This type trait offers a modular way to both find an Connection type and
     * assert strict conformance to the expected interface, using two nested
     * template type aliases \c predicate and \c check respectively.
     *
     * \see IsConnectionsModule, AssertValidConnectionsModule
     */
    struct ConnectionsTrait {
      //! The predicate template alias
      template <typename T>
      using predicate = IsConnectionsModule<T>;

      //! The check template alias
      template <typename T>
      using check = AssertValidConnectionsModule<T>;
    };
    //**************************************************************************

  }  // namespace tt

}  // namespace elastica
