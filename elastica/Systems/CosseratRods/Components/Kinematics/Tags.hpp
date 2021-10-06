//==============================================================================
/*!
//  \file
//  \brief Tags belonging to the Kinematics component of Cosserat Rods
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include "Systems/common/Tags.hpp" // velocity and angular velocity

namespace elastica{
  namespace tags{

    //==========================================================================
    //
    //  TAG DEFINITIONS
    //
    //==========================================================================

    // Scalar

    //**************************************************************************
    /*!\brief Tag indicating mass and density
     * \ingroup systems
     */
    struct Mass;
    struct Density;

    struct ExternalLoads;
    struct ExternalTorques;

  }
}
