//
// Created by Tejaswin Parthasarathy on 6/18/21.
//
#include "Framework/TestingFramework.hpp"
#include "Simulator/Frames/EulerianFrame.hpp"

namespace {

  // clang-format off
  ELASTICA_TEST_CASE(
  "Unit.Simulator.Frames.EulerianFrame",
  "[Unit][Simulator]") {
    // clang-format on
    using namespace elastica::detail;
    static_assert(EulerianFrame::Dimension == 3UL, "Frame dimension mismatch!");

    /// [cast_along_eg]
    static_assert(cast_along(EulerianFrame::X) == 0UL, "Frame mismatch!");
    static_assert(cast_along(EulerianFrame::Y) == 1UL, "Frame mismatch!");
    static_assert(cast_along(EulerianFrame::Z) == 2UL, "Frame mismatch!");
    /// [cast_along_eg]
  }


}  // namespace