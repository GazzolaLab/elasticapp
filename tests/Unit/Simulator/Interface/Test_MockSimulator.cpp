//
// Created by Tejaswin Parthasarathy on 6/21/21.
//

#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/MockComponent.hpp"
#include "Simulator/Interface/MockSimulator.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TaggedTuple.hpp"

namespace {

  template <typename Sim>
  void check_algorithms_in(Sim const& sim) {
    using TimeType = typename Sim::TimeType;
    tmpl::for_each<typename Sim::AlgorithmSteps>(
        [&, time_v = TimeType{}](auto v) {
          using Step = tmpl::type_from<decltype(v)>;
          auto& update_step = tuples::get<Step>(sim.algorithms);
          CHECK(update_step.counter == 0UL);
          update_step(time_v);
          CHECK(update_step.counter == 1UL);
        });
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.TestMockSimulator", "[Unit]") {
    // clang-format on
    auto sim_config =
        mock::make_simulator_configuration(mock::EmptyConfiguration{});
    using TimeType = typename decltype(sim_config)::TimeType;

    {
      const bool all_registered = false;
      auto sim =
          mock::make_simulator<mock::EmptyModule>(sim_config, all_registered);
      CHECK_FALSE(sim.ready());
      CHECK_NOTHROW(sim.check_if_not_final());
      tmpl::for_each<tmpl::list<char, int, float, double>>([&](auto t) {
        using type = tmpl::type_from<decltype(t)>;
        CHECK_FALSE(sim.check_if_registered(type{2}));
        CHECK_THROWS(sim.confirm_as_registered(type{2}));
      });

      TimeType time_v{};
      sim.finalize(time_v);

      CHECK(sim.ready());
      CHECK_THROWS(sim.check_if_not_final());

      check_algorithms_in(sim);
    }

    {
      const bool all_registered = true;
      auto sim =
          mock::make_simulator<mock::EmptyModule>(sim_config, all_registered);
      CHECK_FALSE(sim.ready());
      CHECK_NOTHROW(sim.check_if_not_final());

      tmpl::for_each<tmpl::list<char, int, float, double>>([&](auto t) {
        using type = tmpl::type_from<decltype(t)>;
        CHECK(sim.check_if_registered(type{2}));
        CHECK_NOTHROW(sim.confirm_as_registered(type{2}));
      });

      TimeType time_v{};
      sim.finalize(time_v);

      CHECK(sim.ready());
      CHECK_THROWS(sim.check_if_not_final());

      check_algorithms_in(sim);
    }
  }

}  // namespace
