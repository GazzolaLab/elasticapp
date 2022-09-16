//==============================================================================
/*!
//  \file
//  \brief Block view definition
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

#include "Systems/Block/Block/Types.hpp"
//
#include "Systems/Block/Block/AsVariables.hpp"
#include "Systems/Block/Block/BlockSliceFacade.hpp"
#include "Systems/Block/Block/TypeTraits.hpp"
#include "Systems/Block/BlockVariables/TypeTraits.hpp"
//
#include "Utilities/AsConst.hpp"
#include "Utilities/End.hpp"
#include "Utilities/TMPL.hpp"
//
#include <cassert>
#include <utility>

namespace blocks {

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  template <class Plugin>
  inline constexpr auto operator==(BlockView<Plugin> const& lhs,
                                   BlockView<Plugin> const& rhs) noexcept
      -> bool;
  template <class Plugin>
  inline constexpr auto operator==(ConstBlockView<Plugin> const& lhs,
                                   ConstBlockView<Plugin> const& rhs) noexcept
      -> bool;
  // template <class Plugin>
  // inline auto size(BlockView<Plugin> const& view) -> bool;
  // template <class Plugin>
  // inline auto size(ConstBlockView<Plugin> const& view) -> bool;
  /*! \endcond */
  //****************************************************************************

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  // FIXME : replace with symbolic indices if needed.
  //****************************************************************************
  //! Region over a block
  struct Region {
    //! Start index
    std::size_t start;
    //! Region size
    std::size_t size;
  };
  //****************************************************************************

  //**Equality operator*********************************************************
  /*!\brief Equality comparison between two Region objects.
   *
   * \param lhs The left-hand side region.
   * \param rhs The right-hand side region.
   * \return \a true if the regions are over the same space, else \a false
   */
  inline constexpr auto operator==(Region const& lhs,
                                   Region const& rhs) noexcept -> bool {
    // Brings in the tuple header
    // return std::tie(lhs.start, lhs.size) == std::tie(rhs.start, rhs.size);
    return (lhs.start == rhs.start) && (lhs.size == rhs.size);
  }
  /*! \endcond */
  //****************************************************************************

  //============================================================================
  //
  //  CLASS DEFINITION
  //
  //============================================================================

  //****************************************************************************
  /*!\brief Implementation of a view/view on an \elastica Block
   * \ingroup blocks
   *
   * \details
   * The BlockView template class provides a view (or) view into a Block of
   * `data` and `operations`. It also models the `ComputationalBlock` concept
   * and hence can be used across places where a \elastica Block is used as
   * a template parameter.
   *
   * Similar to Block, BlockView can also be customized (specialized) for any
   * Lagrangian entity, but experience suggests that extensive customization is
   * not needed. For a concrete example, please see specializations of
   * BlockView in @ref cosserat_rod.
   *
   * BlockView is different from a BlockSlice : a BlockSlice represents a single
   * physical entity, meanwhile BlockView may represent a group of entities.
   *
   * \usage
   * The intended usage of a BlockView is when a view is required into the data
   * held by a block---this is frequently the case when a user either
   * 1. adds an entity into the Simulator, or
   * 2. requires access (for e.g. reading/writing to disk) to only a portion of
   * the block.
   * The pattern that is most commonly seen in the use case (1) is for a
   * BlockView templated on some `Plugin` type to be only used with a Block
   * of the same `Plugin` type, when adding new entities to the Block.
   * For use case (2) we suggest the user to the blocks::slice() function, which
   * has an intuitive, explicit view syntax, or even the subscript operator[].
   * We note that we might explicitly disable the subscript operator [] for
   * slicing in the future, as the semantics are potentially unclear.
   *
   * Finally, with no additional coding effort, the BlockView has exactly the
   * same operations as the mother Block (aka Block of the same `Plugin` type),
   * but now it operates only on that view of the data. This means that a
   * BlockView can be used as a small Block in itself which greatly simplifies
   * interfacing different components of \elastica---the user need not care or
   * even know about whether the data that she has is a Block or a BlockView!
   *
   * \tparam Plugin The computational plugin modeling a Lagrangian entity
   *
   * \see Block, blocks::slice
   */
  template <class Plugin>
  class BlockView : public detail::BlockSliceFacade<Plugin> {
   protected:
    //**Type definitions********************************************************
    //! Type of the parent plugin
    using Parent = detail::BlockSliceFacade<Plugin>;
    //! Type of the view
    using This = BlockView<Plugin>;
    //**************************************************************************

   public:
    //**Type definitions********************************************************
    //! Type of the parent plugin
    using typename Parent::PluginType;
    //! Slice variables
    using BlockVariableViews = typename Parent::BlockVariableSlices;
    //! Type of the parent block
    using ParentBlock = typename PluginFrom<This>::template to<Block>::type;
    //**************************************************************************

    //**Friendships*************************************************************
    //! Friend the main block
    friend ParentBlock;
    //**************************************************************************

   public:
    //**Constructors************************************************************
    /*!\name Constructors */
    //@{

    //**************************************************************************
    /*!\brief The default constructor.
     *
     * \param parent_block The parent block that
     * \param start_index The start index
     * \param region_size The stop index
     * \param views view to be transferred to the underlying container
     */
    explicit BlockView(
        ParentBlock& parent_block, std::size_t start_index,
        std::size_t region_size,
        BlockVariableViews views) noexcept(noexcept(Parent(std::move(views))))
        : Parent(std::move(views)),
          parent_block_(parent_block),
          region_{start_index, region_size} {}
    //**************************************************************************

    //**************************************************************************
    /*!\brief The copy constructor.
     *
     * \param other view to copy
     */
    BlockView(BlockView const& other)
        : Parent(static_cast<Parent const&>(other)),
          parent_block_(other.parent_block_),
          region_(other.region_){};
    //**************************************************************************

    //**************************************************************************
    /*!\brief The move constructor.
     *
     * \param other view to move from
     */
    BlockView(BlockView&& other) noexcept
        : Parent(static_cast<Parent&&>(other)),
          parent_block_(other.parent_block_),
          region_(std::move(other.region_)){};
    //**************************************************************************

    //@}
    //**************************************************************************

   public:
    //**Destructor**************************************************************
    /*!\name Destructor */
    //@{
    ~BlockView() = default;
    //@}
    //**************************************************************************

    //**Utility methods*********************************************************
    /*!\name Utility methods*/
    //@{

    //**************************************************************************
    /*!\brief Gets the parent block
     */
    inline constexpr auto parent() const noexcept -> ParentBlock& {
      return parent_block_;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Gets the region
     */
    inline constexpr auto region() const noexcept -> Region const& {
      return region_;
    }
    //**************************************************************************

    //@}
    //**************************************************************************

   private:
    //**Member variables********************************************************
    /*!\name Member variables */
    //@{
    //! Reference to the parent block
    ParentBlock& parent_block_;
    //! Region of parent block
    Region region_;
    //@}
    //**************************************************************************
  };
  //****************************************************************************

  //****************************************************************************
  /*!\brief Implementation of a view/view on a const \elastica Block
   * \ingroup blocks
   *
   * \details
   * The ConstBlockView template class provides a view (or) view into a
   * constant Block of \a data and \a operations. It also models the
   * `ComputationalBlock` concept and hence can be used across places where a
   * \elastica Block is used as a template parameter.
   *
   * Notably, it differs from BlockView in one key aspect : the underlying
   * data/view is always constant and cannot be modified (it is only read only).
   * Hence ConstBlock is useful for propagating const-correctness throughout the
   * code. It can be used in places where one needs to pass (const)-data to the
   * user which she can then copy and use it for her own purposes.
   *
   * \tparam Plugin The computational plugin modeling a Lagrangian entity
   *
   * \see BlockView
   */
  template <class Plugin>
  class ConstBlockView : public detail::ConstBlockSliceFacade<Plugin> {
   protected:
    //**Type definitions********************************************************
    //! Type of the parent plugin
    using Parent = detail::ConstBlockSliceFacade<Plugin>;
    //! Type of the view
    using This = ConstBlockView<Plugin>;
    //**************************************************************************

   public:
    //**Type definitions********************************************************
    //! Type of the parent plugin
    using typename Parent::PluginType;
    //! Slice variables
    using BlockVariableViews = typename Parent::BlockVariableSlices;
    //! Type of the parent block
    using ParentBlock = typename PluginFrom<This>::template to<Block>::type;
    //**************************************************************************

    //**Friendships*************************************************************
    //! Friend the main block
    friend ParentBlock;
    //**************************************************************************

   public:
    //**Constructors************************************************************
    /*!\name Constructors */
    //@{

    //**************************************************************************
    /*!\brief The default constructor.
     *
     * \param parent_block The parent block that
     * \param start_index The start index
     * \param region_size The stop index
     * \param views view to be transferred to the underlying container
     */
    explicit ConstBlockView(
        ParentBlock const& parent_block, std::size_t start_index,
        std::size_t region_size,
        BlockVariableViews views) noexcept(noexcept(Parent(std::move(views))))
        : Parent(std::move(views)),
          parent_block_(parent_block),
          region_{start_index, region_size} {}
    //**************************************************************************

    //**************************************************************************
    /*!\brief The copy constructor.
     *
     * \param other view to copy
     */
    ConstBlockView(ConstBlockView const& other)
        : Parent(static_cast<Parent const&>(other)),
          parent_block_(other.parent_block_),
          region_(other.region_){};
    //**************************************************************************

    //**************************************************************************
    /*!\brief The move constructor.
     *
     * \param other view to move from
     */
    ConstBlockView(ConstBlockView&& other) noexcept
        : Parent(static_cast<Parent&&>(other)),
          parent_block_(other.parent_block_),
          region_(std::move(other.region_)){};
    //**************************************************************************

    //@}
    //**************************************************************************

   public:
    //**Destructor**************************************************************
    /*!\name Destructor */
    //@{
    ~ConstBlockView() = default;
    //@}
    //**************************************************************************

    //**Utility methods*********************************************************
    /*!\name Utility methods*/
    //@{

    //**************************************************************************
    /*!\brief Gets the parent block
     */
    inline constexpr auto parent() const noexcept -> ParentBlock const& {
      return parent_block_;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Gets the region
     */
    inline constexpr auto region() const noexcept -> Region const& {
      return region_;
    }
    //**************************************************************************

    //@}
    //**************************************************************************

   private:
    //**Member variables********************************************************
    /*!\name Member variables */
    //@{
    //! Reference to the parent block
    ParentBlock const& parent_block_;
    //! Region of parent block
    Region region_;
    //@}
    //**************************************************************************
  };
  //****************************************************************************

  //============================================================================
  //
  //  GLOBAL OPERATORS
  //
  //============================================================================

  //**Equality operator*********************************************************
  /*!\brief Equality comparison between two BlockView objects.
   *
   * \param lhs The left-hand side view.
   * \param rhs The right-hand side view.
   * \return \a true if the views are same, else \a false
   */
  template <class Plugin>
  inline constexpr auto operator==(BlockView<Plugin> const& lhs,
                                   BlockView<Plugin> const& rhs) noexcept
      -> bool {
    return (&lhs.parent() == &rhs.parent()) and (lhs.region() == rhs.region());
  }
  //****************************************************************************

  //**Equality operator*********************************************************
  /*!\brief Equality comparison between two ConstBlockView objects.
   *
   * \param lhs The left-hand side const view.
   * \param rhs The right-hand side const view.
   * \return \a true if the const views are same, else \a false
   */
  template <class Plugin>
  inline constexpr auto operator==(ConstBlockView<Plugin> const& lhs,
                                   ConstBlockView<Plugin> const& rhs) noexcept
      -> bool {
    return (&lhs.parent() == &rhs.parent()) and (lhs.region() == rhs.region());
  }
  //****************************************************************************

  namespace detail {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief View operator for mutable blocks
     * \ingroup blocks
     *
     * \details
     * Helps take slices of a mutable lvalue reference to block.
     * BlockView<PluginTrait<BlockLike>::template type<BlockView>>
     * PluginFrom<BlockLike>::template to<BlockView>::type;
     */
    template <typename Plugin, typename... Vars,
              typename ReturnType = typename PluginFrom<
                  Block<Plugin>>::template to<BlockView>::type>
    inline decltype(auto) slice_impl(Block<Plugin>& block_like,
                                     std::size_t start_index,
                                     std::size_t region_size,
                                     tmpl::list<Vars...> /* meta */) {
      return ReturnType{
          block_like, start_index, region_size,
          typename ReturnType::BlockVariableViews{
              block_like.template slice<Vars>(start_index, region_size)...}};
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief View operator for constant blocks
     * \ingroup blocks
     *
     * \details
     * Helps take slices of a constant lvalue reference to a block.
     */
    template <typename Plugin, typename... Vars,
              typename ReturnType = const typename PluginFrom<
                  Block<Plugin>>::template to<ConstBlockView>::type>
    inline decltype(auto) slice_impl(Block<Plugin> const& block_like,
                                     std::size_t start_index,
                                     std::size_t region_size,
                                     tmpl::list<Vars...> /* meta */) {
      return ReturnType{
          block_like, start_index, region_size,
          typename ReturnType::BlockVariableViews{
              block_like.template slice<Vars>(start_index, region_size)...}};
    }
    /*! \endcond */
    //**************************************************************************

  }  // namespace detail

  //============================================================================
  //
  //  BLOCK SLICE FUNCTIONS
  //
  //============================================================================

  //**Block slice***************************************************************
  /*!\name Block slice functions */
  //@{

  //****************************************************************************
  /*!\brief View into blocks
   * \ingroup blocks
   *
   * \details
   * Implements contiguous views on entities modeling block concept.
   *
   * \param block_like Data-structure modeling the block concept
   * \param start_index The start index of the slice (int or from_end)
   * \param stop_index The stop index of the slice (int or from_end)
   *
   * \example
   * Given a block or a view `b`, we can obtain a view into it by using
   * \code
   * auto b_view = block::slice(b, 5UL, 10UL); // gets b[5, 6, 7, 8, 9]
   * // Gets b[end - 5, end - 4, end - 3]
   * auto b_view_from_end = block::slice(b, elastica::end - 5UL, elastica::end -
   * 2UL);
   * \endcode
   *
   * \note
   * not marked noexcept because we can have out of range indices
   */
  template <typename Plugin, typename StartIndex, typename StopIndex>
  inline decltype(auto) slice(Block<Plugin>& block_like, StartIndex start_index,
                              StopIndex stop_index) {
    auto const start = units_check(block_like, std::move(start_index));
    // workaround to support slices with last index : stop_index - 1 does not
    // throw in check(), and once we have the numeric index, we add one to
    // recover original index.
    auto const stop =
        units_check(block_like, std::move(stop_index) - 1UL) + 1UL;
    assert(stop > start);
    return detail::slice_impl(block_like, start, stop - start,
                              typename Plugin::Variables{});
  }
  //****************************************************************************

  //****************************************************************************
  template <typename Plugin, typename StartIndex, typename StopIndex>
  inline decltype(auto) slice(Block<Plugin> const& block_like,
                              StartIndex start_index, StopIndex stop_index) {
    auto const start = units_check(block_like, std::move(start_index));
    // workaround to support slices with last index : stop_index - 1 does not
    // throw in check(), and once we have the numeric index, we add one to
    // recover original index.
    auto const stop =
        units_check(block_like, std::move(stop_index) - 1UL) + 1UL;
    assert(stop > start);
    return detail::slice_impl(static_cast<Block<Plugin> const&>(block_like),
                              start, stop - start,
                              typename Plugin::Variables{});
  }
  //****************************************************************************

  namespace detail {
    inline constexpr auto as_index(std::size_t n_units, std::size_t index)
        -> std::size_t {
      // Equality is added here because the stop index is not-inclusive.
      // For example in a blockview of size 5 a sub-view of size a[3:5] is still
      // valid, as the slice only contains a[3], a[4] by the indexing scheme
      // used.
      if (index <= n_units) {
        return index;
      } else {
        throw std::out_of_range(
            "Index to be sliced does not lie within the block-view range");
      }
    }

    inline constexpr auto as_index(std::size_t n_units,
                                   ::elastica::from_end index) -> std::size_t {
      // This is technically incorrect, but for the range of units we work with
      // it should be okay
      return as_index(n_units, n_units - index.i);
    }

  }  // namespace detail

  //****************************************************************************
  template <typename Plugin, typename StartIndex, typename StopIndex>
  inline decltype(auto) slice(BlockView<Plugin>& block_like,
                              StartIndex start_index, StopIndex stop_index) {
    // We should avoid the block check() here as it may contain more elements
    auto const start =
        detail::as_index(block_like.region().size, std::move(start_index));
    // stop_index - 0 to also include (end - 0), the last index
    auto const stop =
        detail::as_index(block_like.region().size, std::move(stop_index) - 0UL);
    assert(stop > start);
    return detail::slice_impl(block_like.parent(),
                              block_like.region().start + start, stop - start,
                              typename Plugin::Variables{});
  }
  //****************************************************************************

  //****************************************************************************
  template <typename Plugin, typename StartIndex, typename StopIndex>
  inline decltype(auto) slice(ConstBlockView<Plugin> const& block_like,
                              StartIndex start_index, StopIndex stop_index) {
    // We should avoid the block check() here as it may contain more elements
    auto const start =
        detail::as_index(block_like.region().size, std::move(start_index));
    // stop_index - 0 to also include (end - 0), the last index
    auto const stop =
        detail::as_index(block_like.region().size, std::move(stop_index) - 0UL);
    assert(stop > start);
    return detail::slice_impl(block_like.parent(),
                              block_like.region().start + start, stop - start,
                              typename Plugin::Variables{});
  }
  //****************************************************************************

  //****************************************************************************
  template <typename Plugin, typename StartIndex, typename StopIndex>
  inline decltype(auto) slice(BlockView<Plugin> const& block_like,
                              StartIndex start_index, StopIndex stop_index) {
    // We should avoid the block check() here as it may contain more elements
    auto const start =
        detail::as_index(block_like.region().size, std::move(start_index));
    // stop_index - 0 to also include (end - 0), the last index
    auto const stop =
        detail::as_index(block_like.region().size, std::move(stop_index) - 0UL);
    assert(stop > start);
    return detail::slice_impl(cpp17::as_const(block_like.parent()),
                              block_like.region().start + start, stop - start,
                              typename Plugin::Variables{});
  }
  //****************************************************************************

  //@}
  //****************************************************************************

  namespace detail {

    //**************************************************************************
    //!\brief Slice operator for block views and const-views
    //!\ingroup blocks
    template <typename B, typename... Vars>
    inline decltype(auto) slice_impl(B&& block_like,
                                     std::size_t numeric_index) {
      return slice_impl(block_like.parent(),
                        block_like.region().start + numeric_index,
                        typename cpp20::remove_cvref_t<B>::Variables{});
    }
    //**************************************************************************

  }  // namespace detail

  //**BlockView slice***********************************************************
  /*!\name BlockView slice functions */
  //@{

  //****************************************************************************
  /*!\brief View into blocks
   * \ingroup blocks
   *
   * \details
   * Implements slices on entities modeling block concept.
   *
   * \param block_like Data-structure modeling the block concept
   * \param index The index of the slice (int or from_end)
   *
   * \example
   * Given a view `b`, we can obtain a slice
   * \code
   * auto b_slice = block::slice(b, 5UL); // at index 5 from start
   * auto b_slice_from_end = block::slice(b, elastica::end - 2UL); // 2 from end
   * \endcode
   *
   * \note
   * not marked noexcept because we can have out of range indices
   */
  template <typename Plugin, typename Index>
  inline decltype(auto) slice(BlockView<Plugin>& block_like, Index index) {
    return detail::slice_impl(
        block_like.parent(),
        block_like.region().start +
            detail::as_index(block_like.region().size, std::move(index)),
        typename Plugin::Variables{});
  }
  //****************************************************************************

  //****************************************************************************
  template <typename Plugin, typename Index>
  inline decltype(auto) slice(ConstBlockView<Plugin> const& block_like,
                              Index index) {
    return detail::slice_impl(
        block_like.parent(),
        block_like.region().start +
            detail::as_index(block_like.region().size, std::move(index)),
        typename Plugin::Variables{});
  }
  //****************************************************************************

  //****************************************************************************
  template <typename Plugin, typename Index>
  inline decltype(auto) slice(BlockView<Plugin> const& block_like,
                              Index index) {
    return detail::slice_impl(
        cpp17::as_const(block_like.parent()),
        block_like.region().start +
            detail::as_index(block_like.region().size, std::move(index)),
        typename Plugin::Variables{});
  }
  //****************************************************************************

  //@}
  //****************************************************************************

}  // namespace blocks