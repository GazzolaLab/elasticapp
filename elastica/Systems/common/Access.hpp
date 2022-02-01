//==============================================================================
/*!
//  \file
//  \brief Access API for all \elastica systems
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

#include <cstddef>  // size_t

namespace elastica {

  //============================================================================
  //
  //  ACCESS API FUNCTIONS
  //
  //============================================================================

  struct AnyPhysicalSystem {};

  //**Position functions********************************************************
  /*!\name Position functions */
  //@{
  /*!\brief Retrieves position from the systems
   * \ingroup systems
   */
  inline constexpr decltype(auto) position(AnyPhysicalSystem& t) noexcept;
  inline constexpr decltype(auto) position(AnyPhysicalSystem const& t) noexcept;
  inline constexpr decltype(auto) position(AnyPhysicalSystem& t,
                                           std::size_t index) noexcept;
  inline constexpr decltype(auto) position(AnyPhysicalSystem const& t,
                                           std::size_t index) noexcept;
  //@}
  //****************************************************************************

  //**Director functions********************************************************
  /*!\name Director functions */
  //@{
  /*!\brief Retrieves directors from the systems
   * \ingroup systems
   */
  inline constexpr decltype(auto) director(AnyPhysicalSystem& t) noexcept;
  inline constexpr decltype(auto) director(AnyPhysicalSystem const& t) noexcept;
  inline constexpr decltype(auto) director(AnyPhysicalSystem& t,
                                           std::size_t index) noexcept;
  inline constexpr decltype(auto) director(AnyPhysicalSystem const& t,
                                           std::size_t index) noexcept;
  //@}
  //****************************************************************************

  //**Velocity functions********************************************************
  /*!\name Velocity functions */
  //@{
  /*!\brief Retrieves velocities from the systems
   * \ingroup systems
   */
  inline constexpr decltype(auto) velocity(AnyPhysicalSystem& t) noexcept;
  inline constexpr decltype(auto) velocity(AnyPhysicalSystem const& t) noexcept;
  inline constexpr decltype(auto) velocity(AnyPhysicalSystem& t,
                                           std::size_t index) noexcept;
  inline constexpr decltype(auto) velocity(AnyPhysicalSystem const& t,
                                           std::size_t index) noexcept;
  //@}
  //****************************************************************************

  //**Angular Velocity functions************************************************
  /*!\name Angular Velocity functions */
  //@{
  /*!\brief Retrieves angular velocities from the systems
   * \ingroup systems
   */
  inline constexpr decltype(auto) angular_velocity(
      AnyPhysicalSystem& t) noexcept;
  inline constexpr decltype(auto) angular_velocity(
      AnyPhysicalSystem const& t) noexcept;
  inline constexpr decltype(auto) angular_velocity(AnyPhysicalSystem& t,
                                                   std::size_t index) noexcept;
  inline constexpr decltype(auto) angular_velocity(AnyPhysicalSystem const& t,
                                                   std::size_t index) noexcept;
  //@}
  //****************************************************************************

}  // namespace elastica
