//
// Created by Tejaswin Parthasarathy on 6/18/21.
//
#include "Framework/TestingFramework.hpp"
#include "Simulator/Frames/LagrangianFrame.hpp"

namespace {

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Frames.LagrangianFrame",
                     "[Unit][Simulator]") {
    // clang-format on
    using namespace elastica::detail;
    static_assert(LagrangianFrame::Dimension == 3UL,
                  "Frame dimension mismatch!");

    /// [cast_along_eg]
    static_assert(cast_along(LagrangianFrame::D1) == 0UL, "Frame mismatch!");
    static_assert(cast_along(LagrangianFrame::D2) == 1UL, "Frame mismatch!");
    static_assert(cast_along(LagrangianFrame::D3) == 2UL, "Frame mismatch!");
    /// [cast_along_eg]
  }

}  // namespace
