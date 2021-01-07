//
// Created by Tejaswin Parthasarathy on 12/29/20.
//

#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Constraints/ConstraintUUID.hpp"

namespace {

  using namespace ::elastica::interface::constraints;

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Constraints.ConstraintUUID.Constructor",
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

}  // namespace
