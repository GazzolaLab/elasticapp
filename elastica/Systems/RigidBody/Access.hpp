//==============================================================================
/*!
//  \file
//  \brief Access API for Cosserat rods
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

#include <cstddef>  // size_t

#include "Systems/common/Access.hpp"
/// Forward declarations
#include "Systems/RigidBody/Types.hpp"
///
#include "Systems/RigidBody/RigidBodyPlugin.hpp"

namespace elastica {

  //============================================================================
  //
  //  ACCESS API FUNCTIONS
  //
  //============================================================================

  //**Position functions********************************************************
  /*!\name Position functions */
  //@{
  /*!\brief Specialization of elastica::position() for Cosserat rods
   * \ingroup rigid_body
   */
  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) position(
      ::elastica::rigid_body::RigidBodyPlugin<
          CRT, ComputationalBlock, Components...>& rigid_body) noexcept {
    // get_position() wraps around a free function, so the API is still not bad
    return rigid_body.self().get_position();
  }
  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) position(
      ::elastica::rigid_body::RigidBodyPlugin<CRT, ComputationalBlock,
                                              Components...>& rigid_body,
      std::size_t index) noexcept {
    // tentative, can change
    // ideally should be position(blocks::slice(rigid_body, index));
    // but slice(rigid_body_slice) is not defined (and we do not need it per
    // se).

    // Other option is
    // blocks::slice(position(rigid_body), index);
    // which is very close to what we have below. The problem here is that
    // position(rigid_body) is not a block anymore, so the semantics are not
    // well formed.
    return rigid_body.self().get_position(index);
  }

  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) position(
      ::elastica::rigid_body::RigidBodyPlugin<
          CRT, ComputationalBlock, Components...> const& rigid_body) noexcept {
    return rigid_body.self().get_position();
  }
  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) position(
      ::elastica::rigid_body::RigidBodyPlugin<CRT, ComputationalBlock,
                                              Components...> const& rigid_body,
      std::size_t index) noexcept {
    return rigid_body.self().get_position(index);
  }
  //@}
  //****************************************************************************

  //**Director functions********************************************************
  /*!\name Director functions */
  //@{
  /*!\brief Specialization of elastica::director() for Cosserat rods
   * \ingroup rigid_body
   */
  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) director(
      ::elastica::rigid_body::RigidBodyPlugin<
          CRT, ComputationalBlock, Components...>& rigid_body) noexcept {
    return rigid_body.self().get_director();
  }
  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) director(
      ::elastica::rigid_body::RigidBodyPlugin<CRT, ComputationalBlock,
                                              Components...>& rigid_body,
      std::size_t index) noexcept {
    return rigid_body.self().get_director(index);
  }

  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) director(
      ::elastica::rigid_body::RigidBodyPlugin<
          CRT, ComputationalBlock, Components...> const& rigid_body) noexcept {
    return rigid_body.self().get_director();
  }
  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) director(
      ::elastica::rigid_body::RigidBodyPlugin<CRT, ComputationalBlock,
                                              Components...> const& rigid_body,
      std::size_t index) noexcept {
    return rigid_body.self().get_director(index);
  }
  //@}
  //****************************************************************************

  //**Velocity functions********************************************************
  /*!\name Velocity functions */
  //@{
  /*!\brief Specialization of elastica::velocity() for Cosserat rods
   * \ingroup rigid_body
   */
  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) velocity(
      ::elastica::rigid_body::RigidBodyPlugin<
          CRT, ComputationalBlock, Components...>& rigid_body) noexcept {
    return rigid_body.self().get_velocity();
  }
  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) velocity(
      ::elastica::rigid_body::RigidBodyPlugin<CRT, ComputationalBlock,
                                              Components...>& rigid_body,
      std::size_t index) noexcept {
    return rigid_body.self().get_velocity(index);
  }

  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) velocity(
      ::elastica::rigid_body::RigidBodyPlugin<
          CRT, ComputationalBlock, Components...> const& rigid_body) noexcept {
    return rigid_body.self().get_velocity();
  }
  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) velocity(
      ::elastica::rigid_body::RigidBodyPlugin<CRT, ComputationalBlock,
                                              Components...> const& rigid_body,
      std::size_t index) noexcept {
    return rigid_body.self().get_velocity(index);
  }
  //@}
  //****************************************************************************

  //**Angular Velocity functions************************************************
  /*!\name Angular Velocity functions */
  //@{
  /*!\brief Specialization of elastica::angular_velocity() for Cosserat rods
   * \ingroup rigid_body
   */
  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) angular_velocity(
      ::elastica::rigid_body::RigidBodyPlugin<
          CRT, ComputationalBlock, Components...>& rigid_body) noexcept {
    return rigid_body.self().get_angular_velocity();
  }
  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) angular_velocity(
      ::elastica::rigid_body::RigidBodyPlugin<CRT, ComputationalBlock,
                                              Components...>& rigid_body,
      std::size_t index) noexcept {
    return rigid_body.self().get_angular_velocity(index);
  }

  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) angular_velocity(
      ::elastica::rigid_body::RigidBodyPlugin<
          CRT, ComputationalBlock, Components...> const& rigid_body) noexcept {
    return rigid_body.self().get_angular_velocity();
  }
  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) angular_velocity(
      ::elastica::rigid_body::RigidBodyPlugin<CRT, ComputationalBlock,
                                              Components...> const& rigid_body,
      std::size_t index) noexcept {
    return rigid_body.self().get_angular_velocity(index);
  }
  //@}
  //****************************************************************************

  //**External loads functions**************************************************
  /*!\name External loads functions */
  //@{
  /*!\brief Specialization of elastica::external_loads() for Cosserat rods
   * \ingroup rigid_body
   */
  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) external_loads(
      ::elastica::rigid_body::RigidBodyPlugin<
          CRT, ComputationalBlock, Components...>& rigid_body) noexcept {
    return rigid_body.self().get_external_loads();
  }
  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) external_loads(
      ::elastica::rigid_body::RigidBodyPlugin<CRT, ComputationalBlock,
                                              Components...>& rigid_body,
      std::size_t index) noexcept {
    return rigid_body.self().get_external_loads(index);
  }
  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) external_loads(
      ::elastica::rigid_body::RigidBodyPlugin<
          CRT, ComputationalBlock, Components...> const& rigid_body) noexcept {
    return rigid_body.self().get_external_loads();
  }
  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) external_loads(
      ::elastica::rigid_body::RigidBodyPlugin<CRT, ComputationalBlock,
                                              Components...> const& rigid_body,
      std::size_t index) noexcept {
    return rigid_body.self().get_external_loads(index);
  }
  //@}
  //****************************************************************************

  //**External torques functions************************************************
  /*!\name External torques functions */
  //@{
  /*!\brief Specialization of elastica::external_torques() for Cosserat rods
   * \ingroup rigid_body
   */
  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) external_torques(
      ::elastica::rigid_body::RigidBodyPlugin<
          CRT, ComputationalBlock, Components...>& rigid_body) noexcept {
    return rigid_body.self().get_external_torques();
  }
  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) external_torques(
      ::elastica::rigid_body::RigidBodyPlugin<CRT, ComputationalBlock,
                                              Components...>& rigid_body,
      std::size_t index) noexcept {
    return rigid_body.self().get_external_torques(index);
  }
  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) external_torques(
      ::elastica::rigid_body::RigidBodyPlugin<
          CRT, ComputationalBlock, Components...> const& rigid_body) noexcept {
    return rigid_body.self().get_external_torques();
  }
  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) external_torques(
      ::elastica::rigid_body::RigidBodyPlugin<CRT, ComputationalBlock,
                                              Components...> const& rigid_body,
      std::size_t index) noexcept {
    return rigid_body.self().get_external_torques(index);
  }
  //@}
  //****************************************************************************

}  // namespace elastica
