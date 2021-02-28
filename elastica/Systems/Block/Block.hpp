//==============================================================================
/*!
//  \file
//  \brief The Blocks module in \elastica
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
////
#include "TypeTraits.hpp"
#include "Types.hpp"
////
//// include implementations now
#include "Block/Block.hpp"
#include "Block/BlockFacade.hpp"
#include "Block/BlockSlice.hpp"
#include "BlockVariables/BlockInitializer.hpp"
#include "BlockVariables/BlockVariables.hpp"

//==============================================================================
//
//  DOXYGEN DOCUMENTATION
//
//==============================================================================

//******************************************************************************
/*!\defgroup blocks Blocks
 * \ingroup systems
 *
 * The blocks module contains the interface for efficiently programming
 * different \elastica entities (such as rods, rigid bodies etc.) and more
 * generally, Lagrangian data-structures.
 */
//******************************************************************************

/*!\brief Block data-structures and routines
// \ingroup blocks
*/
namespace blocks {}

namespace blocks {

  namespace detail {
    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    struct tag {
      //**Type definitions******************************************************
      //! Tag for marking a block variable
      using is_a_block_variable = std::true_type;
      //! Tag for marking a not block variable
      using is_not_a_block_variable = std::false_type;
      //! Variable template for determining if VarTag is a block variable
      template <typename VarTag>
      using is_block_variable = IsVariable<VarTag>;
      //************************************************************************
    };

    // Implementation for a block variable inside the hierarchy, can use
    // SFINAE too instead of overloads. We use tuples::get() directly, since
    // I don't see any point in customizing the tag tuples
    template <typename BlockVariableTag, template <typename> class B,
              typename Plugin>
    inline constexpr decltype(auto) get_impl(
        B<Plugin>& block, tag::is_a_block_variable /* meta*/) noexcept {
      return tuples::get<BlockVariableTag>(block.data());
    }

    template <typename BlockVariableTag, template <typename> class B,
              typename Plugin>
    inline constexpr decltype(auto) get_impl(
        B<Plugin> const& block, tag::is_a_block_variable /* meta*/) noexcept {
      // return block.template get<BlockVariableTag>();
      return tuples::get<BlockVariableTag>(block.data());
    }

    template <typename BlockVariableTag, template <typename> class B,
              typename Plugin>
    inline constexpr decltype(auto) get_impl(
        B<Plugin>&& block, tag::is_a_block_variable /* meta*/) noexcept {
      return tuples::get<BlockVariableTag>(
          static_cast<B<Plugin>&&>(block).data());
    }

    template <typename BlockVariableTag, template <typename> class B,
              typename Plugin>
    inline constexpr decltype(auto) get_impl(
        B<Plugin> const&& block, tag::is_a_block_variable /* meta*/) noexcept {
      return tuples::get<BlockVariableTag>(
          static_cast<B<Plugin> const&&>(block).data());
    }

    template <typename Tag, typename BlockLike>
    inline constexpr decltype(auto) get_impl(
        BlockLike&& block_like,
        tag::is_not_a_block_variable /* meta*/) noexcept {
      using Block_t = std::decay_t<BlockLike>;
      // here we need to make sure that all blocks give a templated type trait
      // to recover block variables from a given "outside" Tag
      // using BlockVariableTag = typename Block_t::template variable_from<Tag>;
      using BlockVariableTag =
          typename Block_t::VariableMap::template variable_from<Tag>;
      return get_impl<BlockVariableTag>(std::forward<BlockLike>(block_like),
                                        tag::is_a_block_variable{});
    }
    /*! \endcond */
    //**************************************************************************

  }  // namespace detail

  //****************************************************************************
  /*!\brief Extract element from a type modeling the `Block` concept
   * \ingroup blocks
   *
   * \details
   * Extracts the element of the `BlockLike` type modeling the `Block` concept
   * `block_like` whose tag type is `Tag`. Fails to compile unless the block has
   * the `Tag` being extracted. Additionally, it chooses the correct overload
   * for (const) lvalue and rvalue blocks.
   *
   * \usage
   * The usage is similar to std::get(), shown below
   * \code
     Block<...> b;
     auto my_tag_data = blocks::get<tags::MyTag>(b);
   * \endcode
   *
   * \tparam Tag Tag to extract
   *
   * \param block_like any object modeling the `block` concept
   */
  template <typename Tag, typename BlockLike>
  inline constexpr decltype(auto) get(BlockLike&& block_like) noexcept {
    using Block_t = std::decay_t<BlockLike>;
    static_assert(
        ModelsBlockConcept<Block_t>::value,
        "Could not retrieve Tag from Block as the type passed in is not"
        "a valid elastica Block type.");
    return detail::get_impl<Tag>(std::forward<BlockLike>(block_like),
                                 detail::tag::is_block_variable<Tag>{});
  }
  //****************************************************************************

}  // namespace blocks
