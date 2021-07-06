//
// Created by Tejaswin Parthasarathy on 6/24/21.
//

#include <algorithm>
#include <memory>
#include <tuple>
#include <vector>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Forcing/ExecutionPolicy.hpp"
#include "Simulator/Interface/Forcing/MockForcing.hpp"
#include "Utilities/IgnoreUnused.hpp"

namespace {
  using namespace ::elastica::interface::forcing;
  using namespace ::elastica::configuration;
}  // namespace

namespace {

  //  template <typename Config>
  //  auto make_simulator_with_forcing(Config&& config) {
  //    return mock::SimulatorWithForcing<std::decay_t<Config>>{};
  //  }

  template <typename... ConfigArgs>
  auto test_execution_algorithms(ConfigArgs&&... config_args) {
    auto config = ::mock::make_forcing_configuration(
        std::forward<ConfigArgs>(config_args)...);
    //
    using Config = std::decay_t<decltype(config)>;
    using Sim = mock::SimulatorWithForcing<Config>;
    using Algorithm = typename Sim::Algorithm;
    using C = typename Sim::C;
    using TimeType = typename Sim::TimeType;
    //
    auto sim = Sim{};
    {
      // test if the default non-tuple constructor works
      Algorithm algorithm(&sim, config);
      (void)algorithm;
    }
    Algorithm algorithm(std::forward_as_tuple(&sim, config));
    //    Algorithm algorithm(&sim, config);
    //    RateAlgorithm rate_algorithm(&sim, config);

    // push back unique ptrs to sim.
    std::vector<int> fill_values(10UL);
    std::iota(fill_values.begin(), fill_values.end(), 0);

    for (std::size_t i = 0UL; i < fill_values.size(); ++i) {
      sim.forcers().emplace_back(std::make_unique<C>(i));
    }

    TimeType time_v{};
    // each call increments i value by 2 and decrements by 1, so total +1
    algorithm(time_v);

    // now check
    std::vector<int> check_values;
    std::transform(fill_values.begin(), fill_values.end(),
                   std::back_inserter(check_values),
                   [](int i) -> int { return i + 1; });

    //
    std::vector<int> current_values;
    std::transform(sim.forcers().begin(), sim.forcers().end(),
                   std::back_inserter(current_values),
                   [](auto const& ptr) -> int { return ptr->value(); });

    CHECK(current_values == check_values);

    return tmpl::type_<Sim>{};
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Forcing.Execution.sequential",
      "[Unit][Simulator][Interface][Forcing]") {
    // clang-format on
    auto sim_type = test_execution_algorithms(seq);
    elastica::ignore_unused(sim_type);

    using Sim = tmpl::type_from<decltype(sim_type)>;
    static_assert(cpp17::is_same_v<
                      typename Sim::Algorithm,
                      typename Sim::ForceSystemsOp<sequential_policy>>,
                  "Algorithm meta-failure!");
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Forcing.Execution.parallel",
      "[Unit][Simulator][Interface][Forcing]") {
    // clang-format on
    auto sim_type = test_execution_algorithms(par);
    elastica::ignore_unused(sim_type);

    using Sim = tmpl::type_from<decltype(sim_type)>;
    static_assert(
        cpp17::is_same_v<typename Sim::Algorithm,
            typename Sim::ForceSystemsOp<parallel_policy>>,
        "Algorithm meta-failure!");
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Forcing.Execution.hybrid",
      "[Unit][Simulator][Interface][Forcing]") {
    // clang-format on
    auto sim_type = test_execution_algorithms(hybrid, 5UL);
    elastica::ignore_unused(sim_type);

    using Sim = tmpl::type_from<decltype(sim_type)>;
    static_assert(
        cpp17::is_same_v<typename Sim::Algorithm,
            typename Sim::ForceSystemsOp<hybrid_policy>>,
        "Algorithm meta-failure!");
  }

}  // namespace
