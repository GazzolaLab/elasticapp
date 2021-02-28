//
// Created by Tejaswin Parthasarathy on 2/28/21.
//
#include "Framework/TestingFramework.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/SystemClock.hpp"

namespace {

  ELASTICA_TEST_CASE("Unit.Utilities.system_clock", "[Unit][Utilities]") {
    /// [system_clock_creation]
    auto clock_handle = elastica::the_system_clock();
    /// [system_clock_creation]

    /// [system_clock_usage]
    std::time_t start = clock_handle->start();
    std::time_t now = clock_handle->now();
    std::time_t elapsed = clock_handle->elapsed();
    /// [system_clock_usage]

    elastica::ignore_unused(start, now, elapsed);
  }

}  // namespace
