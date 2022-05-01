//==============================================================================
/*!
//  \file
//  \brief Types in \elastica collision configuration
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

namespace elastica {

  namespace collision {
    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <
        // Type of the coarse collision detection algorithm
        template <typename> class CD,
        // Type of the fine collision detection algorithm
        typename FD,
        // Type of the batch generation algorithm
        template <typename> class BG,
        // Type of the collision response algorithm
        template <typename, typename, typename> class CR>
    struct CollisionSystemConfiguration;
    /*! \endcond */
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica