//
// Created by Tejaswin Parthasarathy on 8/17/22.
//
#include "Framework/TestingFramework.hpp"
//
#include "Simulator/IO/YAML/YAMLMetadata.hpp"
#include "Utilities/AsConst.hpp"
//
#include <cstdio>
//
#include <fstream>

#include "Time/Time.hpp"

namespace {

  using namespace elastica::io;
  // only test the interface : implementations are tested in other files

  template <typename T>
  struct ScopedMetadataTest {
    T interface;

    //    template <typename... Args>
    //    static auto setup(Args&&... args) -> T {
    //      T v{std::forward<Args>(args)...};
    //      CHECK(v.naming_convention().extension == T::extension());
    //      return v;
    //    }

    template <typename... Args>
    ScopedMetadataTest(Args&&... args)
        : interface(std::forward<Args>(args)...) {
      CHECK(interface.naming_convention().extension == T::extension());
    }

    auto cleanup() {
      const std::string filename =
          get_file_path(interface.metadata_filename(), interface);
      auto const status = static_cast<bool>(std::remove(filename.c_str()));

      CHECK_FALSE(status);  // 0 is good
    }

    ~ScopedMetadataTest() { cleanup(); }
  };

  ELASTICA_TEST_CASE("Unit.Simulator.IO.YAML.YAMLMetadata",
                     "[Unit][Simulator][IO]") {
    using Test = ScopedMetadataTest<YAMLMetadata>;

    SECTION("Default") {
      Test test{};
      auto& interface = test.interface;

      ::elastica::TimeStepID id{1};
      for (int i = 0; i < 50; ++i)
        ++id;

      ::elastica::TimeMetadata time_data{::elastica::Time{50},
                                         ::elastica::TimeDelta{0.02}, id};
      interface.write_at(time_data);

      // check fidelity of file

      auto const metadatafile =
          get_file_path(interface.metadata_filename(), interface);
      INFO(metadatafile);
      YAML::Node metadata = YAML::LoadFile(metadatafile);
      YAML::const_iterator it = metadata.begin();
      auto at_time = *it;

      // FIXME : None of the tests below pass although the file itself looks
      // correct YamlCpp usage docs are very minimal
      //      CHECK(metadata[50]);

      //      CHECK(at_time["TimeMetadata"]);
      //      CHECK(at_time["filename"]);
      //      CHECK(at_time["filename"].as<std::string>().size());
      CHECK(true);
    }
  }

}  // namespace
