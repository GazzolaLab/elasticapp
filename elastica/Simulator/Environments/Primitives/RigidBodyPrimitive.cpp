//==============================================================================
/*!
//  \file
//  \brief Source file for the RigidBodyPrimitive
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
*/
//==============================================================================

//******************************************************************************
// Includes
//******************************************************************************

#include "Simulator/Environments/Primitives/RigidBodyPrimitive.hpp"

#include "Utilities/MakeSignalingNan.hpp"
#include "Utilities/Math/Identity.hpp"

namespace elastica {

  namespace collision {

    //==========================================================================
    //
    //  CONSTRUCTORS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Constructor for the RigidBodyPrimitive class.
     *
     * \param body The body ID of this rigid body.
     */
    RigidBodyPrimitive::RigidBodyPrimitive(BodyID body, bool finite, id_t sid,
                                           id_t uid)
        : CCDBT(body),  // Initialization of the coarse collision detection body
                        // trait base class
          FCDBT(),  // Initialization of the fine collision detection body trait
                    // base class
          BGBT(),   // Initialization of the batch generation body trait base
                    // class
          CRBT(),   // Initialization of the collision response body trait base
                    // class
          fixed_(false),
#ifdef ELASTICA_DEBUG
          mass_(make_signaling_NaN<real_t>()),  // Total mass of the rigid body
          gpos_(make_signaling_NaN<real_t>()),  // Global position of the center
                                                // of mass
          //          Q_(make_signaling_NaN<real_t>()),  // Rotation matrix
          Q_(elastica::identity3()),         // Rotation matrix
          v_(make_signaling_NaN<real_t>()),  // Linear velocity
          w_(make_signaling_NaN<real_t>()),  // Angular velocity
          // Forces and torques are always transferred to the final rigid body
          // so make sure they aren't nan-initialized
          force_(real_t(0.0)),   // Total force
          torque_(real_t(0.0)),  // Total torque
#else
          mass_(0),   // Total mass of the rigid body
          gpos_(),    // Global position of the center of mass
          Q_(),       // Rotation matrix
          v_(),       // Linear velocity
          w_(),       // Angular velocity
          force_(),   // Total force
          torque_(),  // Total torque
#endif
          finite_(finite),
          sid_(sid),
          uid_(uid) {
    }

    //==========================================================================
    //
    //  DESTRUCTOR
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Destructor for the RigidBodyBase class.
     */
    RigidBodyPrimitive::~RigidBodyPrimitive() = default;
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica
