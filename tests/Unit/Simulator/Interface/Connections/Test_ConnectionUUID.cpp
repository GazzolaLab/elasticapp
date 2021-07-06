//
// Created by Tejaswin Parthasarathy on 1/28/21.
//

#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Connections/ConnectionUUID.hpp"

namespace {

  using namespace ::elastica::interface::connections;

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Connections.ConnectionUUID.Constructor",
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

}  // namespace
