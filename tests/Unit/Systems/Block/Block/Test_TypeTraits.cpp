//
// Created by Tejaswin Parthasarathy on 2/26/21.
//
#include <type_traits>

#include "Systems/Block/Block/TypeTraits.hpp"
#include "Systems/Block/Block/Types.hpp"

/// [is_block_example]
struct ExamplePlugin {};

namespace blocks {

  template <>
  class Block<ExamplePlugin> {};

}  // namespace blocks

class NotABlock {};

using Test1 = blocks::Block<ExamplePlugin>;
static_assert(blocks::IsBlock<Test1>::value, "Block type traits failure!");
// or alternatively
static_assert(blocks::is_block_v<Test1>, "Block type traits failure!");

using Test2 = NotABlock;
static_assert(!blocks::IsBlock<Test2>::value, "Block type traits failure!");

/// [is_block_example]

/// [is_blockslice_example]

namespace blocks {

  template <>
  class BlockSlice<ExamplePlugin> {};

}  // namespace blocks

class NotABlockSlice {};

using Test3 = blocks::BlockSlice<ExamplePlugin>;
static_assert(blocks::IsBlockSlice<Test3>::value, "Block type traits failure!");
// or alternatively
static_assert(blocks::is_block_slice_v<Test3>, "Block type traits failure!");

using Test4 = NotABlockSlice;
static_assert(!blocks::IsBlockSlice<Test4>::value,
              "Block type traits failure!");

/// [is_blockslice_example]

// also test others
static_assert(!blocks::IsBlock<Test3>::value, "Block type traits failure!");
static_assert(!blocks::IsBlock<Test4>::value, "Block type traits failure!");

static_assert(!blocks::IsBlockSlice<Test1>::value,
              "Block type traits failure!");
static_assert(!blocks::IsBlockSlice<Test2>::value,
              "Block type traits failure!");

/// [is_const_blockslice_example]

namespace blocks {

  template <>
  class ConstBlockSlice<ExamplePlugin> {};

}  // namespace blocks

class NotAConstBlockSlice {};

using Test5 = blocks::ConstBlockSlice<ExamplePlugin>;
static_assert(blocks::IsConstBlockSlice<Test5>::value,
              "Block type traits failure!");
// or alternatively
static_assert(blocks::is_const_block_slice_v<Test5>,
              "Block type traits failure!");

using Test6 = NotAConstBlockSlice;
static_assert(!blocks::IsConstBlockSlice<Test6>::value,
              "Block type traits failure!");

/// [is_const_blockslice_example]

static_assert(!blocks::IsBlock<Test5>::value, "Block type traits failure!");
static_assert(!blocks::IsBlock<Test6>::value, "Block type traits failure!");
static_assert(!blocks::IsBlockSlice<Test5>::value,
              "Block type traits failure!");
static_assert(!blocks::IsBlockSlice<Test6>::value,
              "Block type traits failure!");

static_assert(!blocks::IsConstBlockSlice<Test1>::value,
              "Block type traits failure!");
static_assert(!blocks::IsConstBlockSlice<Test2>::value,
              "Block type traits failure!");
static_assert(!blocks::IsConstBlockSlice<Test3>::value,
              "Block type traits failure!");
static_assert(!blocks::IsConstBlockSlice<Test4>::value,
              "Block type traits failure!");

/// [models_block_concept_example]
using Test1 = blocks::Block<ExamplePlugin>;
static_assert(blocks::ModelsBlockConcept<Test1>::value,
              "Block type traits failure!");

using Test3 = blocks::BlockSlice<ExamplePlugin>;
static_assert(blocks::ModelsBlockConcept<Test3>::value,
              "Block type traits failure!");

using Test5 = blocks::ConstBlockSlice<ExamplePlugin>;
static_assert(blocks::ModelsBlockConcept<Test5>::value,
              "Block type traits failure!");
/// [models_block_concept_example]

static_assert(!blocks::ModelsBlockConcept<Test2>::value,
              "Block type traits failure!");
static_assert(!blocks::ModelsBlockConcept<Test4>::value,
              "Block type traits failure!");
static_assert(!blocks::ModelsBlockConcept<Test6>::value,
              "Block type traits failure!");

/// [plugin_trait_example]
using Test1 = blocks::Block<ExamplePlugin>;
static_assert(std::is_same<typename blocks::PluginTrait<Test1>::type,
                           ExamplePlugin>::value,
              "Block type traits failure!");

using Test3 = blocks::BlockSlice<ExamplePlugin>;
static_assert(std::is_same<typename blocks::PluginTrait<Test3>::type,
                           ExamplePlugin>::value,
              "Block type traits failure!");

using Test5 = blocks::ConstBlockSlice<ExamplePlugin>;
static_assert(std::is_same<typename blocks::PluginTrait<Test5>::type,
                           ExamplePlugin>::value,
              "Block type traits failure!");

/// [plugin_trait_example]

/// [plugin_t_example]
using Test1 = blocks::Block<ExamplePlugin>;
using F = typename blocks::PluginTrait<Test1>::type;
using S = blocks::plugin_t<Test1>;

static_assert(std::is_same<F, S>::value, "Block type traits failure!");
/// [plugin_t_example]
