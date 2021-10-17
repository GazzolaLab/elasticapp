//
// Created by Tejaswin Parthasarathy on 10/13/21.
//

//
// Created by Tejaswin Parthasarathy on 10/13/21.
//
#include "Framework/TestingFramework.hpp"
#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"
#include "Simulator/Interface/Forcing/BatchingPolicy.hpp"
#include "Utilities/TMPL.hpp"

namespace {
  using namespace ::elastica::interface::forcing;
  using namespace ::elastica::configuration;
}  // namespace

namespace {

  template <typename Policy>
  struct MockConstraintTraits {
    using ForcingExecutionPolicy = Policy;
    std::size_t n_threads() const { return 1UL; }
    std::size_t forcing_parallelism_threshold() const { return 5UL; }
  };

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Forcing.BatchingPolicy",
      "[Unit][Simulator][Interface][Forcing]") {
    // clang-format on

    SECTION("sequential") {
      using MCT = MockConstraintTraits<sequential_policy>;
      MCT traits;
      using B = BatchingPolicy<MCT>;
      using Algorithm =
          typename tmpl::front<typename B::BatchingAlgorithm>::type;
      Algorithm algo(traits);
    }
    SECTION("Parallel") {
      using MCT = MockConstraintTraits<parallel_policy>;
      MCT traits;
      using B = BatchingPolicy<MCT>;
      using Algorithm =
          typename tmpl::front<typename B::BatchingAlgorithm>::type;
      Algorithm algo(traits);
    }
    SECTION("Hybrid") {
      using MCT = MockConstraintTraits<hybrid_policy>;
      MCT traits;
      using B = BatchingPolicy<MCT>;
      using Algorithm =
          typename tmpl::front<typename B::BatchingAlgorithm>::type;
      Algorithm algo(traits);
    }
  }

}  // namespace
