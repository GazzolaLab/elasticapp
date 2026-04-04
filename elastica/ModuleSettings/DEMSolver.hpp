//==============================================================================
/*!
//  \file
//  \brief Module settings for the DEM / contact response solver
//
//  \ingroup collision_response
//
//  \details
//  Includes linear spring--dashpot DEM defaults from `Configuration/DEMSolver.hpp`.
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include "Simulator/Environments/CollisionSystem/Response/DEMForces/LinearSpringDashpot.hpp"

//==============================================================================
//
//  DEM SOLVER CONFIGURATION
//
//==============================================================================

namespace elastica {

  namespace collision {

#include "Configuration/DEMSolver.hpp"

  }

}  // namespace elastica
