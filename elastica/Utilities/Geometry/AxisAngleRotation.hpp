//==============================================================================
/*!
//  \file
//  \brief Routines for axis angle rotation
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include <cmath>
#include <type_traits>

#include "Utilities/DefineTypes.h"
#include "Utilities/Geometry/Angles.hpp"
#include "Utilities/Math/Rot3.hpp"
#include "Utilities/Math/Vec3.hpp"

namespace elastica {

  namespace geometry {

    namespace detail {
      auto axis_angle_rotation_impl_normalized(Vec3 const& normalized_axis,
                                               real_t angle) noexcept -> Rot3;
      auto axis_angle_rotation_impl(Vec3 const& axis, real_t angle) noexcept
          -> Rot3;
    }  // namespace detail

    //**************************************************************************
    /*!\brief Converts angles in degrees to radians
    // \ingroup geometry
    //
    // \param axis Axis of rotation
    // \param angle Angle of rotation
    // \param radians Is angle passed in radians, true if in radians, false
    // otherwise
    // \return Rotation matrix
    */
    inline auto rotate_via_axis_angle(Vec3 const& axis, real_t angle,
                                      bool radians = true) -> Rot3 {
      return detail::axis_angle_rotation_impl(axis,
                                              radians ? angle : deg2rad(angle));
    }
    //**************************************************************************

  }  // namespace geometry

}  // namespace elastica