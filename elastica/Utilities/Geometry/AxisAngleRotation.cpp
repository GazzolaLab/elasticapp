//==============================================================================
/*!
//  \file
//  \brief Routines for axis angle rotation
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#include "Utilities/Geometry/AxisAngleRotation.hpp"

#include "Utilities/Math/Identity.hpp"
#include "Utilities/Math/Length.hpp"

namespace elastica {

  namespace geometry {

    namespace detail {

      auto axis_angle_rotation_impl_normalized(Vec3 const& normalized_axis,
                                               real_t angle) noexcept -> Rot3 {
        // we perform a transpose here to remain consistent with elastica's
        // definition of Q. So we change the definition of angle to -angle
        const real_t c_angle = std::cos(angle);
        const real_t s_angle = -std::sin(angle);
        const auto I = identity3();

        // the skew symmetric matrix
        constexpr auto zero = static_cast<real_t>(0.0);
        const Rot3 K = {{zero, -normalized_axis[2], normalized_axis[1]},
                        {normalized_axis[2], zero, -normalized_axis[0]},
                        {-normalized_axis[1], normalized_axis[0], zero}};
        return (I + s_angle * K + (static_cast<real_t>(1.0) - c_angle) * K * K);
      }

      auto axis_angle_rotation_impl(Vec3 const& axis, real_t angle) noexcept
          -> Rot3 {
        return axis_angle_rotation_impl_normalized(normalize(axis),
                                                   angle * length(axis));
      }

    }  // namespace detail

  }  // namespace geometry

}  // namespace elastica