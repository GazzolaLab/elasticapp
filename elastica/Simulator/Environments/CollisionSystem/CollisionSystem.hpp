//==============================================================================
/*!
//  \file
//  \brief Inclusion of all collision components of \elastica
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
#include "Simulator/Environments/CollisionSystem/Types.hpp"

//==============================================================================
//
//  DOXYGEN DOCUMENTATION
//
//==============================================================================

//******************************************************************************
/*!\defgroup collision_system CollisionSystem
 * \ingroup environments
 * \brief Contains an engine to detect collisions between bodies efficiently and
 * apply forces to resolve them.
 */
//******************************************************************************

namespace elastica {

  //****************************************************************************
  /*!\brief Collision system data-structures and subroutines
  // \ingroup collision_system
  */
  namespace collision {}
  //****************************************************************************

}  // namespace elastica

namespace elastica {

  namespace collision {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Collision system of the \elastica simulation world.
     * \ingroup collision_system
     *
     * The CollisionSystem class represents the collision system of the rigid
     * body simulation world, which is a combination of all collision related
     * algorithms. The collision system provides all functionality for the
     * coarse collision detection (see \ref ELASTICA_COARSE_COLLISION_DETECTOR),
     * the fine collision detection (see \ref ELASTICA_FINE_COLLISION_DETECTOR),
     * the batch generation process (see \ref ELASTICA_BATCH_GENERATOR), and the
     * collision response (see \ref ELASTICA_CONSTRAINT_SOLVER).
     *
     * The CollisionSystem class is implemented as an elastica::Environment and
     * uses its primitive and contact storage. The CollisionSystem needs to be
     * instantiated with two template template parameters, one from the
     * simulator, and one containing the configuration of the collision detector
     *
     * \tparam SimulatorConfig_ Configuration of \elastica simulation
     * \tparam CollisionConfig_ Configuration specific to collision system
     */
    template <class SimulatorConfig_, class CollisionConfig_>
    class CollisionSystem;
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica

//******************************************************************************
// Post-includes
//******************************************************************************
#include "elastica/Simulator/Environments/CollisionSystem/DEMSolver.hpp"