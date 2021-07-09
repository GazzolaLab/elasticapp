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
//
#include "Systems/Block/Types.hpp"
//
#include "Systems/Block/Protocols.hpp"
#include "Systems/Block/TypeTraits.hpp"
// include implementations now
#include "Systems/Block/Block/Block.hpp"
#include "Systems/Block/Block/BlockFacade.hpp"
#include "Systems/Block/Block/BlockSlice.hpp"
#include "Systems/Block/Block/DisplayVariables.hpp"
// Variable implementations
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

//******************************************************************************
/*!\brief Block data-structures and routines
// \ingroup blocks
*/
namespace blocks {}
//******************************************************************************

namespace blocks {

  //****************************************************************************
  /*!\brief Get number of units from a Block
   * \ingroup blocks
   *
   * \details
   * Get number of units from a Block. By 'units' we mean the number of
   * individual BlockSlice(s) composing the Block.
   *
   * \usage
   * \code
   * Block<...> b;
   * std::size_t n_units = blocks::n_units(b);
   * \endcode
   *
   * \param block_like The block whose number of units is to be extracted.
   *
   * \note
   * These need to be customized for your block type.
   */
  template <typename Plugin>
  inline auto n_units(Block<Plugin> const& block) noexcept -> std::size_t;
  //****************************************************************************

  //****************************************************************************
  /*!\brief Adapt a function to be applied over slices instead of blocks
   * \ingroup blocks
   *
   * \details
   * Given a callable `func`, adapt it so that it can be applied on a slice-wise
   * basis rather than a blockwise basis. This proves useful for iteration in
   * the ::elastica::modules::Systems module of Simulator.
   *
   * \usage
   * \code
   * auto func = [](auto & system){ // User code to iterate on systems; }
   * // use to apply over slices
   * auto func_to_be_applied_over_slices =
   * blocks::apply_over_slice_adapter(func);
   * \endcode
   *
   * \param func Callable to be applied over slices
   */
  template <typename F>
  constexpr auto apply_over_slice_adapter(F func) noexcept {
    return [func = std::move(func)](auto& system_block) /*mutable*/ -> void {
      using ::blocks::n_units;
      const std::size_t cardinality = n_units(system_block);
      // iteration within a block is always serial
      for (std::size_t idx = 0UL; idx < cardinality; ++idx)
        // block[] returns not a "tagged" rod but the rod itself. So overloading
        // based on tags do not work as expected
        // so use slice() instead, which doesn't have this problem
        func(blocks::slice(system_block, idx));
    };
  }
  //****************************************************************************

}  // namespace blocks

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
      //! Tag for marking a block initializer
      using is_a_block_initializer = std::true_type;
      //! Tag for marking a not block initializer
      using is_not_a_block_initializer = std::false_type;
      //! Variable template for determining if  is a block variable
      template <typename B>
      using is_block_initializer = IsBlockInitializer<B>;
      //************************************************************************
    };

    // Implementation for a block variable inside the hierarchy, can use
    // SFINAE too instead of overloads. We use tuples::get() directly, since
    // I don't see any point in customizing the tag tuples
    template <typename BlockVariableTag, template <typename> class B,
              typename Plugin>
    inline constexpr decltype(auto) get_impl(tag::is_a_block_variable /* meta*/,
                                             B<Plugin>& block) noexcept {
      return tuples::get<BlockVariableTag>(block.data());
    }
    //    template <typename BlockVariableTag, template <typename> class B,
    //              typename Plugin>
    //    inline constexpr decltype(auto) get_impl(tag::is_a_block_variable tag,
    //                                             B<Plugin>& block,
    //                                             std::size_t index) noexcept {
    //      return BlockVariableTag::slice(get_impl<BlockVariableTag>(tag,
    //      block),
    //                                     index);
    //    }

    template <typename BlockVariableTag, template <typename> class B,
              typename Plugin>
    inline constexpr decltype(auto) get_impl(tag::is_a_block_variable /* meta*/,
                                             B<Plugin> const& block) noexcept {
      // return block.template get<BlockVariableTag>();
      return tuples::get<BlockVariableTag>(block.data());
    }
    //    template <typename BlockVariableTag, template <typename> class B,
    //              typename Plugin>
    //    inline constexpr decltype(auto) get_impl(tag::is_a_block_variable tag,
    //                                             B<Plugin> const& block,
    //                                             std::size_t index) noexcept {
    //      return BlockVariableTag::slice(get_impl<BlockVariableTag>(tag,
    //      block),
    //                                     index);
    //    }

    template <typename BlockVariableTag, template <typename> class B,
              typename Plugin>
    inline constexpr decltype(auto) get_impl(tag::is_a_block_variable /* meta*/,
                                             B<Plugin>&& block) noexcept {
      return tuples::get<BlockVariableTag>(
          static_cast<B<Plugin>&&>(block).data());
    }

    template <typename BlockVariableTag, template <typename> class B,
              typename Plugin>
    inline constexpr decltype(auto) get_impl(tag::is_a_block_variable /* meta*/,
                                             B<Plugin> const&& block) noexcept {
      return tuples::get<BlockVariableTag>(
          static_cast<B<Plugin> const&&>(block).data());
    }

    //    template <typename Tag, typename BlockLike>
    //    inline constexpr decltype(auto) get_impl(
    //        tag::is_not_a_block_variable /* meta*/,
    //        BlockLike&& block_like) noexcept {
    //      using Block_t = std::decay_t<BlockLike>;
    //      // here we need to make sure that all blocks give a templated type
    //      trait
    //      // to recover block variables from a given "outside" Tag
    //      // using BlockVariableTag = typename Block_t::template
    //      variable_from<Tag>; using BlockVariableTag =
    //          typename Block_t::VariableMap::template variable_from<Tag>;
    //      return get_impl<BlockVariableTag>(tag::is_a_block_variable{},
    //                                        std::forward<BlockLike>(block_like));
    //    }

    template <typename Tag, typename BlockLike, typename... Args>
    inline constexpr decltype(auto) get_impl(
        tag::is_not_a_block_variable /* meta*/, BlockLike&& block_like,
        Args&&... args) noexcept {
      using Block_t = std::decay_t<BlockLike>;
      // here we need to make sure that all blocks give a templated type trait
      // to recover block variables from a given "outside" Tag
      // using BlockVariableTag = typename Block_t::template variable_from<Tag>;
      using BlockVariableTag =
          typename Block_t::VariableMap::template variable_from<Tag>;
      return get_impl<BlockVariableTag>(tag::is_a_block_variable{},
                                        std::forward<BlockLike>(block_like),
                                        std::forward<Args>(args)...);
    }

    /*! \endcond */
    //**************************************************************************

  }  // namespace detail

  //============================================================================
  //
  //  FORWARD DECLARATION OF GETTERS
  //
  //============================================================================

  //**Get functions*************************************************************
  /*!\name Get functions for Block */
  //@{

  //****************************************************************************
  /*!\brief Extract element from a Block
   * \ingroup blocks
   *
   * \details
   * Extracts the element of the Block `block_like` whose tag type is `Tag`.
   * Fails to compile unless the block has the `Tag` being extracted.
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
   * \param block_like The block to extract the tag from
   */
  template <typename Tag, typename Plugin>
  inline constexpr decltype(auto) get(Block<Plugin>& block_like) noexcept;
  template <typename Tag, typename Plugin>
  inline constexpr decltype(auto) get(Block<Plugin> const& block_like) noexcept;
  template <typename Tag, typename Plugin>
  inline constexpr decltype(auto) get(Block<Plugin>&& block_like) noexcept;
  template <typename Tag, typename Plugin>
  inline constexpr decltype(auto) get(
      Block<Plugin> const&& block_like) noexcept;
  //****************************************************************************

  //@}
  //****************************************************************************

  //**Get functions*************************************************************
  /*!\name Get functions for BlockSlice */
  //@{

  //****************************************************************************
  /*!\brief Extract element from a BlockSlice
   * \ingroup blocks
   *
   * \details
   * Extracts the element of the BlockSlice `block_like` whose tag type is
   `Tag`.
   * Fails to compile unless the block has the `Tag` being extracted.
   *
   * \usage
   * The usage is similar to std::get(), shown below
   * \code
     BlockSlice<...> b;
     auto my_tag_data = blocks::get<tags::MyTag>(b);
   * \endcode
   *
   * \tparam Tag Tag to extract
   *
   * \param block_like The block to extract the tag from
   */
  template <typename Tag, typename Plugin>
  inline constexpr decltype(auto) get(BlockSlice<Plugin>& block_like) noexcept;
  template <typename Tag, typename Plugin>
  inline constexpr decltype(auto) get(
      BlockSlice<Plugin> const& block_like) noexcept;
  template <typename Tag, typename Plugin>
  inline constexpr decltype(auto) get(BlockSlice<Plugin>&& block_like) noexcept;
  template <typename Tag, typename Plugin>
  inline constexpr decltype(auto) get(
      BlockSlice<Plugin> const&& block_like) noexcept;
  //****************************************************************************

  //@}
  //****************************************************************************

  //**Get functions*************************************************************
  /*!\name Get functions for ConstBlockSlice */
  //@{

  //****************************************************************************
  /*!\brief Extract element from a ConstBlockSlice
   * \ingroup blocks
   *
   * \details
   * Extracts the element of the ConstBlockSlice `block_like` whose tag type is
   `Tag`.
   * Fails to compile unless the block has the `Tag` being extracted.
   *
   * \usage
   * The usage is similar to std::get(), shown below
   * \code
     ConstBlockSlice<...> b;
     auto my_tag_data = blocks::get<tags::MyTag>(b);
   * \endcode
   *
   * \tparam Tag Tag to extract
   *
   * \param block_like The block to extract the tag from
   */
  template <typename Tag, typename Plugin>
  inline constexpr decltype(auto) get(
      ConstBlockSlice<Plugin>& block_like) noexcept;
  template <typename Tag, typename Plugin>
  inline constexpr decltype(auto) get(
      ConstBlockSlice<Plugin> const& block_like) noexcept;
  template <typename Tag, typename Plugin>
  inline constexpr decltype(auto) get(
      ConstBlockSlice<Plugin>&& block_like) noexcept;
  template <typename Tag, typename Plugin>
  inline constexpr decltype(auto) get(
      ConstBlockSlice<Plugin> const&& block_like) noexcept;
  //****************************************************************************

  //@}
  //****************************************************************************

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  /*!\brief Define getter for block types
  // \ingroup blocks
  //
  // \details
  // Helper macro for defining get() for block types
   */
#define DEFINE_GETTERS_FOR(BLOCKLIKE)                                   \
                                                                        \
  template <typename Tag, typename Plugin>                              \
  inline constexpr decltype(auto) get(                                  \
      BLOCKLIKE<Plugin>& block_like) noexcept {                         \
    return detail::get_impl<Tag>(detail::tag::is_block_variable<Tag>{}, \
                                 block_like);                           \
  }                                                                     \
                                                                        \
  template <typename Tag, typename Plugin>                              \
  inline constexpr decltype(auto) get(                                  \
      BLOCKLIKE<Plugin> const& block_like) noexcept {                   \
    return detail::get_impl<Tag>(detail::tag::is_block_variable<Tag>{}, \
                                 block_like);                           \
  }                                                                     \
                                                                        \
  template <typename Tag, typename Plugin>                              \
  inline constexpr decltype(auto) get(                                  \
      BLOCKLIKE<Plugin>&& block_like) noexcept {                        \
    return detail::get_impl<Tag>(detail::tag::is_block_variable<Tag>{}, \
                                 std::move(block_like));                \
  }                                                                     \
                                                                        \
  template <typename Tag, typename Plugin>                              \
  inline constexpr decltype(auto) get(                                  \
      BLOCKLIKE<Plugin> const&& block_like) noexcept {                  \
    return detail::get_impl<Tag>(detail::tag::is_block_variable<Tag>{}, \
                                 std::move(block_like));                \
  }

  DEFINE_GETTERS_FOR(Block)
  DEFINE_GETTERS_FOR(BlockSlice)
  DEFINE_GETTERS_FOR(ConstBlockSlice)

#undef DEFINE_GETTERS_FOR

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  /*!\brief Define getter for block types
   * \ingroup blocks
   *
   * \details
   * Helper macro for defining get() for block types
   */
#define DEFINE_INDEXED_GETTERS_FOR(BLOCKLIKE)                              \
                                                                           \
  template <typename Tag, typename Plugin>                                 \
  inline constexpr decltype(auto) get(BLOCKLIKE<Plugin>& block_like,       \
                                      std::size_t index) noexcept {        \
    return detail::get_impl<Tag>(detail::tag::is_block_variable<Tag>{},    \
                                 block_like, index);                       \
  }                                                                        \
                                                                           \
  template <typename Tag, typename Plugin>                                 \
  inline constexpr decltype(auto) get(BLOCKLIKE<Plugin> const& block_like, \
                                      std::size_t index) noexcept {        \
    return detail::get_impl<Tag>(detail::tag::is_block_variable<Tag>{},    \
                                 block_like, index);                       \
  }
  // ! \endcond */
  //****************************************************************************

  //  DEFINE_INDEXED_GETTERS_FOR(Block)
  //  DEFINE_INDEXED_GETTERS_FOR(BlockSlice)
  //  DEFINE_INDEXED_GETTERS_FOR(ConstBlockSlice)

#undef DEFINE_INDEXED_GETTERS_FOR

}  // namespace blocks
