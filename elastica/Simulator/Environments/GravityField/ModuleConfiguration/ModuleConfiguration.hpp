//==============================================================================
/*!
//  \file
//  \brief User configuration for GravityField of an \elastica Simulator
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

#include "Simulator/Environments/CollisionSystem/ModuleConfiguration/Types.hpp"
//
#include "Utilities/DefineTypes.h"
#include "Utilities/Math/Vec3.hpp"

namespace elastica {

  namespace configuration {

    //**************************************************************************
    struct GravityFieldUserConfiguration {
     public:
      //************************************************************************
      /*!\brief Obtains the force model
       */
      inline auto gravity() const& noexcept -> ::elastica::Vec3 const& {
        return gravity_;
      }
      //************************************************************************

      //**Member variables******************************************************
      //! The gravity
      ::elastica::Vec3 gravity_;
      //************************************************************************
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Idiomatically makes a gravity configuration
     * \ingroup config
     *
     * \param g Gravity vector
     * \return  GravityFieldUserConfiguration
     */
    inline constexpr decltype(auto) make_gravity_configuration(
        ::elastica::Vec3 g) noexcept {
      return GravityFieldUserConfiguration{std::move(g)};
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Idiomatically makes a gravity configuration
     * \ingroup config
     *
     * \param x x component of gravity
     * \param y y component of gravity
     * \param z z component of gravity
     * \return  GravityFieldUserConfiguration
     */
    inline constexpr decltype(auto) make_gravity_configuration(
        real_t x, real_t y, real_t z) noexcept {
      return make_gravity_configuration(::elastica::Vec3{x, y, z});
    }
    //**************************************************************************

  }  // namespace configuration

}  // namespace elastica