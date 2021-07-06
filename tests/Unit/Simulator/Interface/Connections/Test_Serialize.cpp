//
// Created by Tejaswin Parthasarathy on 6/19/21.
//
#include <sstream>

#include "Framework/TestNestedExceptions.hpp"
#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Connections/MockConnects.hpp"
#include "Simulator/Interface/Connections/Serialize.hpp"

namespace {

  using ::elastica::interface::connections::ConnecterBase;
  using ::elastica::interface::connections::ConnectionUUID;

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Connections.ConnectionUUID.Serialize",
                     "[Unit][Simulator][Interface][Connections][Serialization]") {
    // clang-format on
    using namespace elastica::io;

    // no throw case
    {
      std::stringstream ss;  // any stream can be used
      {
        BinaryOutputArchive oarchive(ss);  //  Create an output archive

        ConnectionUUID id(1, "test");
        oarchive(id);
      }

      {
        BinaryInputArchive iarchive(ss);  //  Create an output archive
        ConnectionUUID id(1, "test");

        iarchive(id);
      }
    }

    // throw up
    {
      std::stringstream ss;  // any stream can be used
      {
        BinaryOutputArchive oarchive(ss);  //  Create an output archive

        ConnectionUUID id(1, "test");
        oarchive(id);
      }

      {
        BinaryInputArchive iarchive(ss);  //  Create an output archive
        ConnectionUUID id(2, "testing");

        CHECK_THROWS_NESTED_WITH(iarchive(id), "Loading ConnectionUUID failed",
                                 "does not match");
      }
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Connections.ConnecterBase.Serialize",
                     "[Unit][Simulator][Interface][Connections][Serialization]") {
    // clang-format on
    using namespace elastica::io;

    // no throw case
    {
      std::stringstream ss;  // any stream can be used
      {
        BinaryOutputArchive oarchive(ss);  //  Create an output archive

        ::mock::Connecter cb(ConnectionUUID(4, "test"));
        oarchive(static_cast<typename ::mock::Connecter::Parent&>(cb));
      }

      {
        BinaryInputArchive iarchive(ss);  //  Create an output archive
        ::mock::Connecter cb(ConnectionUUID(4, "test"));

        iarchive(static_cast<typename ::mock::Connecter::Parent&>(cb));
      }
    }
    // throw up
    {
      std::stringstream ss;  // any stream can be used
      {
        BinaryOutputArchive oarchive(ss);  //  Create an output archive

        ::mock::Connecter cb(ConnectionUUID(4, "test"));
        oarchive(static_cast<typename ::mock::Connecter::Parent&>(cb));
      }

      {
        BinaryInputArchive iarchive(ss);  //  Create an output archive
        ::mock::Connecter cb(ConnectionUUID(3, "testing"));

        CHECK_THROWS_NESTED_WITH(
            iarchive(static_cast<typename ::mock::Connecter::Parent&>(cb)),
            "Loading ConnectionUUID failed", "does not match");
      }
    }
  }

}  // namespace
