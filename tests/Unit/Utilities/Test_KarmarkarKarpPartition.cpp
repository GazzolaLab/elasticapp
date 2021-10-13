//
// Created by Tejaswin Parthasarathy on 10/10/21.
//

#include "Framework/TestingFramework.hpp"
#include "Utilities/KarmarkarKarpPartition.hpp"

namespace {

  ELASTICA_TEST_CASE("Unit.Utilities.karmarkar_karp_partitioner",
                     "[Unit][Utilities]") {
    const std::size_t partition_size = 3UL;
    auto partition_result = elastica::karmarkar_karp_partition(
        std::vector<int>({1, 7, 5, 10, 9, 3}), partition_size);
    //    CHECK(partitions.subset(0UL).numbers() == {7, 5});
    //    CHECK(partitions.subset(1UL).numbers() == {9, 3});
    //    CHECK(partitions.subset(2UL).numbers() == {1, 10});
    //    CHECK(partitions.subset(0UL).numbers().begin()->value() == 7);
    //    CHECK(std::next(partitions.subset(0UL).numbers().begin())->value() ==
    //    5); CHECK(partitions.subset(1UL).numbers().begin()->value() == 9);
    CHECK(partition_result.n_subsets() == partition_size);
    CHECK(partition_result.subset(0UL) == std::vector<int>{7, 5});
    CHECK(partition_result.subset(1UL) == std::vector<int>{9, 3});
    CHECK(partition_result.subset(2UL) == std::vector<int>{1, 10});
  }

  struct NForcings {
    auto value() const -> std::size_t { return f; }
    std::size_t f;
  };
  bool operator==(NForcings const one, NForcings const two) {
    return one.value() == two.value();
  }

  ELASTICA_TEST_CASE("Unit.Utilities.karmarkar_karp_partitioner.with_op",
                     "[Unit][Utilities]") {
    const std::size_t partition_size = 3UL;
    auto partition_result = elastica::karmarkar_karp_partition(
        std::vector<NForcings>({NForcings{1}, NForcings{7}, NForcings{5},
                                NForcings{10}, NForcings{9}, NForcings{3}}),
        partition_size, [](NForcings const& n) { return n.value(); });

    CHECK(partition_result.n_subsets() == partition_size);
    CHECK(partition_result.subset(0UL) ==
          std::vector<NForcings>{NForcings{7}, NForcings{5}});
    CHECK(partition_result.subset(1UL) ==
          std::vector<NForcings>{NForcings{9}, NForcings{3}});
    CHECK(partition_result.subset(2UL) ==
          std::vector<NForcings>{NForcings{1}, NForcings{10}});
  }

}  // namespace