//==============================================================================
/*!
//  \file
//  \brief Rigid bodies in \elastica
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
/// include aliased classes now
/// Developer note:
/// These can be instantiated in C++ files to massively speed up compilation,
/// but we don't do that here to give full flexibility to the user for
/// initializing it. In our applications, we can do an instantiation if needed,
/// according to the initializers we use.
///
#include "Systems/RigidBody/Aliases.hpp"

//==============================================================================
//
//  DOXYGEN DOCUMENTATION
//
//==============================================================================

//******************************************************************************
/*!\defgroup rigid_body RigidBody
 * \ingroup systems
 *
 * The RigidBody rods module implements data and operations for manipulating
 * rigid bodies, in the style of @ref blocks within \elastica.
 */
//******************************************************************************

namespace elastica {

  //****************************************************************************
  /*!\brief RigidBody data-structures and routines
  // \ingroup rigid_body
  */
  namespace rigid_body {}
  //****************************************************************************

}  // namespace elastica
