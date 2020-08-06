
#pragma once
#include <cmath>
#include <type_traits>

#include "elastica/ErrorHandling/Assert.hpp"
#include "elastica/Utilities/DefineTypes.h"
#include "elastica/Utilities/Math/Rot3.hpp"
#include "elastica/Utilities/Math/SqrLength.hpp"
#include "elastica/Utilities/Math/Normalize.hpp"
#include "elastica/Utilities/Math/Vec3.hpp"

namespace elastica {

  Rot3 rotation_from_axis_angle(const Vec3& axis, real_t angle) {
    //    static_assert(std::is_floating_point<Axis>::value, "Not floating!");

    ELASTICA_ASSERT((sqrLength(axis) > real_t(0) or angle == real_t(0)),
                    "Invalid rotation parameters");

    const real_t sina(std::sin(angle));
    const real_t cosa(std::cos(angle));
    const real_t tmp(real_t(1) - cosa);

    normalize(axis);

    Rot3 rot_;
    rot_(0, 0) = cosa + axis[0] * axis[0] * tmp;
    rot_(0, 1) = axis[0] * axis[1] * tmp - axis[2] * sina;
    rot_(0, 2) = axis[0] * axis[2] * tmp + axis[1] * sina;
    rot_(1, 0) = axis[1] * axis[0] * tmp + axis[2] * sina;
    rot_(1, 1) = cosa + axis[1] * axis[1] * tmp;
    rot_(1, 2) = axis[1] * axis[2] * tmp - axis[0] * sina;
    rot_(2, 0) = axis[2] * axis[0] * tmp - axis[1] * sina;
    rot_(2, 1) = axis[2] * axis[1] * tmp + axis[0] * sina;
    rot_(2, 2) = cosa + axis[2] * axis[2] * tmp;

    return rot_;
  }
}  // namespace elastica