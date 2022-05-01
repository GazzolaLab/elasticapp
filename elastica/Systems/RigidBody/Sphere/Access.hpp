//==============================================================================
/*!
//  \file
//  \brief Access to default "Sphere" members in \elastica
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

#include "Systems/RigidBody/BlockSlice.hpp"
#include "Systems/RigidBody/Sphere/Sphere.hpp"
#include "Systems/common/SymplecticStepperAdapter.hpp"  // for definition

namespace elastica {

  //============================================================================
  //
  //  ACCESS API FUNCTIONS
  //
  //============================================================================

  //**Position functions********************************************************
  /*!\name Position functions */
  //@{
  /*!\brief Specialization of elastica::position() for SphereBlockSlice
   * \ingroup rigid_body
   */
  inline decltype(auto) position(
      rigid_body::detail::SphereBlockSlice const& rigid_body) noexcept {
    return rigid_body.get_position();
  }
  inline decltype(auto) position(
      rigid_body::detail::SphereBlockSlice const& rigid_body,
      std::size_t index) noexcept {
    return rigid_body.get_position(index);
  }
  //@}
  //****************************************************************************

  //**Director functions********************************************************
  /*!\name Director functions */
  //@{
  /*!\brief Specialization of elastica::director() for Cosserat rods
   * \ingroup rigid_body
   */
  inline decltype(auto) director(
      rigid_body::detail::SphereBlockSlice const& rigid_body) noexcept {
    return rigid_body.get_director();
  }
  inline decltype(auto) director(
      rigid_body::detail::SphereBlockSlice const& rigid_body,
      std::size_t index) noexcept {
    return rigid_body.get_director(index);
  }
  //@}
  //****************************************************************************

  //**Velocity functions********************************************************
  /*!\name Velocity functions */
  //@{
  /*!\brief Specialization of elastica::velocity() for Cosserat rods
   * \ingroup rigid_body
   */
  inline decltype(auto) velocity(
      rigid_body::detail::SphereBlockSlice const& rigid_body) noexcept {
    return rigid_body.get_velocity();
  }
  inline decltype(auto) velocity(
      rigid_body::detail::SphereBlockSlice const& rigid_body,
      std::size_t index) noexcept {
    return rigid_body.get_velocity(index);
  }
  //@}
  //****************************************************************************

  //**Angular Velocity functions************************************************
  /*!\name Angular Velocity functions */
  //@{
  /*!\brief Specialization of elastica::angular_velocity() for Cosserat rods
   * \ingroup rigid_body
   */
  inline decltype(auto) angular_velocity(
      rigid_body::detail::SphereBlockSlice const& rigid_body) noexcept {
    return rigid_body.get_angular_velocity();
  }
  inline decltype(auto) angular_velocity(
      rigid_body::detail::SphereBlockSlice const& rigid_body,
      std::size_t index) noexcept {
    return rigid_body.get_angular_velocity(index);
  }
  //@}
  //****************************************************************************

  //**External loads functions**************************************************
  /*!\name External loads functions */
  //@{
  /*!\brief Specialization of elastica::external_loads() for Cosserat rods
   * \ingroup rigid_body
   */
  inline decltype(auto) external_loads(
      rigid_body::detail::SphereBlockSlice const& rigid_body) noexcept {
    return rigid_body.get_external_loads();
  }
  inline decltype(auto) external_loads(
      rigid_body::detail::SphereBlockSlice const& rigid_body,
      std::size_t index) noexcept {
    return rigid_body.get_external_loads(index);
  }
  //@}
  //****************************************************************************

  //**External torques functions************************************************
  /*!\name External torques functions */
  //@{
  /*!\brief Specialization of elastica::external_torques() for Cosserat rods
   * \ingroup rigid_body
   */
  inline decltype(auto) external_torques(
      rigid_body::detail::SphereBlockSlice const& rigid_body) noexcept {
    return rigid_body.get_external_torques();
  }
  inline decltype(auto) external_torques(
      rigid_body::detail::SphereBlockSlice const& rigid_body,
      std::size_t index) noexcept {
    return rigid_body.get_external_torques(index);
  }
  //@}
  //****************************************************************************

}  // namespace elastica
