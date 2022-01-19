//
// Created by Tejaswin Parthasarathy on 1/17/22.
//

#include <cmath>

#include "Framework/TestingFramework.hpp"
#include "Utilities/Geometry/AxisAngleRotation.hpp"
#include "Utilities/Math/Normalize.hpp"
#include "Utilities/Math/Trans.hpp"

namespace {
  using namespace elastica::geometry;

  ELASTICA_TEST_CASE("Unit.Utilities.Geometry.axis_angle",
                     "[Unit][Utilities]") {
    SECTION("Rotation about z-axis") {
      ::elastica::Vec3 axis{0.0, 0.0, 1.0};
      ::elastica::real_t angle{M_PI / 4.0};
      auto R = rotate_via_axis_angle(axis, angle);
      ::elastica::Rot3 expected_R{{std::cos(angle), std::sin(angle), 0.0},
                                  {-std::sin(angle), std::cos(angle), 0.0},
                                  {0.0, 0.0, 1.0}};

      CHECK(R == expected_R);

      CHECK(rotate_via_axis_angle(axis, 45.0, false) == expected_R);
    }
    SECTION("Rotation about y-axis") {
      ::elastica::Vec3 axis{0.0, 1.0, 0.0};
      ::elastica::real_t angle{M_PI / 4.0};
      auto R = rotate_via_axis_angle(axis, angle);
      ::elastica::Rot3 expected_R{{std::cos(angle), 0.0, -std::sin(angle)},
                                  {0.0, 1.0, 0.0},
                                  {std::sin(angle), 0.0, std::cos(angle)}};
      CHECK(R == expected_R);

      CHECK(rotate_via_axis_angle(axis, 45.0, false) == expected_R);
    }
    SECTION("Rotation about x-axis") {
      ::elastica::Vec3 axis{1.0, 0.0, 0.0};
      ::elastica::real_t angle{M_PI / 4.0};
      auto R = rotate_via_axis_angle(axis, angle);
      ::elastica::Rot3 expected_R{{1.0, 0.0, 0.0},
                                  {0.0, std::cos(angle), std::sin(angle)},
                                  {0.0, -std::sin(angle), std::cos(angle)}};
      CHECK(R == expected_R);

      CHECK(rotate_via_axis_angle(axis, 45.0, false) == expected_R);
    }
    SECTION("Rotation preserves symmetry") {
      // A rotation of 120 degrees about x=y=z gives
      // the permutation matrix P
      // {\begin{bmatrix}0&0&1\\1&0&0\\0&1&0\end{bmatrix}}
      // Basically x becomes y, y becomes z, z becomes x
      // For our case then (with directors aligned on the rows,
      // rather than columns)
      // we have
      // {\begin{bmatrix}0&1&0\\0&0&1\\1&0&0\end{bmatrix}}
      ::elastica::real_t length(std::sqrt(3.0));
      ::elastica::Vec3 axis{1.0 / length, 1.0 / length, 1.0 / length};
      ::elastica::real_t angle{2.0 * M_PI / 3.0};

      ::elastica::Rot3 expected_R{
          {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 0.0, 0.0}};

      CHECK(rotate_via_axis_angle(axis, angle) == expected_R);
      CHECK(rotate_via_axis_angle(axis, 120.0, false) == expected_R);
    }
    SECTION("Rotation in 3D") {
      ::elastica::Vec3 axis{1, 3.2, 7.0};
      axis = ::elastica::normalize(axis);
      ::elastica::real_t angle_in_degrees{76.0};

      ::elastica::Rot3 expected_R{{0.254506, -0.834834, 0.488138},
                                  {0.915374, 0.370785, 0.156873},
                                  {-0.311957, 0.406903, 0.858552}};
      // expect to mathch only 5 digits since expected_R is only known
      Approx approx = Approx::custom().epsilon(1e-5);
      CHECK_MATRIX_CUSTOM_APPROX(
          rotate_via_axis_angle(axis, angle_in_degrees, false),
          ::elastica::trans(expected_R), approx);
    }
  }

}  // namespace
