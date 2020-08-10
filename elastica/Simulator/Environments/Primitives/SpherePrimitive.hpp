//=================================================================================================
/*!
 *  \file pe/core/rigidbody/SphereBase.h
 *  \brief Base class for the sphere geometry
 *
 *  Copyright (C) 2009 Klaus Iglberger
 *                2013-2014 Tobias Scharpff
 *
 *  This file is part of pe.
 *
 *  pe is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 *  pe is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with
 * pe. If not, see <http://www.gnu.org/licenses/>.
 */
//=================================================================================================

#pragma once

#include "elastica/ErrorHandling/Assert.hpp"
#include "elastica/Simulator/Environments/CollisionSystem/Types.hpp"
#include "elastica/Simulator/Environments/Primitives/GeomPrimitive.hpp"
#include "elastica/Simulator/Environments/Primitives/Protocols.hpp"
#include "elastica/Simulator/Thresholds.hpp"
#include "elastica/Utilities/DefineTypes.h"
#include "elastica/Utilities/Math/Equal.hpp"
#include "elastica/Utilities/Math/SqrLength.hpp"
#include "elastica/Utilities/Math/Vec3.hpp"
#include "elastica/Utilities/ProtocolHelpers.hpp"

namespace elastica {

  //  class Sphere;

  namespace collision {

    class SpherePrimitive final : public GeomPrimitive,
                                  ::tt::ConformsTo<protocols::ConvexPrimitive> {
     public:
      // TODO Make the non-default constructors private
      explicit SpherePrimitive(id_t sid, id_t uid, bool finite,
                               MaterialID material);
      SpherePrimitive(id_t sid, id_t uid, bool finite, const Vec3& gpos,
                      real_t radius, MaterialID material);
      ~SpherePrimitive() noexcept final;
      SpherePrimitive(SpherePrimitive&&) noexcept;

     public:
      inline real_t get_radius() const noexcept;

      // For fine detection
      inline Vec3 support(const Vec3& d) const;
      inline Vec3 support_contact_threshold(const Vec3& d) const;
      inline void
      calculate_bounding_box();  // Calculation of the axis-aligned bounding box
      template <typename Vec3Convertible>
      inline void transfer(real_t radius, Vec3Convertible const& gpos,
                           Vec3Convertible const& velocity,
                           Vec3Convertible const& omega) noexcept;

     private:
      real_t radius_;

      //      template <typename SphereIterator>
      //      friend void transfer(Sphere const&, SphereIterator,
      //      SphereIterator,
      //                           std::true_type);
      //      template <typename ConstSphereIterator>
      //      friend void transfer(ConstSphereIterator, ConstSphereIterator,
      //      Sphere&,
      //                           std::true_type);
    };

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
    /*!\brief Calculation of the bounding box of the sphere.
     *
     * \return void
     *
     * This function updates the axis-aligned bounding box of the sphere
     * primitive according to the current position and orientation of the
     * sphere. Note that the bounding box is increased in all dimensions by
     * elastica::contact_threshold to guarantee that rigid bodies in close
     * proximity of the sphere are also considered during the collision
     * detection process.
     */
    inline void SpherePrimitive::calculate_bounding_box() {
      const real_t length(radius_ + contact_threshold);

      aabb_[0] = gpos_[0] - length;
      aabb_[1] = gpos_[1] - length;
      aabb_[2] = gpos_[2] - length;
      aabb_[3] = gpos_[0] + length;
      aabb_[4] = gpos_[1] + length;
      aabb_[5] = gpos_[2] + length;

      ELASTICA_ASSERT(aabb_.isValid(), "Invalid bounding box detected");
      ELASTICA_ASSERT(aabb_.contains(gpos_), "Invalid bounding box detected");
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Estimates the point which is farthest in direction \a d.
     *
     * \param d The normalized search direction in world-frame coordinates.
     * \return The support point in world-frame coordinates in direction a\ d.
     */
    inline Vec3 SpherePrimitive::support(const Vec3& d) const {
      ELASTICA_ASSERT(!elastica::equal(elastica::sqrLength(d), 0.0),
                      "Zero length search direction");
      ELASTICA_ASSERT(
          !elastica::equal(std::abs(1.0 - elastica::sqrLength(d)), 0.0),
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
      ELASTICA_ASSERT(
          !elastica::equal(std::abs(1.0 - elastica::sqrLength(d)), 0.0),
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
    /*!\brief Transfers information from the super-ordinate entitry to the
     * primitive
     *
     * TODO Fill in
     */
    template <typename Vec3Like>
    inline void SpherePrimitive::transfer(real_t radius, Vec3Like const& gpos,
                                          Vec3Like const& velocity,
                                          Vec3Like const& omega) noexcept {
      radius_ = radius;

      // Checking the radius
      // Since the sphere transfer is never directly called but only used in
      // a small number of internal functions,
      // only asserts are used here to double check the arguments.
      ELASTICA_ASSERT(radius > real_t(0), "Invalid sphere radius");

      gpos_ = gpos;
      v_ = velocity;
      w_ = omega;

      // Setting the axis-aligned bounding box
      SpherePrimitive::calculate_bounding_box();

        // We can reset the forces and torques to be zero here, but its not SRP
        // so shifted the functionality in the TransferHelper class
    }
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica
