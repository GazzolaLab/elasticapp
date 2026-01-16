//
// Created by Tejaswin Parthasarathy on 7/30/22.
//
#include "Framework/TestingFramework.hpp"
//
#include "Simulator/IO/H5/Interface/H5Metadata.hpp"
// Also needs implementation of writer
#include "Simulator/IO/H5/Interface/H5Interface.hpp"
//
#include <cstdio>
//
#include "Time/Time.hpp"

namespace {

  using namespace elastica::io;
  // only test the interface : implementations are tested in other files

  template <typename T>
  struct ScopedMetadataTest {
    T interface;

    template <typename... Args>
    static auto setup(Args&&... args) -> T {
      auto const v = T{std::forward<Args>(args)...};
      CHECK(v.naming_convention().extension == T::extension());
      return v;
    }

    template <typename... Args>
    ScopedMetadataTest(Args&&... args)
        : interface(setup(std::forward<Args>(args)...)) {}

    auto cleanup() {
      const std::string filename =
          get_file_path(interface.metadata_filename(), interface);
      auto const status = static_cast<bool>(std::remove(filename.c_str()));

      CHECK_FALSE(status);  // 0 is good
    }

    ~ScopedMetadataTest() { cleanup(); }
  };

  ELASTICA_TEST_CASE("Unit.Simulator.IO.H5.H5Metadata",
                     "[Unit][Simulator][IO]") {
    using Test = ScopedMetadataTest<H5Metadata>;

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
      auto group = interface.metadata_file().getGroup("0000000050");

      CHECK(group.getLinkType("data") == HighFive::LinkType::External);

      REQUIRE(group.exist("TimeMetadata"));
      CHECK(group.getGroup("TimeMetadata").exist("Timestep"));
    }
  }

}  // namespace
