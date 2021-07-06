//
// Created by Tejaswin Parthasarathy on 6/19/21.
//

#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Wrappers/UUID.hpp"

namespace {

  using namespace ::elastica::interface::detail;

  struct TestTag;
  using TestUUID = UUID<TestTag>;

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.UUID",
                     "[Unit][Simulator][Interface]") {
    {
      TestUUID id(1, "test");
      CHECK(id.uuid == 1);
      CHECK(id.name == "test");
    }

    {
      CHECK(TestUUID(2, "second").uuid == 2);
      CHECK(TestUUID(5, "third").name == "third");
    }

    {
      TestUUID uuid(1, "test");
      CHECK(id(uuid) == 1);
      CHECK(name(uuid) == "test");
    }

    {
      CHECK(id(TestUUID(2, "second")) == 2);
      CHECK(name(TestUUID(5, "third")) == "third");
    }
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.UUID.comparison",
                     "[Unit][Simulator][Interface]") {
    {
      TestUUID lhs(2, "second");
      TestUUID rhs(2, "second");
      CHECK(lhs == rhs);

      TestUUID dhs(2, "recond");
      CHECK(dhs != rhs);
    }
    {
      CHECK(TestUUID(2, "second") == TestUUID(2, "second"));
      CHECK(TestUUID(2, "second") != TestUUID(2, "recond"));
    }
  }

}  // namespace
