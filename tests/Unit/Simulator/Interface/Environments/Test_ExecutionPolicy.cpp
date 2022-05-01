//
// Created by Tejaswin Parthasarathy on 2/17/22.
//

#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Environments/ExecutionPolicy.hpp"
#include "Simulator/Interface/Environments/MockEnvironments.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/MakeArray.hpp"

namespace {
  using namespace ::elastica::configuration;
}  // namespace

namespace {
  template <typename... ConfigArgs>
  auto test_execution_algorithms(ConfigArgs&&... config_args) {
    auto config = ::mock::make_environments_configuration(
        std::forward<ConfigArgs>(config_args)...);

    using Config = std::decay_t<decltype(config)>;
    using Sim = mock::SimulatorWithEnvironments<Config>;
    using Algorithm = typename Sim::Algorithm;
    using E = typename Sim::E;
    using TimeType = typename Sim::TimeType;

    //
    auto sim = Sim{};

    std::array<std::size_t, sim.n_environments()> counters;
    std::array<std::size_t, sim.n_environments()> expected_counters;

    auto envs_tuple = index_apply<sim.n_environments()>([&](auto... Is) {
      return std::make_tuple(
          [Is, &i = counters[Is]](TimeType) mutable { i += (Is + 1); }...);
    });

    std::fill(std::begin(counters), std::end(counters), 0UL);
    std::iota(std::begin(expected_counters), std::end(expected_counters), 1UL);

    sim.register_envs(std::move(envs_tuple));

    {
      // test if the default non-tuple constructor works
      Algorithm algorithm(&sim, config);
      (void)algorithm;
    }
    Algorithm algorithm(std::forward_as_tuple(&sim, config));

    TimeType time_v{};
    algorithm(time_v);

    CHECK(counters == expected_counters);

    return tmpl::type_<Sim>{};
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Environments.Execution.sequential",
      "[Unit][Simulator][Interface][Environments]") {
    // clang-format on
    auto sim_type = test_execution_algorithms(seq);
    elastica::ignore_unused(sim_type);

    using Sim = tmpl::type_from<decltype(sim_type)>;
    static_assert(
        cpp17::is_same_v<
            typename Sim::Algorithm,
            typename Sim::ApplyEnvironmentForcesOp<sequential_policy>>,
        "Algorithm meta-failure!");
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Environments.Execution.parallel",
      "[Unit][Simulator][Interface][Environments]") {
    // clang-format on
    auto sim_type = test_execution_algorithms(par);
    elastica::ignore_unused(sim_type);

    using Sim = tmpl::type_from<decltype(sim_type)>;
    static_assert(cpp17::is_same_v<
                      typename Sim::Algorithm,
                      typename Sim::ApplyEnvironmentForcesOp<parallel_policy>>,
                  "Algorithm meta-failure!");
  }

}  // namespace