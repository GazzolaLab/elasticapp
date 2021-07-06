//==============================================================================
/*!
//  \file
//  \brief Systems configuration type aliases
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

namespace elastica {

  namespace tt {

    //==========================================================================
    //
    //  ALIAS DECLARATIONS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Gets the nested type `AdmissibleSystemPlugins` from `T`.
     * \ingroup config_type_traits
     *
     * \details
     * Provides a convenient shortcut to access the nested
     * `AdmissibleSystemPlugins` type definition from the given type `T`. The
     * following code example shows both ways to access the nested type
     * definition:
     *
     * \example
     * \code
     *   using Type1 = typename T::AdmissibleSystemPlugins;
     *   using Type2 = elastica::tt::admissible_system_plugins_t<T>;
     * \endcode
     */
    template <typename T>
    using admissible_system_plugins_t = typename T::AdmissibleSystemPlugins;
    //**************************************************************************

    //**************************************************************************
    /*!\brief Gets the nested type `BlockingPolicy` from `T`.
     * \ingroup config_type_traits
     *
     * \details
     * Provides a convenient shortcut to access the nested
     * `BlockingPolicy` type definition from the given type `T`. The
     * following code example shows both ways to access the nested type
     * definition:
     *
     * \example
     * \code
     *   using Type1 = typename T::BlockingPolicy;
     *   using Type2 = elastica::tt::blocking_policy_t<T>;
     * \endcode
     */
    template <typename T>
    using blocking_policy_t = typename T::BlockingPolicy;
    //**************************************************************************

    //**************************************************************************
    /*!\brief Gets the nested type `IterationPolicy` from `T`.
     * \ingroup config_type_traits
     *
     * \details
     * Provides a convenient shortcut to access the nested
     * `IterationPolicy` type definition from the given type `T`. The
     * following code example shows both ways to access the nested type
     * definition:
     *
     * \example
     * \code
     *   using Type1 = typename T::IterationPolicy;
     *   using Type2 = elastica::tt::iteration_policy_t<T>;
     * \endcode
     */
    template <typename T>
    using iteration_policy_t = typename T::IterationPolicy;
    //**************************************************************************

    //**************************************************************************
    /*!\brief Alias declaration for nested \c PrimitiveType type
    // definitions.
    // \ingroup config_type_traits
    //
    // The primitive_type_t alias declaration provides a convenient
    // shortcut to access the nested \a PrimitiveType type definition
    // of the given type \a T. The following code example shows both ways to
    // access the nested type definition:
    //
    // \example
       \code
         using Type1 = typename T::PrimitiveType;
         using Type2 = primitive_type_t<T>;
       \endcode
    */
    template <typename SystemTag>
    using primitive_type_t = typename SystemTag::PrimitiveType;
    //**************************************************************************

    //**************************************************************************
    /*!\brief Alias declaration for nested \c SystemType type definitions.
    // \ingroup config_type_traits
    //
    // The system_type_t alias declaration provides a convenient
    // shortcut to access the nested \a SystemType type definition
    // of the given type \a T. The following code example shows both ways to
    // access the nested type definition:
    //
    // \example
       \code
         using Type1 = typename T::SystemType;
         using Type2 = system_type_t<T>;
       \endcode
    */
    template <typename SystemTag>
    using system_type_t = typename SystemTag::SystemType;
    //**************************************************************************

  }  // namespace tt

}  // namespace elastica
