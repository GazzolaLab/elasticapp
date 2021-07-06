//
// Created by Tejaswin Parthasarathy on 6/23/21.
//
#include <utility>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Constraints/Constrainer.hpp"
#include "Time/SimulationTime.hpp"
#include "Utilities/MakeArray.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/NumericAt.hpp"

namespace {

  using namespace ::elastica::interface::constraints;
  using TimeType = elastica::Time;

  struct MockConstraint {
    template <typename... Args>
    void constrain_primitives_internal(Args const&...) const noexcept {}
    template <typename... Args>
    void constrain_rates_internal(Args const&...) const noexcept {}
  };

  struct MockConstraintWithSideEffects : public elastica::NonCopyable {
    MockConstraintWithSideEffects(std::size_t& force_counter,
                                  std::size_t& torque_counter)
        : f_counter(force_counter), t_counter(torque_counter) {}

    template <typename... Args>
    void constrain_primitives_internal(Args const&...) const noexcept {
      ++f_counter;
    }
    template <typename... Args>
    void constrain_rates_internal(Args const&...) const noexcept {
      ++t_counter;
    }
    std::size_t &f_counter, &t_counter;
  };

  struct MockConstraintWithoutSideEffects : public elastica::NonCopyable {
    MockConstraintWithoutSideEffects() = default;

    template <typename FirstSystem, typename IndexSet>
    void constrain_primitives_internal(TimeType, FirstSystem& first,
                                       IndexSet const&) const noexcept {
      first += 3;
    }
    template <typename FirstSystem, typename IndexSet>
    void constrain_rates_internal(TimeType, FirstSystem& first,
                                  IndexSet const&) const noexcept {
      first += 4;
    }
  };

  template <typename System, typename ConcreteConstraint, typename IndexSet>
  class TestConstrainer final
      : public Constrainer<System, ConcreteConstraint, IndexSet> {
   public:
    using Parent = Constrainer<System, ConcreteConstraint, IndexSet>;
    using Parent::Parent;
    using typename Parent::TimeType;

   public:
    // using Parent::first_index;
    using Parent::id;
    using Parent::indices;
    // using Parent::second_index;
  };

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Constraints.Constrainer.constructor",
      "[Unit][Simulator][Interface][Constraints]") {
    // clang-format on

    using System = float;
    System system = 5.0F;

    {
      static constexpr std::uint8_t N = 2;
      using IndexSet = ::elastica::detail::numeric_at_t<N>;
      using Constrainer = TestConstrainer<System, MockConstraint, IndexSet>;
      Constrainer constrainer(ConstraintUUID(4, "test"), system,
                              MockConstraint(),
                              ::elastica::numeric_at(5UL, 6UL));

      // base
      CHECK(constrainer.id().uuid == 4);
      CHECK(constrainer.id().name == "test");
      //
      CHECK(constrainer.indices().args == make_array(5UL, 6UL));
    }
    {
      static constexpr std::uint8_t N = 3;
      using IndexSet = ::elastica::detail::numeric_at_t<N>;
      using Constrainer =
          TestConstrainer<System, MockConstraintWithSideEffects, IndexSet>;
      std::size_t force_counter = 0UL;
      std::size_t torque_counter = 0UL;
      Constrainer constrainer(
          ConstraintUUID(4, "test"), system,
          MockConstraintWithSideEffects(force_counter, torque_counter),
          ::elastica::numeric_at(5UL, 6UL, 7UL));

      // base
      CHECK(constrainer.id().uuid == 4);
      CHECK(constrainer.id().name == "test");
      //
      CHECK(constrainer.indices().args == make_array(5UL, 6UL, 7UL));
    }
    {
      static constexpr std::uint8_t N = 1;
      using IndexSet = ::elastica::detail::numeric_at_t<N>;
      using Constrainer =
          TestConstrainer<System, MockConstraintWithoutSideEffects, IndexSet>;
      Constrainer constrainer(ConstraintUUID(4, "test"), system,
                              MockConstraintWithoutSideEffects(),
                              ::elastica::numeric_at(5UL));

      // base
      CHECK(constrainer.id().uuid == 4);
      CHECK(constrainer.id().name == "test");
      //
      CHECK(constrainer.indices().args == make_array(5UL));
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Constraints.Constrainer.interface",
      "[Unit][Simulator][Interface][Constraints]") {
    // clang-format on

    using System = int;
    System system = 5;

    {
      static constexpr std::uint8_t N = 2;
      using IndexSet = ::elastica::detail::numeric_at_t<N>;
      using Constrainer = TestConstrainer<System, MockConstraint, IndexSet>;
      Constrainer constrainer(ConstraintUUID(4, "test"), system,
                              MockConstraint(),
                              ::elastica::numeric_at(5UL, 6UL));

      //
      typename Constrainer::TimeType time_v{};
      constrainer.constrain_primitives(time_v);
      constrainer.constrain_rates(time_v);
    }
    {
      static constexpr std::uint8_t N = 3;
      using IndexSet = ::elastica::detail::numeric_at_t<N>;
      using Constrainer =
          TestConstrainer<System, MockConstraintWithSideEffects, IndexSet>;
      std::size_t force_counter = 0UL;
      std::size_t torque_counter = 0UL;
      Constrainer constrainer(
          ConstraintUUID(4, "test"), system,
          MockConstraintWithSideEffects(force_counter, torque_counter),
          ::elastica::numeric_at(5UL, 6UL, 7UL));

      typename Constrainer::TimeType time_v{};
      CHECK((constrainer.constrain_primitives(time_v), force_counter) == 1UL);
      CHECK((constrainer.constrain_rates(time_v), torque_counter) == 1UL);
      CHECK((constrainer.constrain_primitives(time_v), force_counter) == 2UL);
      CHECK((constrainer.constrain_rates(time_v), torque_counter) == 2UL);
    }
    {
      static constexpr std::uint8_t N = 1;
      using IndexSet = ::elastica::detail::numeric_at_t<N>;
      using Constrainer =
          TestConstrainer<System, MockConstraintWithoutSideEffects, IndexSet>;
      Constrainer constrainer(ConstraintUUID(4, "test"), system,
                              MockConstraintWithoutSideEffects(),
                              ::elastica::numeric_at(5UL));

      typename Constrainer::TimeType time_v{};
      constrainer.constrain_primitives(time_v);
      CHECK(system == 8);

      constrainer.constrain_rates(time_v);
      CHECK(system == 12);
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Constraints.Constrainer.make_constrainer",
      "[Unit][Simulator][Interface][Constraints]") {
    // clang-format on

    using System = int;
    System system = 5;

    {
      auto conn =
          make_constrainer(ConstraintUUID(4, "test"), system, MockConstraint(),
                           ::elastica::numeric_at(5UL, 6UL));

      TimeType time_v{};
      conn->constrain_primitives(time_v);
      conn->constrain_rates(time_v);
    }

    {
      std::size_t force_counter = 0UL;
      std::size_t torque_counter = 0UL;
      auto conn = make_constrainer(
          ConstraintUUID(4, "test"), system,
          MockConstraintWithSideEffects(force_counter, torque_counter),
          ::elastica::numeric_at(5UL, 6UL, 7UL));

      TimeType time_v{};
      CHECK((conn->constrain_primitives(time_v), force_counter) == 1UL);
      CHECK((conn->constrain_rates(time_v), torque_counter) == 1UL);
      CHECK((conn->constrain_primitives(time_v), force_counter) == 2UL);
      CHECK((conn->constrain_rates(time_v), torque_counter) == 2UL);
    }

    {
      auto conn = make_constrainer(ConstraintUUID(4, "test"), system,
                                   MockConstraintWithoutSideEffects(),
                                   ::elastica::numeric_at(5UL));

      TimeType time_v{};
      CHECK((conn->constrain_primitives(time_v), system) == 8);
      CHECK((conn->constrain_rates(time_v), system) == 12);
    }
  }

}  // namespace