//
// Created by Tejaswin Parthasarathy on 7/30/22.
//
#include "Framework/TestingFramework.hpp"
//
#include "Simulator/IO/Interface/MetadataInterface.hpp"
#include "Utilities/AsConst.hpp"

namespace {

  using namespace elastica::io;
  // only test the interface : implementations are tested in other files

  ELASTICA_TEST_CASE("Unit.Simulator.IO.MetadataInterface",
                     "[Unit][Simulator][IO]") {
    MetadataInterface interface {};

    // setters
    interface.cwd().working_directory = "../";
    interface.naming_convention().delimiter = '-';
    interface.naming_convention().format.pad = '9';

    // getters
    CHECK(std::as_const(interface).cwd().working_directory == "../");
    CHECK(std::as_const(interface).naming_convention().delimiter == '-');
    CHECK(std::as_const(interface).naming_convention().format.pad == '9');

    (void)(format("abc", interface));
    (void)(get_file_name("abc", interface));
    (void)(get_file_path("abc", interface));
  }

}  // namespace
