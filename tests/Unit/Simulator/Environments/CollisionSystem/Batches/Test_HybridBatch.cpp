//
// Created by Tejaswin Parthasarathy on 4/29/22.
//
#include "Simulator/Environments/CollisionSystem/Batches/HybridBatch.hpp"
#include "Simulator/Environments/CollisionSystem/Configuration.hpp"
#include "Simulator/Environments/Primitives.hpp" // why?

namespace {

  template <typename ExecPolicy>
  struct TestCollisionConfiguration
      : public ::elastica::collision::CollisionConfig {
    struct KernelExecutionPolicy {
      using ExecutionPolicy = ExecPolicy;
    };
  };
  using elastica::collision::batches::HybridBatch;

  using C =
      TestCollisionConfiguration<::elastica::configuration::sequential_policy>;
  static_assert(
      std::is_same_v<typename HybridBatch<C>::BatchingAlgorithm,
                       ::elastica::collision::batches::SingleBatch<C>>,
      "Meta-failure");


  using CP =
      TestCollisionConfiguration<::elastica::configuration::parallel_policy>;
  static_assert(std::is_same_v<typename HybridBatch<CP>::BatchingAlgorithm,
                                 ::elastica::collision::batches::UnionFind<CP>>,
                "Meta-failure");

  // cannot have a hybrid policy here

}  // namespace
