//
// Created by Tejaswin Parthasarathy on 6/18/21.
//
#include "Framework/TestingFramework.hpp"
#include "Simulator/Frames.hpp"

namespace {

  // clang-format off
  ELASTICA_TEST_CASE(
  "Unit.Simulator.Frames.EulerianFrame",
  "[Unit][Simulator]") {
    // clang-format on
    EMPTY_TEST();

    static_assert(elastica::EulerianFrame::Dimension == 3UL,
                  "Frame dimension mismatch!");

    /// [cast_along_eg]
    static_assert(cast_along(elastica::EulerianFrame::X) == 0UL,
                  "Frame mismatch!");
    static_assert(cast_along(elastica::EulerianFrame::Y) == 1UL,
                  "Frame mismatch!");
    static_assert(cast_along(elastica::EulerianFrame::Z) == 2UL,
                  "Frame mismatch!");
    /// [cast_along_eg]

    /// [cast_along_template_eg]
    static_assert(elastica::cast_along<elastica::EulerianFrame::X>() ==
                      elastica::cast_along(elastica::EulerianFrame::X),
                  "Frame mismatch!");
    static_assert(elastica::cast_along<elastica::EulerianFrame::Y>() ==
                      elastica::cast_along(elastica::EulerianFrame::Y),
                  "Frame mismatch!");
    static_assert(elastica::cast_along<elastica::EulerianFrame::Z>() ==
                      elastica::cast_along(elastica::EulerianFrame::Z),
                  "Frame mismatch!");
    /// [cast_along_template_eg]

    static_assert(
        elastica::get_unit_vector_along<elastica::EulerianFrame::X>()[0] == 1.0,
        "Frame mismatch!");
    static_assert(
        elastica::get_unit_vector_along<elastica::EulerianFrame::X>()[1] == 0.0,
        "Frame mismatch!");
    static_assert(
        elastica::get_unit_vector_along<elastica::EulerianFrame::X>()[2] == 0.0,
        "Frame mismatch!");

    static_assert(
        elastica::get_unit_vector_along<elastica::EulerianFrame::Y>()[0] == 0.0,
        "Frame mismatch!");
    static_assert(
        elastica::get_unit_vector_along<elastica::EulerianFrame::Y>()[1] == 1.0,
        "Frame mismatch!");
    static_assert(
        elastica::get_unit_vector_along<elastica::EulerianFrame::Y>()[2] == 0.0,
        "Frame mismatch!");

    /// [vector_along_template_eg]
    static_assert(
        elastica::get_unit_vector_along<elastica::EulerianFrame::Z>()[0] == 0.0,
        "Frame mismatch!");
    static_assert(
        elastica::get_unit_vector_along<elastica::EulerianFrame::Z>()[1] == 0.0,
        "Frame mismatch!");
    static_assert(
        elastica::get_unit_vector_along<elastica::EulerianFrame::Z>()[2] == 1.0,
        "Frame mismatch!");
    /// [vector_along_template_eg]

    /// [vector_along_eg]
    static_assert(
        elastica::get_unit_vector_along(elastica::EulerianFrame::Z)[0] == 0.0,
        "Frame mismatch!");
    static_assert(
        elastica::get_unit_vector_along(elastica::EulerianFrame::Z)[1] == 0.0,
        "Frame mismatch!");
    static_assert(
        elastica::get_unit_vector_along(elastica::EulerianFrame::Z)[2] == 1.0,
        "Frame mismatch!");
    /// [vector_along_eg]
  }

}  // namespace