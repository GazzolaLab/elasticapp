//
// Created by Tejaswin Parthasarathy on 1/20/21.
//
#include "Framework/TestingFramework.hpp"
#include "MockSystems.hpp"
#include "Simulator/Interface/Indexers/Pair.hpp"
#include "Utilities/End.hpp"
#include "Utilities/Defer.hpp"

namespace {

  using namespace elastica::interface::detail;
  using namespace elastica::interface;
  using elastica::end;
  using elastica::from_end;
  using elastica::defer;

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Indexers.Pair.pair",
                     "[Unit][Simulator][Interface]") {
    {  // integer pairs
      /// [pair_example]
      // includes 1, but not 56
      auto p = pair(1, 56);
      /// [pair_example]
      CHECK(p.first() == 1);
      CHECK(p.second() == 56);
    }

    {  // non-integer pairs
      auto p = pair(end - 20, end - 10);
      CHECK(p.first().i == from_end{20}.i);
      CHECK(p.second().i == from_end{10}.i);
    }

    {  // non-integer pairs with end
      /// [pair_end_example]
      auto r = pair(20, end - 2);
      /// [pair_end_example]
      CHECK(r.first() == 20);
      CHECK(r.second().i == from_end{2}.i);
    }
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Indexers.Pair.to_numeric",
                     "[Unit][Simulator][Interface]") {
    auto first_sys = NoThrowSystem{};
    auto second_sys = NoThrowSystem{};
    {  // purely numeric
      auto p = pair(2, 10);
      auto numerical_indices = to_numeric(p, first_sys, second_sys);
      CHECK(numerical_indices.first == 3);
      CHECK(numerical_indices.second == 11);
    }
    {  // endss
      auto p = pair(end - 40, end - 10);
      auto numerical_indices = to_numeric(p, first_sys, second_sys);
      CHECK(numerical_indices.first == 41);
      CHECK(numerical_indices.second == 11);
    }
    {  // mixed
      /// [pair_to_numeric]
      auto first_system = NoThrowSystem{};
      auto second_system = NoThrowSystem{};
      auto p = pair(1, end - 2);
      auto numerical_indices = to_numeric(p, first_system, second_system);
      CHECK(numerical_indices.first == 2);
      CHECK(numerical_indices.second == 3);
      /// [pair_to_numeric]
    }
    {
      // deferred
      auto p = pair(defer, defer);
      auto deferred_indices = to_numeric(p, first_sys, second_sys);
      CHECK(&deferred_indices.first == &defer);
      CHECK(&deferred_indices.second == &defer);
    }
  }

  // [[OutputRegex, "as the number of degrees of freedom is only 100"]]
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Indexers.Pair.to_numeric_throws",
      "[Unit][Simulator][Interface]") {
    ERROR_TEST();
    auto first_sys = ThrowSystem{};
    auto second_sys = ThrowSystem{};
    {  //
      auto p = pair(1, 5);
      CHECK_THROWS_WITH(to_numeric(p, first_sys, second_sys),
                        Catch::Contains("as the number of degrees of freedom"));
    }

    {  //
      auto p = pair(end - 2, end - 5);
      CHECK_THROWS_WITH(to_numeric(p, first_sys, second_sys),
                        Catch::Contains("as the number of degrees of freedom"));
    }
  }

}  // namespace