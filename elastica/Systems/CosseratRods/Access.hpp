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

#include <cstddef>  // size_t

#include "Systems/common/Access.hpp"
/// Forward declarations
#include "Systems/CosseratRods/Types.hpp"
///
#include "Systems/CosseratRods/Block.hpp"
#include "Systems/CosseratRods/BlockSlice.hpp"
#include "Systems/CosseratRods/CosseratRodPlugin.hpp"

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
   * \ingroup cosserat_rod
   */
  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) position(
      ComputationalBlock<::elastica::cosserat_rod::CosseratRodPlugin<
          CRT, ComputationalBlock, Components...>>& cosserat_rod) noexcept {
    // get_position() wraps around a free function, so the API is still not bad
    return cosserat_rod.get_position();
  }
  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) position(
      ComputationalBlock<::elastica::cosserat_rod::CosseratRodPlugin<
          CRT, ComputationalBlock, Components...>>& cosserat_rod,
      std::size_t index) noexcept {
    // tentative, can change
    // ideally should be position(blocks::slice(cosserat_rod, index));
    // but slice(cosserat_rod_slice) is not defined (and we do not need it per
    // se).

    // Other option is
    // blocks::slice(position(cosserat_rod), index);
    // which is very close to what we have below. The problem here is that
    // position(cosserat_rod) is not a block anymore, so the semantics are not
    // well formed.
    return cosserat_rod.get_position(index);
  }

  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) position(
      ComputationalBlock<::elastica::cosserat_rod::CosseratRodPlugin<
          CRT, ComputationalBlock, Components...>> const&
          cosserat_rod) noexcept {
    return cosserat_rod.get_position();
  }
  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) position(
      ComputationalBlock<::elastica::cosserat_rod::CosseratRodPlugin<
          CRT, ComputationalBlock, Components...>> const& cosserat_rod,
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
  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) director(
      ComputationalBlock<::elastica::cosserat_rod::CosseratRodPlugin<
          CRT, ComputationalBlock, Components...>>& cosserat_rod) noexcept {
    return cosserat_rod.get_director();
  }
  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) director(
      ComputationalBlock<::elastica::cosserat_rod::CosseratRodPlugin<
          CRT, ComputationalBlock, Components...>>& cosserat_rod,
      std::size_t index) noexcept {
    return cosserat_rod.get_director(index);
  }

  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) director(
      ComputationalBlock<::elastica::cosserat_rod::CosseratRodPlugin<
          CRT, ComputationalBlock, Components...>> const&
          cosserat_rod) noexcept {
    return cosserat_rod.get_director();
  }
  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) director(
      ComputationalBlock<::elastica::cosserat_rod::CosseratRodPlugin<
          CRT, ComputationalBlock, Components...>> const& cosserat_rod,
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
  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) velocity(
      ComputationalBlock<::elastica::cosserat_rod::CosseratRodPlugin<
          CRT, ComputationalBlock, Components...>>& cosserat_rod) noexcept {
    return cosserat_rod.get_velocity();
  }
  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) velocity(
      ComputationalBlock<::elastica::cosserat_rod::CosseratRodPlugin<
          CRT, ComputationalBlock, Components...>>& cosserat_rod,
      std::size_t index) noexcept {
    return cosserat_rod.get_velocity(index);
  }

  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) velocity(
      ComputationalBlock<::elastica::cosserat_rod::CosseratRodPlugin<
          CRT, ComputationalBlock, Components...>> const&
          cosserat_rod) noexcept {
    return cosserat_rod.get_velocity();
  }
  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) velocity(
      ComputationalBlock<::elastica::cosserat_rod::CosseratRodPlugin<
          CRT, ComputationalBlock, Components...>> const& cosserat_rod,
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
  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) angular_velocity(
      ComputationalBlock<::elastica::cosserat_rod::CosseratRodPlugin<
          CRT, ComputationalBlock, Components...>>& cosserat_rod) noexcept {
    return cosserat_rod.get_angular_velocity();
  }
  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) angular_velocity(
      ComputationalBlock<::elastica::cosserat_rod::CosseratRodPlugin<
          CRT, ComputationalBlock, Components...>>& cosserat_rod,
      std::size_t index) noexcept {
    return cosserat_rod.get_angular_velocity(index);
  }

  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) angular_velocity(
      ComputationalBlock<::elastica::cosserat_rod::CosseratRodPlugin<
          CRT, ComputationalBlock, Components...>> const&
          cosserat_rod) noexcept {
    return cosserat_rod.get_angular_velocity();
  }
  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>
  inline constexpr decltype(auto) angular_velocity(
      ComputationalBlock<::elastica::cosserat_rod::CosseratRodPlugin<
          CRT, ComputationalBlock, Components...>> const& cosserat_rod,
      std::size_t index) noexcept {
    return cosserat_rod.get_angular_velocity(index);
  }
  //@}
  //****************************************************************************

}  // namespace elastica