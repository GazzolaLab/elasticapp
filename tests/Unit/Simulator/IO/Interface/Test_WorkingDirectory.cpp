//
// Created by Tejaswin Parthasarathy on 7/26/22.
//
#include "Framework/TestingFramework.hpp"
//
#include "Simulator/IO/Interface/WorkingDirectory.hpp"

namespace {

  using namespace elastica::io;

  ELASTICA_TEST_CASE("Unit.Simulator.IO.WorkingDirectory",
                     "[Unit][Simulator][IO]") {
    // may need to revisit this one given this is not system agnostic
    CHECK(get_file_path("100.txt", WorkingDirectory{}) == "./100.txt");
  }

}  // namespace
