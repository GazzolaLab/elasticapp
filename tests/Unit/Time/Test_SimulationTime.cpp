//
// Created by Tejaswin Parthasarathy on 5/25/21.
//

///
#include "Framework/TestingFramework.hpp"
///
#include "Framework/TestHelpers.hpp"
#include "Time/SimulationTime.hpp"
#include "Utilities/IgnoreUnused.hpp"

namespace {
  using ::elastica::Time;
  using ::elastica::TimeDelta;

  ELASTICA_TEST_CASE("Unit.Time.Time", "[Unit][Time]") {
    const double tstart_d = 0.68138945475734402635;
    const double tend_d = 76.34481744714527451379;
    // Make sure we're using values that will trigger rounding errors.
    CHECK_FALSE(tstart_d + (tend_d - tstart_d) == tend_d);
  }

  ELASTICA_TEST_CASE("Unit.Time.Time.serialization",
                     "[Unit][Time][Serialization]") {
    Time time(5.4125);
    test_serialization(time);
  }

  ELASTICA_TEST_CASE("Unit.Time.TimeDelta", "[Unit][Time]") {
    TimeDelta td;
    elastica::ignore_unused(td);
  }

  ELASTICA_TEST_CASE("Unit.Time.TimeDelta.serialization",
                     "[Unit][Time][Serialization]") {
    Time start(5.4125), end(56.24124125);
    const TimeDelta dt = end - start;
    test_serialization(dt);
  }

}  // namespace