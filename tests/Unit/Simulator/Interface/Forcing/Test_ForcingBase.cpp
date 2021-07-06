//
// Created by Tejaswin Parthasarathy on 1/25/21.
//

#include <utility>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Forcing/ForcingBase.hpp"
#include "Utilities/AsConst.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/TMPL.hpp"

namespace {

  using namespace ::elastica::interface::forcing;

  template <typename ForcingRegistrars>
  class MockForcing final : public ForcingBase<ForcingRegistrars> {
   public:
    using Parent = ForcingBase<ForcingRegistrars>;
    using IndexType = typename Parent::IndexType;
    using ForcerID = typename Parent::ForcerID;

    MockForcing(IndexType id, std::string&& name)
        : Parent(id, std::move(name)){};

    ForcerID finalize() const override { return nullptr; };

    using Parent::concrete_forcing_type;
    using Parent::concrete_system_type;

    using Parent::set_concrete_forcing_type;
    using Parent::set_concrete_system_type;
    using Parent::set_name;

    bool is_not_valid() const noexcept override { return true; };
  };

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Forcing.ForcingBase.constructor",
                     "[Unit][Simulator][Interface][Forcing]") {
    {
      using RegistrarList = tmpl::list<>;
      using ConcreteForcing = MockForcing<RegistrarList>;
      ConcreteForcing cb(4, "test");
    }

    {
      using RegistrarList = tmpl::list<int, float, char>;
      using ConcreteForcing = MockForcing<RegistrarList>;
      elastica::ignore_unused(ConcreteForcing(4, "test"));
    }
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Forcing.ForcingBase.id",
                     "[Unit][Simulator][Interface][Forcing]") {
    using RegistrarList = tmpl::list<>;
    {
      using ConcreteForcing = MockForcing<RegistrarList>;
      ConcreteForcing cb(4, "test");
      CHECK(cb.id().uuid == 4);
      CHECK(cb.id().name == "test");
    }

    {
      using ConcreteForcing = MockForcing<RegistrarList>;
      CHECK(ConcreteForcing(5, "test").id().uuid == 5);
      CHECK(ConcreteForcing(4, "test").id().name == "test");
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Forcing.ForcingBase.set_name",
                     "[Unit][Simulator][Interface][Forcing]") {
    // clang-format on
    using RegistrarList = tmpl::list<>;
    {
      using ConcreteForcing = MockForcing<RegistrarList>;
      ConcreteForcing cb(4, "test");
      CHECK(cb.id().uuid == 4);
      CHECK(cb.id().name == "test");
      cb.set_name() = "jun23";
      CHECK(cb.id().name == "jun23");
    }
  }

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Forcing.ForcingBase.concrete_name",
      "[Unit][Simulator][Interface][Forcing]") {
    using RegistrarList = tmpl::list<int, float>;
    using ConcreteForcing = MockForcing<RegistrarList>;
    ConcreteForcing cb(4, "test");
    {
      CHECK(cb.concrete_forcing_type().empty());
      CHECK(cb.concrete_system_type().empty());
    }
    {
      CHECK(cpp17::as_const(cb).concrete_forcing_type().empty());
      CHECK(cpp17::as_const(cb).concrete_system_type().empty());
    }
    {
      cb.set_concrete_forcing_type() = "Boo";
      cb.set_concrete_system_type() = "As";
      CHECK_FALSE(cpp17::as_const(cb).concrete_forcing_type().empty());
      CHECK_FALSE(cpp17::as_const(cb).concrete_system_type().empty());
    }
  }

}  // namespace
