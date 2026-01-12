//
// Created by Tejaswin Parthasarathy on 6/24/21.
//

#include <algorithm>
#include <memory>
#include <tuple>
#include <vector>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Constraints/ExecutionPolicy.hpp"
#include "Simulator/Interface/Constraints/MockConstraints.hpp"
#include "Utilities/IgnoreUnused.hpp"

namespace {
  using namespace ::elastica::interface::constraints;
  using namespace ::elastica::configuration;
}  // namespace

namespace {

  //  template <typename Config>
  //  auto make_simulator_with_constraints(Config&& config) {
  //    return mock::SimulatorWithConstraints<std::decay_t<Config>>{};
  //  }

  template <typename... ConfigArgs>
  auto test_execution_algorithms(ConfigArgs&&... config_args) {
    auto config = ::mock::make_constraints_configuration(
        std::forward<ConfigArgs>(config_args)...);
    //
    using Config = std::decay_t<decltype(config)>;
    using Sim = mock::SimulatorWithConstraints<Config>;
    using PrimitiveAlgorithm = typename Sim::PrimitiveAlgorithm;
    using RateAlgorithm = typename Sim::RateAlgorithm;
    using C = typename Sim::C;
    using TimeType = typename Sim::TimeType;
    //
    auto sim = Sim{};
    {
      // test if the default non-tuple constructor works
      PrimitiveAlgorithm algorithm(&sim, config);
      (void)algorithm;
    }
    {
      // test if the default non-tuple constructor works
      RateAlgorithm algorithm(&sim, config);
      (void)algorithm;
    }
    PrimitiveAlgorithm primitive_algorithm(std::make_tuple(&sim, config));
    RateAlgorithm rate_algorithm(std::forward_as_tuple(&sim, config));
    //    PrimitiveAlgorithm primitive_algomake_tuplerithm(&sim, config);
    //    RateAlgorithm rate_algorithm(&sim, config);

    // push back unique ptrs to sim.
    std::vector<int> fill_values(10UL);
    std::iota(fill_values.begin(), fill_values.end(), 0);

    sim.constrainer_batches().resize(5UL);
    for (std::size_t i = 0UL; i < fill_values.size(); ++i) {
      for (std::size_t idx = 0UL; idx < 5UL; ++idx)
        sim.constrainer_batches()[idx].emplace_back(std::make_unique<C>(i));
    }

    TimeType time_v{};
    // each call increments i value by 2
    primitive_algorithm(time_v);

    // now check
    std::vector<int> check_values;
    for (std::size_t idx = 0UL; idx < 5UL; ++idx) {
      std::transform(fill_values.begin(), fill_values.end(),
                     std::back_inserter(check_values),
                     [](int i) -> int { return i + 2; });
    }

    //
    std::vector<int> current_values;
    for (auto const& batch : sim.constrainer_batches())
      std::transform(batch.begin(), batch.end(),
                     std::back_inserter(current_values),
                     [](auto const& ptr) -> int { return ptr->value(); });

    CHECK(current_values == check_values);

    // each call decreases i value by 1
    rate_algorithm(time_v);
    rate_algorithm(time_v);

    current_values.clear();
    for (auto const& batch : sim.constrainer_batches())
      std::transform(batch.begin(), batch.end(),
                     std::back_inserter(current_values),
                     [](auto const& ptr) -> int { return ptr->value(); });

    std::vector<int> filled_values;
    filled_values.reserve(5UL * (fill_values).size());
    for (std::size_t idx = 0UL; idx < 5UL; ++idx) {
      std::copy(fill_values.begin(), fill_values.end(),
                std::back_inserter(filled_values));
    }
    // std::iota(fill_values.begin(), fill_values.end(), 0);

    CHECK(current_values == filled_values);

    return tmpl::type_<Sim>{};
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Constraints.Execution.sequential",
      "[Unit][Simulator][Interface][Constraints]") {
    // clang-format on
    auto sim_type = test_execution_algorithms(seq);
    elastica::ignore_unused(sim_type);

    using Sim = tmpl::type_from<decltype(sim_type)>;
    static_assert(std::is_same_v<
                      typename Sim::PrimitiveAlgorithm,
                      typename Sim::ConstrainPrimitivesOp<sequential_policy>>,
                  "Algorithm meta-failure!");
    static_assert(
        std::is_same_v<typename Sim::RateAlgorithm,
                         typename Sim::ConstrainRatesOp<sequential_policy>>,
        "Algorithm meta-failure!");
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Constraints.Execution.parallel",
      "[Unit][Simulator][Interface][Constraints]") {
    // clang-format on
    auto sim_type = test_execution_algorithms(par);
    elastica::ignore_unused(sim_type);

    using Sim = tmpl::type_from<decltype(sim_type)>;
    static_assert(
        std::is_same_v<typename Sim::PrimitiveAlgorithm,
                         typename Sim::ConstrainPrimitivesOp<parallel_policy>>,
        "Algorithm meta-failure!");
    static_assert(
        std::is_same_v<typename Sim::RateAlgorithm,
                         typename Sim::ConstrainRatesOp<parallel_policy>>,
        "Algorithm meta-failure!");
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Constraints.Execution.hybrid",
      "[Unit][Simulator][Interface][Constraints]") {
    // clang-format on
    auto sim_type = test_execution_algorithms(hybrid, 5UL);
    elastica::ignore_unused(sim_type);

    using Sim = tmpl::type_from<decltype(sim_type)>;
    static_assert(
        std::is_same_v<typename Sim::PrimitiveAlgorithm,
                         typename Sim::ConstrainPrimitivesOp<hybrid_policy>>,
        "Algorithm meta-failure!");
    static_assert(
        std::is_same_v<typename Sim::RateAlgorithm,
                         typename Sim::ConstrainRatesOp<hybrid_policy>>,
        "Algorithm meta-failure!");
  }

}  // namespace
