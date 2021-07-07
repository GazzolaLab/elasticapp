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

namespace {
  template <template <typename> class B>
  struct ExamplePluginWithCRTP {
    using Derived = B<ExamplePluginWithCRTP<B>>;
  };
}  // namespace

namespace blocks {

  template <>
  class Block<ExamplePluginWithCRTP<Block>> : ExamplePluginWithCRTP<Block> {};

}  // namespace blocks

namespace {

  struct DummyTraits {};

  template <typename Traits, template <typename> class B>
  struct ExamplePluginWithCRTPAndTraits {
    using Derived = B<ExamplePluginWithCRTPAndTraits<Traits, B>>;
  };

}  // namespace

namespace blocks {

  template <>
  class Block<ExamplePluginWithCRTPAndTraits<DummyTraits, Block>>
      : ExamplePluginWithCRTPAndTraits<DummyTraits, Block> {};

}  // namespace blocks

namespace {

  struct DummyTag;

  template <typename Traits, template <typename> class B, typename Tag>
  struct ExamplePluginWithCRTPAndTraitsAndTag {
    using Derived = B<ExamplePluginWithCRTPAndTraitsAndTag<Traits, B, Tag>>;
  };

}  // namespace

namespace blocks {

  template <>
  class Block<
      ExamplePluginWithCRTPAndTraitsAndTag<DummyTraits, Block, DummyTag>>
      : ExamplePluginWithCRTPAndTraitsAndTag<DummyTraits, Block, DummyTag> {};

}  // namespace blocks

/// [plugin_trait_example]
using Test1 = blocks::Block<ExamplePlugin>;
static_assert(
    std::is_same<typename blocks::PluginTrait<Test1>::type<blocks::Block>,
                 ExamplePlugin>::value,
    "Block type traits failure!");

using Test101 = blocks::Block<ExamplePluginWithCRTP<blocks::Block>>;
static_assert(
    std::is_same<typename blocks::PluginTrait<Test101>::type<blocks::Block>,
                 ExamplePluginWithCRTP<blocks::Block>>::value,
    "Block type traits failure!");
static_assert(
    std::is_same<
        typename blocks::PluginTrait<Test101>::type<blocks::BlockSlice>,
        ExamplePluginWithCRTP<blocks::BlockSlice>>::value,
    "Block type traits failure!");
static_assert(
    std::is_same<
        typename blocks::PluginTrait<Test101>::type<blocks::ConstBlockSlice>,
        ExamplePluginWithCRTP<blocks::ConstBlockSlice>>::value,
    "Block type traits failure!");

using Test102 =
    blocks::Block<ExamplePluginWithCRTPAndTraits<DummyTraits, blocks::Block>>;
static_assert(
    std::is_same<
        typename blocks::PluginTrait<Test102>::type<blocks::Block>,
        ExamplePluginWithCRTPAndTraits<DummyTraits, blocks::Block>>::value,
    "Block type traits failure!");
static_assert(
    std::is_same<
        typename blocks::PluginTrait<Test102>::type<blocks::BlockSlice>,
        ExamplePluginWithCRTPAndTraits<DummyTraits, blocks::BlockSlice>>::value,
    "Block type traits failure!");
static_assert(
    std::is_same<
        typename blocks::PluginTrait<Test102>::type<blocks::ConstBlockSlice>,
        ExamplePluginWithCRTPAndTraits<DummyTraits,
                                       blocks::ConstBlockSlice>>::value,
    "Block type traits failure!");

using Test103 = blocks::Block<
    ExamplePluginWithCRTPAndTraitsAndTag<DummyTraits, blocks::Block, DummyTag>>;
static_assert(
    std::is_same<typename blocks::PluginTrait<Test103>::type<blocks::Block>,
                 ExamplePluginWithCRTPAndTraitsAndTag<
                     DummyTraits, blocks::Block, DummyTag>>::value,
    "Block type traits failure!");
static_assert(
    std::is_same<
        typename blocks::PluginTrait<Test103>::type<blocks::BlockSlice>,
        ExamplePluginWithCRTPAndTraitsAndTag<DummyTraits, blocks::BlockSlice,
                                             DummyTag>>::value,
    "Block type traits failure!");
static_assert(
    std::is_same<
        typename blocks::PluginTrait<Test103>::type<blocks::ConstBlockSlice>,
        ExamplePluginWithCRTPAndTraitsAndTag<
            DummyTraits, blocks::ConstBlockSlice, DummyTag>>::value,
    "Block type traits failure!");

using Test3 = blocks::BlockSlice<ExamplePlugin>;
static_assert(
    std::is_same<typename blocks::PluginTrait<Test3>::type<blocks::BlockSlice>,
                 ExamplePlugin>::value,
    "Block type traits failure!");

using Test301 = blocks::BlockSlice<ExamplePluginWithCRTP<blocks::BlockSlice>>;
static_assert(
    std::is_same<typename blocks::PluginTrait<Test301>::type<blocks::Block>,
                 ExamplePluginWithCRTP<blocks::Block>>::value,
    "Block type traits failure!");
static_assert(
    std::is_same<
        typename blocks::PluginTrait<Test301>::type<blocks::BlockSlice>,
        ExamplePluginWithCRTP<blocks::BlockSlice>>::value,
    "Block type traits failure!");
static_assert(
    std::is_same<
        typename blocks::PluginTrait<Test301>::type<blocks::ConstBlockSlice>,
        ExamplePluginWithCRTP<blocks::ConstBlockSlice>>::value,
    "Block type traits failure!");

using Test302 = blocks::BlockSlice<
    ExamplePluginWithCRTPAndTraits<DummyTraits, blocks::BlockSlice>>;
static_assert(
    std::is_same<
        typename blocks::PluginTrait<Test302>::type<blocks::Block>,
        ExamplePluginWithCRTPAndTraits<DummyTraits, blocks::Block>>::value,
    "Block type traits failure!");
static_assert(
    std::is_same<
        typename blocks::PluginTrait<Test302>::type<blocks::BlockSlice>,
        ExamplePluginWithCRTPAndTraits<DummyTraits, blocks::BlockSlice>>::value,
    "Block type traits failure!");
static_assert(
    std::is_same<
        typename blocks::PluginTrait<Test302>::type<blocks::ConstBlockSlice>,
        ExamplePluginWithCRTPAndTraits<DummyTraits,
                                       blocks::ConstBlockSlice>>::value,
    "Block type traits failure!");

using Test303 = blocks::BlockSlice<ExamplePluginWithCRTPAndTraitsAndTag<
    DummyTraits, blocks::BlockSlice, DummyTag>>;
static_assert(
    std::is_same<typename blocks::PluginTrait<Test303>::type<blocks::Block>,
                 ExamplePluginWithCRTPAndTraitsAndTag<
                     DummyTraits, blocks::Block, DummyTag>>::value,
    "Block type traits failure!");
static_assert(
    std::is_same<
        typename blocks::PluginTrait<Test303>::type<blocks::BlockSlice>,
        ExamplePluginWithCRTPAndTraitsAndTag<DummyTraits, blocks::BlockSlice,
                                             DummyTag>>::value,
    "Block type traits failure!");
static_assert(
    std::is_same<
        typename blocks::PluginTrait<Test303>::type<blocks::ConstBlockSlice>,
        ExamplePluginWithCRTPAndTraitsAndTag<
            DummyTraits, blocks::ConstBlockSlice, DummyTag>>::value,
    "Block type traits failure!");

using Test5 = blocks::ConstBlockSlice<ExamplePlugin>;
static_assert(
    std::is_same<
        typename blocks::PluginTrait<Test5>::type<blocks::ConstBlockSlice>,
        ExamplePlugin>::value,
    "Block type traits failure!");

using Test501 = blocks::BlockSlice<ExamplePluginWithCRTP<blocks::BlockSlice>>;
static_assert(
    std::is_same<typename blocks::PluginTrait<Test501>::type<blocks::Block>,
                 ExamplePluginWithCRTP<blocks::Block>>::value,
    "Block type traits failure!");
static_assert(
    std::is_same<
        typename blocks::PluginTrait<Test501>::type<blocks::BlockSlice>,
        ExamplePluginWithCRTP<blocks::BlockSlice>>::value,
    "Block type traits failure!");
static_assert(
    std::is_same<
        typename blocks::PluginTrait<Test501>::type<blocks::ConstBlockSlice>,
        ExamplePluginWithCRTP<blocks::ConstBlockSlice>>::value,
    "Block type traits failure!");

using Test502 = blocks::ConstBlockSlice<
    ExamplePluginWithCRTPAndTraits<DummyTraits, blocks::ConstBlockSlice>>;
static_assert(
    std::is_same<
        typename blocks::PluginTrait<Test502>::type<blocks::Block>,
        ExamplePluginWithCRTPAndTraits<DummyTraits, blocks::Block>>::value,
    "Block type traits failure!");
static_assert(
    std::is_same<
        typename blocks::PluginTrait<Test502>::type<blocks::BlockSlice>,
        ExamplePluginWithCRTPAndTraits<DummyTraits, blocks::BlockSlice>>::value,
    "Block type traits failure!");
static_assert(
    std::is_same<
        typename blocks::PluginTrait<Test502>::type<blocks::ConstBlockSlice>,
        ExamplePluginWithCRTPAndTraits<DummyTraits,
                                       blocks::ConstBlockSlice>>::value,
    "Block type traits failure!");

using Test503 = blocks::ConstBlockSlice<ExamplePluginWithCRTPAndTraitsAndTag<
    DummyTraits, blocks::ConstBlockSlice, DummyTag>>;
static_assert(
    std::is_same<typename blocks::PluginTrait<Test503>::type<blocks::Block>,
                 ExamplePluginWithCRTPAndTraitsAndTag<
                     DummyTraits, blocks::Block, DummyTag>>::value,
    "Block type traits failure!");
static_assert(
    std::is_same<
        typename blocks::PluginTrait<Test503>::type<blocks::BlockSlice>,
        ExamplePluginWithCRTPAndTraitsAndTag<DummyTraits, blocks::BlockSlice,
                                             DummyTag>>::value,
    "Block type traits failure!");
static_assert(
    std::is_same<
        typename blocks::PluginTrait<Test503>::type<blocks::ConstBlockSlice>,
        ExamplePluginWithCRTPAndTraitsAndTag<
            DummyTraits, blocks::ConstBlockSlice, DummyTag>>::value,
    "Block type traits failure!");
/// [plugin_trait_example]

/// [plugin_t_example]
using Test302 = blocks::BlockSlice<
    ExamplePluginWithCRTPAndTraits<DummyTraits, blocks::BlockSlice>>;
using F =
    blocks::Block<typename blocks::PluginTrait<Test102>::type<blocks::Block>>;
using S =
    typename blocks::PluginFrom<Test102>::template to<blocks::Block>::type;

static_assert(std::is_same<F, S>::value, "Block type traits failure!");
/// [plugin_t_example]
