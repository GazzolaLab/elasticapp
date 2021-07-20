//
// Created by Tejaswin Parthasarathy on 1/28/21.
//

#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Connections/ConnectionUUID.hpp"

namespace {

  using namespace ::elastica::interface::connections;

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Connections.ConnectionUUID",
      "[Unit][Simulator][Interface][Connections]") {
    {
      ConnectionUUID id(1, "test");
      CHECK(id.uuid == 1);
      CHECK(id.name == "test");
    }

    {
      CHECK(ConnectionUUID(2, "second").uuid == 2);
      CHECK(ConnectionUUID(5, "third").name == "third");
    }
  }

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Connections.ConnectionQueryID",
      "[Unit][Simulator][Interface][Connections]") {
    using Type_ = int;
    using Q = ConnectionQueryID<Type_>;
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
