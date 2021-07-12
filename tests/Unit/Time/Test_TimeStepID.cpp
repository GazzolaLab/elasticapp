//
// Created by Tejaswin Parthasarathy on 7/12/21.
//

#include <sstream>
///
#include "Framework/TestHelpers.hpp"
#include "Framework/TestNestedExceptions.hpp"
#include "Framework/TestingFramework.hpp"
///
#include "Simulator/IO/Serialization.hpp"
#include "Time/Serialize.hpp"
#include "Time/TimeStepID.hpp"
#include "Utilities/IgnoreUnused.hpp"

namespace {
  using ::elastica::TimeStepID;

  ELASTICA_TEST_CASE("Unit.Time.TimeStepID.constructor", "[Unit][Time]") {
    TimeStepID time_step(1UL);
    CHECK(time_step.step() == 0UL);
    CHECK(time_step.substep() == 0U);
    CHECK(time_step.step_threshold() == 1UL);
  }

  ELASTICA_TEST_CASE("Unit.Time.TimeStepID.pre_increment", "[Unit][Time]") {
    TimeStepID time_step(2UL);
    CHECK(time_step.step() == 0UL);
    CHECK(time_step.substep() == 0U);
    ++time_step;
    CHECK(time_step.step() == 0UL);
    CHECK(time_step.substep() == 1U);
    ++time_step;
    CHECK(time_step.step() == 1UL);
    CHECK(time_step.substep() == 0U);
  }

  ELASTICA_TEST_CASE("Unit.Time.TimeStepID.post_increment", "[Unit][Time]") {
    TimeStepID time_step(2UL);
    CHECK(time_step.step() == 0UL);
    CHECK(time_step.substep() == 0U);

    auto another_time_step = time_step++;
    CHECK(time_step.step() == 0UL);
    CHECK(time_step.substep() == 1U);
    CHECK(another_time_step.step() == 0UL);
    CHECK(another_time_step.substep() == 0U);

    auto yet_another_time_step = time_step++;
    CHECK(time_step.step() == 1UL);
    CHECK(time_step.substep() == 0U);
    CHECK(another_time_step.step() == 0UL);
    CHECK(another_time_step.substep() == 0U);
    CHECK(yet_another_time_step.step() == 0UL);
    CHECK(yet_another_time_step.substep() == 1U);
  }

  ELASTICA_TEST_CASE("Unit.Time.TimeStepID.pre_decrement", "[Unit][Time]") {
    TimeStepID time_step(2UL);
    ++time_step;
    ++time_step;
    ++time_step;
    CHECK(time_step.step() == 1UL);
    CHECK(time_step.substep() == 1U);

    --time_step;
    CHECK(time_step.step() == 1UL);
    CHECK(time_step.substep() == 0U);
    --time_step;
    CHECK(time_step.step() == 0UL);
    CHECK(time_step.substep() == 1U);
    --time_step;
    CHECK(time_step.step() == 0UL);
    CHECK(time_step.substep() == 0U);
  }

  ELASTICA_TEST_CASE("Unit.Time.TimeStepID.post_decrement", "[Unit][Time]") {
    TimeStepID time_step(3UL);
    ++time_step;
    ++time_step;
    ++time_step;
    ++time_step;
    CHECK(time_step.step() == 1UL);
    CHECK(time_step.substep() == 1U);

    time_step--;
    CHECK(time_step.step() == 1UL);
    CHECK(time_step.substep() == 0U);
    time_step--;
    CHECK(time_step.step() == 0UL);
    CHECK(time_step.substep() == 2U);
    time_step--;
    CHECK(time_step.step() == 0UL);
    CHECK(time_step.substep() == 1U);
  }

  ELASTICA_TEST_CASE("Unit.Time.TimeStepID.logical", "[Unit][Time]") {
    TimeStepID time_step(3UL);
    ++time_step;
    ++time_step;
    ++time_step;

    TimeStepID another_time_step(3UL);
    ++another_time_step;
    ++another_time_step;

    CHECK(time_step != another_time_step);

    CHECK(another_time_step < time_step);
    CHECK(another_time_step <= time_step);

    CHECK(time_step > another_time_step);
    CHECK(time_step >= another_time_step);

    CHECK(time_step == ++another_time_step);
    CHECK(another_time_step <= time_step);
    CHECK(time_step >= another_time_step);
  }

  auto fetch_timestep() noexcept {
    TimeStepID timestep(5UL);
    for (const auto IGNORE_UNUSED _ : {1, 2, 3, 4, 5, 6})
      ++timestep;
    CHECK(timestep.step() == 1UL);
    CHECK(timestep.substep() == 1U);

    return timestep;
  }

  ELASTICA_TEST_CASE("Unit.Time.TimeStepID.serialization", "[Unit][Time]") {
    TimeStepID timestep = fetch_timestep();

    std::stringstream ss;  // any stream can be used
    elastica::io::BinaryOutputArchive oarchive(
        ss);  //  Create an output archive
    oarchive(timestep);

    elastica::io::BinaryInputArchive iarchive(ss);  //  Create an output archive
    TimeStepID another_timestep(timestep.step_threshold());
    CHECK(another_timestep.step() == 0UL);
    CHECK(another_timestep.substep() == 0U);

    iarchive(another_timestep);

    CHECK(another_timestep.step() == timestep.step());
    CHECK(another_timestep.substep() == timestep.substep());
    CHECK(another_timestep.step_threshold() == timestep.step_threshold());
  }

  ELASTICA_TEST_CASE("Unit.Time.TimeStepID.exception_in_serialization",
                     "[Unit][Time]") {
    TimeStepID timestep = fetch_timestep();

    std::stringstream ss;  // any stream can be used
    elastica::io::BinaryOutputArchive oarchive(
        ss);  //  Create an output archive
    oarchive(timestep);

    // Try to retrieve an archive that has the wrong number of substeps
    {
      elastica::io::BinaryInputArchive iarchive(
          ss);  //  Create an output archive
      TimeStepID another_timestep(timestep.step_threshold() - 1);

      CHECK_THROWS_NESTED_WITH(iarchive(another_timestep),
                               "Loading TimeStepID failed",
                               "does not match the threshold from");
    }
  }

}  // namespace