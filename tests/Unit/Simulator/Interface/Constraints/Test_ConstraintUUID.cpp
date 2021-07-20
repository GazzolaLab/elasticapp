//
// Created by Tejaswin Parthasarathy on 12/29/20.
//

#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Constraints/ConstraintUUID.hpp"

namespace {

  using namespace ::elastica::interface::constraints;

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Constraints.ConstraintUUID",
                     "[Unit][Simulator][Interface][Constraints]") {
    {
      ConstraintUUID id(1, "test");
      CHECK(id.uuid == 1);
      CHECK(id.name == "test");
    }

    {
      CHECK(ConstraintUUID(2, "second").uuid == 2);
      CHECK(ConstraintUUID(5, "third").name == "third");
    }
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Constraints.ConstraintQueryID",
                     "[Unit][Simulator][Interface][Constraints]") {
    using Type_ = int;
    using Q = ConstraintQueryID<Type_>;
    {
      Q id(1UL);
      CHECK(id.uuid == 1UL);
    }

    {
      CHECK(Q(2UL).uuid == 2UL);
      CHECK(id(Q(5UL)) == 5UL);
    }
  }

}  // namespace
