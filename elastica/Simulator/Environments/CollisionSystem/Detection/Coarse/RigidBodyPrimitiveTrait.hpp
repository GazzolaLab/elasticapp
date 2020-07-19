//=================================================================================================
/*!
 *  \file pe/core/detection/coarse/RigidBodyPrimitiveTrait.h
 *  \brief Rigid body customization class for the coarse collision detection
 *
 *  Copyright (C) 2009 Klaus Iglberger
 *
 *  This file is part of pe.
 *
 *  pe is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 *  pe is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with
 * pe. If not, see <http://www.gnu.org/licenses/>.
 */
//=================================================================================================

#pragma once

#include "BoundingBox.hpp"
#include "Simulator/Environments/CollisionSystem/Types.hpp"
#include "elastica/Utilities/DefineTypes.h"

namespace elastica {

  namespace collision {

    namespace detection {

      namespace coarse {

        //=================================================================================================
        //
        //  COARSE COLLISION DETECTION BODY TRAIT
        //
        //=================================================================================================

        //*************************************************************************************************
        /*!\brief Rigid body customization class for the coarse collision
         * detection. \ingroup coarse_collision_detection
         *
         * The coarse detection module RigidBodyPrimitiveTrait class template is
         * used to adapt the RigidBody class to the used coarse collision
         * detection algorithm. For instance, this class configures the
         * axis-aligned bounding box for rigid bodies depending on the
         * requirements of the selected coarse collision detection algorithm.\n
         * In order to adapt the RigidBody class to a particular algorithm, the
         * base template needs to be specialized.
         */
        template <typename C>  // Type of the configuration
        class RigidBodyPrimitiveTrait {
         public:
          //**Type
          // definitions****************************************************************************
          using AABB =
              BoundingBox<real_t>;  //!< Type of the axis-aligned bounding box.
          //**********************************************************************************************

         protected:
          //**Constructor*********************************************************************************
          /*!\name Constructor */
          //@{
          explicit inline RigidBodyPrimitiveTrait(BodyID body);
          //@}
          //**********************************************************************************************

          //**Member
          // variables****************************************************************************
          /*!\name Member variables */
          //@{
          AABB aabb_;  //!< Axis-aligned bounding box for the rigid body.
                       //@}
          //**********************************************************************************************
        };
        //*************************************************************************************************

        //*************************************************************************************************
        /*!\brief Default implementation of the RigidBodyPrimitiveTrait
         * constructor.
         *
         * \param body The rigid body containing this bounding box.
         */
        template <typename C>  // Type of the configuration
        inline RigidBodyPrimitiveTrait<C>::RigidBodyPrimitiveTrait(
            BodyID /*body*/)
            : aabb_()  // Axis-aligned bounding box for the rigid body
        {}
        //*************************************************************************************************

      }  // namespace coarse

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica
