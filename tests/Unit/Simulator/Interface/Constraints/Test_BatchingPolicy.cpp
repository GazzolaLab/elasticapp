//
// Created by Tejaswin Parthasarathy on 10/13/21.
//
#include "Framework/TestingFramework.hpp"
#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"
#include "Simulator/Interface/Constraints/BatchingPolicy.hpp"
#include "Utilities/TMPL.hpp"

namespace {
  using namespace ::elastica::interface::constraints;
  using namespace ::elastica::configuration;
}  // namespace

namespace {

  template <typename Policy>
  struct MockConstraintTraits {
    using ConstraintsExecutionPolicy = Policy;
    std::size_t n_threads() const { return 1UL; }
    std::size_t constraints_parallelism_threshold() const { return 5UL; }
  };

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Constraints.BatchingPolicy",
      "[Unit][Simulator][Interface][Constraints]") {
    // clang-format on
    SECTION("sequential") {
      EMPTY_TEST();

      using MCT = MockConstraintTraits<sequential_policy>;
      MCT traits;
      using B = BatchingPolicy<MCT>;
      using Algorithm =
          typename tmpl::front<typename B::BatchingAlgorithm>::type;
      Algorithm algo(traits);
    }
    SECTION("Parallel") {
      EMPTY_TEST();

      using MCT = MockConstraintTraits<parallel_policy>;
      MCT traits;
      using B = BatchingPolicy<MCT>;
      using Algorithm =
          typename tmpl::front<typename B::BatchingAlgorithm>::type;
      Algorithm algo(traits);
    }
    SECTION("Hybrid") {
      EMPTY_TEST();

      using MCT = MockConstraintTraits<hybrid_policy>;
      MCT traits;
      using B = BatchingPolicy<MCT>;
      using Algorithm =
          typename tmpl::front<typename B::BatchingAlgorithm>::type;
      Algorithm algo(traits);
    }
  }

}  // namespace