//==============================================================================
/*!
//  \file
//  \brief Functions for a Block view for Rigid bodys
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

#include "ErrorHandling/ExpectsAndEnsures.hpp"
/// Forward declarations
#include "Systems/RigidBody/Types.hpp"
///
#include "Systems/Block/Block.hpp"
#include "Systems/RigidBody/RigidBodyPlugin.hpp"
#include "Utilities/End.hpp"  // from_end

namespace blocks {

  //============================================================================
  //
  //  FREE FUNCTIONS
  //
  //============================================================================

  //****************************************************************************
  /*!\brief Returns the size of the Rigid body.
   * \ingroup blocks
   *
   * \param rigid_body The current rigid body.
   * \return The  number of elements in the rigid body.
   */
  template <typename RBT,
            template <typename /*RBT*/, typename /*InitializedBlock*/>
            class... Components>
  inline constexpr auto size(
      BlockView<::elastica::rigid_body::RigidBodyPlugin<
          RBT, BlockView, Components...>> const& rigid_body) noexcept {
    // it might be not the best idea to have a size() member
    return rigid_body.size();
  }
  //****************************************************************************

  //****************************************************************************
  /*!\brief Returns the size of the Rigid body.
   * \ingroup blocks
   *
   * \param rigid_body The current rigid body.
   * \return The  number of elements in the rigid body.
   */
  template <typename RBT,
            template <typename /*RBT*/, typename /*InitializedBlock*/>
            class... Components>
  inline constexpr auto size(
      ConstBlockView<::elastica::rigid_body::RigidBodyPlugin<
          RBT, ConstBlockView, Components...>> const& rigid_body) noexcept {
    // it might be not the best idea to have a size() member
    return rigid_body.size();
  }
  //****************************************************************************

}  // namespace blocks
