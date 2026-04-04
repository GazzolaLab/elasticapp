//==============================================================================
/*!
//  \file
//  \brief Block initializer functions for Rigid bodys
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
/// Forward declarations
#include "Systems/RigidBody/Types.hpp"
///
#include "Systems/Block.hpp"
#include "Systems/RigidBody/RigidBodyPlugin.hpp"
#include "Utilities/End.hpp"  // from_end

namespace blocks {

  //==========================================================================
  //
  //  FREE FUNCTIONS
  //
  //==========================================================================

  //**************************************************************************
  /*!\brief Returns the logical size of a rigid-body block initializer.
   * \ingroup rigid_body
   *
   * \param block_initializer The rigid-body block initializer (size is 1).
   * \return The number of elements (always \c 1 for rigid bodies).
   */
  template <typename RBT,
            template <typename /*RBT*/, typename /*InitializedBlock*/>
            class... Components,
            class... Initializers>
  inline constexpr auto size(
      BlockInitializer<
          elastica::rigid_body::RigidBodyPlugin<RBT, Block, Components...>,
          Initializers...> const& block_initializer) noexcept {
    (void)block_initializer;
    return 1UL;
  }
  //****************************************************************************

}  // namespace blocks