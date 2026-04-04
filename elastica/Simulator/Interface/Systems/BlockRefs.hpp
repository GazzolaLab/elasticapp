//==============================================================================
/*!
//  \file
//  \brief Implementation of reference generators for Blocks
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include "Systems/Block.hpp"
//
#include "Utilities/GSL.hpp"
//
#include <vector>

namespace blocks {

  inline std::size_t cantor_pair(std::size_t a, std::size_t b) noexcept {
    return (a + b) * (a + b + 1) / 2 + a;
  }

  template <typename GeneratorLike>
  class BlockGenerator
      : public elastica::CRTPHelper<GeneratorLike, BlockGenerator> {
   private:
    //**Type definitions********************************************************
    //! CRTP Type
    using CRTP = elastica::CRTPHelper<GeneratorLike, BlockGenerator>;
    //**************************************************************************

   public:
    //**Self methods************************************************************
    //! CRTP methods
    using CRTP::self;
    //**************************************************************************
  };

  //**************************************************************************
  template <typename GeneratorLike>
  decltype(auto) generate_from(
      BlockGenerator<GeneratorLike> const& generator) /*noexcept*/ {
    return generator.self().get();
  }
  //**************************************************************************

  template <typename Plugin>
  class BlockRefGenerator : public Spannable<BlockRefGenerator<Plugin>>,
                            public BlockGenerator<BlockRefGenerator<Plugin>> {
   private:
    //**Type definitions********************************************************
    //! Type of this
    using This = BlockRefGenerator<Plugin>;
    //! Type of sliceable
    using SpanAffordance = Spannable<This>;
    //! Type of generator
    using GeneratorAffordance = BlockGenerator<This>;
    //**************************************************************************

   public:
    using BlockType = Block<Plugin>;
    using Source = std::vector<BlockType>;
    // Cannot use ref wrapper, raw pointer needed to store invalid references
    using Pointer = Source*;

    BlockRefGenerator()
        : SpanAffordance(),
          GeneratorAffordance(),
          src_(nullptr),
          index_(std::numeric_limits<std::size_t>::max()) {}

    BlockRefGenerator(Source& v, std::size_t index)
        : SpanAffordance(), GeneratorAffordance(), src_(&v), index_(index){};

    auto source() const noexcept -> Pointer { return src_; }
    auto index() const noexcept -> std::size_t { return index_; }
    auto parent() & -> This& { return *this; }
    auto parent() const& -> This const& { return *this; }

    using type = BlockType;
    inline auto get() const /* noexcept*/ -> type& {
      gsl::not_null<Pointer> ptr_to_collection(src_);
      return (*ptr_to_collection)[index_];
    }

    // note : I could use std::hash() here and avoid this uuid() function
    // definition. But I am pretty sure this is an imperfect hash.
    // https://en.wikipedia.org/wiki/Pairing_function#Inverting_the_Cantor_pairing_function
    // for now its okay as it does what we want.
    //! Returns uuid of a block
    inline std::size_t uuid() const {
      gsl::not_null<Pointer> ptr_to_collection(src_);
      return cantor_pair(
          reinterpret_cast<uintptr_t>(std::addressof(*ptr_to_collection)),
          index_);
    }

   public:
    //**Access operators********************************************************
    /*!\name Access operators */
    //@{
    //! Operator for slicing and viewing
    using SpanAffordance::operator[];
    //@}
    //**************************************************************************

   private:
    Pointer src_;
    std::size_t index_;
  };
  //****************************************************************************

  template <class Plugin>
  inline constexpr auto operator==(
      BlockRefGenerator<Plugin> const& lhs,
      BlockRefGenerator<Plugin> const& rhs) noexcept -> bool {
    return (lhs.source() == rhs.source()) and (lhs.index() == rhs.index());
  }

  template <class Plugin>
  inline constexpr auto operator!=(
      BlockRefGenerator<Plugin> const& lhs,
      BlockRefGenerator<Plugin> const& rhs) noexcept -> bool {
    return not(lhs == rhs);
  }
  template <class Plugin>
  inline constexpr auto operator<(BlockRefGenerator<Plugin> const& lhs,
                                  BlockRefGenerator<Plugin> const& rhs) noexcept
      -> bool {
    // https://stackoverflow.com/a/9086675
    // Relational ordering for std::less always returns the right result
    return std::less_equal<typename BlockRefGenerator<Plugin>::Pointer>{}(
               lhs.source(), rhs.source()) and
           (lhs.index() < rhs.index());
  }

  template <class Plugin>
  inline constexpr auto operator>(BlockRefGenerator<Plugin> const& lhs,
                                  BlockRefGenerator<Plugin> const& rhs) noexcept
      -> bool {
    return (rhs < lhs);
  }

  template <class Plugin>
  inline constexpr auto operator<=(
      BlockRefGenerator<Plugin> const& lhs,
      BlockRefGenerator<Plugin> const& rhs) noexcept -> bool {
    return not(lhs > rhs);
  }

  template <class Plugin>
  inline constexpr auto operator>=(
      BlockRefGenerator<Plugin> const& lhs,
      BlockRefGenerator<Plugin> const& rhs) noexcept -> bool {
    return not(lhs < rhs);
  }

  template <typename Plugin>
  class BlockViewGenerator : public Spannable<BlockViewGenerator<Plugin>>,
                             public BlockGenerator<BlockViewGenerator<Plugin>> {
   private:
    //**Type definitions********************************************************
    //! Type of this
    using This = BlockViewGenerator<Plugin>;
    //! Type of sliceable
    using SpanAffordance = Spannable<This>;
    //! Type of generator
    using GeneratorAffordance = BlockGenerator<This>;
    //**************************************************************************

   public:  // in case some metavariables are needed.
    using Parent = BlockRefGenerator<Plugin>;
    using BlockType = typename Parent::BlockType;

    BlockViewGenerator()
        : SpanAffordance(),
          GeneratorAffordance(),
          parent_(),
          region_{std::numeric_limits<std::size_t>::max(),
                  std::numeric_limits<std::size_t>::max()} {}

    BlockViewGenerator(Parent parent, std::size_t slice_index,
                       std::size_t slice_size)
        : SpanAffordance(),
          GeneratorAffordance(),
          parent_(std::move(parent)),
          region_{slice_index, slice_size} {};

    BlockViewGenerator(BlockViewGenerator const& other) = default;
    //
    BlockViewGenerator& operator=(BlockViewGenerator const& other) = default;

   public:
    // We move away from iterator semantics here and give a reference
    // wrapper semantics instead. To form an iterator, we use the function
    //**************************************************************************
    //! Type of the slice
    using type = typename PluginFrom<typename Parent::type>::template to<
        BlockView>::type;

    auto parent() & -> Parent& { return parent_; }
    auto parent() const& -> Parent const& { return parent_; }

    // generates a slice
    /* implicit conversion to a blockslice */
    inline operator type() const /* noexcept*/ { return this->get(); }
    inline auto get() const /* noexcept*/ -> type {
      // The call to .get() does not return a temporary lvalue, so it is safe
      // to propogate.
      // This may throw for now, but can be refactored to not throw.
      using blocks::slice;
      return slice(parent_.get(), region().start,
                   region().start + region().size);
    }
    inline std::size_t uuid() const { return parent_.uuid(); }

    //**************************************************************************
    /*!\brief Returns the slice region
     */
    inline constexpr auto region() const noexcept -> detail::Region const& {
      return region_;
    }
    //**************************************************************************

   public:
    //**Access operators********************************************************
    /*!\name Access operators */
    //@{
    //! Operator for slicing and viewing
    using SpanAffordance::operator[];
    //@}
    //**************************************************************************

   private:
    Parent parent_;
    //! View region
    detail::Region region_;
  };
  //****************************************************************************

  template <class Plugin>
  inline constexpr auto operator==(
      BlockViewGenerator<Plugin> const& lhs,
      BlockViewGenerator<Plugin> const& rhs) noexcept -> bool {
    return (lhs.parent() == rhs.parent()) and (lhs.region() == rhs.region());
  }

  template <class Plugin>
  inline constexpr auto operator!=(
      BlockViewGenerator<Plugin> const& lhs,
      BlockViewGenerator<Plugin> const& rhs) noexcept -> bool {
    return not(lhs == rhs);
  }

  template <typename Plugin>
  class BlockSliceGenerator
      : public BlockGenerator<BlockSliceGenerator<Plugin>> {
   private:
    //**Type definitions********************************************************
    //! Type of this
    using This = BlockSliceGenerator<Plugin>;
    //! Type of generator
    using GeneratorAffordance = BlockGenerator<This>;
    //**************************************************************************

   public:  // in case some metavariables are needed.
    using Parent = BlockRefGenerator<Plugin>;
    using BlockType = typename Parent::BlockType;

    BlockSliceGenerator()
        : GeneratorAffordance(),
          parent_(),
          slice_index_(std::numeric_limits<std::size_t>::max()) {}

    BlockSliceGenerator(Parent parent, std::size_t slice_index)
        : GeneratorAffordance(),
          parent_(std::move(parent)),
          slice_index_(slice_index){};

    BlockSliceGenerator(BlockSliceGenerator const& other) = default;
    //
    BlockSliceGenerator& operator=(BlockSliceGenerator const& other) = default;

   public:
    // We move away from iterator semantics here and give a reference
    // wrapper semantics instead. To form an iterator, we use the function
    //**************************************************************************
    //! Type of the slice
    using type = typename PluginFrom<typename Parent::type>::template to<
        BlockSlice>::type;

    auto parent() & -> Parent& { return parent_; }
    auto parent() const& -> Parent const& { return parent_; }
    auto index() const noexcept -> std::size_t { return slice_index_; }

    // generates a slice
    /* implicit conversion to a blockslice */
    inline operator type() const /* noexcept*/ { return this->get(); }
    inline auto get() const /* noexcept*/ -> type {
      // The call to .get() does not return a temporary lvalue, so it is safe
      // to propogate.
      using blocks::slice;
      return slice(parent_.get(), slice_index_);
    }
    inline std::size_t uuid() const { return parent_.uuid(); }

   private:
    Parent parent_;
    std::size_t slice_index_;
  };

  template <class Plugin>
  inline constexpr auto operator==(
      BlockSliceGenerator<Plugin> const& lhs,
      BlockSliceGenerator<Plugin> const& rhs) noexcept -> bool {
    return (lhs.parent() == rhs.parent()) and (lhs.index() == rhs.index());
  }

  template <class Plugin>
  inline constexpr auto operator!=(
      BlockSliceGenerator<Plugin> const& lhs,
      BlockSliceGenerator<Plugin> const& rhs) noexcept -> bool {
    return not(lhs == rhs);
  }

  //============================================================================
  //
  //  N_UNITS FUNCTIONS
  //
  //============================================================================

  //**Units funcs***************************************************************
  /*!\name Units functions*/
  //@{

  //****************************************************************************
  /*!\brief Get number of units from BlockGenerators
   * \ingroup blocks
   *
   * \details
   * Get number of units from a BlockGenerators. By 'units' we mean the number
   * of individual BlockSlice(s) composing the Generators.
   *
   * \usage
   * \code
   * BlockLike<...> b;
   * std::size_t n_units = blocks::n_units(b);
   * \endcode
   *
   * \param generator The generator whose number of units is to be extracted.
   */

  //****************************************************************************
  template <typename Plugin>
  inline auto n_units(BlockRefGenerator<Plugin> const& generator) noexcept
      -> std::size_t {
    return n_units(generator.get());
  }
  //****************************************************************************

  //****************************************************************************
  template <typename Plugin>
  inline auto n_units(BlockViewGenerator<Plugin> const& generator) noexcept
      -> std::size_t {
    return generator.region().size;
  }
  //****************************************************************************

  //@}
  //****************************************************************************

  //============================================================================
  //
  //  VIEW FUNCTIONS
  //
  //============================================================================

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  /*!\name BlockRef view functions */
  //@{

  //****************************************************************************
  /*!\brief View into blockref
   * \ingroup block_customization
   *
   * \details
   * Implements contiguous views on BlockRefs
   *
   * \param generator Data-structure modeling the block concept
   * \param start_index The start index of the slice
   * \param region_size The size of the slice
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
  template <typename Plugin>
  inline auto slice_backend(BlockRefGenerator<Plugin> generator,
                            std::size_t start_index,
                            std::size_t region_size) noexcept
      -> BlockViewGenerator<Plugin> {
    return BlockViewGenerator<Plugin>{std::move(generator), start_index,
                                      region_size};
  }
  //****************************************************************************

  //****************************************************************************
  template <typename Plugin>
  inline decltype(auto) slice_backend(BlockViewGenerator<Plugin> generator,
                                      std::size_t start_index,
                                      std::size_t region_size) noexcept {
    return slice_backend(generator.parent(),
                         generator.region().start + start_index, region_size);
  }
  //****************************************************************************

  //@}
  /*! \endcond */
  //****************************************************************************

  //============================================================================
  //
  //  SLICE FUNCTIONS
  //
  //============================================================================

  //**Slice support*************************************************************
  /*!\name Whole block slice */
  //@{

  //****************************************************************************
  /*!\brief Slice operator for an entire block ref generator
   * \ingroup blocks
   *
   * \details
   * Gets a view into the entire block ref generator
   *
   * \param block_like A ref generator
   *
   * \example
   * Given a block ref `b`, we can slice it using
   * \code
   * auto b_slice = block::slice(b); // Generates a view into the block
   * \endcode
   *
   * \note
   * not marked noexcept because we can have out of range indices
   */
  template <typename Plugin>
  inline constexpr decltype(auto) slice(BlockRefGenerator<Plugin>& block_like) {
    return slice(block_like, 0UL, ::elastica::end);
  }
  //****************************************************************************

  //****************************************************************************
  template <typename Plugin>
  inline constexpr decltype(auto) slice(
      BlockRefGenerator<Plugin> const& block_like) {
    return slice(block_like, 0UL, ::elastica::end);
  }
  //****************************************************************************

  // @}
  //****************************************************************************

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  /*!\name BlockRef slice functions */
  //@{

  //****************************************************************************
  /*!\brief Slice into blockref
   * \ingroup block_customization
   *
   * \details
   * Implements slices on blockrefs
   *
   * \param generator Data-structure modeling the block concept
   * \param slice_index The index of the slice (int or from_end)
   *
   * \example
   * Given a view `b`, we can obtain a slice
   * \code
   * auto b_slice = block::slice(b, 5UL); // at index 5 from start
   * // 2 from end
   * auto b_slice_from_end = block::slice(b, elastica::end - 2UL);
   * \endcode
   */
  template <typename Plugin>
  inline auto slice_backend(BlockRefGenerator<Plugin> generator,
                            std::size_t slice_index) noexcept
      -> BlockSliceGenerator<Plugin> {
    return BlockSliceGenerator<Plugin>(std::move(generator), slice_index);
  }

  template <typename Plugin>
  inline decltype(auto) slice_backend(BlockViewGenerator<Plugin> generator,
                                      std::size_t slice_index) noexcept {
    return slice_backend(generator.parent(),
                         generator.region().start + slice_index);
  }

  //@}
  /*! \endcond */
  //****************************************************************************

}  // namespace blocks