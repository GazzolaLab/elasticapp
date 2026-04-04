//==============================================================================
/*!
//  \file
//  \brief A capsule primitive for independent, rigid bodies
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

//////// Types first
#include "Simulator/Environments/Primitives/Types.hpp"
#include "Utilities/DefineTypes.h"
////////

#include "Simulator/Environments/Primitives/detail/CapsulePrimitiveBase.hpp"

namespace elastica {

  namespace collision {

    //**************************************************************************
    /*!\brief Capsule geometrical primitive.
     * \ingroup primitives
     *
     * \details
     * The Capsule class represents the geometric primitive capsule, which is
     * one of the basic geometric primitives of \elastica.
     *
     * A capsule is the combination of a cylinder and two hemisphere caps at
     * both ends of the cylinder. This combination allows to calculate a unique
     * normal on each point of the capsule's surface. In order to setup a
     * capsule, the two values radius and length are required: radius specifies
     * the radius of both the cylinder part and the two hemispheres, length is
     * the length of the cylinder part. A capsule is created axis-aligned with
     * the x-axis (the length of the cylinder part is parallel to the x-axis).
     */
    class CapsulePrimitive : public detail::CapsulePrimitiveBase {
     public:
      //**Constructor***********************************************************
      /*!\name Constructor */
      //@{
      using CapsulePrimitiveBase::CapsulePrimitiveBase;
      CapsulePrimitive(CapsulePrimitive&&) noexcept = default;
      //@}
      //************************************************************************

      //**Destructor************************************************************
      /*!\name Destructor */
      //@{
      ~CapsulePrimitive() noexcept /*override*/;
      //@}
      //************************************************************************

      //**Utility functions*****************************************************
      /*!\name Utility functions */
      //@{
      template <typename Vec3Convertible, typename Rot3Convertible>
      inline void transfer(Vec3Convertible const& gpos,
                           Rot3Convertible const& rot_mat,
                           Vec3Convertible const& velocity,
                           Vec3Convertible const& omega) noexcept {
        CapsulePrimitiveBase::transfer_impl(gpos, rot_mat, velocity, omega);
      }
      //@}
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica
