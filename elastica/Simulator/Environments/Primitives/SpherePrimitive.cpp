//==============================================================================
/*!
//  \file
//  \brief Source file for SpherePrimitive
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

//******************************************************************************
// Includes
//******************************************************************************

#include "Simulator/Environments/Primitives/SpherePrimitive.hpp"

#include "Utilities/MakeSignalingNan.hpp"

namespace elastica {

  namespace collision {

    //==========================================================================
    //
    //  CONSTRUCTOR
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Constructor for the SpherePrimitive class.
     *
     * \param sid Unique system-specific ID for the sphere.
     * \param uid User-specific ID for the sphere.
     * \param material The material of the sphere.
     */
    SpherePrimitive::SpherePrimitive(id_t sid, id_t uid, bool finite,
                                     MaterialID material)
        :  // Initializing the base object
          GeomPrimitive(finite, sid, uid, material),
#ifdef ELASTICA_DEBUG
          radius_(make_signaling_NaN<real_t>())  // Radius of the sphere
#else
          radius_(0.0)  // Radius of the sphere
#endif
    {
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Constructor for the SpherePrimitive class.
     *
     * \param sid Unique system-specific ID for the sphere.
     * \param uid User-specific ID for the sphere.
     * \param gpos Global geometric center of the sphere.
     * \param radius The radius of the sphere \f$ (0..\infty) \f$.
     * \param material The material of the sphere.
     */
    SpherePrimitive::SpherePrimitive(id_t sid, id_t uid, bool finite,
                                     const Vec3& gpos, real_t radius,
                                     MaterialID material)
        :  // Initializing the base object
          GeomPrimitive(finite, sid, uid, material),
          //
          radius_(radius) {
      ELASTICA_ASSERT(radius > real_t(0), "Invalid sphere radius");

      // Setting the center of the sphere
      gpos_ = gpos;

      // Setting the axis-aligned bounding box
      SpherePrimitive::calculate_bounding_box();
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Constructor for the SpherePrimitive class.
     */
    SpherePrimitive::SpherePrimitive(SpherePrimitive&& other) noexcept
        : GeomPrimitive(std::move(other)),
          radius_(std::move(other.radius_))  // Radius of the sphere
    {}
    //**************************************************************************

    //==========================================================================
    //
    //  DESTRUCTOR
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Destructor for the SpherePrimitive class.
     */
    SpherePrimitive::~SpherePrimitive() noexcept = default;
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
    void SpherePrimitive::calculate_bounding_box() {
      const real_t length(radius_ + contact_threshold);

      aabb_[0] = gpos_[0] - length;
      aabb_[1] = gpos_[1] - length;
      aabb_[2] = gpos_[2] - length;
      aabb_[3] = gpos_[0] + length;
      aabb_[4] = gpos_[1] + length;
      aabb_[5] = gpos_[2] + length;

      ELASTICA_ASSERT(aabb_.is_valid(), "Invalid bounding box detected");
      ELASTICA_ASSERT(aabb_.contains(gpos_), "Invalid bounding box detected");
    }
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica
