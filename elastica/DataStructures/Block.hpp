#pragma once

#include <type_traits>
#include <vector>

#include "DataStructures/Groups.hpp"
#include "DataStructures/StorageTraits.hpp"
#include "DataStructures/TaggedTuple.hpp"
#include "DataStructures/Tags.hpp"
#include "Utilities/DefineTypes.h"
#include "Utilities/Requires.hpp"

// 1. User defines some types
// 2. Parse user-defined types and make AOS/SOA based on requirements
// 3. TaggedTuple for this SOA/AOS
// 4. Block structure uses this tagged tuple

/*
 * TODO:
 * 1. Composition of simple blocks into big blocks
 * 2. Clean separation of content from aggregation
 */
namespace Blocks {
  template <size_t Dimension, size_t Size,
            storage_detail::StorageOption SO =
                storage_detail::StorageOption::STDArray>
  class BlockData {
   public:
    static constexpr size_t Dim = Dimension;
    static constexpr size_t BlockSize = Size;
    static constexpr auto storage_option = SO;
  };

  namespace detail {
    template <typename BM, typename = cpp17::void_t<>>
    struct IsValidBlockMetaData : std::false_type {};

    template <typename BM>
    struct IsValidBlockMetaData<
        BM, cpp17::void_t<decltype(BM::Dim), decltype(BM::BlockSize),
                          decltype(BM::storage_option)>> : std::true_type {};
  }  // namespace detail

  template <typename BM>
  constexpr bool is_valid_blockmetadata_v =
      detail::IsValidBlockMetaData<BM>::value;

  namespace detail {
    /*
     *  Validity of tags are checked when building the Tagged Tuple
     */
    template <class BlockMetaData, class TagToBeConstructed>
    class BlockHelper
        : public MetaTaggedTuple<BlockMetaData, TagToBeConstructed> {
     public:
      using MetaData = BlockMetaData;

      template <typename _MetaData, typename _Tags>
      using container_type = MetaTaggedTuple<_MetaData, _Tags>;
    };

    template <class BlockMetaData, template <typename...> class TagList,
              typename... Tags>
    class BlockHelper<BlockMetaData, TagList<Tags...>>
        : public MetaTaggedTuple<BlockMetaData, Tags...> {
     public:
      using MetaData = BlockMetaData;

      template <typename _MetaData, typename... _Tags>
      using container_type = MetaTaggedTuple<_MetaData, _Tags...>;
    };
  }  // namespace detail

  template <class GroupToBeConstructed, class BlockMetaData,
            Requires<is_valid_group_v<GroupToBeConstructed> and
                     is_valid_blockmetadata_v<BlockMetaData>> = nullptr>
  class Block
      : public detail::BlockHelper<
            BlockMetaData, typename GroupToBeConstructed::construct_tags> {};

  namespace detail {
    template <typename B, typename = cpp17::void_t<>>
    struct IsValidBlock : std::false_type {};

    template <typename B>
    struct IsValidBlock<B, cpp17::void_t<typename B::MetaData>>
        : std::true_type {};
  }  // namespace detail

  template <typename B>
  constexpr bool is_valid_block_v = detail::IsValidBlock<B>::value;

}  // namespace Blocks
