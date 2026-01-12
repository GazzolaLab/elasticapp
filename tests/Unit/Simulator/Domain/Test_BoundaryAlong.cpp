//
// Created by Tejaswin Parthasarathy on 6/17/21.
//
#include "Framework/TestingFramework.hpp"
#include "Simulator/Domain/BoundaryAlong.hpp"
#include "Simulator/Frames.hpp"
#include "Utilities/DefineTypes.h"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"
#include "Utilities/TypeTraits/Cpp20.hpp"

namespace {

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Domain.BoundaryAlong.make_periodic_boundary",
      "[Unit][Simulator][Domain]") {
    // clang-format on

    /// [periodic_eg]
    auto&& x_boundary =
        elastica::boundary_along<elastica::EulerianFrame::X,
                                 elastica::IsPeriodic>(-5.0, 5.0);
    auto&& y_boundary =
        elastica::boundary_along<elastica::EulerianFrame::Y,
                                 elastica::IsPeriodic>(-50.0, 50.0);
    // argument order can be switched, we correct it inside
    auto&& z_boundary =
        elastica::boundary_along<elastica::EulerianFrame::Z,
                                 elastica::IsPeriodic>(50.0, -50.0);
    /// [periodic_eg]

    static_assert(std::is_same_v<std::remove_cvref_t<decltype(x_boundary)>,
                                   elastica::detail::BoundaryAlongWrapper<
                                       elastica::EulerianFrame::X,
                                       elastica::IsPeriodic, elastica::real_t>>,
                  "meta failure");
    CHECK(x_boundary[x_boundary.min_idx] == approx(-5.0));
    CHECK(x_boundary[x_boundary.max_idx] == approx(5.0));

    static_assert(std::is_same_v<std::remove_cvref_t<decltype(y_boundary)>,
                                   elastica::detail::BoundaryAlongWrapper<
                                       elastica::EulerianFrame::Y,
                                       elastica::IsPeriodic, elastica::real_t>>,
                  "meta failure");
    CHECK(y_boundary[y_boundary.min_idx] == approx(-50.0));
    CHECK(y_boundary[y_boundary.max_idx] == approx(50.0));

    static_assert(std::is_same_v<std::remove_cvref_t<decltype(z_boundary)>,
                                   elastica::detail::BoundaryAlongWrapper<
                                       elastica::EulerianFrame::Z,
                                       elastica::IsPeriodic, elastica::real_t>>,
                  "meta failure");
    CHECK(z_boundary[z_boundary.min_idx] == approx(-50.0));
    CHECK(z_boundary[z_boundary.max_idx] == approx(50.0));
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Domain.BoundaryAlong.make_unbounded_boundary",
      "[Unit][Simulator][Domain]") {
    // clang-format on
    EMPTY_TEST();

    /// [unbounded_eg]
    auto&& x_boundary = elastica::boundary_along<elastica::EulerianFrame::X,
                                                 elastica::IsUnbounded>();
    auto&& y_boundary = elastica::boundary_along<elastica::EulerianFrame::Y,
                                                 elastica::IsUnbounded>();
    auto&& z_boundary = elastica::boundary_along<elastica::EulerianFrame::Z,
                                                 elastica::IsUnbounded>();
    /// [unbounded_eg]

    static_assert(
        std::is_same_v<std::remove_cvref_t<decltype(x_boundary)>,
                         elastica::detail::BoundaryAlongWrapper<
                             elastica::EulerianFrame::X, elastica::IsUnbounded,
                             elastica::real_t>>,
        "meta failure");
    static_assert(
        std::is_same_v<std::remove_cvref_t<decltype(y_boundary)>,
                         elastica::detail::BoundaryAlongWrapper<
                             elastica::EulerianFrame::Y, elastica::IsUnbounded,
                             elastica::real_t>>,
        "meta failure");
    static_assert(
        std::is_same_v<std::remove_cvref_t<decltype(z_boundary)>,
                         elastica::detail::BoundaryAlongWrapper<
                             elastica::EulerianFrame::Z, elastica::IsUnbounded,
                             elastica::real_t>>,
        "meta failure");

    elastica::ignore_unused(x_boundary, y_boundary, z_boundary);
  }

}  // namespace