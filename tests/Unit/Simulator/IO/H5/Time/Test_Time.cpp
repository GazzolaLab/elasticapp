//
// Created by Tejaswin Parthasarathy on 7/25/22.
//
#include "Framework/TestingFramework.hpp"
#include "Simulator/IO/H5/CreateH5.hpp"

//
#include "Simulator/IO/H5/Time/Time.hpp"
//
#include "Time/Time.hpp"

namespace {

  template <typename T>
  void run_h5_test() {
    auto group = generate_h5();
    // Setup
    T data{50};

    // Write and flush
    elastica::io::write_data(group, data, "data");
    group.getFile().flush();

    // Clobber data
    T another_data{2};
    CHECK_FALSE(data == another_data);

    // Read data back
    elastica::io::read_data(group, another_data, "data");

    // Post check
    CHECK(data == another_data);
  }

  ELASTICA_TEST_CASE("Unit.Simulator.IO.H5.Time", "[Unit][Simulator][IO]") {
    run_h5_test<::elastica::Time>();
  }

  ELASTICA_TEST_CASE("Unit.Simulator.IO.H5.TimeDelta",
                     "[Unit][Simulator][IO]") {
    run_h5_test<::elastica::TimeDelta>();
  }

  ELASTICA_TEST_CASE("Unit.Simulator.IO.H5.TimeStepID",
                     "[Unit][Simulator][IO]") {
    auto group = generate_h5();
    // Setup
    using T = ::elastica::TimeStepID;
    T data{1};
    for (int i = 0; i < 50; ++i)
      ++data;

    // Write and flush
    elastica::io::write_data(group, data, "data");
    group.getFile().flush();

    // Clobber data
    T another_data{1};
    for (int i = 0; i < 25; ++i)
      ++another_data;
    CHECK_FALSE(data == another_data);

    // Read data back
    elastica::io::read_data(group, another_data, "data");

    // Post check
    CHECK(data == another_data);
  }

  ELASTICA_TEST_CASE("Unit.Simulator.IO.H5.TimeMetadata",
                     "[Unit][Simulator][IO]") {
    ::elastica::TimeStepID id{1};
    for (int i = 0; i < 50; ++i)
      ++id;

    ::elastica::TimeMetadata data{::elastica::Time{50},
                                  ::elastica::TimeDelta{0.02}, id};

    auto group = generate_h5();
    elastica::io::write_data(group, data, "data");
    group.getFile().flush();

    // Clobber data
    ::elastica::TimeStepID another_id{1};
    for (int i = 0; i < 25; ++i)
      ++another_id;

    ::elastica::TimeMetadata another_data{
        ::elastica::Time{20}, ::elastica::TimeDelta{0.01}, another_id};

    CHECK_FALSE(data == another_data);

    // Read data back
    elastica::io::read_data(group, another_data, "data");

    // Post check
    CHECK(data == another_data);
  }

}  // namespace
