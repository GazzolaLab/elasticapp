//==============================================================================
/*!
//  \file
//  \brief Contact specialization for coarse-collision detection
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
//
//  This file contains implementation from the pe library with the following
//  copyrights
**
**  Project home: https://www.cs10.tf.fau.de/research/software/pe/
**
**  Copyright (C) 2009 Klaus Iglberger
**
**  This file is part of pe.
**
**  pe is free software: you can redistribute it and/or modify it under the
** terms of the GNU General Public License as published by the Free Software
** Foundation, either version 3 of the License, or (at your option) any later
** version.
**
**  pe is distributed in the hope that it will be useful, but WITHOUT ANY
** WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
** A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License along with
** pe. If not, see <http://www.gnu.org/licenses/>.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include "Simulator/Environments/CollisionSystem/Detection/Coarse/Types.hpp"

namespace elastica {

  namespace collision {

    namespace detection {

      namespace coarse {

        //======================================================================
        //
        //  CLASS DEFINITION
        //
        //======================================================================

        //**********************************************************************
        /*!\brief Contact customization class for the coarse collision
         * detection.
         * \ingroup coarse_collision_detection
         *
         * The coarse collision detection ContactTrait class template is used to
         * adapt the Contact class to the used coarse collision detection
         * algorithm. Depending on the algorithm, a contact requires
         * additional data or functionality to efficiently support the coarse
         * collision detection process.
         *
         * In order to adapt the Contact class to a particular algorithm, this
         * base template needs to be specialized.
         *
         * \tparam CollisionConfig Type of the collision configuration
         */
        template <typename CollisionConfig>
        class ContactTrait {};
        //**********************************************************************

      }  // namespace coarse

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica
