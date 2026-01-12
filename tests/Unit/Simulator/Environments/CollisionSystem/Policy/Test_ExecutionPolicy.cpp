//
// Created by Tejaswin Parthasarathy on 3/24/22.
//
#include <atomic>
#include <vector>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Environments/CollisionSystem/Policy/ExecutionPolicy.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/TMPL.hpp"

namespace {
  using namespace elastica::collision::detail;

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Environments.CollisionSystem.ExecutionPolicy.ForEach",
      "[Unit][Environments]") {
    // clang-format on
    using value_type = int;

    struct Accumulator {
      Accumulator(value_type v) : val{v} {}
      void operator()(value_type in) { val += in; }
      explicit operator value_type() const { return val; }
      std::atomic<value_type> val{0};
    };

    std::size_t n_values = 50UL;
    std::vector<value_type> data(n_values);
    std::iota(std::begin(data), std::end(data), 0);
    auto expected_answer =
        static_cast<value_type>(n_values * (n_values - 1) / 2);

    SECTION("Sequential") {
      using Policy = ::elastica::configuration::sequential_policy;
      using F = ForEachOp<Policy>;
      Accumulator answer{0UL};
      F::for_each(std::begin(data), std::end(data), std::ref(answer));
      CHECK(static_cast<value_type>(answer) == expected_answer);
    }
    SECTION("Parallel") {
      using Policy = ::elastica::configuration::parallel_policy;
      using F = ForEachOp<Policy>;
      Accumulator answer{0UL};
      F::for_each(std::begin(data), std::end(data), std::ref(answer));
      CHECK(static_cast<value_type>(answer) == expected_answer);
    }
    SECTION("Hybrid") {
      using Policy = ::elastica::configuration::hybrid_policy;
      using F = ForEachOp<Policy>;
      Accumulator answer{0UL};
      F::for_each(std::begin(data), std::end(data), std::ref(answer));
      CHECK(static_cast<value_type>(answer) == expected_answer);
    }
  }

  struct RepeatCounter {
    explicit RepeatCounter() : val{0} {}

    template <typename T, typename U>
    void operator()(T /*first*/, U /*second*/) {
      using F = tmpl::type_from<T>;
      using G = tmpl::type_from<U>;
      val += ((F::value) == (G::value));
    }
    explicit operator std::size_t() const { return val; }
    std::atomic<std::size_t> val;
  };

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Environments.CollisionSystem.ExecutionPolicy.OverPairs",
      "[Unit][Environments]") {
    // clang-format on
    using types = tmpl::range<int, 1, 6>;
    using distinct_matrix = elastica::combinations<types>;
    std::size_t expected_answer = 5UL;  // 6 - 1

    SECTION("Sequential") {
      using Policy = ::elastica::configuration::sequential_policy;
      using F = OverPairsOp<Policy>;
      RepeatCounter answer{};
      F::pairwise_for_each<distinct_matrix>(std::ref(answer));
      CHECK(static_cast<std::size_t>(answer) == expected_answer);
    }

    SECTION("Parallel") {
      using Policy = ::elastica::configuration::parallel_policy;
      using F = OverPairsOp<Policy>;
      RepeatCounter answer{};
      F::pairwise_for_each<distinct_matrix>(std::ref(answer));
      CHECK(static_cast<std::size_t>(answer) == expected_answer);
    }

    SECTION("Hybrid") {
      using Policy = ::elastica::configuration::hybrid_policy;
      using F = OverPairsOp<Policy>;
      RepeatCounter answer{};
      F::pairwise_for_each<distinct_matrix>(std::ref(answer));
      CHECK(static_cast<std::size_t>(answer) == expected_answer);
    }
  }

  // meta-tests
  template <typename F, typename P>
  using KEP = elastica::collision::KernelExecutionPolicy<F, P>;

  template <typename T>
  struct CheckExecutionPolicy : public std::true_type {
    using ForEachPolicy = tmpl::at_c<T, 0UL>;
    using OverPairPolicy = tmpl::at_c<T, 1UL>;
    using ExpectedAnswer = tmpl::at_c<T, 2UL>;

    using Policy = KEP<ForEachPolicy, OverPairPolicy>;
    static_assert(
        std::is_same_v<typename Policy::ExecutionPolicy, ExpectedAnswer>,
        "Meta-failure");
  };

  using AllPossibleConfigurations =
      tmpl::list<tmpl::list<::elastica::configuration::sequential_policy,
                            ::elastica::configuration::sequential_policy,
                            ::elastica::configuration::sequential_policy>,
                 tmpl::list<::elastica::configuration::parallel_policy,
                            ::elastica::configuration::sequential_policy,
                            ::elastica::configuration::parallel_policy>,
                 tmpl::list<::elastica::configuration::sequential_policy,
                            ::elastica::configuration::parallel_policy,
                            ::elastica::configuration::parallel_policy>,
                 tmpl::list<::elastica::configuration::parallel_policy,
                            ::elastica::configuration::parallel_policy,
                            ::elastica::configuration::parallel_policy>,
                 tmpl::list<::elastica::configuration::hybrid_policy,
                            ::elastica::configuration::sequential_policy,
                            ::elastica::configuration::hybrid_policy>,
                 tmpl::list<::elastica::configuration::sequential_policy,
                            ::elastica::configuration::hybrid_policy,
                            ::elastica::configuration::hybrid_policy>,
                 tmpl::list<::elastica::configuration::hybrid_policy,
                            ::elastica::configuration::parallel_policy,
                            ::elastica::configuration::hybrid_policy>,
                 tmpl::list<::elastica::configuration::parallel_policy,
                            ::elastica::configuration::hybrid_policy,
                            ::elastica::configuration::hybrid_policy>,
                 tmpl::list<::elastica::configuration::hybrid_policy,
                            ::elastica::configuration::hybrid_policy,
                            ::elastica::configuration::hybrid_policy>>;

  // force execution
  using expected_configuration_checks IGNORE_UNUSED =
      decltype(tmpl::transform<AllPossibleConfigurations,
                               CheckExecutionPolicy<tmpl::_1>>{});

}  // namespace
