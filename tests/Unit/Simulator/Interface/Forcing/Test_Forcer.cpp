//
// Created by Tejaswin Parthasarathy on 6/24/21.
//
#include <utility>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Forcing/Forcer.hpp"
#include "Time/SimulationTime.hpp"
#include "Utilities/MakeArray.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/NumericAt.hpp"

namespace {

  using namespace ::elastica::interface::forcing;
  using TimeType = elastica::Time;

  struct MockForcing {
    template <typename... Args>
    void apply_forces_internal(Args const&...) const noexcept {}
    template <typename... Args>
    void apply_torques_internal(Args const&...) const noexcept {}
  };

  struct MockForcingWithSideEffects : public elastica::NonCopyable {
    MockForcingWithSideEffects(std::size_t& force_counter,
                               std::size_t& torque_counter)
        : f_counter(force_counter), t_counter(torque_counter) {}

    template <typename... Args>
    void apply_forces_internal(Args const&...) const noexcept {
      ++f_counter;
    }
    template <typename... Args>
    void apply_torques_internal(Args const&...) const noexcept {
      ++t_counter;
    }
    std::size_t &f_counter, &t_counter;
  };

  struct MockForcingWithoutSideEffects : public elastica::NonCopyable {
    MockForcingWithoutSideEffects() = default;

    template <typename FirstSystem, typename IndexSet>
    void apply_forces_internal(TimeType, FirstSystem& first,
                               IndexSet const&) const noexcept {
      first += 3;
    }
    template <typename FirstSystem, typename IndexSet>
    void apply_torques_internal(TimeType, FirstSystem& first,
                                IndexSet const&) const noexcept {
      first += 4;
    }
  };

  template <typename System, typename ConcreteForcing, typename IndexSet>
  class TestForcer final : public Forcer<System, ConcreteForcing, IndexSet> {
   public:
    using Parent = Forcer<System, ConcreteForcing, IndexSet>;
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
      "Unit.Simulator.Interface.Forcing.Forcer.constructor",
      "[Unit][Simulator][Interface][Forcing]") {
    // clang-format on

    using System = float;
    System system = 5.0F;

    {
      static constexpr std::uint8_t N = 2;
      using IndexSet = ::elastica::detail::numeric_at_t<N>;
      using Forcer = TestForcer<System, MockForcing, IndexSet>;
      Forcer forcer(ForcingUUID(4, "test"), system, MockForcing(),
                    ::elastica::numeric_at(5UL, 6UL));

      // base
      CHECK(forcer.id().uuid == 4);
      CHECK(forcer.id().name == "test");
      //
      CHECK(forcer.indices().args == make_array(5UL, 6UL));
    }
    {
      static constexpr std::uint8_t N = 3;
      using IndexSet = ::elastica::detail::numeric_at_t<N>;
      using Forcer = TestForcer<System, MockForcingWithSideEffects, IndexSet>;
      std::size_t force_counter = 0UL;
      std::size_t torque_counter = 0UL;
      Forcer forcer(ForcingUUID(4, "test"), system,
                    MockForcingWithSideEffects(force_counter, torque_counter),
                    ::elastica::numeric_at(5UL, 6UL, 7UL));

      // base
      CHECK(forcer.id().uuid == 4);
      CHECK(forcer.id().name == "test");
      //
      CHECK(forcer.indices().args == make_array(5UL, 6UL, 7UL));
    }
    {
      static constexpr std::uint8_t N = 1;
      using IndexSet = ::elastica::detail::numeric_at_t<N>;
      using Forcer =
          TestForcer<System, MockForcingWithoutSideEffects, IndexSet>;
      Forcer forcer(ForcingUUID(4, "test"), system,
                    MockForcingWithoutSideEffects(),
                    ::elastica::numeric_at(5UL));

      // base
      CHECK(forcer.id().uuid == 4);
      CHECK(forcer.id().name == "test");
      //
      CHECK(forcer.indices().args == make_array(5UL));
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Forcing.Forcer.interface",
      "[Unit][Simulator][Interface][Forcing]") {
    // clang-format on

    using T = int;
    using System = std::reference_wrapper<T>;
    T system_value = 5;
    System system{system_value};

    {
      static constexpr std::uint8_t N = 2;
      using IndexSet = ::elastica::detail::numeric_at_t<N>;
      using Forcer = TestForcer<System, MockForcing, IndexSet>;
      Forcer forcer(ForcingUUID(4, "test"), system, MockForcing(),
                    ::elastica::numeric_at(5UL, 6UL));

      //
      typename Forcer::TimeType time_v{};
      forcer.apply_forces(time_v);
      forcer.apply_torques(time_v);
    }
    {
      static constexpr std::uint8_t N = 3;
      using IndexSet = ::elastica::detail::numeric_at_t<N>;
      using Forcer = TestForcer<System, MockForcingWithSideEffects, IndexSet>;
      std::size_t force_counter = 0UL;
      std::size_t torque_counter = 0UL;
      Forcer forcer(ForcingUUID(4, "test"), system,
                    MockForcingWithSideEffects(force_counter, torque_counter),
                    ::elastica::numeric_at(5UL, 6UL, 7UL));

      typename Forcer::TimeType time_v{};
      CHECK((forcer.apply_forces(time_v), force_counter) == 1UL);
      CHECK((forcer.apply_torques(time_v), torque_counter) == 1UL);
      CHECK((forcer.apply_forces(time_v), force_counter) == 2UL);
      CHECK((forcer.apply_torques(time_v), torque_counter) == 2UL);
    }
    {
      static constexpr std::uint8_t N = 1;
      using IndexSet = ::elastica::detail::numeric_at_t<N>;
      using Forcer =
          TestForcer<System, MockForcingWithoutSideEffects, IndexSet>;
      Forcer forcer(ForcingUUID(4, "test"), system,
                    MockForcingWithoutSideEffects(),
                    ::elastica::numeric_at(5UL));

      typename Forcer::TimeType time_v{};
      forcer.apply_forces(time_v);
      CHECK(system == 8);

      forcer.apply_torques(time_v);
      CHECK(system == 12);
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Forcing.Forcer.make_forcer",
      "[Unit][Simulator][Interface][Forcing]") {
    // clang-format on

    using T = int;
    using System = std::reference_wrapper<T>;
    T system_value = 5;
    System system{system_value};

    {
      auto conn = make_forcer(ForcingUUID(4, "test"), system, MockForcing(),
                              ::elastica::numeric_at(5UL, 6UL));

      TimeType time_v{};
      conn->apply_forces(time_v);
      conn->apply_torques(time_v);
    }

    {
      std::size_t force_counter = 0UL;
      std::size_t torque_counter = 0UL;
      auto conn =
          make_forcer(ForcingUUID(4, "test"), system,
                      MockForcingWithSideEffects(force_counter, torque_counter),
                      ::elastica::numeric_at(5UL, 6UL, 7UL));

      TimeType time_v{};
      CHECK((conn->apply_forces(time_v), force_counter) == 1UL);
      CHECK((conn->apply_torques(time_v), torque_counter) == 1UL);
      CHECK((conn->apply_forces(time_v), force_counter) == 2UL);
      CHECK((conn->apply_torques(time_v), torque_counter) == 2UL);
    }

    {
      auto conn = make_forcer(ForcingUUID(4, "test"), system,
                              MockForcingWithoutSideEffects(),
                              ::elastica::numeric_at(5UL));

      TimeType time_v{};
      CHECK((conn->apply_forces(time_v), system) == 8);
      CHECK((conn->apply_torques(time_v), system) == 12);
    }
  }

}  // namespace
