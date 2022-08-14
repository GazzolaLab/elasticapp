//
// Created by Tejaswin Parthasarathy on 7/26/22.
//
#include "Framework/TestingFramework.hpp"
//
#include "Simulator/IO/Interface/Helpers.hpp"

namespace {

  using namespace elastica::io;

  ELASTICA_TEST_CASE("Unit.Simulator.IO.format_helpers",
                     "[Unit][Simulator][IO]") {
    CHECK(format_id(5, "1", '0') == "00001");
    CHECK(format_id(5, "100", '0') == "00100");
    CHECK(format_id(5, "10000", '0') == "10000");
    CHECK(format_id(6, "10000", '0') == "010000");
    CHECK(format_id(4, "10000", '0') == "10000");
    CHECK(format_id(5, "123", '*') == "**123");
    CHECK(format_id(5, "abc", '9') == "99abc");
    CHECK(format_id(5, "", '9') == "99999");

    CHECK(format_suffix("test", '-') == "-test");
    CHECK(format_suffix("test", '_') == "_test");
    //    CHECK(format_suffix("test", '') == "test");
    CHECK(format_suffix("", '_').empty());
    //    CHECK(format_suffix("", '').empty());
  }

}  // namespace