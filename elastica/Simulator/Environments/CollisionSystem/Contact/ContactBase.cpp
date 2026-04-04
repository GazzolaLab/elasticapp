//==============================================================================
/*!
//  \file
//  \brief Source file for ContactBase
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
**/
//==============================================================================

//******************************************************************************
// Includes
//******************************************************************************

#include "ContactBase.hpp"

namespace elastica {

  namespace collision {

    //==========================================================================
    //
    //  DEFINITION AND INITIALIZATION OF THE STATIC MEMBER VARIABLES
    //
    //==========================================================================

    // id_t ContactBase::nextContact_(0);

    //==========================================================================
    //
    //  GET FUNCTIONS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Calculation of the normal relative acceleration between the two
     * contacting rigid bodies.
     *
     * \return The relative acceleration in normal direction.
     */
    real_t ContactBase::get_normal_relative_acceleration() const {
      return trans(normal_) * (b1_->acceleration_from_world_FOR(gpos_) -
                               b2_->acceleration_from_world_FOR(gpos_)) +
             real_t(2.0) * get_normal_relative_velocity();
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Calculation of the normal relative force between the two
     * contacting rigid bodies.
     *
     * \return The relative force in normal direction.
     */
    real_t ContactBase::get_normal_relative_force() const {
      Vec3 force;

      if (!b1_->is_fixed())
        force += b1_->get_force();
      if (!b2_->is_fixed())
        force -= b2_->get_force();

      return dot(normal_, force);
    }
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica
