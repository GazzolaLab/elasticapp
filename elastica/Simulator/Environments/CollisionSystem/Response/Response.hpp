//==============================================================================
/*!
//  \file
//  \brief Types in the response subsystem of the Collision system
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

#include "Simulator/Environments/CollisionSystem/Response/Types.hpp"
//
#include "Simulator/Environments/CollisionSystem/Response/DEMSolver.hpp"

//==============================================================================
//
//  DOXYGEN DOCUMENTATION
//
//==============================================================================

//******************************************************************************
/*!\defgroup collision_response Collision response
 * \ingroup collision_system
 * \brief Contains facilities for resolving collision between soft-bodies
 *
 * The collision response module processes the detected collisions by applying
 * physical forces and torques to the colliding rigid bodies
 */
//******************************************************************************

namespace elastica {

  namespace collision {

    //**************************************************************************
    /*!\brief Namespace for all collision response entities
    // \ingroup collision_response
    */
    //**************************************************************************
    namespace response {}  // namespace response

  }  // namespace collision

}  // namespace elastica
