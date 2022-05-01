//==============================================================================
/*!
//  \file
//  \brief System-wide configuration of the collision system
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

//
#include "ModuleSettings/Collisions.hpp"
//
#include "Simulator/Environments/CollisionSystem/Configuration/DEMSolver.hpp"
#include "Simulator/Environments/CollisionSystem/Configuration/Default.hpp"

namespace elastica {

  namespace collision {

    //**************************************************************************
    // Type Definitions
    //**************************************************************************

    //**************************************************************************
    /*!\brief The configuration of the collision module in \elastica.
     * \ingroup collision_system
     *
     * The actual configuration is an instance of CollisionSystemConfiguration
     * for the current configuration set using \refitem config
     */
    using CollisionConfig = typename CollisionSystemConfiguration<
        // Type of the coarse collision detection algorithm
        ELASTICA_COARSE_COLLISION_DETECTOR,
        // Type of the fine collision detection algorithm
        ELASTICA_FINE_COLLISION_DETECTOR,
        // Type of the batch generation algorithm
        ELASTICA_BATCH_GENERATOR,
        // Type of the collision response algorithm
        ELASTICA_CONSTRAINT_SOLVER>::CollisionConfig;
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica
