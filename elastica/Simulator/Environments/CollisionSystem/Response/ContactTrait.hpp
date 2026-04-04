//==============================================================================
/*!
//  \file
//  \brief Contact specialization for contact response
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

#include "Simulator/Environments/CollisionSystem/Response/Types.hpp"
#include "Simulator/Environments/Primitives/Types.hpp"
#include "Utilities/Math/Vec3.hpp"

namespace elastica {

  namespace collision {

    namespace response {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Contact customization class for the collision response.
       * \ingroup collision_response
       *
       * The ContactTrait class template is used to adapt the Contact class to
       * the used collision response algorithm. Depending on the used algorithm,
       * a contact may require additional data or functionality to efficiently
       * support the collision response calculations.
       *
       * In order to adapt the Contact class to a particular algorithm, the base
       * template needs to be specialized.
       */
      template <typename C>  // Type of the configuration
      class ContactTrait {
       public:
        //**Constructor*********************************************************
        /*!\name Constructor */
        //@{
        explicit inline ContactTrait(GeomID g1, GeomID g2, const Vec3& gpos,
                                     const Vec3& normal);
        //@}
        //**********************************************************************
      };
      //************************************************************************

      //************************************************************************
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
      //************************************************************************

    }  // namespace response

  }  // namespace collision

}  // namespace elastica
