//==============================================================================
/*!
//  \file
//  \brief Tags for linear hyperElasticity component of Cosserat Rods
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

  //============================================================================
  //
  //  TAG DEFINITIONS
  //
  //============================================================================
  namespace tags {

    //**************************************************************************
    /*!\brief Tag indicating rigidities for the bending and twisting mode of
     * of a cosserat rod
     * \ingroup system_tags
     */
    struct BendingTwistRigidityMatrix {};
    //**************************************************************************

    //**************************************************************************
    /*!\brief Tag indicating rigidities for the shearing and stretching mode of
     * of a cosserat rod
     * \ingroup system_tags
     */
    struct ShearStretchRigidityMatrix {};
    //**************************************************************************

  }  // namespace tags

}  // namespace elastica
