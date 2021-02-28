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

#include "Systems/Block/Block/AsVariables.hpp"
#include "Systems/Block/Block/TypeTraits.hpp"
#include "Systems/Block/Block/Types.hpp"
#include "Systems/Block/BlockVariables/TypeTraits.hpp"

namespace blocks {

  //============================================================================
  //
  //  CLASS DEFINITION
  //
  //============================================================================

  //****************************************************************************
  /*!\brief Metafunction to obtain slice types of a typelist of block variables
  // \ingroup blocks
  //
  // \details
  // The as_slices template type helps obtain the slice types of a typelist of
  // declared blocks::Variable, used as shown below. The resulting type contains
  // nested types `slices` and `const_slices` which are typelists containing
  // the slice types and constant slice types of the blocks::Variable.
  //
  // \example
  // \code
      // ... Declare variables Var1, Var2, Var3

      // Typelist of all variable slices
      using VariablesList = tmpl::list<Var1, Var2, Var3>;

      // Type containing information about slices
      using VariableSlicesInformation = as_slices<VariablesList>;

      // TypeList of slices of all variables
      using VariableSlices = typename VariableSlicesInformation::slices;

      // TypeList of constant slices of all variables
      using VariableConstSlices
        = typename VariableSlicesInformation::const_slices;

     \endcode
  //
  // \tparam L typelist of block::Variables
  //
  // \see blocks::Variables, Block
  */
  template <typename L>
  struct as_slices {
    //**Type definitions********************************************************
    //! Typelist of slices
    using slices = tmpl::transform<L, tmpl::bind<slice_type_t, tmpl::_1>>;
    //! Typelist of const slices
    using const_slices =
        tmpl::transform<L, tmpl::bind<const_slice_type_t, tmpl::_1>>;
    //**************************************************************************
  };
  //****************************************************************************

  //****************************************************************************
  /*!\brief Implementation of a slice/view on an \elastica Block
   * \ingroup blocks
   *
   * \details
   * The BlockSlice template class provides a slice (or) view into a Block of
   * \a data and \a operations. It also models the `ComputationalBlock` concept
   * and hence can be used across places where a \elastica Block is used as
   * a template parameter.
   *
   * Similar to Block, BlockSlice can also be customized (specialized) for any
   * Lagrangian entity, but experience suggests that extensive customization is
   * not needed. For a concrete example, please see specializations of
   * BlockSlice in @ref cosserat_rod
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
     \code
         // ... make simulator etc ...
         auto my_rod = simulator.emplace_back<CosseratRod>( * ...args... *);
         // the args go and form a Block
         // which in turn returns a BlockSlice
         // which the user gets as my_rod

         // use my_rod like a regular cosserat rod
         simulator->constrain(my_rod)->using<SomeConstraint>( *...args... *);
     \endcode
   * This abstraction helped us constrain \c my_rod, embedded in a Block of
   * data, using \c SomeConstraint just like any non-Blocked item of the
   * \elastica library.
   *
   * \tparam Plugin The computational plugin modeling a Lagrangian entity
   *
   * \see Block, blocks::slice
  */
  template <class Plugin>
  class BlockSlice : public Plugin {
   protected:
    //**Type definitions********************************************************
    //! Type of the parent plugin
    using Parent = Plugin;
    //! Type of the slice
    using This = BlockSlice<Parent>;
    //**************************************************************************

    //**Friendships*************************************************************
    //! Friend the plugin
    friend Parent;
    //! Friend the main block too
    friend class Block<Parent>;
    //**************************************************************************

   public:
    //**Type definitions********************************************************
    //! List of variables
    using typename Parent::Variables;
    //! Typelist of all variable slices
    using VariableSlicesList = typename as_slices<Variables>::slices;
    //! Values of all variable slices
    using BlockVariableSlices = as_block_variables<VariableSlicesList>;
    //! Conformant mapping between tags and variables
    using VariableMap = VariableMapping<VariableSlicesList>;
    //**************************************************************************

   public:
    //**Constructors************************************************************
    /*!\name Constructors */
    //@{

    //**************************************************************************
    /*!\brief The default constructor.
     *
     * \param args... slice types to be transferred to the underlying container
     */
    template <typename... Args>
    explicit BlockSlice(Args&&... args) noexcept(
        noexcept(BlockVariableSlices(std::forward<Args>(args)...)))
        : slices_(std::forward<Args>(args)...){};
    //**************************************************************************

    //**************************************************************************
    /*!\brief The copy constructor.
     *
     * \param other slice to copy
     */
    BlockSlice(BlockSlice const& other) : slices_(other.slices_){};
    //**************************************************************************

    //**************************************************************************
    /*!\brief The move constructor.
     *
     * \param other slice to move from
     */
    BlockSlice(BlockSlice&& other) noexcept
        : slices_(std::move(other.slices_)){};
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

    //**Data access*************************************************************
    /*!\name Data access */
    //@{

    //**************************************************************************
    /*!\brief Access to the underlying data
    //
    // \return Mutable lvalue reference to the underlying data
    */
    inline constexpr BlockVariableSlices& data() & noexcept { return slices_; }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Access to the underlying data
    //
    // \return Constant lvalue reference to the underlying data
    */
    inline constexpr BlockVariableSlices const& data() const& noexcept {
      return slices_;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Access to the underlying data
    //
    // \return Mutable rvalue reference to the underlying data
    */
    inline constexpr BlockVariableSlices&& data() && noexcept {
      return static_cast<BlockVariableSlices&&>(slices_);
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Access to the underlying data
    //
    // \return Const rvalue reference to the underlying data
    */
    inline constexpr BlockVariableSlices const&& data() const&& noexcept {
      return static_cast<BlockVariableSlices const&&>(slices_);
    }
    //**************************************************************************

    //@}
    //**************************************************************************

   private:
    //**Member variables********************************************************
    /*!\name Member variables */
    //@{
    //! All slices of Block variables
    BlockVariableSlices slices_;
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
  template <class Plugin>  // The computational plugin type
  class ConstBlockSlice : public Plugin {
   protected:
    //**Type definitions********************************************************
    //! Type of the parent plugin
    using Parent = Plugin;
    //! Type of the slice
    using This = ConstBlockSlice<Parent>;
    //**************************************************************************

    //**Friendships*************************************************************
    //! Friend the plugin
    friend Parent;
    //! Friend the main block too
    friend class Block<Parent>;
    //**************************************************************************

   public:
    //**Type definitions********************************************************
    //! List of variables
    using typename Parent::Variables;
    //! Typelist of all variable slices
    using VariableSlicesList = typename as_slices<Variables>::const_slices;
    //! Values of all variable slices
    using BlockVariableSlices = as_block_variables<VariableSlicesList>;
    //! Conformant mapping between tags and variables
    using VariableMap = VariableMapping<VariableSlicesList>;
    //**************************************************************************

   public:
    //**Constructors************************************************************
    /*!\name Constructors */
    //@{

    //**************************************************************************
    /*!\brief The default constructor.
     *
     * \param value reference to the underlying callable, copied into func
     */
    template <typename... Args>
    explicit ConstBlockSlice(Args&&... args) noexcept(
        noexcept(BlockVariableSlices(std::forward<Args>(args)...)))
        : slices_(std::forward<Args>(args)...){};
    //**************************************************************************

    //**************************************************************************
    /*!\brief The copy constructor.
     *
     * \param other slice to copy
     */
    ConstBlockSlice(ConstBlockSlice const& other) : slices_(other.slices_){};
    //**************************************************************************

    //**************************************************************************
    /*!\brief The move constructor.
     *
     * \param other slice to move from
     */
    ConstBlockSlice(ConstBlockSlice&& other) noexcept
        : slices_(std::move(other.slices_)){};
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

    //**Data access*************************************************************
    /*!\name Data access */
    //@{

    //**************************************************************************
    /*!\brief Access to the underlying data
    //
    // \return Constant lvalue reference to the underlying data
    */
    inline constexpr BlockVariableSlices const& data() const& noexcept {
      return slices_;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Access to the underlying data
    //
    // \return Const rvalue reference to the underlying data
    */
    inline constexpr BlockVariableSlices const&& data() const&& noexcept {
      return static_cast<BlockVariableSlices const&&>(slices_);
    }
    //**************************************************************************

    //@}
    //**************************************************************************

   private:
    //**Member variables********************************************************
    /*!\name Member variables */
    //@{
    //! All slices of constant Block variables
    BlockVariableSlices slices_;
    //@}
    //**************************************************************************
  };
  //****************************************************************************

  namespace detail {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Slice operator for mutable blocks
    // \ingroup blocks
    //
    // \details
    // Helps take slices of a mutable lvalue reference to block.
    */
    template <typename BlockLike, typename... Vars,
              typename ReturnType = BlockSlice<plugin_t<BlockLike>>>
    inline constexpr decltype(auto) slice_impl(
        BlockLike& block_like, std::size_t index,
        tmpl::list<Vars...> /* meta */) noexcept(noexcept(ReturnType{
        block_like.template slice<Vars>(index)...})) {
      return ReturnType{block_like.template slice<Vars>(index)...};
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Slice operator for constant blocks
    // \ingroup blocks
    //
    // \details
    // Helps take slices of a constant levalue reference to a block.
    */
    template <typename BlockLike, typename... Vars,
              typename ReturnType = const ConstBlockSlice<plugin_t<BlockLike>>>
    inline constexpr decltype(auto) slice_impl(
        BlockLike const& block_like, std::size_t index,
        tmpl::list<Vars...> /* meta */) noexcept(noexcept(ReturnType{
        block_like.template slice<Vars>(index)...})) {
      return ReturnType{block_like.template slice<Vars>(index)...};
    }
    /*! \endcond */
    //**************************************************************************

  }  // namespace detail

  //============================================================================
  //
  //  BLOCK SLICE FUNCTION
  //
  //============================================================================

  //****************************************************************************
  /*!\brief Slice operator for blocks
  // \ingroup blocks
  //
  // \details
  // Implements slices/views on entities modeling block concept.
  //
  // \param block_like Data-structure modeling the block concept
  // \param index The index of the slice (int or from_end)
  //
  // \example
  // Given a block `b`, we can slice it using
  // \code
     auto b_slice = block::slice(b, 5UL); // at index 5 from start
     auto b_slice_from_end = block::slice(b, elastica::end - 2UL); // 2 from end
     \endcode
  //
  // \note
  // not marked noexcept because we can have out of range indices
  */
  template <typename BlockLike, typename Index>
  inline constexpr decltype(auto) slice(BlockLike&& block_like,
                                        Index index) /*noexcept*/ {
    using Block_t = std::decay_t<BlockLike>;
    static_assert(
        ModelsBlockConcept<Block_t>::value,
        "Could not slice as the type passed in is not"
        "a valid elastica Block type.");
    return detail::slice_impl(std::forward<BlockLike>(block_like),
                              block_like.check(index),
                              typename Block_t::Variables{});
  }
  //****************************************************************************

}  // namespace blocks
