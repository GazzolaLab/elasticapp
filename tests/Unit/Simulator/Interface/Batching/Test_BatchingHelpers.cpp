//
// Created by Tejaswin Parthasarathy on 10/13/21.
//
#include "Framework/TestingFramework.hpp"
#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"
#include "Simulator/Interface/Batching/BatchingHelpers.hpp"

namespace {
  using namespace ::elastica::interface::detail;
  using namespace ::elastica::configuration;

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.BatchingHelpers",
                     "[Unit][Simulator][Interface]") {
    // clang-format on

    const std::size_t thread_size = 400UL;
    const std::size_t n_dofs = 10000UL;
    const std::size_t hybrid_threshold = 5000UL;

    SECTION("sequential") {
      using parallelism = sequential_policy;
      auto f = DefaultDecision();
      BatchingOperation<parallelism, DefaultDecision> op{thread_size,
                                                         hybrid_threshold, f};
      CHECK(op(n_dofs) == 1UL);
    }
    SECTION("parallel") {
      using parallelism = parallel_policy;
      auto f = DefaultDecision();
      BatchingOperation<parallelism, DefaultDecision> op{thread_size,
                                                         hybrid_threshold, f};
      CHECK(op(n_dofs) == DefaultDecision()(n_dofs, thread_size));
    }
    SECTION("hybrid") {
      using parallelism = hybrid_policy;
      auto f = DefaultDecision();
      BatchingOperation<parallelism, DefaultDecision> op{thread_size,
                                                         hybrid_threshold, f};
      CHECK(op(200UL) == 1UL);
      CHECK(op(n_dofs) == DefaultDecision()(n_dofs, thread_size));
    }
  }

}  // namespace
