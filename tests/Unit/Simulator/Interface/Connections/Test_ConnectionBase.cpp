//
// Created by Tejaswin Parthasarathy on 1/28/21.
//
#include <utility>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Connections/ConnectionBase.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/TMPL.hpp"

namespace {

  using namespace ::elastica::interface::connections;

  template <typename ConnectionRegistrars>
  class MockConnection final : public ConnectionBase<ConnectionRegistrars> {
   public:
    using Parent = ConnectionBase<ConnectionRegistrars>;
    using IndexType = typename Parent::IndexType;
    using ConnecterID = typename Parent::ConnecterID;

    MockConnection(IndexType id, std::string&& name)
        : Parent(id, std::move(name)){};

    ConnecterID finalize() const override { return nullptr; };

    using Parent::concrete_connection_type;
    using Parent::first_concrete_system_type;
    using Parent::second_concrete_system_type;

    using Parent::set_concrete_connection_type;
    using Parent::set_first_concrete_system_type;
    using Parent::set_name;
    using Parent::set_second_concrete_system_type;

    bool is_not_valid() const noexcept override { return true; };
  };

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Connections.ConnectionBase.constructor",
      "[Unit][Simulator][Interface][Connections]") {
    EMPTY_TEST();

    {
      using RegistrarList = tmpl::list<>;
      using ConcreteConnection = MockConnection<RegistrarList>;
      ConcreteConnection cb(4, "test");
    }

    {
      using RegistrarList = tmpl::list<int, float, char>;
      using ConcreteConnection = MockConnection<RegistrarList>;
      elastica::ignore_unused(ConcreteConnection(4, "test"));
    }
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Connections.ConnectionBase.id",
                     "[Unit][Simulator][Interface][Connections]") {
    using RegistrarList = tmpl::list<>;
    {
      using ConcreteConnection = MockConnection<RegistrarList>;
      ConcreteConnection cb(4, "test");
      CHECK(cb.id().uuid == 4);
      CHECK(cb.id().name == "test");
    }

    {
      using ConcreteConnection = MockConnection<RegistrarList>;
      CHECK(ConcreteConnection(5, "test").id().uuid == 5);
      CHECK(ConcreteConnection(4, "test").id().name == "test");
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Connections.ConnectionBase.set_name",
                     "[Unit][Simulator][Interface][Connections]") {
    // clang-format on
    using RegistrarList = tmpl::list<>;
    {
      using ConcreteConnection = MockConnection<RegistrarList>;
      ConcreteConnection cb(4, "test");
      CHECK(cb.id().uuid == 4);
      CHECK(cb.id().name == "test");
      cb.set_name() = "jun23";
      CHECK(cb.id().name == "jun23");
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Connections.ConnectionBase.concrete_names",
      "[Unit][Simulator][Interface][Connections]") {
    // clang-format on
    using RegistrarList = tmpl::list<int, float>;
    using ConcreteConnection = MockConnection<RegistrarList>;
    ConcreteConnection cb(4, "test");
    {
      CHECK(cb.concrete_connection_type().empty());
      CHECK(cb.first_concrete_system_type().empty());
      CHECK(cb.second_concrete_system_type().empty());
    }
    {
      CHECK(std::as_const(cb).concrete_connection_type().empty());
      CHECK(std::as_const(cb).first_concrete_system_type().empty());
      CHECK(std::as_const(cb).second_concrete_system_type().empty());
    }
    {
      cb.set_concrete_connection_type() = "Boo";
      cb.set_first_concrete_system_type() = "As";
      cb.set_second_concrete_system_type() = "If";
      CHECK_FALSE(std::as_const(cb).concrete_connection_type().empty());
      CHECK_FALSE(std::as_const(cb).first_concrete_system_type().empty());
      CHECK_FALSE(std::as_const(cb).second_concrete_system_type().empty());
    }
  }

}  // namespace
