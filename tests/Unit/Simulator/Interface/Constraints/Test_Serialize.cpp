//
// Created by Tejaswin Parthasarathy on 6/23/21.
//
#include <sstream>

#include "Framework/TestNestedExceptions.hpp"
#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Constraints/MockConstraints.hpp"
#include "Simulator/Interface/Constraints/Serialize.hpp"

namespace {

  using ::elastica::interface::constraints::ConstrainerBase;
  using ::elastica::interface::constraints::ConstraintUUID;

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Constraints.ConstraintUUID.Serialize",
                     "[Unit][Simulator][Interface][Constraints][Serialization]") {
    // clang-format on
    using namespace elastica::io;

    // no throw case
    {
      std::stringstream ss;  // any stream can be used
      {
        BinaryOutputArchive oarchive(ss);  //  Create an output archive

        ConstraintUUID id(1, "test");
        oarchive(id);
      }

      {
        BinaryInputArchive iarchive(ss);  //  Create an output archive
        ConstraintUUID id(1, "test");

        iarchive(id);
      }
    }

    // throw up
    {
      std::stringstream ss;  // any stream can be used
      {
        BinaryOutputArchive oarchive(ss);  //  Create an output archive

        ConstraintUUID id(1, "test");
        oarchive(id);
      }

      {
        BinaryInputArchive iarchive(ss);  //  Create an output archive
        ConstraintUUID id(2, "testing");

        CHECK_THROWS_NESTED_WITH(iarchive(id), "Loading ConstraintUUID failed",
                                 "does not match");
      }
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Constraints.ConstrainerBase.Serialize",
                     "[Unit][Simulator][Interface][Constraints][Serialization]") {
    // clang-format on
    using namespace elastica::io;

    // no throw case
    {
      std::stringstream ss;  // any stream can be used
      {
        BinaryOutputArchive oarchive(ss);  //  Create an output archive

        ::mock::Constrainer cb(ConstraintUUID(4, "test"));
        oarchive(static_cast<typename ::mock::Constrainer::Parent&>(cb));
      }

      {
        BinaryInputArchive iarchive(ss);  //  Create an output archive
        ::mock::Constrainer cb(ConstraintUUID(4, "test"));

        iarchive(static_cast<typename ::mock::Constrainer::Parent&>(cb));
      }
    }
    // throw up
    {
      std::stringstream ss;  // any stream can be used
      {
        BinaryOutputArchive oarchive(ss);  //  Create an output archive

        ::mock::Constrainer cb(ConstraintUUID(4, "test"));
        oarchive(static_cast<typename ::mock::Constrainer::Parent&>(cb));
      }

      {
        BinaryInputArchive iarchive(ss);  //  Create an output archive
        ::mock::Constrainer cb(ConstraintUUID(3, "testing"));

        CHECK_THROWS_NESTED_WITH(
            iarchive(static_cast<typename ::mock::Constrainer::Parent&>(cb)),
            "Loading ConstraintUUID failed", "does not match");
      }
    }
  }

}  // namespace
