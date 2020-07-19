//=================================================================================================
/*!
 *  \file src/core/rigidbody/GeomPrimitive.cpp
 *  \brief Source file for the GeomPrimitive class
 *
 *  Copyright (C) 2009 Klaus Iglberger
 *                2012 Tobias Preclik
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

//*************************************************************************************************
// Includes
//*************************************************************************************************

#include "GeomPrimitive.hpp"

namespace elastica {

  namespace collision {

    //=================================================================================================
    //
    //  CONSTRUCTORS
    //
    //=================================================================================================

    //*************************************************************************************************
    /*!\brief Constructor for the RigidBody class.
     *
     * \param type The geometry type of the geometric primitive.
     * \param finite Specifies if the geometric primitive is finite or not.
     * \param visible Specifies if the geometric primitive is visible or not.
     * \param sid The unique system-specific ID of the geometric primitive.
     * \param uid The user-specific ID of the geometric primitive.
     * \param material The material of the geometric primitive.
     */
    GeomPrimitive::GeomPrimitive(bool finite, id_t sid, id_t uid,
                                 MaterialID material)
        : RigidBodyPrimitive(this, finite, sid,
                             uid),  // Initializing the base object
          material_(material)       // The material of the geometric primitive
    {}
    //*************************************************************************************************

    // Put it in an invalid state to detect errors
    GeomPrimitive::GeomPrimitive(GeomPrimitive&& other) noexcept
        : RigidBodyPrimitive(this, false, std::numeric_limits<id_t>::max(),
                             std::numeric_limits<id_t>::max()),
          material_(-1) {}

    //=================================================================================================
    //
    //  DESTRUCTOR
    //
    //=================================================================================================

    //*************************************************************************************************
    /*!\brief Destructor for the Primitive class.
     */
    GeomPrimitive::~GeomPrimitive() {}
    //*************************************************************************************************

  }  // namespace collision

}  // namespace elastica
