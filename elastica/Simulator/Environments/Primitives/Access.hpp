//==============================================================================
/*!
//  \file
//  \brief Access API for RigidBodyPrimitives
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
#include "Simulator/Environments/Primitives/Types.hpp"
//
#include "Simulator/Environments/Primitives/RigidBodyPrimitive.hpp"

namespace elastica {

  //============================================================================
  //
  //  ACCESS API FUNCTIONS
  //
  //============================================================================

  //****************************************************************************
  /*!\brief Retrieves position of a primitive
   * \ingroup primitives
   */
  inline /*constexpr*/ decltype(auto) position(
      collision::RigidBodyPrimitive const& primitive) noexcept {
    return primitive.get_position();
  }
  //****************************************************************************

  //****************************************************************************
  /*!\brief Retrieves director of a primitive
   * \ingroup primitives
   */
  inline /*constexpr*/ decltype(auto) director(
      collision::RigidBodyPrimitive const& primitive) noexcept {
    return primitive.get_rotation_matrix();
  }
  //****************************************************************************

  //****************************************************************************
  /*!\brief Retrieves velocity of a primitive
   * \ingroup primitives
   */
  inline /*constexpr*/ decltype(auto) velocity(
      collision::RigidBodyPrimitive const& primitive) noexcept {
    return primitive.get_velocity();
  }
  //****************************************************************************

  //****************************************************************************
  /*!\brief Retrieves angular velocity of a primitive
   * \ingroup primitives
   */
  inline /*constexpr*/ decltype(auto) angular_velocity(
      collision::RigidBodyPrimitive const& primitive) noexcept {
    return primitive.get_angular_velocity();
  }
  //****************************************************************************

  //****************************************************************************
  /*!\brief Retrieves external loads on a primitive
   * \ingroup primitives
   */
  inline /*constexpr*/ decltype(auto) external_loads(
      collision::RigidBodyPrimitive const& primitive) noexcept {
    return primitive.get_force();
  }
  //****************************************************************************

  //****************************************************************************
  /*!\brief Retrieves external torques on a primitive
   * \ingroup primitives
   */
  inline /*constexpr*/ decltype(auto) external_torques(
      collision::RigidBodyPrimitive const& primitive) noexcept {
    return primitive.get_torque();
  }
  //****************************************************************************

}  // namespace elastica
