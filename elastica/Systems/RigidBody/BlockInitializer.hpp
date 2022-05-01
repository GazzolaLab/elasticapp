//==============================================================================
/*!
//  \file
//  \brief Block initializer functions for Rigid bodys
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
  /*!\brief Returns the size of a (yet to be) Cosserat Rod from the
   * initializer \ingroup cosserat_rod
   *
   * \param initializer The current cosserat rod initializer
   * \return The  number of elements in the cosserat rod.
   */
  template <typename RBT,
            template <typename /*RBT*/, typename /*InitializedBlock*/>
            class... Components,
            class... Initializers>
  inline constexpr auto size(
      BlockInitializer<
          elastica::rigid_body::RigidBodyPlugin<RBT, Block, Components...>,
          Initializers...> const&) noexcept {
    return 1UL;
  }
  //****************************************************************************

}  // namespace blocks