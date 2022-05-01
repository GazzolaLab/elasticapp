//
// Created by Tejaswin Parthasarathy on 10/10/21.
//

#include <algorithm>

#include "Framework/TestingFramework.hpp"
#include "Utilities/KarmarkarKarpPartition.hpp"

namespace {

  ELASTICA_TEST_CASE("Unit.Utilities.karmarkar_karp_partitioner",
                     "[Unit][Utilities]") {
    SECTION("Single partition") {
      const std::size_t partition_size = 1UL;
      std::vector<int> input_vector{1, 7, 5, 10, 9, 3};
      auto partition_result =
          elastica::karmarkar_karp_partition(input_vector, partition_size);
      CHECK(partition_result.n_subsets() == partition_size);

      // we cannot guarntee order, but only guarantee that elements are within
      // the subset
      auto& only_subset = partition_result.subset(0UL);
      for (int item : input_vector) {
        CHECK(std::find(std::begin(only_subset), std::end(only_subset), item) !=
              std::end(only_subset));
      }
    }
    SECTION("Multiple partition") {
      /// [kk_example]
      const std::size_t n_partitions = 3UL;
      auto partition_result = elastica::karmarkar_karp_partition(
          std::vector<int>({1, 7, 5, 10, 9, 3}), n_partitions);
      CHECK(partition_result.n_subsets() == n_partitions);
      CHECK(partition_result.subset(0UL) == std::vector<int>{7, 5});
      CHECK(partition_result.subset(1UL) == std::vector<int>{9, 3});
      CHECK(partition_result.subset(2UL) == std::vector<int>{1, 10});
      /// [kk_example]

      //    CHECK(partitions.subset(0UL).numbers() == {7, 5});
      //    CHECK(partitions.subset(1UL).numbers() == {9, 3});
      //    CHECK(partitions.subset(2UL).numbers() == {1, 10});
      //    CHECK(partitions.subset(0UL).numbers().begin()->value() == 7);
      //    CHECK(std::next(partitions.subset(0UL).numbers().begin())->value()
      //    == 5); CHECK(partitions.subset(1UL).numbers().begin()->value() ==
      //    9);
    }
  }

  /// [kk_lambda_setup]
  struct NForcings {
    auto value() const -> std::size_t { return f; }
    std::size_t f;
  };
  bool operator==(NForcings const one, NForcings const two) {
    return one.value() == two.value();
  }
  /// [kk_lambda_setup]

  ELASTICA_TEST_CASE("Unit.Utilities.karmarkar_karp_partitioner.with_op",
                     "[Unit][Utilities]") {
    /// [kk_lambda_eg]
    const std::size_t n_partitions = 3UL;
    auto partition_result = elastica::karmarkar_karp_partition(
        std::vector<NForcings>({NForcings{1}, NForcings{7}, NForcings{5},
                                NForcings{10}, NForcings{9}, NForcings{3}}),
        n_partitions, [](NForcings const& n) { return n.value(); });

    CHECK(partition_result.n_subsets() == n_partitions);
    CHECK(partition_result.subset(0UL) ==
          std::vector<NForcings>{NForcings{7}, NForcings{5}});
    CHECK(partition_result.subset(1UL) ==
          std::vector<NForcings>{NForcings{9}, NForcings{3}});
    CHECK(partition_result.subset(2UL) ==
          std::vector<NForcings>{NForcings{1}, NForcings{10}});
    /// [kk_lambda_eg]
  }

}  // namespace
