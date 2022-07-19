//==============================================================================
/*!
//  \file
//  \brief Functions for a Block view for Rigid bodys
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
  /*!\brief Get number of units within a RigidBody view
   * \ingroup rigid_body blocks
   *
   * \details
   * Get number of units from a Block. By 'units' we mean the number of
   * individual BlockSlice(s) composing the BlockView.
   *
   * \usage
   * \code
     BlockView<...> b;
     std::size_t n_units = blocks::n_units(b);
   * \endcode
   *
   * \param rigid_body The rigid body whose n_units is to be extracted
   */
  template <typename RBT,
            template <typename /*RBT*/, typename /*InitializedBlock*/>
            class... Components>
  inline auto n_units(
      BlockView<::elastica::rigid_body::RigidBodyPlugin<
          RBT, BlockView, Components...>> const& rigid_body) noexcept
      -> std::size_t {
    return rigid_body.n_units();
  }
  //****************************************************************************

  //****************************************************************************
  /*!\brief Returns the size of the Rigid body.
   * \ingroup rigid_body blocks
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
  /*!\brief Get number of units within a RigidBody view
   * \ingroup rigid_body blocks
   *
   * \details
   * Get number of units from a Block. By 'units' we mean the number of
   * individual BlockSlice(s) composing the ConstBlockView.
   *
   * \usage
   * \code
     ConstBlockView<...> b;
     std::size_t n_units = blocks::n_units(b);
   * \endcode
   *
   * \param rigid_body The rigid body whose n_units is to be extracted
   */
  template <typename RBT,
            template <typename /*RBT*/, typename /*InitializedBlock*/>
            class... Components>
  inline auto n_units(
      ConstBlockView<::elastica::rigid_body::RigidBodyPlugin<
          RBT, ConstBlockView, Components...>> const& rigid_body) noexcept
      -> std::size_t {
    return rigid_body.n_units();
  }
  //****************************************************************************

  //****************************************************************************
  /*!\brief Returns the size of the Rigid body.
   * \ingroup rigid_body blocks
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
