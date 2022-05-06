//==============================================================================
/*!
//  \file
//  \brief A constant magnetic field (vector)
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
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
#include <elastica/Options/Options.hpp>
#include <elastica/Options/Parameter.hpp>
#include <elastica/Time/SimulationTime.hpp>
#include <elastica/Utilities/Math/Vec3.hpp>

#include "RampedAdapter.hpp"

namespace elastica {

  namespace environments {

    namespace detail {

      struct ConstantMagneticFieldParameter
          : public Options::Parameter<::elastica::Vec3> {
        using P = Options::Parameter<::elastica::Vec3>;
        using P::P;
        using P::value;
        static constexpr Options::String help = {
            "Constant Magnetic field value"};

        // also add a ref value
        inline auto value() & -> type& { return t_; }
      };

    }  // namespace detail

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief A spatio-temporally-invariant magnetic field vector
     * \ingroup extension_environments
     */
    struct ConstantMagneticField {
      //**Constructor***********************************************************
      /*!\name Constructor */
      //@{
      ConstantMagneticField() : field_(::elastica::Vec3{0.1, 0., 0.1}){};
      ConstantMagneticField(::elastica::Vec3 field) : field_(field){};
      //@}
      //************************************************************************

      //**Destructor************************************************************
      /*!\name Destructor */
      //@{
      ~ConstantMagneticField() = default;
      //@}
      //************************************************************************

      //**Access operators******************************************************
      /*!\name Access operators */
      //@{
      inline auto field_vector() & noexcept -> ::elastica::Vec3& {
        return field_;
      }
      inline auto field_vector() const& noexcept -> ::elastica::Vec3 const& {
        return field_;
      }
      //@}
      //************************************************************************

      //**Function methods******************************************************
      /*!\name Function methods */
      //@{
      inline auto operator()(::elastica::Time /**/) const noexcept
          -> ::elastica::Vec3 const& {
        return field_vector();
      }
      //@}
      //************************************************************************

     private:
      //**Member variables******************************************************
      /*!\name Member variables */
      //@{
      //! Magnetic field vector
      ::elastica::Vec3 field_;
      //@}
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace environments

}  // namespace elastica
