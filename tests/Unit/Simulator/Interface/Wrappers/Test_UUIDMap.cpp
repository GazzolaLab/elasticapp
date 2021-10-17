//
// Created by Tejaswin Parthasarathy on 10/10/21.
//

#include <algorithm>
#include <string>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Wrappers/UUID.hpp"
#include "Simulator/Interface/Wrappers/UUIDMap.hpp"
#include "Utilities/KarmarkarKarpPartition.hpp"
#include "Utilities/StdHelpers.hpp"
#include "Utilities/StdVectorHelpers.hpp"

namespace {

  using namespace ::elastica::interface::detail;

  struct TestTag;

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.UUIDMap",
                     "[Unit][Simulator][Interface]") {
    using ID = UUID<TestTag>;
    using Map = UUIDMap<ID>;
    Map mapping;

    // map a bunch of systems of different types
    int i = 0UL;
    char c = 'a';
    float f = 3.14F;
    double d = 6.28;

    std::vector<ID> ids{ID(1UL, std::string("")), ID(2UL, std::string("")),
                        ID(3UL, std::string("")), ID(4UL, std::string(""))};

    for (auto const& id : ids) {
      mapping.map(i).with(id);
    }
    mapping.map(c).with(ids[1UL]);
    mapping.map(c).with(ids[3UL]);

    mapping.map(f).with(ids[0UL]);

    mapping.map(d).with(ids[0UL]);
    mapping.map(d).with(ids[1UL]);
    mapping.map(d).with(ids[2UL]);

    auto vec = std::move(mapping).as_vector();

    // we utilize an unordered map in the background, so we can't guarantee
    // order of keys here, so check each element for length and then do
    // more checking
    decltype(vec) expected = {
        std::vector<ID>{ids[0UL]}, std::vector<ID>{ids[1UL], ids[3UL]},
        std::vector<ID>{ids[0UL], ids[1UL], ids[2UL]}, ids};

    for (std::size_t idx = 0UL; idx < 4UL; ++idx) {
      CHECK(vec[idx] == expected[vec[idx].size() - 1]);
      //      CHECK(vec[3UL] == ids);
      //      CHECK(vec[2UL] == std::vector<ID>{});
      //      CHECK(vec[1UL] == std::vector<ID>{ids[0UL]});
      //      CHECK(vec[0UL] == std::vector<ID>{});
    }
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.UUIDToBatch",
                     "[Unit][Simulator][Interface]") {
    using ID = UUID<TestTag>;
    using VID = std::vector<ID>;
    VID ids;
    const std::size_t n_ids(10UL);
    ids.reserve(10UL);
    for (std::size_t idx = 0UL; idx < n_ids; ++idx) {
      ids.push_back(ID(idx, std::string("")));
    }

    std::vector<VID> vec{VID{ids[0UL], ids[9UL], ids[5UL]},
                         VID{ids[1UL], ids[2UL], ids[3UL]},
                         VID{ids[4UL], ids[8UL]}, VID{ids[6UL], ids[7UL]}};
    auto id2batch = make_uuid_to_batch_map(std::move(vec));

    CHECK(id2batch[ids[0UL]] == 0UL);
    CHECK(id2batch[ids[1UL]] == 1UL);
    CHECK(id2batch[ids[2UL]] == 1UL);
    CHECK(id2batch[ids[3UL]] == 1UL);
    CHECK(id2batch[ids[4UL]] == 2UL);
    CHECK(id2batch[ids[5UL]] == 0UL);
    CHECK(id2batch[ids[6UL]] == 3UL);
    CHECK(id2batch[ids[7UL]] == 3UL);
    CHECK(id2batch[ids[8UL]] == 2UL);
    CHECK(id2batch[ids[9UL]] == 0UL);

    SECTION("Test invariance via vectors") {
      auto key_value_tuple = id2batch.as_vectors();
      id2batch.from_vectors(std::move(key_value_tuple));

      CHECK(id2batch[ids[0UL]] == 0UL);
      CHECK(id2batch[ids[1UL]] == 1UL);
      CHECK(id2batch[ids[2UL]] == 1UL);
      CHECK(id2batch[ids[3UL]] == 1UL);
      CHECK(id2batch[ids[4UL]] == 2UL);
      CHECK(id2batch[ids[5UL]] == 0UL);
      CHECK(id2batch[ids[6UL]] == 3UL);
      CHECK(id2batch[ids[7UL]] == 3UL);
      CHECK(id2batch[ids[8UL]] == 2UL);
      CHECK(id2batch[ids[9UL]] == 0UL);
    }
  }

  /*  template <typename T>
    std::vector<T> flatten(std::vector<std::vector<T>>&& v) {
      std::size_t total_size = 0;
      for (const auto& sub : v)
        total_size += sub.size();  // I wish there was a transform_accumulate
      std::vector<T> result;
      result.reserve(total_size);
      for (auto && sub : std::move(v))
        result.insert(result.end(), std::make_move_iterator(sub).begin(),
    std::make_move_iterator(sub).end()); return result;
    }*/

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.UUIDPipeline",
                     "[Unit][Simulator][Interface]") {
    // tests the entire pipeline
    // 1, 7, 5, 10, 9, 3

    using ID = UUID<TestTag>;
    using Map = UUIDMap<ID>;
    Map mapping;

    // need 6 systems for cardinality
    int i = 0UL;
    char c = 'a';
    float f = 3.14F;
    double d = 6.28;
    std::size_t s = 20UL;
    short int si = 8;

    std::vector<std::size_t> entries{1UL, 7UL, 5UL, 10UL, 9UL, 3UL};
    std::vector<std::size_t> prefix_sum;
    std::partial_sum(std::begin(entries), std::end(entries),
                     std::back_inserter(prefix_sum));

    std::size_t entry_counter = 0UL;
    std::size_t id_counter = 0UL;
    // 1
    for (auto index = 0UL; index < entries[entry_counter]; ++index) {
      mapping.map(i).with(ID(id_counter++, ""));
    }
    REQUIRE(id_counter == prefix_sum[entry_counter]);
    ++entry_counter;

    // 7
    for (auto index = 0UL; index < entries[entry_counter]; ++index) {
      mapping.map(c).with(ID(id_counter++, ""));
    }
    REQUIRE(id_counter == prefix_sum[entry_counter]);
    ++entry_counter;

    // 5
    for (auto index = 0UL; index < entries[entry_counter]; ++index) {
      mapping.map(f).with(ID(id_counter++, ""));
    }
    REQUIRE(id_counter == prefix_sum[entry_counter]);
    ++entry_counter;

    // 10
    for (auto index = 0UL; index < entries[entry_counter]; ++index) {
      mapping.map(d).with(ID(id_counter++, ""));
    }
    REQUIRE(id_counter == prefix_sum[entry_counter]);
    ++entry_counter;

    // 9
    for (auto index = 0UL; index < entries[entry_counter]; ++index) {
      mapping.map(s).with(ID(id_counter++, ""));
    }
    REQUIRE(id_counter == prefix_sum[entry_counter]);
    ++entry_counter;

    // 3
    for (auto index = 0UL; index < entries[entry_counter]; ++index) {
      mapping.map(si).with(ID(id_counter++, ""));
    }
    REQUIRE(id_counter == prefix_sum[entry_counter]);

    auto vec = std::move(mapping).as_vector();
    std::size_t partition_size = 3UL;
    auto partition_result = elastica::karmarkar_karp_partition(
        vec, partition_size, [](auto const& v) { return v.size(); });
    //    TypeDisplayer<decltype(partition_result.subsets())> td{};

    using SubsetType = decltype(flatten(partition_result.subsets()));
    SubsetType flattened_subsets;
    flattened_subsets.reserve(partition_size);
    // flatten the inner-most subset dimension
    for (auto& sub : partition_result.subsets()) {
      flattened_subsets.emplace_back(flatten(std::move(sub)));
    }
    // 1, 7, 5, 10, 9, 3
    // prefix sum
    //
    // grouped into
    // (7, 5), (9, 3), (1, 10)

    auto batch_from = make_uuid_to_batch_map(std::move(flattened_subsets));


    // The indices for first and second batch seem to switch around, so in
    // order to get the correct batch we take it from the batch directly

    // std::size_t first_batch = 0UL;
    // std::size_t second_batch = 1UL;
    // std::size_t third_batch = 2UL;
    std::size_t first_batch = batch_from[ID(1UL, "")];
    std::size_t second_batch = batch_from[ID(33UL, "")];
    std::size_t third_batch =  batch_from[ID(0UL, "")];

    // ensure that objects constraining one system belong to the same
    // batch/bucket
    std::map<std::size_t, std::size_t> batches;
    batches[0UL] = third_batch;   // 1
    batches[1UL] = first_batch;   // 7
    batches[2UL] = first_batch;   // 5
    batches[3UL] = third_batch;   // 10
    batches[4UL] = second_batch;  // 9
    batches[5UL] = second_batch;  // 3

    // CHECK(batch_from[ID(0UL, "")] == third_batch);

    auto id2batchidx = [&prefix_sum](std::size_t index) {
      auto const it = std::upper_bound(std::cbegin(prefix_sum),
                                       std::cend(prefix_sum), index);
      auto batch_index =
          static_cast<std::size_t>(std::distance(std::cbegin(prefix_sum), it));
      return batch_index;
    };
    for (std::size_t idx = 0UL; idx < prefix_sum.back(); ++idx) {
      // bool b = batch_from[ID(idx, "")] == id2batch(idx);
      INFO("index is " << idx);
      std::size_t batch_index = id2batchidx(idx);
      INFO("batch index is " << batch_index);
      CHECK(batch_from[ID(idx, "")] == batches[batch_index]);
    }

    //    for (auto const& id : ids) {
    //      mapping.map(i).with(id);
    //    }
  }

}  // namespace