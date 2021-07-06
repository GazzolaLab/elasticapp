//==============================================================================
/*!
//  \file
//  \brief Domain configuration type aliases
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
    /*!\brief Gets the nested type `DomainType` from `T`.
     * \ingroup config_type_traits
     *
     * \details
     * Provides a convenient shortcut to access the nested `DomainType` type
     * definition from the given type `T`. The following code example shows both
     * ways to access the nested type definition:
     *
     * \example
     * \code
     *   using Type1 = typename T::DomainType;
     *   using Type2 = elastica::tt::domain_type_t<T>;
     * \endcode
     */
    template <typename T>
    using domain_type_t = typename T::DomainType;
    //**************************************************************************

  }  // namespace tt

}  // namespace elastica
