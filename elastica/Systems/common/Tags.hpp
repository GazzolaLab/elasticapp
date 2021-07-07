//==============================================================================
/*!
//  \file
//  \brief Common accessor tags for all \elastica systems
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

namespace elastica {

  namespace tags {

    //==========================================================================
    //
    //  TAG DEFINITIONS
    //
    //==========================================================================

    // se3 group

    //**************************************************************************
    /*!\brief Tag indicating element positions
     * \ingroup systems
     */
    struct Position;
    //**************************************************************************

    //**************************************************************************
    /*!\brief Tag indicating velocity
     * \ingroup systems
     */
    struct Velocity;
    //**************************************************************************

    //**************************************************************************
    /*!\brief Tag indicating translational acceleration
     * \ingroup systems
     */
    struct Acceleration;
    //**************************************************************************

    // so3 group

    //**************************************************************************
    /*!\brief Tag indicating element orientations (director)
     * \ingroup systems
     */
    struct Director;
    //**************************************************************************

    //**************************************************************************
    /*!\brief Tag indicating angular velocity
     * \ingroup systems
     */
    struct AngularVelocity;
    //**************************************************************************

    //**************************************************************************
    /*!\brief Tag indicating angular accelerations
     * \ingroup systems
     */
    struct AngularAcceleration;
    //**************************************************************************

    // D1, D2, D3
    // x, y, z
    // accessor

  }  // namespace tags

}  // namespace elastica
