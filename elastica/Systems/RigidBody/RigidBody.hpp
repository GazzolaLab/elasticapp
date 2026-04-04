//==============================================================================
/*!
//  \file
//  \brief Rigid bodies in \elastica
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
////
//#include "Systems/RigidBody/Protocols.hpp"
#include "Systems/RigidBody/Tags.hpp"
//#include "Systems/RigidBody/TypeTraits.hpp"
#include "Systems/RigidBody/Types.hpp"
////
//// include implementations now
#include "Systems/RigidBody/Block.hpp"
#include "Systems/RigidBody/BlockInitializer.hpp"
#include "Systems/RigidBody/BlockSlice.hpp"
#include "Systems/RigidBody/Components.hpp"
#include "Systems/RigidBody/RigidBodyPlugin.hpp"
//#include "Systems/RigidBody/Initializers.hpp"
///
// include aliased classes now
// Developer note:
// These can be instantiated in C++ files to massively speed up compilation,
// but we don't do that here to give full flexibility to the user for
// initializing it. In our applications, we can do an instantiation if needed,
// according to the initializers we use.
///
#include "Systems/RigidBody/Aliases.hpp"

//==============================================================================
//
//  DOXYGEN DOCUMENTATION
//
//==============================================================================
//
//  Module group rigid_body is defined in docs/DoxygenGroups.hpp so
//  @ingroup rigid_body resolves in all rigid-body headers.

namespace elastica {

  //****************************************************************************
  /*!\brief RigidBody data-structures and routines
  // \ingroup rigid_body
  */
  namespace rigid_body {}
  //****************************************************************************

}  // namespace elastica
