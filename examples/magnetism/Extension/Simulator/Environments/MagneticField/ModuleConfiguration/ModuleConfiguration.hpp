//==============================================================================
/*!
//  \file
//  \brief User configuration for MagneticField of an \elastica Simulator
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
#include <utility>

#include "Extension/Simulator/Environments/MagneticField/ModuleConfiguration/Types.hpp"
//
#include "Utilities/DefineTypes.h"
#include "Utilities/Math/Vec3.hpp"

namespace elastica {

  namespace configuration {

    //**************************************************************************
    template <typename MagneticFieldProfile_>
    struct MagneticFieldUserConfiguration {
     public:
      //**Type definitions******************************************************
      //! This type
      using MagneticFieldProfile = MagneticFieldProfile_;
      //************************************************************************

      //************************************************************************
      /*!\brief Obtains the force model
       */
      inline auto magnetic_field_profile() const& noexcept
          -> MagneticFieldProfile_ const& {
        return magnetic_profile_;
      }
      //************************************************************************

      //**Member variables******************************************************
      //! The gravity
      MagneticFieldProfile_ magnetic_profile_;
      //************************************************************************
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Idiomatically makes a magnetic field configuration
     * \ingroup config
     *
     * \param magnetic_field_profile Magnetic field profile
     * \return  MagneticFieldUserConfiguration
     */
    template <typename F>
    inline constexpr decltype(auto) make_magnetic_field_configuration(
        F magnetic_field_profile) noexcept {
      return MagneticFieldUserConfiguration<F>{
          std::move(magnetic_field_profile)};
    }
    //**************************************************************************

  }  // namespace configuration

}  // namespace elastica