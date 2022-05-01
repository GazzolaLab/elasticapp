//==============================================================================
/*!
//  \file
//  \brief Routine for getting closest line segment between two points
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
#include "Utilities/Math/Vec3.hpp"

namespace elastica {

  namespace geometry {

    //==========================================================================
    //
    //  GEOMETRY FUNCTION DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Implementation details of get_closest_line_segment_points
    // \ingroup geometry
    //
    // \see get_closest_line_segment_points
    */
    constexpr struct GetClosestLineSegmentPoints {
      //**Type definitions******************************************************
      //! Status type
      using status_type = unsigned char;
      //! Aggregated status type
      using return_type = struct {
        status_type first_line_status, second_line_status;
      };
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
      return_type operator()(const Vec3& a1, const Vec3& a2, const Vec3& b1,
                             const Vec3& b2, Vec3& cp1,
                             Vec3& cp2) const noexcept;
      //************************************************************************
    }
    //**************************************************************************
    //**************************************************************************
    /*!\brief Find the closest points on two line segments.
     * \ingroup geometry
     *
     * \param a1 The first end point of the first line segment.
     * \param a2 The second end point of the first line segment.
     * \param b1 The first end point of the second line segment.
     * \param b2 The second end point of the second line segment.
     * \param [out] cp1 The closest point on line one.
     * \param [out] cp2 The closest point on line two.
     * \return GetClosestLineSegmentPoints::return_type indicating status
     *
     * Given the two line segments A and B, this function returns the points
     * that are closest to each other. In the case of parallel lines, where
     * multiple solutions are possible, a solution involving the endpoint of at
     * least one line will be returned. This will also work correctly for zero
     * length lines, e.g. if \a a1 = \a a2 and/or \a b1 = \a b2.
     */
    get_closest_line_segment_points{};
    //**************************************************************************

  }  // namespace geometry

}  // namespace elastica
