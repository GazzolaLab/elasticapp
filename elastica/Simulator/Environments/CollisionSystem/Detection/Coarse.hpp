//==============================================================================
/*!
//  \file
//  \brief Inclusion of all coarse collision detection components
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

///
#include "Simulator/Environments/CollisionSystem/Detection/Coarse/Types.hpp"
///
#include "Simulator/Environments/CollisionSystem/Detection/Coarse/Detectors.hpp"

//==============================================================================
//
//  DOXYGEN DOCUMENTATION
//
//==============================================================================

//******************************************************************************
/*!\defgroup coarse_collision_detection Coarse collision detection
 * \ingroup collision_system
 * \brief Contains facilities for culling collision checks between bodies
 *
 * The Coarse collision detection module contains functionality to cull
 * potentially colliding rigid primitives before checking for detailed contacts
 */
//******************************************************************************

namespace elastica {

  namespace collision {

    namespace detection {

      //************************************************************************
      /*!\brief Namespace for coarse detection
      // \ingroup coarse_collision_detection
      */
      namespace coarse {}
      //************************************************************************

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica
