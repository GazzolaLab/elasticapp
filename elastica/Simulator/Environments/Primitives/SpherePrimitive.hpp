//==============================================================================
/*!
//  \file
//  \brief A sphere primitive
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
**                2013-2014 Tobias Scharpff
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

#include "ErrorHandling/Assert.hpp"
///
#include "Simulator/Environments/Primitives/Types.hpp"
//
#include "Simulator/Environments/Primitives/GeomPrimitive.hpp"
#include "Simulator/Environments/Primitives/Protocols.hpp"
///
#include "Utilities/DefineTypes.h"
#include "Utilities/Math/Equal.hpp"
#include "Utilities/Math/Normalize.hpp"
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/Thresholds.hpp"

namespace elastica {

  namespace collision {

    //**************************************************************************
    /*!\brief Sphere geometrical primitive.
     * \ingroup primitives
     *
     * \details
     * The SpherePrimitive represents the geometric primitive sphere.
     *
     * The sphere primitive is the simplest geometric primitive, represented
     * only by its geometric global position and its radius. A special property
     * of the sphere geometry is that in every point of its surface a unique
     * surface normal is defined.
     */
    class SpherePrimitive /*final*/
        : public GeomPrimitive,
          public ::tt::ConformsTo<protocols::ConvexPrimitive> {
     public:
      //**Constructor***********************************************************
      /*!\name Constructor */
      //@{
      // TODO Make the non-default constructors private
      explicit SpherePrimitive(id_t sid, id_t uid, bool finite,
                               MaterialID material);
      SpherePrimitive(id_t sid, id_t uid, bool finite, const Vec3& gpos,
                      real_t radius, MaterialID material);
      SpherePrimitive(SpherePrimitive&&) noexcept;
      //@}
      //************************************************************************

      //**Destructor************************************************************
      /*!\name Destructor */
      //@{
      ~SpherePrimitive() noexcept /*final*/;
      //@}
      //************************************************************************

     public:
      //**Get functions*********************************************************
      /*!\name Get functions */
      //@{
      inline real_t get_radius() const noexcept;
      //@}
      //************************************************************************

      //**Utility functions*****************************************************
      /*!\name Utility functions */
      //@{
      //!< support functions for protocol conformance
      inline Vec3 support(const Vec3& d) const;
      inline Vec3 support_contact_threshold(const Vec3& d) const;
      //!< Calculation of the axis-aligned bounding box
      void calculate_bounding_box();
      //! Updates kinematic state from global position, velocity, and spin.
      template <typename Vec3Convertible>
      inline void transfer(/*real_t radius,*/ Vec3Convertible const& gpos,
                           Vec3Convertible const& velocity,
                           Vec3Convertible const& omega) noexcept;
      //! Updates radius, orientation, and kinematic state.
      template <typename Vec3Convertible, typename Rot3Convertible>
      inline void transfer(real_t radius, Vec3Convertible&& gpos,
                           Rot3Convertible const& rot_mat,
                           Vec3Convertible&& velocity,
                           Vec3Convertible&& omega) noexcept;
      //@}
      //************************************************************************

     protected:
      //**Member variables******************************************************
      /*!\name Member variables */
      //@{
      real_t radius_;
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
    /*!\brief Returns the radius of the sphere.
     *
     * \return The radius of the sphere.
     */
    inline real_t SpherePrimitive::get_radius() const noexcept {
      return radius_;
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
    inline Vec3 SpherePrimitive::support(const Vec3& d) const {
      ELASTICA_ASSERT(!elastica::equal(elastica::sqrLength(d), 0.0),
                      "Zero length search direction");
      ELASTICA_ASSERT(elastica::is_normalized(d),
                      "Search direction is not normalised");
      return gpos_ + radius_ * d;
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
    inline Vec3 SpherePrimitive::support_contact_threshold(
        const Vec3& d) const {
      ELASTICA_ASSERT(!elastica::equal(elastica::sqrLength(d), 0.0),
                      "Zero length search direction");
      ELASTICA_ASSERT(elastica::is_normalized(d),
                      "Search direction is not normalised");
      return gpos_ + d * (radius_ + contact_threshold);
    }
    //**************************************************************************

    // Unsafe function
    // DONE : How much effect does putting a struct here for transfer have?
    // See benchmark gist, relies heavily on optimizer
    // Also pass by value seems to be faster, all these are stack variables
    // DONE : should we template Vec3Like for blaze's overload mechanism to kick
    // in?
    //**************************************************************************
    /*!\brief Transfers kinematic state from the caller into this primitive.
     *
     * \param gpos Global position of the sphere center.
     * \param velocity Linear velocity.
     * \param omega Angular velocity.
     */
    template <typename Vec3Like>
    inline void SpherePrimitive::transfer(
        /*real_t radius, */ Vec3Like const& gpos, Vec3Like const& velocity,
        Vec3Like const& omega) noexcept {
      // radius_ = radius;

      // Checking the radius
      // Since the sphere transfer is never directly called but only used in
      // a small number of internal functions,
      // only asserts are used here to double check the arguments.
      // ELASTICA_ASSERT(radius > real_t(0), "Invalid sphere radius");

      gpos_ = gpos;
      v_ = velocity;
      w_ = omega;

      // Setting the axis-aligned bounding box
      SpherePrimitive::calculate_bounding_box();
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Transfers kinematic state including radius and orientation.
     *
     * \param radius Sphere radius.
     * \param gpos Global position of the sphere center.
     * \param rot_mat Rotation matrix.
     * \param velocity Linear velocity.
     * \param omega Angular velocity.
     */
    template <typename Vec3Like, typename Rot3Like>
    inline void SpherePrimitive::transfer(real_t radius, Vec3Like&& gpos,
                                          Rot3Like const& rot_mat,
                                          Vec3Like&& velocity,
                                          Vec3Like&& omega) noexcept {
      radius_ = radius;
      Q_ = rot_mat;
      transfer(std::forward<Vec3Like>(gpos), std::forward<Vec3Like>(velocity),
               std::forward<Vec3Like>(omega));
    }
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica
