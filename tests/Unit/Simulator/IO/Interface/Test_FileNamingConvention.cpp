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
    CHECK(format("abc", FileNamingConvention{}) == "000abc");

    CHECK(get_file_name("100", FileNamingConvention{}) == "elastica_100.");
    CHECK(get_file_name("100", FileNamingConvention{"el", "viz", "h5"}) ==
          "el_100_viz.h5");
    CHECK(get_file_name("100",
                        FileNamingConvention{"el", "viz", "h5", {6, '*'}}) ==
          "el_100_viz.h5");
    CHECK(get_file_name(
              "100", FileNamingConvention{"el", "viz", "h5", {6, '2'}, '-'}) ==
          "el-100-viz.h5");
  }

}  // namespace
