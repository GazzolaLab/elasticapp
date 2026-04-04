//==============================================================================
/*!
//  \file
//  \brief Sphere in \elastica
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include "Systems/RigidBody/Types.hpp"
//
#include "Systems/RigidBody/Access.hpp"
#include "Systems/RigidBody/Block.hpp"
#include "Systems/RigidBody/BlockInitializer.hpp"
#include "Systems/RigidBody/BlockSlice.hpp"
#include "Systems/RigidBody/RigidBodyPlugin.hpp"
#include "Systems/RigidBody/RigidBodyTraits.hpp"
//
#include "Systems/RigidBody/Sphere/Sphere.hpp"

namespace elastica {

  namespace rigid_body {

    // TODO : document
    using Sphere = detail::Sphere;

  }  // namespace rigid_body

}  // namespace elastica