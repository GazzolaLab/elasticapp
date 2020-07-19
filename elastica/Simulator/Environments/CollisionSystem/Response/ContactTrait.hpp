//=================================================================================================
/*!
 *  \file pe/core/response/ContactTrait.h
 *  \brief Header file for the ContactTrait class
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

//*************************************************************************************************
// Includes
//*************************************************************************************************

#include "Simulator/Environments/Primitives/GeomPrimitive.hpp"
#include "Simulator/Environments/CollisionSystem/Types.hpp"
#include "elastica/Utilities/Math/Vec3.hpp"

namespace elastica {

  namespace collision {

    namespace response {

      //=================================================================================================
      //
      //  CLASS DEFINITION
      //
      //=================================================================================================

      //*************************************************************************************************
      /*!\brief Contact customization class for the collision response.
       * \ingroup collision_response
       *
       * The ContactTrait class template is used to adapt the Contact class to
       * the used collision response algorithm. Depending on the used algorithm,
       * a contact may require additional data or functionality to efficiently
       * support the collision response calculations.\n In order to adapt the
       * Contact class to a particular algorithm, the base template needs to be
       * specialized.
       */
      template <typename C>  // Type of the configuration
      class ContactTrait {
       public:
        //**Constructor*********************************************************************************
        /*!\name Constructor */
        //@{
        explicit inline ContactTrait(GeomID g1, GeomID g2, const Vec3& gpos,
                                     const Vec3& normal);
        //@}
        //**********************************************************************************************
      };
      //*************************************************************************************************

      //*************************************************************************************************
      /*!\brief Default implementation of the ContactTrait constructor.
       *
       * \param g1 The first contacting geometric primitive.
       * \param g2 The second contacting geometric primitive.
       * \param gpos The global position of the contact.
       * \param normal The global normal of the contact (running from body 2 to
       * body 1). \return void
       *
       * Default implementation of the ContactTrait constructor.
       */
      template <typename C>  // Type of the configuration
      inline ContactTrait<C>::ContactTrait(GeomID /*g1*/, GeomID /*g2*/,
                                           const Vec3& /*gpos*/,
                                           const Vec3& /*normal*/) {}
      //*************************************************************************************************

      // NOTE :
      // For more ContactTrait implementations for different solvers,
      // see pe's implementation

    }  // namespace response

  }  // namespace collision

}  // namespace elastica
