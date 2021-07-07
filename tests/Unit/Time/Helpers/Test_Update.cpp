//
// Created by Tejaswin Parthasarathy on 6/1/21.
//

///
#include "Framework/TestingFramework.hpp"
///
#include "Time/Helpers/Update.hpp"
#include "Time/SimulationTime.hpp"
#include "Utilities/DefineTypes.h"

namespace {

  using namespace elastica::timestepper::detail;
  using elastica::TimeDelta;

  struct CustomStep : TimeUpdate<CustomStep> {
    static auto temporal_coefficient() noexcept -> elastica::real_t {
      return static_cast<elastica::real_t>(0.5);
    }
  };

  ELASTICA_TEST_CASE("Unit.Time.Helpers.TimeUpdate", "[Time][Unit]") {
    // check default initialization
    CustomStep const custom_step;
    TimeDelta dt{2.0};

    CHECK(static_cast<elastica::real_t>(custom_step(dt)) == approx(1.0));
  }

  ELASTICA_TEST_CASE("Unit.Time.Helpers.dispatch_based_on", "[Time][Unit]") {
    using elastica::timestepper::dispatch_based_on;

    // check default initialization
    CustomStep const custom_step;
    struct OtherStep : KinematicUpdate<OtherStep> {
    } other_step;
    struct AnotherStep : DynamicUpdate<AnotherStep> {
    } another_step;

    // two argument versions
    {
      enum test_status { system_update, time_update } status = system_update;
      dispatch_based_on(
          custom_step, [&status](auto const&) { status = system_update; },
          [&status](auto const&) { status = time_update; });
      CHECK(status == time_update);

      dispatch_based_on(
          other_step, [&status](auto const&) { status = system_update; },
          [&status](auto const&) { status = time_update; });
      CHECK(status == system_update);

      dispatch_based_on(
          custom_step, [&status](auto const&) { status = system_update; },
          [&status](auto const&) { status = time_update; });
      CHECK(status == time_update);

      dispatch_based_on(
          another_step, [&status](auto const&) { status = system_update; },
          [&status](auto const&) { status = time_update; });
      CHECK(status == system_update);
    }

    // two argument versions
    {
      enum test_status {
        kinematic_update,
        dynamic_update,
        time_update
      } status = kinematic_update;

      auto kin = [&status](auto const&) { status = kinematic_update; };
      auto dyn = [&status](auto const&) { status = dynamic_update; };
      auto tim = [&status](auto const&) { status = time_update; };

      dispatch_based_on(custom_step, kin, dyn, tim);
      CHECK(status == time_update);

      dispatch_based_on(other_step, kin, dyn, tim);
      CHECK(status == kinematic_update);

      dispatch_based_on(custom_step, kin, dyn, tim);
      CHECK(status == time_update);

      dispatch_based_on(another_step, kin, dyn, tim);
      CHECK(status == dynamic_update);
    }
  }

}  // namespace
