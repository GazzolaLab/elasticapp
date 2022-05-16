//==============================================================================
/*!
//  \file
//  \brief An oscillatory magnetic field (vector)
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy, Yashraj Bhosale - All Rights
Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include <elastica/Time/SimulationTime.hpp>
#include <elastica/Utilities/Math/Vec3.hpp>

namespace elastica {

  namespace environments {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief A spatially-invariant magnetic field vector oscillating in time.
     * \ingroup extension_environments
     */
    struct OscillatoryMagneticField {
      //**Constructor***********************************************************
      /*!\name Constructor */
      //@{
      OscillatoryMagneticField()
          : field_amplitude_(::elastica::Vec3{0.0, 0., 0.0}),
            field_angular_frequency_(::elastica::Vec3{0., 0., 0.}),
            field_phase_difference_(::elastica::Vec3{0., 0., 0.}){};
      OscillatoryMagneticField(const ::elastica::Vec3& field_amplitude,
                               const ::elastica::Vec3& field_angular_frequency,
                               const ::elastica::Vec3& field_phase_difference)
          : field_amplitude_(field_amplitude),
            field_angular_frequency_(field_angular_frequency),
            field_phase_difference_(field_phase_difference){};
      //@}
      //************************************************************************

      //**Destructor************************************************************
      /*!\name Destructor */
      //@{
      ~OscillatoryMagneticField() = default;
      //@}
      //************************************************************************

      //**Access operators******************************************************
      /*!\name Access operators */
      //@{
      inline auto field_amplitude() & noexcept -> ::elastica::Vec3& {
        return field_amplitude_;
      }
      inline auto field_angular_frequency() & noexcept -> ::elastica::Vec3& {
        return field_angular_frequency_;
      }
      inline auto field_phase_difference() & noexcept -> ::elastica::Vec3& {
        return field_phase_difference_;
      }

      //@}
      //************************************************************************

      //**Function methods******************************************************
      /*!\name Function methods */
      //@{
      inline auto operator()(::elastica::Time time_v) const noexcept {
        return field_amplitude_ * blaze::sin(time_v * field_angular_frequency_ +
                                             field_phase_difference_);
      }
      //@}
      //************************************************************************

     private:
      //**Member variables******************************************************
      /*!\name Member variables */
      //@{
      //! Magnetic field amplitude
      ::elastica::Vec3 field_amplitude_;
      //! Magnetic field angular frequency
      ::elastica::Vec3 field_angular_frequency_;
      //! Magnetic field phase difference
      ::elastica::Vec3 field_phase_difference_;
      //@}
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace environments

}  // namespace elastica
