//
// Created by Tejaswin Parthasarathy on 6/25/21.
//

#include <atomic>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <unordered_map>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Configuration/ModuleConfiguration/Systems.hpp"
#include "Simulator/Interface/Systems/ExecutionPolicy.hpp"
#include "Simulator/Interface/Systems/Mocks.hpp"
#include "Utilities/AsConst.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/NonCopyable.hpp"

namespace {
  using namespace ::elastica::interface::systems;
  using namespace ::elastica::configuration;
}  // namespace

namespace {
  // defines func to apply over systems
  // we choose one that counts how many of each system types there are
  // since it might need to happen in parallel, we use a std::atomic.
  // we also have a non-copyable version to check forwarding.

  // to make it copyable have a separate cache and separate counter
  template <typename ListOfSystem>
  struct CounterCache {
    using LUT = std::unordered_map<std::type_index, std::atomic<std::size_t>>;
    CounterCache() : type_counts() {
      // https://stackoverflow.com/a/46029455
      tmpl::for_each<ListOfSystem>([&](auto v) {
        using Type = tmpl::type_from<decltype(v)>;
        type_counts.emplace(std::type_index(typeid(Type)), 0UL);
      });
    }
    ~CounterCache() = default;

    LUT type_counts;
  };

  template <typename ListOfSystems>
  struct Counter {
    using Cache = CounterCache<ListOfSystems>;

    // thought i could use type erauser but too mich hassle
    explicit Counter(Cache& arg_cache) : cache(arg_cache), value(0){};
    Counter(Counter const&) = default;
    Counter(Counter&& other) noexcept : cache(other.cache), value(other.value) {
      other.value = 5;
    };

    template <typename System>
    void operator()(System& syst) const {
      // increment and decrement the system for testing modifyability if
      // and when needed
      ++syst.data;
      --syst.data;

      ++cache.type_counts[std::type_index(typeid(System))];
    }

    template <typename System>
    inline std::size_t fetch() {
      return cache.type_counts[std::type_index(typeid(System))];
    }

    Cache& cache;
    int value;
  };

  // its already non-copyable becaues atomics copy constructor is deleted
  template <typename ListOfSystem>
  struct NonCopyableCounter : public Counter<ListOfSystem>,
                              protected ::elastica::NonCopyable {
    using P = Counter<ListOfSystem>;
    using P::P;
    NonCopyableCounter(NonCopyableCounter&&) noexcept = default;
  };

}  // namespace

namespace {
  template <typename... ConfigArgs>
  auto test_execution_algorithms(ConfigArgs&&... config_args) {
    auto make_config = [&config_args...]() {
      return ::mock::make_systems_configuration(
          // this does not matter
          make_blocking_policy(RestrictSizeAcrossBlockTypes{20UL}),
          std::forward<ConfigArgs>(config_args)...);
    };

    auto config = make_config();
    //
    using Config = std::decay_t<decltype(config)>;
    using Sim = ::mock::SimulatorWithSystems<Config>;
    using Algorithm = typename Sim::Algorithm;
    // using TimeType = typename Sim::TimeType;
    using ListOfSystem = typename Sim::ListOfSystem;
    //
    Sim sim(std::move(config));
    {
      auto re_config = make_config();
      // test if the default non-tuple constructor works
      Algorithm algorithm(&sim, re_config);
      (void)algorithm;
    }
    auto re_config = make_config();
    Algorithm algorithm(
        std::forward_as_tuple(&sim, cpp17::as_const(re_config)));

    auto check_counter_for_correctness = [&sim](auto& counter) {
      return [&](auto v) {
        using SystemType = tmpl::type_from<decltype(v)>;
        CHECK(std::get<std::vector<SystemType>>(
                  sim.get_system_collection_container())
                  .size() == counter.template fetch<SystemType>());
      };
    };
    auto check_counter_is_zero = [](auto& counter) {
      return [&](auto v) {
        using SystemType = tmpl::type_from<decltype(v)>;
        CHECK(counter.template fetch<SystemType>() == 0UL);
      };
    };

    // For all types run the functor and see if it updates
    {
      CounterCache<ListOfSystem> cache;
      // can be copied freely inside now that it holds only a reference.
      Counter<ListOfSystem> type_counter(cache);
      algorithm(type_counter);
      tmpl::for_each<ListOfSystem>(check_counter_for_correctness(type_counter));
      CHECK(type_counter.value == 0);

      CounterCache<ListOfSystem> cache_for_noncopyable;
      NonCopyableCounter<ListOfSystem> noncopyable_type_counter(
          cache_for_noncopyable);
      auto updated_cache_for_noncopyable(
          algorithm(std::move(noncopyable_type_counter)));

      tmpl::for_each<ListOfSystem>(
          check_counter_for_correctness(updated_cache_for_noncopyable));
    }

    using TargetSystemType = tmpl::list<tmpl::front<ListOfSystem>>;
    using OtherSystemTypes = tmpl::pop_front<ListOfSystem>;
    // only for one type run the functor and see if it updates
    {
      CounterCache<ListOfSystem> cache_for_single_type;
      Counter<ListOfSystem> single_type_counter(cache_for_single_type);

      auto updated_single_type_counter(
          algorithm.template operator()<TargetSystemType>(single_type_counter));

      CHECK(updated_single_type_counter.value == 0);
      tmpl::for_each<TargetSystemType>(
          check_counter_for_correctness(updated_single_type_counter));
      tmpl::for_each<OtherSystemTypes>(
          check_counter_is_zero(updated_single_type_counter));

      CounterCache<ListOfSystem> cache_noncopyable;
      NonCopyableCounter<ListOfSystem> noncopyable_type_counter(
          cache_noncopyable);
      auto updated_type_counter(algorithm.template operator()<TargetSystemType>(
          std::move(noncopyable_type_counter)));
      tmpl::for_each<TargetSystemType>(
          check_counter_for_correctness(updated_type_counter));
      tmpl::for_each<OtherSystemTypes>(
          check_counter_is_zero(updated_type_counter));
    }

    // For many types run the functor and see if it updates
    {
      CounterCache<ListOfSystem> cache_for_multiple_types;
      Counter<ListOfSystem> multiple_type_counter(cache_for_multiple_types);
      algorithm.template operator()<OtherSystemTypes>(multiple_type_counter);

      CHECK(multiple_type_counter.value == 0);
      tmpl::for_each<TargetSystemType>(
          check_counter_is_zero(multiple_type_counter));
      tmpl::for_each<OtherSystemTypes>(
          check_counter_for_correctness(multiple_type_counter));

      CounterCache<ListOfSystem> cache_noncopyable;
      NonCopyableCounter<ListOfSystem> noncopyable_type_counter(
          cache_noncopyable);
      auto updated_type_counter(algorithm.template operator()<OtherSystemTypes>(
          std::move(noncopyable_type_counter)));
      tmpl::for_each<TargetSystemType>(
          check_counter_is_zero(updated_type_counter));
      tmpl::for_each<OtherSystemTypes>(
          check_counter_for_correctness(updated_type_counter));
    }

    return tmpl::type_<Sim>{};
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Systems.Execution.seq_seq",
      "[Unit][Simulator][Interface][Systems]") {
    // clang-format on
    auto sim_type = test_execution_algorithms(make_iteration_policy(
        iterate_across_system_types_in(seq), iterate_over_systems_in(seq)));
    elastica::ignore_unused(sim_type);
    // auto sim_type = test_execution_algorithms(seq);

    using Sim = tmpl::type_from<decltype(sim_type)>;
    static_assert(
        cpp17::is_same_v<typename Sim::Algorithm,
                         typename Sim::InvokeOverSystemsOp<sequential_policy,
                                                           sequential_policy>>,
        "Algorithm meta-failure!");
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Systems.Execution.seq_par",
      "[Unit][Simulator][Interface][Systems]") {
    // clang-format on
    auto sim_type = test_execution_algorithms(make_iteration_policy(
        iterate_across_system_types_in(seq), iterate_over_systems_in(par)));
    elastica::ignore_unused(sim_type);
    // auto sim_type = test_execution_algorithms(seq);

    using Sim = tmpl::type_from<decltype(sim_type)>;
    static_assert(
        cpp17::is_same_v<typename Sim::Algorithm,
                         typename Sim::InvokeOverSystemsOp<parallel_policy,
                                                           sequential_policy>>,
        "Algorithm meta-failure!");
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Systems.Execution.seq_hybrid",
      "[Unit][Simulator][Interface][Systems]") {
    // clang-format on
    auto sim_type = test_execution_algorithms(make_iteration_policy(
        iterate_across_system_types_in(seq), iterate_over_systems_in(hybrid),
        AllSerialBelow{2UL}));
    elastica::ignore_unused(sim_type);
    // auto sim_type = test_execution_algorithms(seq);

    using Sim = tmpl::type_from<decltype(sim_type)>;
    static_assert(
        cpp17::is_same_v<typename Sim::Algorithm,
                         typename Sim::InvokeOverSystemsOp<hybrid_policy,
                                                           sequential_policy>>,
        "Algorithm meta-failure!");
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Systems.Execution.par_seq",
      "[Unit][Simulator][Interface][Systems]") {
    // clang-format on
    auto sim_type = test_execution_algorithms(make_iteration_policy(
        iterate_across_system_types_in(par), iterate_over_systems_in(seq)));
    elastica::ignore_unused(sim_type);
    // auto sim_type = test_execution_algorithms(seq);

    using Sim = tmpl::type_from<decltype(sim_type)>;
    static_assert(
        cpp17::is_same_v<typename Sim::Algorithm,
                         typename Sim::InvokeOverSystemsOp<sequential_policy,
                                                           parallel_policy>>,
        "Algorithm meta-failure!");
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Systems.Execution.par_par",
      "[Unit][Simulator][Interface][Systems]") {
    // clang-format on
    auto sim_type = test_execution_algorithms(make_iteration_policy(
        iterate_across_system_types_in(par), iterate_over_systems_in(par)));
    elastica::ignore_unused(sim_type);
    // auto sim_type = test_execution_algorithms(seq);

    using Sim = tmpl::type_from<decltype(sim_type)>;
    static_assert(
        cpp17::is_same_v<typename Sim::Algorithm,
                         typename Sim::InvokeOverSystemsOp<parallel_policy,
                                                           parallel_policy>>,
        "Algorithm meta-failure!");
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Systems.Execution.par_hybrid",
      "[Unit][Simulator][Interface][Systems]") {
    // clang-format on
    auto sim_type = test_execution_algorithms(make_iteration_policy(
        iterate_across_system_types_in(par), iterate_over_systems_in(hybrid),
        AllSerialBelow{2UL}));
    elastica::ignore_unused(sim_type);
    // auto sim_type = test_execution_algorithms(seq);

    using Sim = tmpl::type_from<decltype(sim_type)>;
    static_assert(
        cpp17::is_same_v<
            typename Sim::Algorithm,
            typename Sim::InvokeOverSystemsOp<hybrid_policy, parallel_policy>>,
        "Algorithm meta-failure!");
  }

}  // namespace
