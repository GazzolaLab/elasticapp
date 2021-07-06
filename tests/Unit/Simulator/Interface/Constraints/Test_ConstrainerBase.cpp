//
// Created by Tejaswin Parthasarathy on 6/23/21.
//
#include <utility>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Constraints/ConstrainerBase.hpp"
#include "Simulator/Interface/Constraints/MockConstraints.hpp"

namespace {

  using namespace ::elastica::interface::constraints;

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Constraints.ConstrainerBase.constructor",
      "[Unit][Simulator][Interface][Constraints]") {
    {
      using Constrainer = ::mock::Constrainer;
      Constrainer(ConstraintUUID(4, "test"));
    }
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Constraints.ConstrainerBase.id",
                     "[Unit][Simulator][Interface][Constraints]") {
    {
      using Constrainer = ::mock::Constrainer;
      Constrainer cb(ConstraintUUID(4, "test"));
      // id&
      CHECK(cb.id().uuid == 4);
      CHECK(cb.id().name == "test");
    }

    {
      using Constrainer = ::mock::Constrainer;
      // id const &
      CHECK(Constrainer(ConstraintUUID(4, "test")).id().uuid == 4);
      CHECK(Constrainer(ConstraintUUID(4, "test")).id().name == "test");
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Constraints.ConstrainerBase.apply",
                     "[Unit][Simulator][Interface][Constraints]") {
    {
      // clang-format on
      using Constrainer = ::mock::Constrainer;
      Constrainer cb(ConstraintUUID(4, "test"));

      Constrainer::TimeType t{};
      CHECK(cb.value() == 2);
      CHECK((cb.constrain_primitives(t), cb.value()) == 4);
      CHECK((cb.constrain_rates(t), cb.value()) == 3);
    }
  }

}  // namespace
