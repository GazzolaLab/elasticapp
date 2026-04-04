//==============================================================================
/*!
//  \file
//  \brief A cylinder primitive
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
//
//  This file contains implementation from the pe library with the following
//  copyrights
**
**  Project home: https://www.cs10.tf.fau.de/research/software/pe/
**
**  Copyright (C) 2009 Klaus Iglberger
**
**  This file is part of pe.
**
**  pe is free software: you can redistribute it and/or modify it under the
** terms of the GNU General Public License as published by the Free Software
** Foundation, either version 3 of the License, or (at your option) any later
** version.
**
**  pe is distributed in the hope that it will be useful, but WITHOUT ANY
** WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
** A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License along with
** pe. If not, see <http://www.gnu.org/licenses/>.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <cmath>

//////// Types first
#include "Simulator/Environments/Primitives/Types.hpp"
#include "Utilities/DefineTypes.h"
////////

#include "ErrorHandling/Assert.hpp"
#include "Simulator/Environments/Primitives/GeomPrimitive.hpp"
#include "Simulator/Environments/Primitives/Protocols.hpp"
#include "Simulator/Frames.hpp"
#include "Utilities/Math/Equal.hpp"
#include "Utilities/Math/Normalize.hpp"
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/Thresholds.hpp"

namespace elastica {

  namespace collision {

    //**************************************************************************
    /*!\brief Cylinder geometrical primitive.
     * \ingroup primitives
     *
     * \details
     * The CylinderPrimitive class represents the geometric primitive cylinder,
     * which is one of the basic geometric primitives of \elastica.
     *
     * In order to setup a cylinder, it is only necessary to specify its radius
     * (the thickness) and its length. A cylinder is created axis-aligned with
     * the x-axis (the length of the cylinder is parallel to the x-axis).
     */
    class CylinderPrimitive /*final*/
        : public GeomPrimitive,
          public ::tt::ConformsTo<protocols::ConvexPrimitive> {
     public:
      //**Constructor***********************************************************
      /*!\name Constructor */
      //@{
      // Note : these are all purely forwarding constructors
      explicit CylinderPrimitive(id_t sid, id_t uid, bool finite,
                                 MaterialID material);
      CylinderPrimitive(id_t sid, id_t uid, bool finite, const Vec3& gpos,
                        const Vec3& axis_direction, real_t radius,
                        real_t length, MaterialID material);
      CylinderPrimitive(id_t sid, id_t uid, bool finite, const Vec3& gpos,
                        const Rot3& rot, real_t radius, real_t length,
                        MaterialID material);
      //! Move constructor (defaulted).
      CylinderPrimitive(CylinderPrimitive&&) noexcept = default;
      //@}
      //************************************************************************

      //**Destructor************************************************************
      /*!\name Destructor */
      //@{
      ~CylinderPrimitive() noexcept /*override*/;
      //@}
      //************************************************************************

     public:
      //**Get functions*********************************************************
      /*!\name Get functions */
      //@{
      inline real_t get_radius() const noexcept;
      inline real_t get_length() const noexcept;
      //@}
      //************************************************************************

     public:
      //**Utility functions*****************************************************
      /*!\name Utility functions */
      //@{
      //!< support functions for protocol conformance
      inline Vec3 support(const Vec3& d) const;
      inline Vec3 support_contact_threshold(const Vec3& d) const;
      //!< Calculation of the axis-aligned bounding box
      void calculate_bounding_box();
      template <typename Vec3Convertible, typename Rot3Convertible>
      inline void transfer(/*real_t radius, real_t length,*/
                           Vec3Convertible const& gpos,
                           Rot3Convertible const& rot_mat,
                           Vec3Convertible const& velocity,
                           Vec3Convertible const& omega) noexcept;
      //@}
      //************************************************************************

     protected:
      //**Member variables******************************************************
      /*!\name Member variables */
      //@{
      real_t radius_;  //!< The radius of the cylinder
      real_t length_;  //!< The length of the cylinder
      //@}
      //************************************************************************
    };
    //**************************************************************************

    //==========================================================================
    //
    //  GET FUNCTIONS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Returns the radius of the cylinder.
     *
     * \return The radius of the cylinder.
     */
    inline real_t CylinderPrimitive::get_radius() const noexcept {
      return radius_;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Returns the length of the cylinder
     *
     * \return The length of the cylinder .
     */
    inline real_t CylinderPrimitive::get_length() const noexcept {
      return length_;
    }
    //**************************************************************************

    //==========================================================================
    //
    //  UTILITY FUNCTIONS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Estimates the point which is farthest in direction \a d.
     *
     * \param d The normalized search direction in world-frame coordinates.
     * \return The support point in world-frame coordinates in direction a\ d.
     */
    inline Vec3 CylinderPrimitive::support(const Vec3& d) const {
      ELASTICA_ASSERT(!elastica::equal(elastica::sqrLength(d), 0.0),
                      "Zero length search direction");
      ELASTICA_ASSERT(elastica::is_normalized(d),
                      "Search direction is not normalised");

      const Vec3 bfD(
          from_world_FOR_to_body_FOR(d));  // d in body frame coordinates

      constexpr auto d1 = cast_along(LagrangianFrame::D1);
      constexpr auto d2 = cast_along(LagrangianFrame::D2);
      constexpr auto tangent = cast_along(LagrangianFrame::Tangent);

      const Vec3 support_segment =
          Vec3{0.0, 0.0,
               // sign (u, v)
               ((real_t(0.0) < bfD[tangent]) - (bfD[tangent] < real_t(0.0))) *
                   length_ * 0.5};

      const Vec3 support_disc =
          radius_ * normalize(Vec3{bfD[d1], bfD[d2], 0.0});

      return gpos_ + from_body_FOR_to_world_FOR(support_segment + support_disc);
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Estimates the point which is farthest in direction \a d.
     *
     * \param d The normalized search direction in world-frame coordinates
     * \return The support point in world-frame coordinates in direction a\ d
     * extended by a vector in direction \a d of length \a
     * elastica::contact_threshold.
     */
    inline Vec3 CylinderPrimitive::support_contact_threshold(
        const Vec3& d) const {
      ELASTICA_ASSERT(!elastica::equal(elastica::sqrLength(d), 0.0),
                      "Zero length search direction");
      ELASTICA_ASSERT(elastica::is_normalized(d),
                      "Search direction is not normalised");
      return support(d) + d * contact_threshold;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Transfers information from the super-ordinate entity to the
     * primitive
     *
     * TODO Fill in
     */
    template <typename Vec3Convertible, typename Rot3Convertible>
    inline void CylinderPrimitive::transfer(
        /*real_t radius, real_t length,*/ Vec3Convertible const& gpos,
        Rot3Convertible const& rot_mat, Vec3Convertible const& velocity,
        Vec3Convertible const& omega) noexcept {
      //      radius_ = radius;
      //      length_ = length;
      //
      //      // Checking the radius
      //      // Since the sphere transfer is never directly called but only
      //      used in
      //      // a small number of internal functions,
      //      // only asserts are used here to double check the arguments.
      //      ELASTICA_ASSERT(radius > real_t(0), "Invalid cylinder radius");
      //      ELASTICA_ASSERT(length > real_t(0), "Invalid cylinder length");

      gpos_ = gpos;
      Q_ = rot_mat;
      v_ = velocity;
      w_ = omega;

      // Setting the axis-aligned bounding box
      CylinderPrimitive::calculate_bounding_box();

      // We can reset the forces and torques to be zero here, but its not SRP
      // so shifted the functionality in the TransferHelper class
    }
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica
