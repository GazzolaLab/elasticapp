//==============================================================================
/*!
//  \file
//  \brief Routines for converting degrees to radians and vice-versa
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
#include <type_traits>

#include "Utilities/Requires.hpp"

namespace elastica {

  namespace geometry {

    //==========================================================================
    //
    //  GEOMETRY FUNCTION DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Implementation details of deg2rad
    // \ingroup geometry
    //
    // \see deg2rad
    */
    constexpr struct Deg2Rad {
      //**Static variables******************************************************
      //! long!
      static constexpr long double m_pi_over_180 =
          3.1415926535897932384626433832795 / 180.0;
      //************************************************************************

      //************************************************************************
      /*! Geometry function */
      template <typename T,
                Requires<std::is_floating_point<T>::value> = nullptr>
      inline T operator()(T degree_angle) const noexcept {
        return degree_angle * static_cast<T>(m_pi_over_180);
      }
      //************************************************************************
    }
    //**************************************************************************
    //**************************************************************************
    /*!\brief Converts angles in degrees to radians
    // \ingroup geometry
    //
    // \param degree_angle Angles in degrees
    // \return angles in radians
    */
    deg2rad{};
    //**************************************************************************

    constexpr struct Rad2Deg {
      //**Static variables******************************************************
      //! long!
      static constexpr long double m_180_over_pi =
          180.0 / 3.1415926535897932384626433832795;
      //************************************************************************

      //************************************************************************
      /*! Geometry function */
      template <typename T,
                Requires<std::is_floating_point<T>::value> = nullptr>
      inline T operator()(T radian_angle) const noexcept{
        return radian_angle * static_cast<T>(m_180_over_pi);
      }
      //************************************************************************
    }
    //**************************************************************************
    //**************************************************************************
    /*!\brief Converts angles in radians to degrees
    // \ingroup geometry
    //
    // \param radian_angle Angles in radians
    // \return angles in degrees
    */
    rad2deg{};
    //**************************************************************************

  }  // namespace geometry

}  // namespace elastica