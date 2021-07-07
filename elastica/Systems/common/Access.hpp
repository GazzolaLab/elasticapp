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

#include <cstddef> // size_t

namespace elastica {

  //============================================================================
  //
  //  ACCESS API FUNCTIONS
  //
  //============================================================================

  //**Position functions********************************************************
  /*!\name Position functions */
  //@{
  /*!\brief Retrieves position from the systems
   * \ingroup systems
   */
  template <typename T>
  inline constexpr decltype(auto) position(T& t) noexcept;
  template <typename T>
  inline constexpr decltype(auto) position(T const& t) noexcept;
  template <typename T>
  inline constexpr decltype(auto) position(T& t, std::size_t index) noexcept;
  template <typename T>
  inline constexpr decltype(auto) position(T const& t,
                                           std::size_t index) noexcept;
  //@}
  //****************************************************************************

  //**Director functions********************************************************
  /*!\name Director functions */
  //@{
  /*!\brief Retrieves directors from the systems
   * \ingroup systems
   */
  template <typename T>
  inline constexpr decltype(auto) director(T& t) noexcept;
  template <typename T>
  inline constexpr decltype(auto) director(T const& t) noexcept;
  template <typename T>
  inline constexpr decltype(auto) director(T& t, std::size_t index) noexcept;
  template <typename T>
  inline constexpr decltype(auto) director(T const& t,
                                           std::size_t index) noexcept;
  //@}
  //****************************************************************************

  //**Velocity functions********************************************************
  /*!\name Velocity functions */
  //@{
  /*!\brief Retrieves velocities from the systems
   * \ingroup systems
   */
  template <typename T>
  inline constexpr decltype(auto) velocity(T& t) noexcept;
  template <typename T>
  inline constexpr decltype(auto) velocity(T const& t) noexcept;
  template <typename T>
  inline constexpr decltype(auto) velocity(T& t, std::size_t index) noexcept;
  template <typename T>
  inline constexpr decltype(auto) velocity(T const& t,
                                           std::size_t index) noexcept;
  //@}
  //****************************************************************************

  //**Angular Velocity functions************************************************
  /*!\name Angular Velocity functions */
  //@{
  /*!\brief Retrieves angular velocities from the systems
   * \ingroup systems
   */
  template <typename T>
  inline constexpr decltype(auto) angular_velocity(T& t) noexcept;
  template <typename T>
  inline constexpr decltype(auto) angular_velocity(T const& t) noexcept;
  template <typename T>
  inline constexpr decltype(auto) angular_velocity(T& t,
                                                   std::size_t index) noexcept;
  template <typename T>
  inline constexpr decltype(auto) angular_velocity(T const& t,
                                                   std::size_t index) noexcept;
  //@}
  //****************************************************************************

}  // namespace elastica
