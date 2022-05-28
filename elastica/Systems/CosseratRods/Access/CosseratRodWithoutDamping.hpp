//==============================================================================
/*!
//  \file
//  \brief Access to default "CosseratRodWithoutDamping" members in \elastica
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

#include "Systems/CosseratRods/Aliases/CosseratRodWithoutDamping.hpp"
#include "Systems/CosseratRods/BlockSlice.hpp"
#include "Systems/CosseratRods/Components.hpp"
#include "Systems/CosseratRods/CosseratRodPlugin.hpp"
#include "Systems/CosseratRods/CosseratRodTraits.hpp"  // for definition of default traits
#include "Systems/common/SymplecticStepperAdapter.hpp"

namespace elastica {

  //============================================================================
  //
  //  ACCESS API FUNCTIONS
  //
  //============================================================================

  //**Position functions********************************************************
  /*!\name Position functions */
  //@{
  /*!\brief Specialization of elastica::position() for
   * CosseratRodWithoutDampingBlockSlice \ingroup cosserat_rod
   */
  inline decltype(auto) position(
      cosserat_rod::detail::CosseratRodWithoutDampingBlockSlice const&
          cosserat_rod) noexcept {
    return cosserat_rod.get_position();
  }
  inline decltype(auto) position(
      cosserat_rod::detail::CosseratRodWithoutDampingBlockSlice const&
          cosserat_rod,
      std::size_t index) noexcept {
    return cosserat_rod.get_position(index);
  }
  //@}
  //****************************************************************************

  //**Director functions********************************************************
  /*!\name Director functions */
  //@{
  /*!\brief Specialization of elastica::director() for Cosserat rods
   * \ingroup cosserat_rod
   */
  inline decltype(auto) director(
      cosserat_rod::detail::CosseratRodWithoutDampingBlockSlice const&
          cosserat_rod) noexcept {
    return cosserat_rod.get_director();
  }
  inline decltype(auto) director(
      cosserat_rod::detail::CosseratRodWithoutDampingBlockSlice const&
          cosserat_rod,
      std::size_t index) noexcept {
    return cosserat_rod.get_director(index);
  }
  //@}
  //****************************************************************************

  //**Velocity functions********************************************************
  /*!\name Velocity functions */
  //@{
  /*!\brief Specialization of elastica::velocity() for Cosserat rods
   * \ingroup cosserat_rod
   */
  inline decltype(auto) velocity(
      cosserat_rod::detail::CosseratRodWithoutDampingBlockSlice const&
          cosserat_rod) noexcept {
    return cosserat_rod.get_velocity();
  }
  inline decltype(auto) velocity(
      cosserat_rod::detail::CosseratRodWithoutDampingBlockSlice const&
          cosserat_rod,
      std::size_t index) noexcept {
    return cosserat_rod.get_velocity(index);
  }
  //@}
  //****************************************************************************

  //**Angular Velocity functions************************************************
  /*!\name Angular Velocity functions */
  //@{
  /*!\brief Specialization of elastica::angular_velocity() for Cosserat rods
   * \ingroup cosserat_rod
   */
  inline decltype(auto) angular_velocity(
      cosserat_rod::detail::CosseratRodWithoutDampingBlockSlice const&
          cosserat_rod) noexcept {
    return cosserat_rod.get_angular_velocity();
  }
  inline decltype(auto) angular_velocity(
      cosserat_rod::detail::CosseratRodWithoutDampingBlockSlice const&
          cosserat_rod,
      std::size_t index) noexcept {
    return cosserat_rod.get_angular_velocity(index);
  }
  //@}
  //****************************************************************************

  //**External loads functions**************************************************
  /*!\name External loads functions */
  //@{
  /*!\brief Specialization of elastica::external_loads() for Cosserat rods
   * \ingroup cosserat_rod
   */
  inline decltype(auto) external_loads(
      cosserat_rod::detail::CosseratRodWithoutDampingBlockSlice const&
          cosserat_rod) noexcept {
    return cosserat_rod.get_external_loads();
  }
  inline decltype(auto) external_loads(
      cosserat_rod::detail::CosseratRodWithoutDampingBlockSlice const&
          cosserat_rod,
      std::size_t index) noexcept {
    return cosserat_rod.get_external_loads(index);
  }
  //@}
  //****************************************************************************

  //**External torques functions************************************************
  /*!\name External torques functions */
  //@{
  /*!\brief Specialization of elastica::external_torques() for Cosserat rods
   * \ingroup cosserat_rod
   */
  inline decltype(auto) external_torques(
      cosserat_rod::detail::CosseratRodWithoutDampingBlockSlice const&
          cosserat_rod) noexcept {
    return cosserat_rod.get_external_torques();
  }
  inline decltype(auto) external_torques(
      cosserat_rod::detail::CosseratRodWithoutDampingBlockSlice const&
          cosserat_rod,
      std::size_t index) noexcept {
    return cosserat_rod.get_external_torques(index);
  }
  //@}
  //****************************************************************************

}  // namespace elastica
