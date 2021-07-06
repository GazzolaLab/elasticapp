#include <utility>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Constraints/ConstraintBase.hpp"
#include "Utilities/AsConst.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/TMPL.hpp"

namespace {

  using namespace ::elastica::interface::constraints;

  template <typename ConstraintRegistrars>
  class MockConstraint final : public ConstraintBase<ConstraintRegistrars> {
   public:
    using Parent = ConstraintBase<ConstraintRegistrars>;
    using IndexType = typename Parent::IndexType;
    using ConstrainerID = typename Parent::ConstrainerID;

    MockConstraint(IndexType id, std::string&& name)
        : Parent(id, std::move(name)){};

    ConstrainerID finalize() const override { return nullptr; };

    using Parent::concrete_constraint_type;
    using Parent::concrete_system_type;

    using Parent::set_concrete_constraint_type;
    using Parent::set_concrete_system_type;
    using Parent::set_name;

    bool is_not_valid() const noexcept override { return true; };
  };

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Constraints.ConstraintBase.constructor",
      "[Unit][Simulator][Interface][Constraints]") {
    {
      using RegistrarList = tmpl::list<>;
      using ConcreteConstraint = MockConstraint<RegistrarList>;
      ConcreteConstraint cb(4, "test");
    }

    {
      using RegistrarList = tmpl::list<int, float, char>;
      using ConcreteConstraint = MockConstraint<RegistrarList>;
      elastica::ignore_unused(ConcreteConstraint(4, "test"));
    }
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Constraints.ConstraintBase.id",
                     "[Unit][Simulator][Interface][Constraints]") {
    using RegistrarList = tmpl::list<>;
    {
      using ConcreteConstraint = MockConstraint<RegistrarList>;
      ConcreteConstraint cb(4, "test");
      CHECK(cb.id().uuid == 4);
      CHECK(cb.id().name == "test");
    }

    {
      using ConcreteConstraint = MockConstraint<RegistrarList>;
      CHECK(ConcreteConstraint(5, "test").id().uuid == 5);
      CHECK(ConcreteConstraint(4, "test").id().name == "test");
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Constraints.ConstraintBase.set_name",
                     "[Unit][Simulator][Interface][Constraints]") {
    // clang-format on
    using RegistrarList = tmpl::list<>;
    {
      using ConcreteConstraint = MockConstraint<RegistrarList>;
      ConcreteConstraint cb(4, "test");
      CHECK(cb.id().uuid == 4);
      CHECK(cb.id().name == "test");
      cb.set_name() = "jun23";
      CHECK(cb.id().name == "jun23");
    }
  }

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Constraints.ConstraintBase.concrete_name",
      "[Unit][Simulator][Interface][Constraints]") {
    using RegistrarList = tmpl::list<int, float>;
    using ConcreteConstraint = MockConstraint<RegistrarList>;
    ConcreteConstraint cb(4, "test");
    {
      CHECK(cb.concrete_constraint_type().empty());
      CHECK(cb.concrete_system_type().empty());
    }
    {
      CHECK(cpp17::as_const(cb).concrete_constraint_type().empty());
      CHECK(cpp17::as_const(cb).concrete_system_type().empty());
    }
    {
      cb.set_concrete_constraint_type() = "Boo";
      cb.set_concrete_system_type() = "As";
      CHECK_FALSE(cpp17::as_const(cb).concrete_constraint_type().empty());
      CHECK_FALSE(cpp17::as_const(cb).concrete_system_type().empty());
    }
  }

}  // namespace
