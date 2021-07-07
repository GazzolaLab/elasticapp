//==============================================================================
/*!
//  \file
//  \brief Helper types belonging to the Geometry component of Cosserat Rods
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
#include "Systems/CosseratRods/Components/Geometry/detail/Tags.hpp"

namespace elastica {

  namespace cosserat_rod {

    namespace component {

      namespace detail {

        ////////////////////////////////////////////////////////////////////////
        //
        // Forward declarations of helper geometry components
        //
        ////////////////////////////////////////////////////////////////////////

        //**********************************************************************
        /*! \cond ELASTICA_INTERNAL */
        template <typename /* Rod Traits */, typename /*Block*/>
        class RodSpanwiseGeometry;

        template <typename /* Rod Traits */, typename /*Block*/>
        class CosseratRodSpanwiseGeometry;

        template <typename /* Rod Traits */, typename /* Block */,
                  // template <typename /* Rod Traits */, typename /* Block */>
                  class /*Cross Section Implementation*/>
        class CosseratRodCrossSectionInterface;
        /*! \endcond */
        //**********************************************************************

      }  // namespace detail

    }  // namespace component

  }  // namespace cosserat_rod

}  // namespace elastica
