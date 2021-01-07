#include <utility>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Constraints/ConstraintBase.hpp"
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

    ConstrainerID finalize() const override {
      return nullptr;
    };

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
      auto sink = [](auto&&...) {};
      sink(ConcreteConstraint(4, "test"));
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
      CHECK(ConcreteConstraint(4, "test").id().uuid == 4);
      CHECK(ConcreteConstraint(4, "test").id().name == "test");
    }
  }

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Constraints.ConstraintBase.concrete_name",
      "[Unit][Simulator][Interface][Constraints]") {
    using RegistrarList = tmpl::list<int, float>;
    {
      using ConcreteConstraint = MockConstraint<RegistrarList>;
      ConcreteConstraint cb(4, "test");
      CHECK(cb.concrete_type().empty());
    }
  }

}  // namespace
