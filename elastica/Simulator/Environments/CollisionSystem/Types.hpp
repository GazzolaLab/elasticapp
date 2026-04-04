//==============================================================================
/*!
//  \file
//  \brief All types belonging to the collision system
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
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
      struct CollisionSystem;
    }
    /*! \endcond */
    //**************************************************************************

  }  // namespace environments

}  // namespace elastica