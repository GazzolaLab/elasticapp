//
// Created by Tejaswin Parthasarathy on 6/16/21.
//
#include <cstddef>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"

namespace {

  using namespace elastica::configuration;

  /// [execution_setup]
  void perform_algorithm(elastica::configuration::sequential_policy) {
    /* does sequential processing */
  }
  void perform_algorithm(elastica::configuration::parallel_policy) {
    /* does parallel processing */
  }
  void perform_algorithm(elastica::configuration::hybrid_policy) {
    /* does sequential or parallel processing, using some thresholds on the
     * size of the data*/
  }
  /// [execution_setup]

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Configuration.ModuleConfiguration.Execution.constants",
      "[Unit][Simulator][Configuration]") {
    // clang-format on
    EMPTY_TEST();

    /// [seq_eg]
    perform_algorithm(elastica::configuration::seq);
    /// [seq_eg]

    /// [par_eg]
    perform_algorithm(elastica::configuration::par);
    /// [par_eg]

    /// [hybrid_eg]
    perform_algorithm(elastica::configuration::hybrid);
    /// [hybrid_eg]
  }

  template <typename ParallelPolicy, typename T>
  struct ToBeMade {};
  template <typename T>
  struct ToBeMade<hybrid_policy, T> {
    explicit ToBeMade(std::size_t threshold_parameter)
        : param_(threshold_parameter) {}
    std::size_t param_ IGNORE_UNUSED;
  };

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Configuration.ModuleConfiguration.Execution.make_executed",
      "[Unit][Simulator][Configuration]") {
    // clang-format on
    EMPTY_TEST();

    using namespace elastica::configuration::detail;
    using T = int;

    auto seq_made = make_executed<ToBeMade, T>(seq);
    static_assert(
        std::is_same_v<decltype(seq_made), ToBeMade<sequential_policy, int>>,
        "meta failure");

    auto par_made = make_executed<ToBeMade, T>(par);
    static_assert(
        std::is_same_v<decltype(par_made), ToBeMade<parallel_policy, int>>,
        "meta failure");

    // passes 5UL into the constructor
    auto hybrid_made = make_executed<ToBeMade, T>(hybrid, 5UL);
    static_assert(
        std::is_same_v<decltype(hybrid_made), ToBeMade<hybrid_policy, int>>,
        "meta failure");

    elastica::ignore_unused(seq_made, par_made, hybrid_made);
  }

}  // namespace