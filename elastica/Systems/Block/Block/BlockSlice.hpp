//==============================================================================
/*!
//  \file
//  \brief Block slice definition
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

namespace blocks {

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  template <class Plugin>
  //, typename Parent = detail::BlockSliceFacade<Plugin>>
  inline constexpr auto operator==(
      BlockSlice<Plugin> const& lhs,
      BlockSlice<Plugin> const&
          rhs)  // noexcept(noexcept(detail::blocks_equality_impl(lhs,
                // rhs)))
      /*
      noexcept(noexcept(detail::
                                     blocks_equality_impl(
                                         static_cast<Parent const&>(lhs),
                                         static_cast<Parent const&>(rhs))))
                                         */
      -> bool;
  template <class Plugin>
  // typename Parent = detail::ConstBlockSliceFacade<Plugin>>
  inline constexpr auto operator==(
      ConstBlockSlice<Plugin> const& lhs,
      ConstBlockSlice<Plugin> const&
          rhs)  // noexcept(noexcept(detail::blocks_equality_impl(lhs, rhs)))
      /*
      noexcept(noexcept(detail::
                                     blocks_equality_impl(
                                         static_cast<Parent const&>(lhs),
                                         static_cast<Parent const&>(rhs))))
                                         */
      -> bool;
  /*! \endcond */
  //****************************************************************************

  //============================================================================
  //
  //  CLASS DEFINITION
  //
  //============================================================================

  //****************************************************************************
  /*!\brief Implementation of a slice/view on an \elastica Block
   * \ingroup blocks
   *
   * \details
   * The BlockSlice template class provides a slice (or) view into a Block of
   * `data` and `operations`. It also models the `ComputationalBlock` concept
   * and hence can be used across places where a \elastica Block is used as
   * a template parameter.
   *
   * Similar to Block, BlockSlice can also be customized (specialized) for any
   * Lagrangian entity, but experience suggests that extensive customization is
   * not needed. For a concrete example, please see specializations of
   * BlockSlice in @ref cosserat_rod.
   *
   * \usage
   * The intended usage of a BlockSlice is when a view is required into the data
   * held by a block---this is frequently the case when a user either
   * 1. adds an entity into the Simulator, or
   * 2. requires access (for e.g. reading/writing to disk) to only a portion of
   * the block.
   * The pattern that is most commonly seen in the use case (1) is for a
   * BlockSlice templated on some `Plugin` type to be only used with a Block
   * of the same `Plugin` type, when adding new entities to the Block.
   * For use case (2) we suggest the user to the blocks::slice() function, which
   * has an intuitive, explicit slice syntax, or even the subscript operator[].
   * We note that we might explicitly disable the subscript operator [] for
   * slicing in the future, as the semantics are potentially unclear.
   *
   * Finally, with no additional coding effort, the BlockSlice has exactly the
   * same operations as the mother Block (aka Block of the same `Plugin` type),
   * but now it operates only on that slice of the data. This means that a
   * BlockSlice can be used as a small Block in itself which greatly simplifies
   * interfacing different components of \elastica---the user need not care or
   * even know about whether the data that she has is a Block or a BlockSlice!
   * For example,
   * \code
   *     // ... make simulator etc ...
   *     auto my_rod = simulator.emplace_back<CosseratRod>( * ...args... *);
   *     // the args go and form a Block
   *     // which in turn returns a BlockSlice
   *     // which the user gets as my_rod
   *
   *     // use my_rod like a regular cosserat rod
   *     simulator->constrain(my_rod)->using<SomeConstraint>( *...args... *);
   * \endcode
   * This abstraction helped us constrain \c my_rod, embedded in a Block of
   * data, using \c SomeConstraint just like any non-Blocked item of the
   * \elastica library.
   *
   * \tparam Plugin The computational plugin modeling a Lagrangian entity
   *
   * \see Block, blocks::slice
   */
  template <class Plugin>
  class BlockSlice : public detail::BlockSliceFacade<Plugin> {
   protected:
    //**Type definitions********************************************************
    //! Type of the parent plugin
    using Parent = detail::BlockSliceFacade<Plugin>;
    //! Type of the slice
    using This = BlockSlice<Plugin>;
    //**************************************************************************

   public:
    //**Type definitions********************************************************
    //! Type of the parent plugin
    using typename Parent::PluginType;
    //! Slice variables
    using typename Parent::BlockVariableSlices;
    //! Type of the parent block
    using ParentBlock = typename PluginFrom<This>::template to<Block>::type;
    //**************************************************************************

    //**Friendships*************************************************************
    //! Friend the main block
    friend ParentBlock;
    //! Friend the equality operator
    friend bool operator==
        <Plugin>(BlockSlice<Plugin> const& lhs, BlockSlice<Plugin> const& rhs);
    // noexcept(noexcept(detail::blocks_equality_impl(lhs, rhs)));
    //**************************************************************************

   public:
    //**Constructors************************************************************
    /*!\name Constructors */
    //@{

    //**************************************************************************
    /*!\brief The default constructor.
     *
     * \param slices slices to be transferred to the underlying container
     */
    explicit BlockSlice(
        ParentBlock& parent_block,
        BlockVariableSlices
            slices) noexcept(noexcept(Parent(std::move(slices))))
        : Parent(std::move(slices)), parent_block_(parent_block) {}
    //**************************************************************************

    //**************************************************************************
    /*!\brief The copy constructor.
     *
     * \param other slice to copy
     */
    BlockSlice(BlockSlice const& other)
        : Parent(static_cast<Parent const&>(other)),
          parent_block_(other.parent_block_){};
    //**************************************************************************

    //**************************************************************************
    /*!\brief The move constructor.
     *
     * \param other slice to move from
     */
    BlockSlice(BlockSlice&& other) noexcept
        : Parent(static_cast<Parent&&>(other)),
          parent_block_(other.parent_block_){};
    //**************************************************************************

    //@}
    //**************************************************************************

   public:
    //**Destructor**************************************************************
    /*!\name Destructor */
    //@{
    ~BlockSlice() = default;
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

    //@}
    //**************************************************************************

   private:
    //**Member variables********************************************************
    /*!\name Member variables */
    //@{
    //! Reference to the parent block
    ParentBlock& parent_block_;
    //@}
    //**************************************************************************
  };
  //****************************************************************************

  //****************************************************************************
  /*!\brief Implementation of a slice/view on a const \elastica Block
   * \ingroup blocks
   *
   * \details
   * The ConstBlockSlice template class provides a slice (or) view into a
   * constant Block of \a data and \a operations. It also models the
   * `ComputationalBlock` concept and hence can be used across places where a
   * \elastica Block is used as a template parameter.
   *
   * Notably, it differs from BlockSlice in one key aspect : the underlying
   * data/view is always constant and cannot be modified (it is only read only).
   * Hence ConstBlock is useful for propagating const-correctness throughout the
   * code. It can be used in places where one needs to pass (const)-data to the
   * user which she can then copy and use it for her own purposes.
   *
   * \tparam Plugin The computational plugin modeling a Lagrangian entity
   *
   * \see BlockSlice
   */
  template <class Plugin>
  class ConstBlockSlice : public detail::ConstBlockSliceFacade<Plugin> {
   protected:
    //**Type definitions********************************************************
    //! Type of the parent plugin
    using Parent = detail::ConstBlockSliceFacade<Plugin>;
    //! Type of the slice
    using This = ConstBlockSlice<Plugin>;
    //**************************************************************************

   public:
    //**Type definitions********************************************************
    //! Type of the parent plugin
    using typename Parent::PluginType;
    //! Slice variables
    using typename Parent::BlockVariableSlices;
    //! Type of the parent block
    using ParentBlock = typename PluginFrom<This>::template to<Block>::type;
    //**************************************************************************

    //**Friendships*************************************************************
    //! Friend the main block
    friend ParentBlock;
    //! Friend the equality operator
    friend bool operator==<Plugin>(ConstBlockSlice<Plugin> const& lhs,
                                   ConstBlockSlice<Plugin> const& rhs);
    // noexcept(noexcept(detail::blocks_equality_impl(lhs, rhs)));
    //**************************************************************************

   public:
    //**Constructors************************************************************
    /*!\name Constructors */
    //@{

    //**************************************************************************
    /*!\brief The default constructor.
     *
     * \param slices slices to be transferred to the underlying container
     */
    explicit ConstBlockSlice(
        ParentBlock const& parent_block,
        BlockVariableSlices
            slices) noexcept(noexcept(Parent(std::move(slices))))
        : Parent(std::move(slices)), parent_block_(parent_block) {}
    //**************************************************************************

    //**************************************************************************
    /*!\brief The copy constructor.
     *
     * \param other slice to copy
     */
    ConstBlockSlice(ConstBlockSlice const& other)
        : Parent(static_cast<Parent const&>(other)),
          parent_block_(other.parent_block_){};
    //**************************************************************************

    //**************************************************************************
    /*!\brief The copy constructor.
     *
     * \param other slice to copy
     */
    ConstBlockSlice(ConstBlockSlice& other)
        : Parent(static_cast<Parent&>(other)),
          parent_block_(other.parent_block_){};
    //**************************************************************************

    //**************************************************************************
    /*!\brief The move constructor.
     *
     * \param other slice to move from
     */
    ConstBlockSlice(ConstBlockSlice&& other) noexcept
        : Parent(static_cast<Parent&&>(other)),
          parent_block_(other.parent_block_){};
    //**************************************************************************

    //@}
    //**************************************************************************

   public:
    //**Destructor**************************************************************
    /*!\name Destructor */
    //@{
    ~ConstBlockSlice() = default;
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

    //@}
    //**************************************************************************

   private:
    //**Member variables********************************************************
    /*!\name Member variables */
    //@{
    //! Reference to the parent block
    ParentBlock const& parent_block_;
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
  /*!\brief Equality comparison between two BlockSlice objects.
   *
   * \param lhs The left-hand side slice.
   * \param rhs The right-hand side slice.
   * \return \a true if the slices are same, else \a false
   */
  template <class Plugin>
  inline constexpr auto operator==(
      BlockSlice<Plugin> const& lhs,
      BlockSlice<Plugin> const&
          rhs)  // noexcept(noexcept(detail::blocks_equality_impl(lhs,
                // rhs)))
      -> bool {
    // using Parent = typename BlockSlice<Plugin>::Parent;
    // short circuit if the parent blocks dont match
    return (&lhs.parent() == &rhs.parent()) and
           detail::blocks_equality_impl(lhs, rhs);
  }
  //****************************************************************************

  //**Equality operator*********************************************************
  /*!\brief Equality comparison between two ConstBlockSlice objects.
   *
   * \param lhs The left-hand side const slice.
   * \param rhs The right-hand side const slice.
   * \return \a true if the const slices are same, else \a false
   */
  template <class Plugin>
  inline constexpr auto operator==(
      ConstBlockSlice<Plugin> const& lhs,
      ConstBlockSlice<Plugin> const&
          rhs)  // noexcept(noexcept(detail::blocks_equality_impl(lhs, rhs)))
      -> bool {
    // using Parent = typename ConstBlockSlice<Plugin>::Parent;
    // short circuit if the parent blocks dont match
    return (&lhs.parent() == &rhs.parent()) and
           detail::blocks_equality_impl(lhs, rhs);
  }
  //****************************************************************************

  namespace detail {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Slice operator for mutable blocks
     * \ingroup blocks
     *
     * \details
     * Helps take slices of a mutable lvalue reference to block.
     * BlockSlice<PluginTrait<BlockLike>::template type<BlockSlice>>
     * PluginFrom<BlockLike>::template to<BlockSlice>::type;
     */
    template <typename Plugin, typename... Vars,
              typename ReturnType = typename PluginFrom<
                  Block<Plugin>>::template to<BlockSlice>::type>
    inline constexpr decltype(auto) slice_impl(
        Block<Plugin>& block_like, std::size_t index,
        tmpl::list<Vars...> /* meta */) noexcept(noexcept(ReturnType{
        block_like, typename ReturnType::BlockVariableSlices{
                        block_like.template slice<Vars>(index)...}})) {
      return ReturnType{block_like,
                        typename ReturnType::BlockVariableSlices{
                            block_like.template slice<Vars>(index)...}};
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Slice operator for constant blocks
     * \ingroup blocks
     *
     * \details
     * Helps take slices of a constant lvalue reference to a block.
     */
    template <typename Plugin, typename... Vars,
              typename ReturnType = const typename PluginFrom<
                  Block<Plugin>>::template to<ConstBlockSlice>::type>
    inline constexpr decltype(auto) slice_impl(
        Block<Plugin> const& block_like, std::size_t index,
        tmpl::list<Vars...> /* meta */) noexcept(noexcept(ReturnType{
        block_like, typename ReturnType::BlockVariableSlices{
                        block_like.template slice<Vars>(index)...}})) {
      return ReturnType{block_like,
                        typename ReturnType::BlockVariableSlices{
                            block_like.template slice<Vars>(index)...}};
    }
    /*! \endcond */
    //**************************************************************************

  }  // namespace detail

  //============================================================================
  //
  //  BLOCK SLICE FUNCTION
  //
  //============================================================================

  //**Block slice***************************************************************
  /*!\name Block slice functions */
  //@{

  //****************************************************************************
  /*!\brief Slice operator for blocks
   * \ingroup blocks
   *
   * \details
   * Implements slices on entities modeling block concept.
   *
   * \param block_like Data-structure modeling the block concept
   * \param index The index of the slice (int or from_end)
   *
   * \example
   * Given a block `b`, we can slice it using
   * \code
   * auto b_slice = block::slice(b, 5UL); // at index 5 from start
   * auto b_slice_from_end = block::slice(b, elastica::end - 2UL); // 2 from end
   * \endcode
   *
   * \note
   * not marked noexcept because we can have out of range indices
   */
  template <typename Plugin, typename Index>
  inline decltype(auto) slice(Block<Plugin>& block_like,
                              Index index) /*noexcept*/ {
    return detail::slice_impl(block_like, block_like.check(index),
                              typename Plugin::Variables{});
  }
  //****************************************************************************

  //****************************************************************************
  template <typename Plugin, typename Index>
  inline decltype(auto) slice(Block<Plugin> const& block_like,
                              Index index) /*noexcept*/ {
    return detail::slice_impl(static_cast<Block<Plugin> const&>(block_like),
                              block_like.check(index),
                              typename Plugin::Variables{});
  }
  //****************************************************************************

  //@}
  //****************************************************************************

}  // namespace blocks
