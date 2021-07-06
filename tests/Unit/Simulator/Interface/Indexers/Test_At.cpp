//
// Created by Tejaswin Parthasarathy on 1/12/21.
//

#include <array>
#include <utility>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Indexers/At.hpp"
#include "Simulator/Interface/Indexers/Pair.hpp"
#include "Simulator/Interface/Indexers/Range.hpp"
#include "Simulator/Interface/MockSystems.hpp"
#include "Utilities/Defer.hpp"
#include "Utilities/End.hpp"
#include "Utilities/MakeArray.hpp"

namespace {

  using namespace elastica::interface::detail;
  using namespace elastica::interface;
  using elastica::defer;
  using elastica::end;
  using elastica::from_end;
  using mock::NoThrowSystem;
  using mock::ThrowSystem;

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Indexers.At.constructor",
                     "[Unit][Simulator][Interface]") {
    constexpr auto Nints = 1U;
    constexpr auto Nends = 1U;
    {  // pair constructor
      std::array<std::size_t, Nints> first{1};
      std::array<from_end, Nints> second{(end - 2)};
      At_t<Nints, Nends> at{std::make_pair(first, second)};
      CHECK(at.indices_.first[0] == 1);
      CHECK(at.indices_.second[0].i == 2);
    }

    {
      // piecewise
      std::array<std::size_t, Nints> first{3};
      std::array<from_end, Nints> second{(end - 5)};
      At_t<Nints, Nends> at(first, second);
      CHECK(at.indices_.first[0] == 3);
      CHECK(at.indices_.second[0].i == 5);
    }
  }

  template <std::uint8_t Nints, std::uint8_t Nends>
  bool check(
      elastica::interface::detail::At_t<Nints, Nends> const& first,
      elastica::interface::detail::At_t<Nints, Nends> const& second) noexcept {
    bool passed = true;
    // to make the
    // warning: comparison is always false due to limited range of data type
    // [-Wtype-limits]
    // go away
    if (Nints) {
      for (std::uint8_t i = 0; i < Nints; ++i) {
        passed &= (first.indices_.first[i] == second.indices_.first[i]);
      }
    }
    // to make the
    // warning: comparison is always false due to limited range of data type
    // [-Wtype-limits]
    // go away
    if (Nends) {
      for (std::uint8_t i = 0; i < Nends; ++i) {
        passed &= (first.indices_.second[i].i == second.indices_.second[i].i);
      }
    }
    return passed;
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Indexers.At.at",
                     "[Unit][Simulator][Interface]") {
    {  // no ends
      auto first = make_array(1UL, 3UL, 5UL);
      constexpr auto Nints = std::tuple_size<decltype(first)>::value;
      constexpr auto Nends = 0UL;
      {  // prvalue
        auto x = at(1UL, 3UL, 5UL);
        CHECK(check(x, At_t<Nints, Nends>(first, {})));
      }

      {  // lvalue
        std::size_t a = 1, b = 3, c = 5;
        auto x = at(a, b, c);
        CHECK(check(x, At_t<Nints, Nends>(first, {})));
      }
    }

    {  // no ends
      auto second = make_array(end - 2, end - 4, end - 6);
      constexpr auto Nints = 0UL;
      constexpr auto Nends = std::tuple_size<decltype(second)>::value;
      {  // prvalue
        auto x = at(end - 2, end - 4, end - 6);
        CHECK(check(x, At_t<Nints, Nends>({}, second)));
      }
      {  // lvalue
        auto a = end - 2;
        auto b = end - 4;
        auto c = end - 6;
        auto x = at(a, b, c);
        CHECK(check(x, At_t<Nints, Nends>({}, second)));
      }
    }

    {  // both
      auto first = make_array(1UL, 3UL, 5UL);
      auto second = make_array(end - 2, end - 4, end - 6);
      constexpr auto Nints = std::tuple_size<decltype(first)>::value;
      constexpr auto Nends = std::tuple_size<decltype(second)>::value;
      /// [at_example]
      auto x = at(1UL, end - 2, end - 4, 3UL, 5UL, end - 6);
      /// [at_example]
      CHECK(check(x, At_t<Nints, Nends>(first, second)));
    }
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Indexers.At.at_for_range",
                     "[Unit][Simulator][Interface]") {
    {  // constepxr
      /// [at_range_example]
      auto r = range(1, 10, 1);
      static_assert(&r == &at(r), "Invariant violated!");
      /// [at_range_example]
    }

    {
      // lvalue
      auto r = range(1, 10, 1);
      CHECK(&r == &at(r));
    }

    {
      // const lvalue
      auto const r = range(1, 10, 1);
      CHECK(&r == &at(r));
    }

    {
      // prvalue
      CHECK(1 == at(range(1, 10, 1)).start_);
      CHECK(10 == at(range(1, 10, 1)).stop_);
      CHECK(1 == at(range(1, 10, 1)).start_);
    }

    {  // constexpr
      static_assert(1 == at(range(1, 10, 1)).start_,
                    "constexpr range forwarding failure");
      static_assert(10 == at(range(1, 10, 1)).stop_,
                    "constexpr range forwarding failure");
    }

    {
      // xrvalue
      auto r = range(1, 10, 1);
      CHECK(10 == at(std::move(r)).stop_);
    }
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Indexers.At.at_for_pair",
                     "[Unit][Simulator][Interface]") {
    {  // constepxr
      /// [at_pair_example]
      auto r = pair(1UL, 10UL);
      static_assert(&r == &at(r), "Invariant violated!");
      /// [at_pair_example]
    }

    {
      // lvalue
      auto r1 = pair(1UL, 10UL);
      CHECK(&r1 == &at(r1));

      auto r2 = pair(1UL, end - 10UL);
      CHECK(&r2 == &at(r2));

      auto r3 = pair(end - 1UL, 10UL);
      CHECK(&r3 == &at(r3));

      auto r4 = pair(end - 1UL, end - 4UL);
      CHECK(&r4 == &at(r4));

      auto r5 = pair(defer, defer);
      CHECK(&r5 == &at(r5));
    }

    {
      // const lvalue
      const auto r1 = pair(1UL, 10UL);
      CHECK(&r1 == &at(r1));

      const auto r2 = pair(1UL, end - 10UL);
      CHECK(&r2 == &at(r2));

      const auto r3 = pair(end - 1UL, 10UL);
      CHECK(&r3 == &at(r3));

      const auto r4 = pair(end - 1UL, end - 4UL);
      CHECK(&r4 == &at(r4));

      const auto r5 = pair(defer, defer);
      CHECK(&r5 == &at(r5));
    }

    {
      // prvalue
      CHECK(1UL == at(pair(1UL, 10UL)).first());
      CHECK(10UL == at(pair(1UL, 10UL)).second());

      CHECK(1UL == at(pair(1UL, end - 10UL)).first());
      CHECK((end - 10UL) == at(pair(1UL, end - 10UL)).second());

      CHECK((end - 1UL) == at(pair(end - 1UL, 10UL)).first());
      CHECK(10UL == at(pair(end - 1UL, 10UL)).second());

      CHECK((end - 1UL) == at(pair(end - 1UL, end - 10UL)).first());
      CHECK((end - 10UL) == at(pair(end - 1UL, end - 10UL)).second());
    }

    {  // constexpr
      static_assert(1UL == at(pair(1UL, 10UL)).first(),
                    "constexpr pair forwarding failure");
      static_assert(10UL == at(pair(1UL, 10UL)).second(),
                    "constexpr pair forwarding failure");
      static_assert((end - 1UL) == at(pair(end - 1UL, end - 10UL)).first(),
                    "constexpr pair forwarding failure");
      static_assert((end - 10UL) == at(pair(end - 1UL, end - 10UL)).second(),
                    "constexpr pair forwarding failure");
    }

    {
      // xrvalue
      {
        const auto r1 = pair(1UL, 10UL);
        CHECK(1UL == at(std::move(r1)).first());
        const auto r2 = pair(1UL, 10UL);
        CHECK(10UL == at(std::move(r2)).second());
      }

      {
        const auto r1 = pair(1UL, end - 10UL);
        CHECK((1UL) == at(std::move(r1)).first());
        const auto r2 = pair(1UL, end - 10UL);
        CHECK((end - 10UL) == at(std::move(r2)).second());
      }

      {
        const auto r1 = pair(end - 1UL, 10UL);
        CHECK((end - 1UL) == at(std::move(r1)).first());
        const auto r2 = pair(end - 1UL, 10UL);
        CHECK((10UL) == at(std::move(r2)).second());
      }

      {
        const auto r1 = pair(end - 1UL, end - 4UL);
        CHECK((end - 1UL) == at(std::move(r1)).first());
        const auto r2 = pair(end - 1UL, end - 4UL);
        CHECK((end - 4UL) == at(std::move(r2)).second());
      }
    }
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Indexers.At.to_numeric",
                     "[Unit][Simulator][Interface]") {
    auto sys = NoThrowSystem{};
    {  // good types
      /// [to_numeric_example]
      auto x = at(1UL, 3UL, 5UL);
      auto numerical_indices = to_numeric(x, sys);
      /// [to_numeric_example]
      CHECK(numerical_indices.args == make_array(2UL, 4UL, 6UL));
    }

    {  // good types
      auto x = at(end - 1, end - 3, end - 5);
      auto numerical_indices = to_numeric(x, sys);
      CHECK(numerical_indices.args ==
            make_array((200UL - 1), 200UL - 3, 200UL - 5));
    }

    {  // good types
      auto x = at(1UL, end - 2, end - 4, 3UL, 5UL, end - 6);
      auto numerical_indices = to_numeric(x, sys);
      // returns an ordered index of (1, 3, 5, end - 2 , end -4 , end - 6);
      CHECK(numerical_indices.args ==
            make_array(2UL, 4UL, 6UL, 200UL - 2, 200UL - 4, 200UL - 6));
    }
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Indexers.At.to_numeric_runtime",
                     "[Unit][Simulator][Interface]") {
    auto sys = NoThrowSystem{};
    using concrete_at_type =
        decltype(at(1UL, end - 2, end - 4, 3UL, 5UL, end - 6));
    /// [to_numeric_runtime_example]
    auto at_ptr = std::unique_ptr<At_base_t>(
        new concrete_at_type(at(1UL, end - 2, end - 4, 3UL, 5UL, end - 6)));
    auto numerical_indices_ptr = to_numeric(std::move(at_ptr), sys);
    // access like a ptr with At_base_t interface
    auto const size = numerical_indices_ptr->size();
    /// [to_numeric_runtime_example]
    auto x = make_array(2, 4, 6, 200 - 2, 200 - 4, 200 - 6);
    CHECK(size == 6);
    for (std::size_t i = 0; i < size; ++i) {
      CHECK(numerical_indices_ptr->data()[i] == x[i]);
    }
  }

  // [[OutputRegex, "as the number of degrees of freedom is only 100"]]
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Indexers.At.to_numeric_throws",
                     "[Unit][Simulator][Interface]") {
    ERROR_TEST();
    auto sys = ThrowSystem{};
    {  //
      auto x = at(1UL, 3UL, 5UL);
      CHECK_THROWS_WITH(to_numeric(x, sys),
                        Catch::Contains("Number of degrees of freedom"));
    }

    {  //
      auto x = at(end - 2, end - 5, end - 10);
      CHECK_THROWS_WITH(to_numeric(x, sys),
                        Catch::Contains("Number of degrees of freedom"));
    }
  }

}  // namespace