//==============================================================================
/*!
//  \file
//  \brief Implementation of reference generators for Blocks
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

#include "Systems/Block.hpp"
//
#include "Utilities/GSL.hpp"
//
#include <vector>

namespace blocks {

  inline std::size_t cantor_pair(std::size_t a, std::size_t b) noexcept {
    return (a + b) * (a + b + 1) / 2 + a;
  }

  // TODO : Add sliceable, viewable semantics
  template <typename Plugin>
  class BlockRefGenerator {
   private:
    using This = BlockRefGenerator<Plugin>;

   public:
    using BlockType = Block<Plugin>;
    using Source = std::vector<BlockType>;
    // Cannot use ref wrapper, raw pointer needed to store invalid references
    using Pointer = Source*;

    BlockRefGenerator()
        : src_(nullptr), index_(std::numeric_limits<std::size_t>::max()) {}

    BlockRefGenerator(Source& v, std::size_t index) : src_(&v), index_(index){};

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

   private:
    Pointer src_;
    std::size_t index_;
  };

  template <class Plugin>
  inline constexpr auto operator==(
      BlockRefGenerator<Plugin> const& lhs,
      BlockRefGenerator<Plugin> const& rhs) noexcept -> bool {
    return (lhs.source() == rhs.source()) and (lhs.index() == rhs.index());
  }

  template <typename Plugin>
  class BlockViewGenerator {
   public:  // in case some metavariables are needed.
    using Parent = BlockRefGenerator<Plugin>;
    using BlockType = typename Parent::BlockType;

    BlockViewGenerator()
        : parent_(),
          slice_index_(std::numeric_limits<std::size_t>::max()),
          slice_stop_(std::numeric_limits<std::size_t>::max()) {}

    BlockViewGenerator(Parent parent, std::size_t slice_index,
                       std::size_t slice_size)
        : parent_(parent), slice_index_(slice_index), slice_stop_(slice_size){};

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
      return blocks::slice(parent_.get(), slice_index_, slice_stop_);
    }
    inline std::size_t uuid() const { return parent_.uuid(); }
    auto start_index() const noexcept -> std::size_t { return slice_index_; }
    auto stop_index() const noexcept -> std::size_t { return slice_stop_; }

   private:
    Parent parent_;
    std::size_t slice_index_;
    std::size_t slice_stop_;
  };

  template <class Plugin>
  inline constexpr auto operator==(
      BlockViewGenerator<Plugin> const& lhs,
      BlockViewGenerator<Plugin> const& rhs) noexcept -> bool {
    return (lhs.parent() == rhs.parent()) and
           (lhs.start_index() == rhs.start_index()) and
           (lhs.stop_index() == rhs.stop_index());
  }

  template <typename Plugin>
  class BlockSliceGenerator {
   public:  // in case some metavariables are needed.
    using Parent = BlockRefGenerator<Plugin>;
    using BlockType = typename Parent::BlockType;

    BlockSliceGenerator()
        : parent_(), slice_index_(std::numeric_limits<std::size_t>::max()) {}

    BlockSliceGenerator(Parent parent, std::size_t slice_index)
        : parent_(parent), slice_index_(slice_index){};

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
      return blocks::slice(parent_.get(), slice_index_);
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

  template <typename Plugin>
  inline auto slice(BlockRefGenerator<Plugin> parent, std::size_t start,
                    std::size_t stop) -> BlockViewGenerator<Plugin> {
    return BlockViewGenerator<Plugin>(std::move(parent), start, stop);
  }

  template <typename Plugin>
  inline auto slice(BlockRefGenerator<Plugin> parent, std::size_t slice_index)
      -> BlockSliceGenerator<Plugin> {
    return BlockSliceGenerator<Plugin>(std::move(parent), slice_index);
  }

}  // namespace blocks