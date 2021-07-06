//
// Created by Tejaswin Parthasarathy on 6/19/21.
//
#include <utility>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Connections/Connecter.hpp"
#include "Time/SimulationTime.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/NonCopyable.hpp"

namespace {

  using namespace ::elastica::interface::connections;
  using TimeType = elastica::Time;

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Connections.Connecter.EagerPolicy",
      "[Unit][Simulator][Interface][Connections]") {
    // clang-format on
    EagerPolicy eager(std::make_pair(5UL, 6UL));
    CHECK(eager.first_index() == 5UL);
    CHECK(eager.second_index() == 6UL);
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Connections.Connecter.DeferredToUserPolicy",
      "[Unit][Simulator][Interface][Connections]") {
    // clang-format on
    DeferredToUserPolicy deferred{
        std::make_pair(elastica::defer, elastica::defer)};
    elastica::ignore_unused(deferred);
  }

  struct MockConnection {
    void apply_forces_internal(...) const noexcept {}
    void apply_torques_internal(...) const noexcept {}
  };

  struct MockConnectionWithSideEffects : public elastica::NonCopyable {
    MockConnectionWithSideEffects(std::size_t& force_counter,
                                  std::size_t& torque_counter)
        : f_counter(force_counter), t_counter(torque_counter) {}

    void apply_forces_internal(...) const noexcept { ++f_counter; }
    void apply_torques_internal(...) const noexcept { ++t_counter; }
    std::size_t &f_counter, &t_counter;
  };

  struct MockConnectionWithoutSideEffects : public elastica::NonCopyable {
    MockConnectionWithoutSideEffects() = default;

    template <typename FirstSystem, typename SecondSystem>
    void apply_forces_internal(TimeType, FirstSystem& first, std::size_t,
                               SecondSystem& second,
                               std::size_t) const noexcept {
      first += 1;
      second += 1.0;
    }
    template <typename FirstSystem, typename SecondSystem>
    void apply_torques_internal(TimeType, FirstSystem& first, std::size_t,
                                SecondSystem& second,
                                std::size_t) const noexcept {
      first += 2;
      second += 2.0;
    }

    // defer types

    template <typename FirstSystem, typename SecondSystem>
    void apply_forces_internal(TimeType, FirstSystem& first,
                               SecondSystem& second) const noexcept {
      first += 3;
      second += 3.0;
    }
    template <typename FirstSystem, typename SecondSystem>
    void apply_torques_internal(TimeType, FirstSystem& first,
                                SecondSystem& second) const noexcept {
      first += 4;
      second += 4.0;
    }
  };

  template <typename ConnectionPolicy, typename FirstSystem,
            typename SecondSystem, typename ConcreteConnection>
  class TestConnecter final
      : public Connecter<ConnectionPolicy, FirstSystem, SecondSystem,
                         ConcreteConnection> {
   public:
    using Parent = Connecter<ConnectionPolicy, FirstSystem, SecondSystem,
                             ConcreteConnection>;
    using Parent::Parent;
    using typename Parent::TimeType;

   public:
    using Parent::first_index;
    using Parent::id;
    using Parent::second_index;
  };

  template <typename FirstSystem, typename SecondSystem,
            typename ConcreteConnection>
  class TestConnecter<DeferredToUserPolicy, FirstSystem, SecondSystem,
                      ConcreteConnection>
      final : public Connecter<DeferredToUserPolicy, FirstSystem, SecondSystem,
                               ConcreteConnection> {
   public:
    using Parent = Connecter<DeferredToUserPolicy, FirstSystem, SecondSystem,
                             ConcreteConnection>;
    using Parent::Parent;
    using typename Parent::TimeType;

   public:
    using Parent::id;
  };

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Connections.EagerPolicyConnecter.constructor",
      "[Unit][Simulator][Interface][Connections]") {
    // clang-format on
    using FirstSystem = int;
    using SecondSystem = float;
    FirstSystem first = 5;
    SecondSystem second = 5.0F;
    using Policy = EagerPolicy;
    {
      using Connecter =
          TestConnecter<Policy, FirstSystem, SecondSystem, MockConnection>;
      Connecter connecter(ConnectionUUID(4, "test"), first, second,
                          MockConnection(), std::make_pair(5UL, 6UL));

      // base
      CHECK(connecter.id().uuid == 4);
      CHECK(connecter.id().name == "test");
      //
      // policy class
      CHECK(connecter.first_index() == 5UL);
      CHECK(connecter.second_index() == 6UL);
    }
    {
      using Connecter = TestConnecter<Policy, FirstSystem, SecondSystem,
                                      MockConnectionWithSideEffects>;
      std::size_t force_counter = 0UL;
      std::size_t torque_counter = 0UL;
      Connecter connecter(
          ConnectionUUID(4, "test"), first, second,
          MockConnectionWithSideEffects(force_counter, torque_counter),
          std::make_pair(5UL, 6UL));

      // base
      CHECK(connecter.id().uuid == 4);
      CHECK(connecter.id().name == "test");
      //
      // policy class
      CHECK(connecter.first_index() == 5UL);
      CHECK(connecter.second_index() == 6UL);
    }
    {
      using Connecter = TestConnecter<Policy, FirstSystem, SecondSystem,
                                      MockConnectionWithoutSideEffects>;
      Connecter connecter(ConnectionUUID(4, "test"), first, second,
                          MockConnectionWithoutSideEffects(),
                          std::make_pair(5UL, 6UL));

      // base
      CHECK(connecter.id().uuid == 4);
      CHECK(connecter.id().name == "test");
      //
      // policy class
      CHECK(connecter.first_index() == 5UL);
      CHECK(connecter.second_index() == 6UL);
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Connections.EagerPolicyConnecter.interface",
      "[Unit][Simulator][Interface][Connections]") {
    // clang-format on
    using FirstSystem = int;
    using SecondSystem = float;
    FirstSystem first = 5;
    SecondSystem second = 5.0F;
    using Policy = EagerPolicy;
    {
      using Connecter =
          TestConnecter<Policy, FirstSystem, SecondSystem, MockConnection>;
      Connecter connecter(ConnectionUUID(4, "test"), first, second,
                          MockConnection(), std::make_pair(5UL, 6UL));

      //
      typename Connecter::TimeType time_v{};
      connecter.apply_forces(time_v);
      connecter.apply_torques(time_v);
    }
    {
      using Connecter = TestConnecter<Policy, FirstSystem, SecondSystem,
                                      MockConnectionWithSideEffects>;
      std::size_t force_counter = 0UL;
      std::size_t torque_counter = 0UL;
      Connecter connecter(
          ConnectionUUID(4, "test"), first, second,
          MockConnectionWithSideEffects(force_counter, torque_counter),
          std::make_pair(5UL, 6UL));

      typename Connecter::TimeType time_v{};
      CHECK((connecter.apply_forces(time_v), force_counter) == 1UL);
      CHECK((connecter.apply_torques(time_v), torque_counter) == 1UL);
      CHECK((connecter.apply_forces(time_v), force_counter) == 2UL);
      CHECK((connecter.apply_torques(time_v), torque_counter) == 2UL);
    }
    {
      using Connecter = TestConnecter<Policy, FirstSystem, SecondSystem,
                                      MockConnectionWithoutSideEffects>;
      Connecter connecter(ConnectionUUID(4, "test"), first, second,
                          MockConnectionWithoutSideEffects(),
                          std::make_pair(5UL, 6UL));
      typename Connecter::TimeType time_v{};
      connecter.apply_forces(time_v);
      CHECK(first == 6);
      CHECK(second == approx(6.0));

      connecter.apply_torques(time_v);
      CHECK(first == 8);
      CHECK(second == approx(8.0));
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Connections.DeferPolicyConnecter.constructor",
      "[Unit][Simulator][Interface][Connections]") {
    // clang-format on
    using FirstSystem = int;
    using SecondSystem = float;
    FirstSystem first = 5;
    SecondSystem second = 5.0F;
    using Policy = DeferredToUserPolicy;
    {
      using Connecter =
          TestConnecter<Policy, FirstSystem, SecondSystem, MockConnection>;
      Connecter connecter(ConnectionUUID(4, "test"), first, second,
                          MockConnection(),
                          std::make_pair(elastica::defer, elastica::defer));

      // base
      CHECK(connecter.id().uuid == 4);
      CHECK(connecter.id().name == "test");
      //
    }
    {
      using Connecter = TestConnecter<Policy, FirstSystem, SecondSystem,
                                      MockConnectionWithSideEffects>;
      std::size_t force_counter = 0UL;
      std::size_t torque_counter = 0UL;
      Connecter connecter(
          ConnectionUUID(4, "test"), first, second,
          MockConnectionWithSideEffects(force_counter, torque_counter),
          std::make_pair(elastica::defer, elastica::defer));

      // base
      CHECK(connecter.id().uuid == 4);
      CHECK(connecter.id().name == "test");
      //
    }
    {
      using Connecter = TestConnecter<Policy, FirstSystem, SecondSystem,
                                      MockConnectionWithoutSideEffects>;
      Connecter connecter(ConnectionUUID(4, "test"), first, second,
                          MockConnectionWithoutSideEffects(),
                          std::make_pair(elastica::defer, elastica::defer));

      // base
      CHECK(connecter.id().uuid == 4);
      CHECK(connecter.id().name == "test");
      //
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Connections.DeferPolicyConnecter.interface",
      "[Unit][Simulator][Interface][Connections]") {
    // clang-format on
    using FirstSystem = int;
    using SecondSystem = float;
    FirstSystem first = 5;
    SecondSystem second = 5.0F;
    using Policy = DeferredToUserPolicy;
    {
      using Connecter =
          TestConnecter<Policy, FirstSystem, SecondSystem, MockConnection>;
      Connecter connecter(ConnectionUUID(4, "test"), first, second,
                          MockConnection(),
                          std::make_pair(elastica::defer, elastica::defer));
      //
      typename Connecter::TimeType time_v{};
      connecter.apply_forces(time_v);
      connecter.apply_torques(time_v);
    }
    {
      using Connecter = TestConnecter<Policy, FirstSystem, SecondSystem,
                                      MockConnectionWithSideEffects>;
      std::size_t force_counter = 0UL;
      std::size_t torque_counter = 0UL;
      Connecter connecter(
          ConnectionUUID(4, "test"), first, second,
          MockConnectionWithSideEffects(force_counter, torque_counter),
          std::make_pair(elastica::defer, elastica::defer));

      // base
      CHECK(connecter.id().uuid == 4);
      CHECK(connecter.id().name == "test");
      //
      typename Connecter::TimeType time_v{};
      CHECK((connecter.apply_forces(time_v), force_counter) == 1UL);
      CHECK((connecter.apply_torques(time_v), torque_counter) == 1UL);
      CHECK((connecter.apply_forces(time_v), force_counter) == 2UL);
      CHECK((connecter.apply_torques(time_v), torque_counter) == 2UL);
    }
    {
      using Connecter = TestConnecter<Policy, FirstSystem, SecondSystem,
                                      MockConnectionWithoutSideEffects>;
      Connecter connecter(ConnectionUUID(4, "test"), first, second,
                          MockConnectionWithoutSideEffects(),
                          std::make_pair(elastica::defer, elastica::defer));

      //
      typename Connecter::TimeType time_v{};
      connecter.apply_forces(time_v);
      CHECK(first == 8);
      CHECK(second == approx(8.0));

      connecter.apply_torques(time_v);
      CHECK(first == 12);
      CHECK(second == approx(12.0));
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Connections.Connecter.make_connecter",
      "[Unit][Simulator][Interface][Connections]") {
    // clang-format on
    using FirstSystem = int;
    using SecondSystem = float;
    FirstSystem first = 5;
    SecondSystem second = 5.0F;

    {
      auto conn = make_connecter(ConnectionUUID(4, "test"), first, second,
                                 MockConnection(), std::make_pair(5UL, 6UL));
      TimeType time_v{};
      conn->apply_forces(time_v);
      conn->apply_torques(time_v);
    }

    {
      std::size_t force_counter = 0UL;
      std::size_t torque_counter = 0UL;
      auto conn = make_connecter(
          ConnectionUUID(4, "test"), first, second,
          MockConnectionWithSideEffects(force_counter, torque_counter),
          std::make_pair(5UL, 6UL));
      TimeType time_v{};
      CHECK((conn->apply_forces(time_v), force_counter) == 1UL);
      CHECK((conn->apply_torques(time_v), torque_counter) == 1UL);
      CHECK((conn->apply_forces(time_v), force_counter) == 2UL);
      CHECK((conn->apply_torques(time_v), torque_counter) == 2UL);
    }

    {
      auto conn = make_connecter(ConnectionUUID(4, "test"), first, second,
                                 MockConnectionWithoutSideEffects(),
                                 std::make_pair(5UL, 6UL));

      TimeType time_v{};
      conn->apply_forces(time_v);
      CHECK(first == 6);
      CHECK(second == approx(6.0));

      conn->apply_torques(time_v);
      CHECK(first == 8);
      CHECK(second == approx(8.0));
    }

    {
      auto conn = make_connecter(
          ConnectionUUID(4, "test"), first, second, MockConnection(),
          std::make_pair(elastica::defer, elastica::defer));
      TimeType time_v{};
      conn->apply_forces(time_v);
      conn->apply_torques(time_v);
    }

    {
      std::size_t force_counter = 0UL;
      std::size_t torque_counter = 0UL;
      auto conn = make_connecter(
          ConnectionUUID(4, "test"), first, second,
          MockConnectionWithSideEffects(force_counter, torque_counter),
          std::make_pair(elastica::defer, elastica::defer));
      TimeType time_v{};
      CHECK((conn->apply_forces(time_v), force_counter) == 1UL);
      CHECK((conn->apply_torques(time_v), torque_counter) == 1UL);
      CHECK((conn->apply_forces(time_v), force_counter) == 2UL);
      CHECK((conn->apply_torques(time_v), torque_counter) == 2UL);
    }

    {
      auto conn =
          make_connecter(ConnectionUUID(4, "test"), first, second,
                         MockConnectionWithoutSideEffects(),
                         std::make_pair(elastica::defer, elastica::defer));

      TimeType time_v{};
      conn->apply_forces(time_v);
      CHECK(first == 11);
      CHECK(second == approx(11.0));

      conn->apply_torques(time_v);
      CHECK(first == 15);
      CHECK(second == approx(15.0));
    }
  }

}  // namespace