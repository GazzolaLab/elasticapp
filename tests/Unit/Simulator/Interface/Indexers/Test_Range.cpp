//
// Created by Tejaswin Parthasarathy on 1/12/21.
//

#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Indexers/Range.hpp"
#include "Simulator/Interface/MockSystems.hpp"

namespace {

  using namespace elastica::interface::detail;
  using namespace elastica::interface;
  using elastica::end;
  using elastica::from_end;
  using mock::NoThrowSystem;
  using mock::ThrowSystem;

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Indexers.Range.range",
                     "[Unit][Simulator][Interface]") {
    {  // integer ranges
      /// [range_example]
      // includes 1, but not 56
      auto r = range(1, 56, 2);
      /// [range_example]
      CHECK(r.start_ == 1);
      CHECK(r.stop_ == 56);
      CHECK(r.step_ == 2);
    }

    {  // non-integer ranges
      auto r = range(end - 20, end - 10, 1);
      CHECK(r.start_.i == from_end{20}.i);
      CHECK(r.stop_.i == from_end{10}.i);
      CHECK(r.step_ == 1);
    }

    {  // non-integer ranges with end
      /// [range_end_example]
      auto r = range(10, end, 1);
      /// [range_end_example]
      CHECK(r.start_ == 10);
      CHECK(r.stop_.i == from_end{0}.i);
      CHECK(r.step_ == 1);
    }
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Indexers.Range.to_numeric",
                     "[Unit][Simulator][Interface]") {
    auto sys = NoThrowSystem{};
    {  // purely numeric
      auto r = range(2UL, 10UL, 2UL);
      auto numerical_indices = to_numeric(r, sys);
      CHECK(numerical_indices.start_ == 3);
      CHECK(numerical_indices.stop_ == 11);
      // step will always be untouched
      CHECK(numerical_indices.step_ == 2);
    }
    {  // endss
      auto r = range(end - 40, end - 10, 1);
      auto numerical_indices = to_numeric(r, sys);
      CHECK(numerical_indices.start_ == (200 - 40));
      CHECK(numerical_indices.stop_ == (200 - 10));
      // step will always be untouched
      CHECK(numerical_indices.step_ == 1);
    }
    {  // mixed
      /// [range_to_numeric]
      auto system = NoThrowSystem{};
      auto r = range(1UL, end - 2, 2UL);
      auto numerical_indices = to_numeric(r, system);
      CHECK(numerical_indices.start_ == 2);
      CHECK(numerical_indices.stop_ == (200 - 2));
      // step will always be untouched
      CHECK(numerical_indices.step_ == 2);
      /// [range_to_numeric]
    }
  }

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Indexers.Range.to_numeric_throws",
      "[Unit][Simulator][Interface]") {
    auto sys = ThrowSystem{};
    {  //
      auto r = range(1UL, 5UL, 1UL);
      CHECK_THROWS_WITH(to_numeric(r, sys),
                        Catch::Contains("Number of degrees of freedom"));
    }

    {  //
      auto r = range(end - 2, end - 5, 2);
      CHECK_THROWS_WITH(to_numeric(r, sys),
                        Catch::Contains("Number of degrees of freedom"));
    }
  }

}  // namespace
