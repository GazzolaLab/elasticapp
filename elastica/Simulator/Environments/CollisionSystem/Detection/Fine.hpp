//==============================================================================
/*!
//  \file
//  \brief Inclusion of all fine collision detection components
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

///
#include "Simulator/Environments/CollisionSystem/Detection/Fine/Types.hpp"
///
#include "Simulator/Environments/CollisionSystem/Detection/Fine/Detectors.hpp"

//==============================================================================
//
//  DOXYGEN DOCUMENTATION
//
//==============================================================================

//******************************************************************************
/*!\defgroup fine_collision_detection Fine collision detection
 * \ingroup collision_system
 * \brief Contains facilities for performing collision checks between bodies
 *
 * The Fine collision detection module contains functionality to detect contacts
 * based on the potentially colliding rigid primitives
 */
//******************************************************************************

namespace elastica {

  namespace collision {

    namespace detection {

      //************************************************************************
      /*!\brief Namespace for fine detection
      // \ingroup fine_collision_detection
      */
      namespace fine {}
      //************************************************************************

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica
