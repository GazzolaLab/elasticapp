//
// Created by Tejaswin Parthasarathy on 1/25/21.
//

#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Forcing/ForcingUUID.hpp"

namespace {

  using namespace ::elastica::interface::forcing;

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Forcing.ForcingUUID.Constructor",
                     "[Unit][Simulator][Interface][Forcing]") {
    {
      ForcingUUID id(1, "test");
      CHECK(id.uuid == 1);
      CHECK(id.name == "test");
    }

    {
      CHECK(ForcingUUID(2, "second").uuid == 2);
      CHECK(ForcingUUID(5, "third").name == "third");
    }
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Forcing.ForcingQueryID",
                     "[Unit][Simulator][Interface][Forcing]") {
    using Type_ = int;
    using Q = ForcingQueryID<Type_>;
    {
      Q id(ForcingUUID(1UL, ""));
      CHECK(id.uuid.uuid == 1UL);
    }

    {
      CHECK(Q(ForcingUUID(2UL, "")).uuid.uuid == 2UL);
      CHECK(id(id(Q(ForcingUUID(5UL, "")))) == 5UL);
    }
  }

}  // namespace
