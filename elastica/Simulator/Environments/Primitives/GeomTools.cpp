//==============================================================================
/*!
 *  \file src/core/GeomTools.cpp
 *  \brief Source file for geometrical utility functions
 *
 *  Copyright (C) 2009 Klaus Iglberger
 *
 *  This file is part of pe.
 *
 *  pe is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 *  pe is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with
 * pe. If not, see <http://www.gnu.org/licenses/>.
 */
//==============================================================================

//******************************************************************************
// Includes
//******************************************************************************

#include "Simulator/Environments/Primitives/GeomTools.hpp"

#include <limits>

#include "ErrorHandling/Assert.hpp"
#include "Utilities/Math/Dot.hpp"

namespace elastica {
  //============================================================================
  //
  //  GEOMETRY FUNCTIONS
  //
  //============================================================================

  //****************************************************************************
  /*!\brief Find the closest points on two line segments.
   * \ingroup geometry
   *
   * \param a1 The first end point of the first line segment.
   * \param a2 The second end point of the first line segment.
   * \param b1 The first end point of the second line segment.
   * \param b2 The second end point of the second line segment.
   * \param [out] cp1 The closest point on line one.
   * \param [out] cp2 The closest point on line two.
   * \return void
   *
   * Given the two line segments A and B, this function returns the points that
   * are closest to each other. In the case of parallel lines, where multiple
   * solutions are possible, a solution involving the endpoint of at least one
   * line will be returned. This will also work correctly for zero length lines,
   * e.g. if \a a1 = \a a2 and/or \a b1 = \a b2.
   */
  GeomTools::ClosestLineSegmentPointsReturnType
  GeomTools::get_closest_line_segment_points(const Vec3& a1, const Vec3& a2,
                                             const Vec3& b1, const Vec3& b2,
                                             Vec3& cp1, Vec3& cp2) {
    //----- Checking the vertex-vertex features -----

    const Vec3 a1a2(a2 - a1);
    const Vec3 b1b2(b2 - b1);
    const Vec3 a1b1(b1 - a1);
    const real_t da1(dot(a1a2, a1b1));
    const real_t db1(dot(b1b2, a1b1));
    if (da1 <= real_t(0) and db1 >= real_t(0)) {
      cp1 = a1;
      cp2 = b1;
      return ClosestLineSegmentPointsReturnType{LEFT, LEFT};
    }

    const Vec3 a1b2(b2 - a1);
    const real_t da2(dot(a1a2, a1b2));
    const real_t db2(dot(b1b2, a1b2));
    if (da2 <= real_t(0) and db2 <= real_t(0)) {
      cp1 = a1;
      cp2 = b2;
      return ClosestLineSegmentPointsReturnType{LEFT, RIGHT};
    }

    const Vec3 a2b1(b1 - a2);
    const real_t da3(dot(a1a2, a2b1));
    const real_t db3(dot(b1b2, a2b1));
    if (da3 >= real_t(0) && db3 >= real_t(0)) {
      cp1 = a2;
      cp2 = b1;
      return ClosestLineSegmentPointsReturnType{RIGHT, LEFT};
    }

    const Vec3 a2b2(b2 - a2);
    const real_t da4(dot(a1a2, a2b2));
    const real_t db4(dot(b1b2, a2b2));
    if (da4 >= real_t(0) && db4 <= real_t(0)) {
      cp1 = a2;
      cp2 = b2;
      return ClosestLineSegmentPointsReturnType{RIGHT, RIGHT};
    }

    //----- Checking the edge-vertex features -----
    // If one or both of the line segments have zero length, we will never get
    // here. Therefore we don't have to worry about possible divisions by zero
    // in the following calculations.

    // n => normal, k => projection of a_i b_j on a_1a_2 or b_1b_2
    Vec3 n, k;

    const real_t la(dot(a1a2, a1a2));
    if (da1 >= real_t(0) && da3 <= real_t(0)) {
      k = (da1 / la) * a1a2;
      n = a1b1 - k;
      if (dot(b1b2, n) >= real_t(0)) {
        cp1 = a1 + k;
        cp2 = b1;
        return ClosestLineSegmentPointsReturnType{MIDDLE, LEFT};
      }
    }

    if (da2 >= real_t(0) && da4 <= real_t(0)) {
      k = (da2 / la) * a1a2;
      n = a1b2 - k;
      if (dot(b1b2, n) <= real_t(0)) {
        cp1 = a1 + k;
        cp2 = b2;
        return ClosestLineSegmentPointsReturnType{MIDDLE, RIGHT};
      }
    }

    const real_t lb(dot(b1b2, b1b2));
    if (db1 <= real_t(0) && db2 >= real_t(0)) {
      k = (-db1 / lb) * b1b2;
      n = -a1b1 - k;
      if (dot(a1a2, n) >= real_t(0)) {
        cp1 = a1;
        cp2 = b1 + k;
        return ClosestLineSegmentPointsReturnType{LEFT, MIDDLE};
      }
    }

    if (db3 <= real_t(0) && db4 >= real_t(0)) {
      k = (-db3 / lb) * b1b2;
      n = -a2b1 - k;
      if (dot(a1a2, n) <= real_t(0)) {
        cp1 = a2;
        cp2 = b1 + k;
        return ClosestLineSegmentPointsReturnType{RIGHT, MIDDLE};
      }
    }

    //----- Checking the edge-edge features -----
    // Assuming the two lines are a_1 + s (a_2 - a_1)
    // and b_1 + t * (b_2 - b_1) with 0 <= s, t <= 1
    // guaranteed, find the values of s and t

    const real_t scale(dot(a1a2, b1b2));
    real_t div(la * lb - scale * scale);

    ELASTICA_ASSERT(div > real_t(0.0), "Invalid division");
    div = real_t(1.0) / div;

    const real_t s((lb * da1 - scale * db1) * div);
    const real_t t((scale * da1 - la * db1) * div);

    cp1 = a1 + s * a1a2;
    cp2 = b1 + t * b1b2;
    return ClosestLineSegmentPointsReturnType{MIDDLE, MIDDLE};
  }
  //****************************************************************************
}  // namespace elastica