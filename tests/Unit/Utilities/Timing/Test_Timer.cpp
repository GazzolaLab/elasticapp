//
// Created by Tejaswin Parthasarathy on 7/7/22.
//

#include <chrono>
#include <sstream>
#include <thread>

#include "Framework/TestingFramework.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/Timing/Timer.h"
#include "Utilities/Timing/WcTimer.h"
#include "Utilities/TypeTraits/Cpp17.hpp"

namespace {

  void sleep_for_10ms() {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  ELASTICA_TEST_CASE("Unit.Utilities.Timing.Timer", "[Unit][Utilities]") {
    SECTION("Single example") {
      // [timer_eg]
      // Creating a new wall clock timer
      elastica::timing::WcTimer timer;

      // Starting the time measurement
      timer.start();

      sleep_for_10ms();  // Program or code fragment to be measured

      // Stopping the time measurement
      timer.stop();

      // Evaluation of the last measured time
      double time_v = timer.last();
      // [timer_eg]

      CHECK(time_v > 1e-6);
    }

    SECTION("Loop example") {
      // [timer_loop_eg]
      // Creating a new wall clock timer
      elastica::timing::WcTimer timer;

      // Starting 10 wall clock time measurements
      for (unsigned int i = 0; i < 10; ++i) {
        timer.start();
        sleep_for_10ms();  // Program or code fragment to be measured
        timer.stop();
      }

      timer.start();

      // Starting 10 wall clock time measurements
      for (unsigned int i = 0; i < 10; ++i) {
        sleep_for_10ms();  // Program or code fragment to be measured
        timer.lap();       // ends the current and starts a new one
      }

      // After the measurements, the desired timing results can be calculated,
      // as for instance the average wall clock time
      double average_time = timer.average();
      double min_time = timer.min();
      double max_time = timer.max();
      double total_time = timer.total();

      // In case you want to reset the state of the timer
      timer.reset();
      // [timer_loop_eg]

      CHECK(total_time > 1e-5);
      elastica::ignore_unused(average_time, min_time, max_time, total_time);
    }
  }

  ELASTICA_TEST_CASE("Unit.Utilities.Timing.Timer.stream",
                     "[Unit][Utilities]") {
    // Creating a new wall clock timer
    elastica::timing::WcTimer timer;

    // Starting 10 wall clock time measurements
    for (unsigned int i = 0; i < 10; ++i) {
      timer.start();
      sleep_for_10ms();  // Program or code fragment to be measured
      timer.stop();
    }

    std::stringstream ss;
    ss << timer;
    INFO(ss.str());

    using namespace std::string_literals;
    for (auto const& substr :
         {"total"s, "counter"s, "average"s, "min"s, "max"s, "last"s}) {
      CHECK(ss.str().find(substr) != std::string::npos);
    }
  }

}  // namespace
