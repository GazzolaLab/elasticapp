//==============================================================================
/*!
//  \file
//  \brief All types belonging to the collision system
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
#include "Simulator/Environments/CollisionSystem/Batches/Types.hpp"
#include "Simulator/Environments/CollisionSystem/Configuration/Types.hpp"
#include "Simulator/Environments/CollisionSystem/Contact/Types.hpp"
#include "Simulator/Environments/CollisionSystem/Detection/Types.hpp"
#include "Simulator/Environments/CollisionSystem/Response/Types.hpp"

namespace elastica {

  namespace collision {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <class SimulatorConfig, class CollisionConfig>
    class CollisionSystem;
    /*! \endcond */
    //**************************************************************************

  }  // namespace collision

  namespace environments {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    namespace tags {
      struct CollisionSystemTag;
    }
    /*! \endcond */
    //**************************************************************************

  }  // namespace environments

}  // namespace elastica