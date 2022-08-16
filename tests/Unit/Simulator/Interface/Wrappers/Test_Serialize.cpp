//
// Created by Tejaswin Parthasarathy on 6/19/21.
//
#include <sstream>

#include "Framework/TestNestedExceptions.hpp"
#include "Framework/TestingFramework.hpp"
#include "Simulator/IO/Serialization/Archives.hpp"
#include "Simulator/Interface/Wrappers/Serialize.hpp"

namespace {

  using namespace ::elastica::interface::detail;

  struct TestTag;
  using TestUUID = UUID<TestTag>;

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.UUID.Serialize",
                     "[Unit][Simulator][Interface][Serialization]") {
    // clang-format on
    using namespace elastica::io;

    // no throw case
    {
      std::stringstream ss;  // any stream can be used
      {
        BinaryOutputArchive oarchive(ss);  //  Create an output archive

        TestUUID id(1, "test");
        oarchive(id);
      }

      {
        BinaryInputArchive iarchive(ss);  //  Create an output archive
        TestUUID id(1, "test");

        iarchive(id);
      }
    }

    // throw up

    {
      std::stringstream ss;  // any stream can be used
      {
        BinaryOutputArchive oarchive(ss);  //  Create an output archive

        TestUUID id(1, "test");
        oarchive(id);
      }

      {
        BinaryInputArchive iarchive(ss);  //  Create an output archive
        TestUUID id(2, "testing");

        CHECK_THROWS_NESTED_WITH(iarchive(id), "Loading UUID failed",
                                 "does not match");
      }
    }
  }

}  // namespace