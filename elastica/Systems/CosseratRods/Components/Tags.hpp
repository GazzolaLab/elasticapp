//==============================================================================
/*!
//  \file
//  \brief Tags belonging to @ref cosserat_rod_component of Cosserat Rods
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
#include "Systems/CosseratRods/Components/Elasticity/Tags.hpp"
#include "Systems/CosseratRods/Components/Geometry/Tags.hpp"
#include "Systems/CosseratRods/Components/Kinematics/Tags.hpp"

namespace elastica {

  namespace tags {

    // Total number of elements within a Cosserat rod
    struct NElement{};

  }  // namespace tags

}  // namespace elastica
