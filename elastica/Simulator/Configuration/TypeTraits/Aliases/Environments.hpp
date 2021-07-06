//==============================================================================
/*!
//  \file
//  \brief Environment configuration type aliases
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
    /*!\brief Gets the nested type `AdmissibleEnvironmentTags` from `T`.
     * \ingroup config_type_traits
     *
     * \details
     * Provides a convenient shortcut to access the nested
     * `AdmissibleEnvironmentTags` type definition of the given type `T`. The
     * following code example shows both ways to access the nested type
     * definition:
     *
     * \example
     * \code
     *   using Type1 = typename T::AdmissibleEnvironmentTags;
     *   using Type2 = elastica::tt::admissible_environment_tags_t<T>;
     * \endcode
     */
    template <typename T>
    using admissible_environment_tags_t = typename T::AdmissibleEnvironmentTags;
    //**************************************************************************

    //**************************************************************************
    /*!\brief Gets the nested type `AdmissibleImposedPrimitives` from `T`.
     * \ingroup config_type_traits
     *
     * Provides a convenient shortcut to access the nested
     * `AdmissibleImposedPrimitives` type definition of the given type `T`. The
     * following code example shows both ways to access the nested type
     * definition:
     *
     * \example
     * \code
     *   using Type1 = typename T::AdmissibleImposedPrimitives;
     *   using Type2 = elastica::tt::admissible_imposed_primitives_t<T>;
     * \endcode
     */
    template <typename T>
    using admissible_imposed_primitives_t =
        typename T::AdmissibleImposedPrimitives;
    //**************************************************************************

  }  // namespace tt

}  // namespace elastica
