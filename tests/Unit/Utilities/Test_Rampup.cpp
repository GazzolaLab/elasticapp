//
// Created by Tejaswin Parthasarathy on 5/29/22.
//

#include "Framework/TestingFramework.hpp"
#include "Utilities/Clamp.hpp"
#include "Utilities/Rampup.hpp"

namespace {

  ELASTICA_TEST_CASE("Unit.Utilities.Rampup.StepProfile",
                     "[Unit][Utilities]") {
    using Real = double;
    using namespace elastica::rampup;
    StepProfile<Real> step;
    CHECK(step(4.0, 0.0, 2.0) == approx(1.0));
    CHECK(step(5.0, 0.0, 2.0) == approx(1.0));
    CHECK(step(5.0, -1.0, 2.0) == approx(1.0));
  }

  ELASTICA_TEST_CASE("Unit.Utilities.Rampup.LinearProfile",
                     "[Unit][Utilities]") {
    using Real = double;
    using namespace elastica::rampup;
    LinearProfile<Real> linear;
    CHECK(linear(4.0, 0.0, 2.0) == approx(2.0));
    CHECK(linear(5.0, 0.0, 2.0) == approx(2.5));
    CHECK(linear(5.0, -1.0, 2.0) == approx(3.0));
  }

  ELASTICA_TEST_CASE("Unit.Utilities.Rampup.SinProfile", "[Unit][Utilities]") {
    using Real = double;
    using namespace elastica::rampup;
    SinProfile<Real> sinp;
    CHECK(sinp(0.0, 0.0, 2.0) == approx(0.0));
    CHECK(sinp(1.0, 0.0, 2.0) == approx(0.5));
    CHECK(sinp(2.0, 0.0, 2.0) == approx(1.0));

    // sanity check two values outside range
    CHECK(sinp(-2.0, 0.0, 2.0) == approx(-1.0));
    CHECK(sinp(4.0, 0.0, 2.0) == approx(2.0));
  }

  ELASTICA_TEST_CASE("Unit.Utilities.Rampup.make_step_rampup",
                     "[Unit][Utilities]") {
    SECTION("canonical") {
      /// [make_step_rampup_eg]
      using Real = double;
      Real start = 0.0, interval = 1.0;
      auto step = elastica::make_step_rampup(start, interval);
      CHECK(step(start) == approx(1.0));
      /// [make_step_rampup_eg]

      for (auto val : {0.0, 0.2, 0.5, 0.8, 1.0, 1.2}) {
        CHECK(step(val) == 1.0);
      }
    }

    SECTION("shifted") {
      using Real = double;
      Real start = -2.0, interval = 4.0;

      auto step = elastica::make_step_rampup(start, interval);
      CHECK(step(-2.5) == approx(0.0));
      CHECK(step(start) == approx(1.0));

      CHECK(step(start + 0.3 * interval) == approx(1.0));
      CHECK(step(start + 0.5 * interval) == approx(1.0));
      CHECK(step(start + 0.7 * interval) == approx(1.0));

      CHECK(step(start + interval) == approx(1.0));
      CHECK(step(start + 1.5 * interval) == approx(1.0));
    }
  }

  ELASTICA_TEST_CASE("Unit.Utilities.Rampup.make_linear_rampup",
                     "[Unit][Utilities]") {
    SECTION("canonical") {
      /// [make_linear_rampup_eg]
      using Real = double;
      Real start = 0.0, interval = 1.0;
      auto linear = elastica::make_linear_rampup(start, interval);
      CHECK(linear(start) == approx(0.0));
      /// [make_linear_rampup_eg]

      for (auto val : {-0.2, 0.0, 0.2, 0.5, 0.8, 1.0, 1.2}) {
        CHECK(linear(val) == std::clamp(val, start, start + interval));
      }
    }

    SECTION("shifted") {
      using Real = double;
      Real start = -2.0, interval = 4.0;

      auto linear = elastica::make_linear_rampup(start, interval);
      CHECK(linear(-2.5) == approx(0.0));
      CHECK(linear(start) == approx(0.0));

      CHECK(linear(start + 0.3 * interval) == approx(0.3));
      CHECK(linear(start + 0.5 * interval) == approx(0.5));
      CHECK(linear(start + 0.7 * interval) == approx(0.7));

      CHECK(linear(start + interval) == approx(1.0));
      CHECK(linear(start + 1.5 * interval) == approx(1.0));
    }
  }

  ELASTICA_TEST_CASE("Unit.Utilities.Rampup.make_sin_rampup",
                     "[Unit][Utilities]") {
    SECTION("canonical") {
      /// [make_sin_rampup_eg]
      using Real = double;
      Real start = 0.0, interval = 1.0;
      auto sinp = elastica::make_sin_rampup(start, interval);
      CHECK(sinp(start) == approx(0.0));
      /// [make_sin_rampup_eg]

      CHECK(sinp(-0.2) == approx(0.0));
      CHECK(sinp(0.0) == approx(0.0));

      // symmetry checks
      CHECK(sinp(0.5) == approx(0.5));
      CHECK(sinp(0.25) + sinp(0.75) == approx(1.0));

      CHECK(sinp(0.5) == approx(0.5));

      CHECK(sinp(1.0) == approx(1.0));
      CHECK(sinp(1.2) == approx(1.0));
    }

    SECTION("shifted") {
      using Real = double;
      Real start = -2.0, interval = 4.0;

      auto sinp = elastica::make_sin_rampup(start, interval);
      CHECK(sinp(-2.5) == approx(0.0));
      CHECK(sinp(start) == approx(0.0));

      CHECK(sinp(start + 0.3 * interval) + sinp(start + 0.7 * interval) ==
            approx(1.0));
      CHECK(sinp(start + 0.5 * interval) == approx(0.5));

      CHECK(sinp(start + interval) == approx(1.0));
      CHECK(sinp(start + 1.5 * interval) == approx(1.0));
    }
  }

}  // namespace