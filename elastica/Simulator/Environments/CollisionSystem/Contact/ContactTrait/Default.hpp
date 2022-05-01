//==============================================================================
/*!
//  \file
//  \brief Default implementation of the ContactTrait class template
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
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
**/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
//
#include "Simulator/Environments/CollisionSystem/Contact/Types.hpp"
///
#include "Simulator/Environments/CollisionSystem/Contact/ContactBase.hpp"
//
#include "Simulator/Environments/Primitives/GeomPrimitive.hpp"  // geomid
//
#include "Utilities/DefineTypes.h"
#include "Utilities/Math/Vec3.hpp"

namespace elastica {

  namespace collision {

    namespace detail {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Contact customization class for the collision response.
       * \ingroup contact
       *
       * The ContactTrait class template is a customization class for contacts
       * in general. Its main purpose is the customization of the Contact class
       * for the selected collision response algorithm (see
       * pe::pe_CONSTRAINT_SOLVER).\n Depending on the used algorithm, a contact
       * may require additional data or functionality to efficiently support the
       * collision response calculations. In order to add this specific
       * functionality or data and to adapt contacts to a particular algorithm,
       * the base template needs to be specialized.
       *
       * \tparam C Type of the collision system configuration
       *
       * \note
       * Empty base optimization applies here for the Default Configuration
       */
      template <typename C>
      class ContactTrait : public ContactBase {
       protected:
        //**Type definitions****************************************************
        using Parent = ContactBase;  //!< The type of the parent class.
        //**********************************************************************

        //**Constructor*********************************************************
        /*!\name Constructor */
        //@{
        using Parent::Parent;
        ContactTrait(ContactTrait&&) noexcept = default;
        //@}
        //**********************************************************************

        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{
        inline ~ContactTrait() = default;
        //@}
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace detail

  }  // namespace collision

}  // namespace elastica
