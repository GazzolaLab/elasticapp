//
// Created by Tejaswin Parthasarathy on 7/30/22.
//
#include "Framework/TestingFramework.hpp"
//
#include "Simulator/IO/H5/Interface/H5Interface.hpp"
//
#include "Simulator/IO/H5/CreateH5.hpp"
#include "Time/Time.hpp"
//
#include <cstdio>
#include <thread>
#include <vector>

namespace {

  using namespace elastica::io;

  template <typename T>
  struct ScopedInterfaceTest {
    T interface;

    static auto filename() -> std::string { return "interface_test"; }

    ScopedInterfaceTest() : interface(generate_h5(filename()).getFile()) {}

    auto cleanup() {
      auto const filename = interface.file().getName();
      auto const status = static_cast<bool>(std::remove(filename.c_str()));
      CHECK_FALSE(status);  // 0 is good
    }

    ~ScopedInterfaceTest() { cleanup(); }
  };

  ELASTICA_TEST_CASE("Unit.Simulator.IO.H5.H5Interface",
                     "[Unit][Simulator][IO]") {
    using ExportTest = ScopedInterfaceTest<H5Writer>;

    // Read and write a vector of times
    std::vector<::elastica::Time> times{
        ::elastica::Time{0.2}, ::elastica::Time{0.4}, ::elastica::Time{0.6},
        ::elastica::Time{0.8}, ::elastica::Time{1.0}};
    std::vector<::elastica::TimeDelta> dts{
        ::elastica::TimeDelta{0.2}, ::elastica::TimeDelta{0.4},
        ::elastica::TimeDelta{0.6}, ::elastica::TimeDelta{0.8},
        ::elastica::TimeDelta{1.0}, ::elastica::TimeDelta{1.2}};

    auto post_check = [](auto& interface) {
      CHECK_FALSE(interface.counters().empty());
      CHECK(interface.counters().at(pretty_type::name<::elastica::Time>()) ==
            5);
      CHECK(interface.counters().at(
                pretty_type::name<::elastica::TimeDelta>()) == 6);
    };
    SECTION("SingleThreaded") {
      ExportTest export_test{};
      auto& interface = export_test.interface;
      CHECK(interface.counters().empty());
      std::for_each(times.cbegin(), times.cend(), std::ref(interface));
      std::for_each(dts.cbegin(), dts.cend(), std::ref(interface));
      post_check(interface);

      // Look whether the file/group has proper indices
      auto& h5file = interface.file();
      using namespace std::literals;
      for (auto const& str : {"0"s, "1"s, "2"s, "3"s, "4"s}) {
        CAPTURE(str);
        CHECK(
            h5file.getGroup(pretty_type::name<::elastica::Time>()).exist(str));
      }

      for (auto const& str : {"0"s, "1"s, "2"s, "3"s, "4"s, "5"s}) {
        CAPTURE(str);
        CHECK(h5file.getGroup(pretty_type::name<::elastica::TimeDelta>())
                  .exist(str));
      }

      H5Reader reader{h5file};
      CHECK(reader.counters().empty());

      auto duplicate_times = times;
      auto duplicate_dts = dts;
      auto increment_by_1 = [](auto& t) { t.value() += 1.0; };
      // garble data
      std::for_each(duplicate_times.begin(), duplicate_times.end(),
                    increment_by_1);
      std::for_each(duplicate_dts.begin(), duplicate_dts.end(), increment_by_1);

      // run reader
      std::for_each(duplicate_times.begin(), duplicate_times.end(),
                    std::ref(reader));
      std::for_each(duplicate_dts.begin(), duplicate_dts.end(),
                    std::ref(reader));

      post_check(reader);

      CHECK(times == duplicate_times);
      CHECK(dts == duplicate_dts);
    }
  };

}  // namespace
