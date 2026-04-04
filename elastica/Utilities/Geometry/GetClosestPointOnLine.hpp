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

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include "Utilities/Math/Vec3.hpp"

namespace elastica {

  namespace geometry {

    //==========================================================================
    //
    //  GEOMETRY FUNCTION DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Implementation details of get_closest_point_on_line
    // \ingroup geometry
    //
    // \see get_closest_point_on_line
    */
    constexpr struct GetClosestPointOnLine {
      //**Type definitions******************************************************
      //! Status type
      using status_type = unsigned char;
      //! Aggregated status type
      using return_type = status_type;
      //************************************************************************

      //**Static variables******************************************************
      // Can wrap in an enum, but makes it more harder at the call site
      /*! \name Status variables */
      // @{
      static constexpr status_type LEFT = 0;
      static constexpr status_type RIGHT = 1;
      static constexpr status_type MIDDLE = 2;
      //@}
      //************************************************************************

      //************************************************************************
      /*! Geometry function */
      return_type operator()(const Vec3& l1, const Vec3& l2, const Vec3& p,
                             Vec3& cp1) const noexcept;
      //************************************************************************
    }
    //**************************************************************************
    //**************************************************************************
    /*!\brief Find the point closest to the given point on a line.
    // \ingroup geometry
    //
    // \param l1 The first end point of the line segment.
    // \param l2 The second end point of the line segment.
    // \param p The first end point of the second line segment.
    // \param [out] cp1 The closest point on line l1-l2.
    // \return GetClosestLineSegmentPoints::return_type indicating status
    //
    // Given the line segment A marked by points l1 and l2, and a point p,
    // this function returns the point closest to the line segment. This will
    // also work correctly for zero length line, e.g. if \a l1 = \a l2.
    */
    get_closest_point_on_line{};
    //**************************************************************************

  }  // namespace geometry

}  // namespace elastica