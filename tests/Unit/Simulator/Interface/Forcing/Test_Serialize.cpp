//
// Created by Tejaswin Parthasarathy on 6/24/21.
//

#include <sstream>

#include "Framework/TestNestedExceptions.hpp"
#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Forcing/MockForcing.hpp"
#include "Simulator/Interface/Forcing/Serialize.hpp"

namespace {

  using ::elastica::interface::forcing::ForcerBase;
  using ::elastica::interface::forcing::ForcingUUID;

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Forcing.ForcingUUID.Serialize",
                     "[Unit][Simulator][Interface][Forcing][Serialization]") {
    // clang-format on
    using namespace elastica::io;

    // no throw case
    {
      std::stringstream ss;  // any stream can be used
      {
        BinaryOutputArchive oarchive(ss);  //  Create an output archive

        ForcingUUID id(1, "test");
        oarchive(id);
      }

      {
        BinaryInputArchive iarchive(ss);  //  Create an output archive
        ForcingUUID id(1, "test");

        iarchive(id);
      }
    }

    // throw up
    {
      std::stringstream ss;  // any stream can be used
      {
        BinaryOutputArchive oarchive(ss);  //  Create an output archive

        ForcingUUID id(1, "test");
        oarchive(id);
      }

      {
        BinaryInputArchive iarchive(ss);  //  Create an output archive
        ForcingUUID id(2, "testing");

        CHECK_THROWS_NESTED_WITH(iarchive(id), "Loading ForcingUUID failed",
                                 "does not match");
      }
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Forcing.ForcerBase.Serialize",
                     "[Unit][Simulator][Interface][Forcing][Serialization]") {
    // clang-format on
    using namespace elastica::io;

    // no throw case
    {
      std::stringstream ss;  // any stream can be used
      {
        BinaryOutputArchive oarchive(ss);  //  Create an output archive

        ::mock::Forcer cb(ForcingUUID(4, "test"));
        oarchive(static_cast<typename ::mock::Forcer::Parent&>(cb));
      }

      {
        BinaryInputArchive iarchive(ss);  //  Create an output archive
        ::mock::Forcer cb(ForcingUUID(4, "test"));

        iarchive(static_cast<typename ::mock::Forcer::Parent&>(cb));
      }
    }
    // throw up
    {
      std::stringstream ss;  // any stream can be used
      {
        BinaryOutputArchive oarchive(ss);  //  Create an output archive

        ::mock::Forcer cb(ForcingUUID(4, "test"));
        oarchive(static_cast<typename ::mock::Forcer::Parent&>(cb));
      }

      {
        BinaryInputArchive iarchive(ss);  //  Create an output archive
        ::mock::Forcer cb(ForcingUUID(3, "testing"));

        CHECK_THROWS_NESTED_WITH(
            iarchive(static_cast<typename ::mock::Forcer::Parent&>(cb)),
            "Loading ForcingUUID failed", "does not match");
      }
    }
  }

}  // namespace
