//
// Created by Tejaswin Parthasarathy on 6/18/21.
//
#include "Framework/TestingFramework.hpp"
#include "Simulator/Frames.hpp"

namespace {

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Frames.LagrangianFrame",
                     "[Unit][Simulator]") {
    // clang-format on
    EMPTY_TEST();

    static_assert(elastica::LagrangianFrame::Dimension == 3UL,
                  "Frame dimension mismatch!");

    /// [cast_along_eg]
    static_assert(cast_along(elastica::LagrangianFrame::D1) == 0UL,
                  "Frame mismatch!");
    static_assert(cast_along(elastica::LagrangianFrame::D2) == 1UL,
                  "Frame mismatch!");
    static_assert(cast_along(elastica::LagrangianFrame::D3) == 2UL,
                  "Frame mismatch!");
    static_assert(cast_along(elastica::LagrangianFrame::Normal) == 0UL,
                  "Frame mismatch!");
    static_assert(cast_along(elastica::LagrangianFrame::Binormal) == 1UL,
                  "Frame mismatch!");
    static_assert(cast_along(elastica::LagrangianFrame::Tangent) == 2UL,
                  "Frame mismatch!");
    /// [cast_along_eg]

    /// [cast_along_template_eg]
    static_assert(elastica::cast_along<elastica::LagrangianFrame::Normal>() ==
                      cast_along(elastica::LagrangianFrame::Normal),
                  "Frame mismatch!");
    static_assert(elastica::cast_along<elastica::LagrangianFrame::Binormal>() ==
                      cast_along(elastica::LagrangianFrame::Binormal),
                  "Frame mismatch!");
    static_assert(elastica::cast_along<elastica::LagrangianFrame::Tangent>() ==
                      cast_along(elastica::LagrangianFrame::Tangent),
                  "Frame mismatch!");
    /// [cast_along_template_eg]

    static_assert(elastica::get_unit_vector_along<
                      elastica::LagrangianFrame::Normal>()[0] == 1.0,
                  "Frame mismatch!");
    static_assert(elastica::get_unit_vector_along<
                      elastica::LagrangianFrame::Normal>()[1] == 0.0,
                  "Frame mismatch!");
    static_assert(elastica::get_unit_vector_along<
                      elastica::LagrangianFrame::Normal>()[2] == 0.0,
                  "Frame mismatch!");

    static_assert(elastica::get_unit_vector_along<
                      elastica::LagrangianFrame::Binormal>()[0] == 0.0,
                  "Frame mismatch!");
    static_assert(elastica::get_unit_vector_along<
                      elastica::LagrangianFrame::Binormal>()[1] == 1.0,
                  "Frame mismatch!");
    static_assert(elastica::get_unit_vector_along<
                      elastica::LagrangianFrame::Binormal>()[2] == 0.0,
                  "Frame mismatch!");

    /// [vector_along_template_eg]
    static_assert(elastica::get_unit_vector_along<
                      elastica::LagrangianFrame::Tangent>()[0] == 0.0,
                  "Frame mismatch!");
    static_assert(elastica::get_unit_vector_along<
                      elastica::LagrangianFrame::Tangent>()[1] == 0.0,
                  "Frame mismatch!");
    static_assert(elastica::get_unit_vector_along<
                      elastica::LagrangianFrame::Tangent>()[2] == 1.0,
                  "Frame mismatch!");
    /// [vector_along_template_eg]

    /// [vector_along_eg]
    static_assert(elastica::get_unit_vector_along(
                      elastica::LagrangianFrame::Tangent)[0] == 0.0,
                  "Frame mismatch!");
    static_assert(elastica::get_unit_vector_along(
                      elastica::LagrangianFrame::Tangent)[1] == 0.0,
                  "Frame mismatch!");
    static_assert(elastica::get_unit_vector_along(
                      elastica::LagrangianFrame::Tangent)[2] == 1.0,
                  "Frame mismatch!");
    /// [vector_along_eg]
  }

}  // namespace
