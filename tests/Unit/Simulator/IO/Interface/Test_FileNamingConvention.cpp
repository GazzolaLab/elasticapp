//
// Created by Tejaswin Parthasarathy on 7/26/22.
//
#include "Framework/TestingFramework.hpp"
//
#include "Simulator/IO/Interface/FileNamingConvention.hpp"

namespace {

  using namespace elastica::io;

  ELASTICA_TEST_CASE("Unit.Simulator.IO.FileNamingConvention",
                     "[Unit][Simulator][IO]") {
    CHECK(format("abc", FileNamingConvention{}) == "0000000abc");

    CHECK(get_file_name("100", FileNamingConvention{}) == "elastica_100.");
    CHECK(get_file_name("100", FileNamingConvention{"el", "viz", "h5"}) ==
          "el_100_viz.h5");
    CHECK(get_file_name("100",
                        FileNamingConvention{"el", "viz", "h5", {'*', 6}}) ==
          "el_100_viz.h5");
    CHECK(get_file_name(
              "100", FileNamingConvention{"el", "viz", "h5", {'2', 6}, '-'}) ==
          "el-100-viz.h5");

    // Prototypical use case
    {
      FileNamingConvention convention{"el", "viz", "h5", {'0', 10}};
      auto s = format("010000000", convention);
      CHECK(s == "0010000000");
      CHECK(get_file_name(s, convention) == "el_0010000000_viz.h5");
    }
  }

}  // namespace
