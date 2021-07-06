//
// Created by Tejaswin Parthasarathy on 6/16/21.
//

#include "Framework/TestingFramework.hpp"
#include "Simulator/Configuration/ModuleConfiguration/IterationPolicy.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"

namespace {

  using namespace elastica::configuration;

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Configuration.ModuleConfiguration.Iteration.SerialBelow",
      "[Unit][Simulator][Configuration]") {
    // clang-format on

    // anything with a valid size
    using Block = int;
    using Blocks = std::vector<Block>;
    SerialBelow<Block> restricter{20UL};

    Blocks b(10UL);
    // if size(vec) <, passes
    CHECK(restricter(b));

    b.resize(25UL);
    CHECK(not restricter(b));
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Configuration.ModuleConfiguration.Iteration.AllSerialBelow",
      "[Unit][Simulator][Configuration]") {
    // clang-format on

    AllSerialBelow restricter{20UL};

    using FirstBlock = int;
    using SecondBlock = float;
    tmpl::for_each<tmpl::list<FirstBlock, SecondBlock>>([&](auto v) {
      using Block = tmpl::type_from<decltype(v)>;
      using Blocks = std::vector<Block>;
      Blocks blocks(10UL);
      CHECK(restricter(blocks));

      blocks.resize(25UL);
      CHECK(not restricter(blocks));
    });
  }

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Configuration.ModuleConfiguration.Iteration.interface",
      "[Unit][Simulator][Configuration]") {
    auto s = iterate_across_system_types_in(seq);
    auto p = iterate_across_system_types_in(par);

    auto for_each_s = iterate_over_systems_in(seq);
    auto for_each_p = iterate_over_systems_in(par);
    auto for_each_h = iterate_over_systems_in(hybrid);

    ::elastica::ignore_unused(s, p, for_each_s, for_each_p, for_each_h);

    {
      /// [seq_eg]
      using namespace ::elastica::configuration;
      auto iteration_policy = make_iteration_policy(
          iterate_across_system_types_in(seq), iterate_over_systems_in(seq));
      /// [seq_eg]

      static_assert(
          cpp17::is_same_v<
              decltype(iteration_policy),
              IterationPolicy<sequential_policy, sequential_policy, void>>,
          "Meta failure!");
      ::elastica::ignore_unused(iteration_policy);
    }

    {
      auto iteration_policy = make_iteration_policy(
          iterate_across_system_types_in(par), iterate_over_systems_in(seq));
      static_assert(
          cpp17::is_same_v<
              decltype(iteration_policy),
              IterationPolicy<parallel_policy, sequential_policy, void>>,
          "Meta failure!");
      ::elastica::ignore_unused(iteration_policy);
    }

    {
      auto iteration_policy = make_iteration_policy(
          iterate_across_system_types_in(par), iterate_over_systems_in(par));
      static_assert(
          cpp17::is_same_v<
              decltype(iteration_policy),
              IterationPolicy<parallel_policy, parallel_policy, void>>,
          "Meta failure!");
      ::elastica::ignore_unused(iteration_policy);
    }

    {
      /// [mixed_eg]
      using namespace ::elastica::configuration;
      auto iteration_policy = make_iteration_policy(
          iterate_across_system_types_in(seq), iterate_over_systems_in(par));
      /// [mixed_eg]

      static_assert(
          cpp17::is_same_v<
              decltype(iteration_policy),
              IterationPolicy<sequential_policy, parallel_policy, void>>,
          "Meta failure!");
      ::elastica::ignore_unused(iteration_policy);
    }

    // confirmed that passing in criteria without a hybrid type does not work.

    {
      /// [hybrid_eg]
      using namespace ::elastica::configuration;
      using Block = int;
      using Criteria = SerialBelow<Block>;
      auto iteration_policy =
          make_iteration_policy(iterate_across_system_types_in(seq),
                                iterate_over_systems_in(hybrid), Criteria{20});
      /// [hybrid_eg]
      static_assert(
          cpp17::is_same_v<
              decltype(iteration_policy),
              IterationPolicy<sequential_policy, hybrid_policy, Criteria>>,
          "Meta failure!");
      using Ints = std::vector<int>;
      Ints iv(10UL);
      CHECK(iteration_policy(iv));
      iv.resize(30UL);
      CHECK(not iteration_policy(iv));

      using AllCriteria = AllSerialBelow;
      auto another_iteration_policy = make_iteration_policy(
          iterate_across_system_types_in(seq), iterate_over_systems_in(hybrid),
          AllCriteria{20});
      static_assert(
          cpp17::is_same_v<
              decltype(another_iteration_policy),
              IterationPolicy<sequential_policy, hybrid_policy, AllCriteria>>,
          "Meta failure!");

      using Chars = std::vector<char>;
      Chars cv(10UL);
      CHECK(another_iteration_policy(cv));
      CHECK(not another_iteration_policy(iv));

      ::elastica::ignore_unused(iteration_policy, another_iteration_policy);
    }

    {
      using IntCriteria = SerialBelow<int>;
      using CharCriteria = SerialBelow<char>;
      auto iteration_policy = make_iteration_policy(
          iterate_across_system_types_in(seq), iterate_over_systems_in(hybrid),
          IntCriteria{20UL}, CharCriteria{40UL});
      static_assert(
          cpp17::is_same_v<
              decltype(iteration_policy),
              IterationPolicy<sequential_policy, hybrid_policy,
                              CompositeCriteria<IntCriteria, CharCriteria>>>,
          "Meta failure!");

      using Ints = std::vector<int>;
      using Chars = std::vector<char>;
      Ints iv(10UL);
      Chars cv(10UL);

      CHECK(iteration_policy(iv));
      iv.resize(30UL);
      CHECK(not iteration_policy(iv));

      CHECK(iteration_policy(cv));
      cv.resize(30UL);
      CHECK(iteration_policy(cv));
      cv.resize(50UL);
      CHECK(not iteration_policy(cv));

      ::elastica::ignore_unused(iteration_policy);
    }
  }

}  // namespace