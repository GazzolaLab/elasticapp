#pragma once

//******************************************************************************
// Forward Declarations
//******************************************************************************
namespace elastica {
  namespace collision {}
}  // namespace elastica

// clang-format off
// Needs to come first to define types for the other headers
// TODO Can be rectified by directly embedding types in the respsective headers
#include "elastica/ModuleSettings/Collisions.hpp"
#include "Simulator/Environments/CollisionSystem/Configuration/Default.hpp"
#include "Simulator/Environments/CollisionSystem/Configuration/DEMSolver.hpp"
// clang-format on

namespace elastica {

  namespace collision {

    //*************************************************************************************************
    // Type Definitions
    //*************************************************************************************************

    //*************************************************************************************************
    /*!\brief The configuration of the collision module for the physics engine.
     * \ingroup core
     *
     * The actual configuration is the concrete instance of the
     * CollisionSystemConfiguration class for the current configuration of the
     * engine.
     */
    using CollisionConfig = typename CollisionSystemConfiguration<
        // Type of the coarse collision detection
        // algorithm
        ELASTICA_COARSE_COLLISION_DETECTOR,
        // Type of the fine collision detection
        // algorithm
        ELASTICA_FINE_COLLISION_DETECTOR,
        // Type of the batch generation algorithm
        ELASTICA_BATCH_GENERATOR,
        // Type of the collision response algorithm
        ELASTICA_CONSTRAINT_SOLVER>::CollisionConfig;  // Type of the active
                                                       // configuration
    //*************************************************************************************************

  }  // namespace collision

}  // namespace elastica
