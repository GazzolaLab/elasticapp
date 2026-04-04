//==============================================================================
/*!
//  \file
//  \brief Routine for getting closest point to a line segment
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

//******************************************************************************
// Includes
//******************************************************************************

#include "Utilities/Geometry/GetClosestPointOnLine.hpp"

#include "Utilities/DefineTypes.h"
#include "Utilities/Math/Dist.hpp"
#include "Utilities/Math/Dot.hpp"

namespace elastica {

  namespace geometry {

    //==========================================================================
    //
    //  GEOMETRY FUNCTIONS
    //
    //==========================================================================

    typename GetClosestPointOnLine::return_type
    GetClosestPointOnLine::operator()(const Vec3& l1, const Vec3& l2,
                                      const Vec3& p, Vec3& cp1) const noexcept {
      Vec3 v1 = p - l1;
      real_t d = dist(l1, l2);
      Vec3 v2 = (l2 - l1) / d;

      real_t t = dot(v2, v1);

      if (t <= 0) {
        cp1 = l1;
        return return_type{LEFT};
      }

      if (t >= d) {
        cp1 = l2;
        return return_type{RIGHT};
      }

      cp1 = l1 + v2 * t;
      return return_type{MIDDLE};
    }
    //**************************************************************************

  }  // namespace geometry

}  // namespace elastica
