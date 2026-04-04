//==============================================================================
/*!
//  \file
//  \brief A capsule primitive for elements from rods
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

#include "ErrorHandling/Assert.hpp"
#include "Simulator/Environments/Primitives/detail/CapsulePrimitiveBase.hpp"

namespace elastica {

  namespace collision {

    //**************************************************************************
    /*!\brief Capsule geometry from rods
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
    class CapsuleFromRodPrimitive : public detail::CapsulePrimitiveBase {
     public:
      //**Constructor***********************************************************
      /*!\name Constructor */
      //@{
      using CapsulePrimitiveBase::CapsulePrimitiveBase;
      CapsuleFromRodPrimitive(CapsuleFromRodPrimitive&&) noexcept = default;
      //@}
      //************************************************************************

      //**Destructor************************************************************
      /*!\name Destructor */
      //@{
      ~CapsuleFromRodPrimitive() noexcept /*override*/;
      //@}
      //************************************************************************

      //**Utility functions*****************************************************
      /*!\name Utility functions */
      //@{
      template <typename Vec3Convertible, typename Rot3Convertible>
      inline void transfer(real_t radius, real_t length,
                           Vec3Convertible const& gpos,
                           Rot3Convertible const& rot_mat,
                           Vec3Convertible const& velocity,
                           Vec3Convertible const& omega) noexcept {
        radius_ = radius;
        length_ = length;

        // Checking the radius
        // Since the sphere transfer is never directly called but only used in
        // a small number of internal functions,
        // only asserts are used here to double check the arguments.
        ELASTICA_ASSERT(radius > real_t(0), "Invalid cylinder radius");
        ELASTICA_ASSERT(length > real_t(0), "Invalid cylinder length");

        CapsulePrimitiveBase::transfer_impl(gpos, rot_mat, velocity, omega);
      }
      //@}
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica