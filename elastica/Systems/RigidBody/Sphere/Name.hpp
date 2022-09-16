//==============================================================================
/*!
//  \file
//  \brief Sphere name
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

#include "Systems/common/Components/NameAdapter.hpp"
//
#include <string>

namespace elastica {

  namespace rigid_body {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Provides a name to Sphere
     * \ingroup rigid_body_component
     */
    struct SphereName {
     public:
      //************************************************************************
      /*!\brief Human-readable name of the current plugin and all derivates
       *
       * \note This is intended to work with pretty_type::name<>
       */
      static std::string name() { return "Sphere"; }
      //************************************************************************
    };
    //**************************************************************************

    template <typename Traits, typename Block>
    using SphereNameAdapted =
        ::elastica::detail::NameAdapter<Traits, Block, SphereName>;

  }  // namespace rigid_body

}  // namespace elastica