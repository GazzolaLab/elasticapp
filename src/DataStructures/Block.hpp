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
  template <int Dimensions, int Size,
            storage_detail::StorageOption SO =
                storage_detail::StorageOption::STDArray>
  class BlockData {
   public:
    static constexpr int Dim = Dimensions;
    static constexpr int BlockSize = Size;
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
        : public MetaTaggedTuple<BlockMetaData, TagToBeConstructed> {};

    template <class BlockMetaData, template <typename...> class TagList,
              typename... Tags>
    class BlockHelper<BlockMetaData, TagList<Tags...>>
        : public MetaTaggedTuple<BlockMetaData, Tags...> {};
  }  // namespace detail

  template <class GroupToBeConstructed, class BlockMetaData,
            Requires<is_valid_group_v<GroupToBeConstructed> and
                     is_valid_blockmetadata_v<BlockMetaData>> = nullptr>
  class Block
      : public detail::BlockHelper<
            BlockMetaData, typename GroupToBeConstructed::construct_tags> {};
}  // namespace Blocks
