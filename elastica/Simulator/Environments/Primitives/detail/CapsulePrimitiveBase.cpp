//==============================================================================
/*!
//  \file
//  \brief Source file for the CapsulePrimitiveBase
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

//******************************************************************************
// Includes
//******************************************************************************

#include "Simulator/Environments/Primitives/detail/CapsulePrimitiveBase.hpp"

#include "Utilities/MakeSignalingNan.hpp"
#include "Utilities/Math/Orthogonal.hpp"

namespace elastica {

  namespace collision {

    namespace detail {

      //========================================================================
      //
      //  CONSTRUCTOR
      //
      //========================================================================

      //************************************************************************
      /*!\brief Constructor for the CapsulePrimitiveBase class.
       *
       * \param sid Unique system-specific ID for the capsule.
       * \param uid User-specific ID for the capsule.
       * \param gpos Global geometric center of the capsule.
       * \param radius The radius of the capsule \f$ (0..\infty) \f$.
       * \param material The material of the capsule.
       */

      CapsulePrimitiveBase::CapsulePrimitiveBase(id_t sid, id_t uid,
                                                 bool finite,
                                                 MaterialID material)
          :  // Initializing the base object
          GeomPrimitive(finite, sid, uid, material),
#ifdef ELASTICA_DEBUG
          radius_(make_signaling_NaN<real_t>()),  // Radius of the capsule
          length_(make_signaling_NaN<real_t>())   // Length of the capsule
#else
      radius_(0.0),  // Radius of the capsule
            length_(0.0)   // Length of the capsule
#endif
      {
      }
      //************************************************************************

      //**************************************************************************
      /*!\brief Constructor for the CapsulePrimitiveBase class.
       *
       * \param sid Unique system-specific ID for the capsule.
       * \param uid User-specific ID for the capsule.
       * \param gpos Global geometric center of the capsule.
       * \param axis_direction Direction of centerline axis of the capsule.
       * \param radius The radius of the  \f$ (0..\infty) \f$.
       * \param length The length of the capsule \f$ (0..\infty) \f$.
       * \param material The material of the capsule.
       */

      CapsulePrimitiveBase::CapsulePrimitiveBase(id_t sid, id_t uid,
                                                 bool finite, const Vec3& gpos,
                                                 const Vec3& axis_direction,
                                                 real_t radius, real_t length,
                                                 MaterialID material)
          : CapsulePrimitiveBase(
          sid, uid, finite, gpos,
          make_orthogonal_bases_from_tangent(axis_direction), radius,
          length, material) {}
      //**************************************************************************

      //************************************************************************
      /*!\brief Constructor for the CapsulePrimitiveBase class.
       *
       * \param sid Unique system-specific ID for the capsule.
       * \param uid User-specific ID for the capsule.
       * \param gpos Global geometric center of the capsule.
       * \param rot The rotation matrix of the capsule.
       * \param radius The radius of the  \f$ (0..\infty) \f$.
       * \param length The length of the capsule \f$ (0..\infty) \f$.
       * \param material The material of the capsule.
       */

      CapsulePrimitiveBase::CapsulePrimitiveBase(id_t sid, id_t uid,
                                                 bool finite, const Vec3& gpos,
                                                 const Rot3& rot, real_t radius,
                                                 real_t length,
                                                 MaterialID material)
          :  // Initializing the base object
          GeomPrimitive(finite, sid, uid, material),
          //
          radius_(radius),
          length_(length) {
        ELASTICA_ASSERT(radius > real_t(0), "Invalid capsule radius");
        ELASTICA_ASSERT(length > real_t(0), "Invalid capsule radius");

        // Setting the center of the capsule
        gpos_ = gpos;
        Q_ = rot;

        // Setting the axis-aligned bounding box
        CapsulePrimitiveBase::calculate_bounding_box();
      }
      //************************************************************************

      //      CapsulePrimitiveBase::CapsulePrimitiveBase(
      //          CapsulePrimitiveBase&& other) noexcept
      //          : GeomPrimitive(std::move(other)),
      //#ifdef ELASTICA_DEBUG
      //            radius_(make_signaling_NaN<real_t>()),  // Radius of the
      //            capsule length_(make_signaling_NaN<real_t>())   // Length
      //            of the capsule
      //#else
      //            radius_(0.0),  // Radius of the capsule
      //            length_(0.0)   // Length of the capsule
      //#endif
      //      {
      //      }

      //========================================================================
      //
      //  DESTRUCTOR
      //
      //========================================================================

      //************************************************************************
      /*!\brief Destructor for the CapsulePrimitiveBase class.
       */

      CapsulePrimitiveBase::~CapsulePrimitiveBase() noexcept = default;
      //************************************************************************

      //========================================================================
      //
      //  UTILITY FUNCTIONS
      //
      //========================================================================

      //************************************************************************
      /*!\brief Calculation of the bounding box of the capsule.
       *
       * \return void
       *
       * This function updates the axis-aligned bounding box of the capsule
       * primitive according to the current position and orientation of the
       * capsule. Note that the bounding box is increased in all dimensions by
       * elastica::contact_threshold to guarantee that rigid bodies in close
       * proximity of the capsule are also considered during the collision
       * detection process.
       */

      void CapsulePrimitiveBase::calculate_bounding_box() {
        const real_t xlength(std::fabs(Q_(2, 0) * length_) * real_t(0.5) +
                             radius_ + contact_threshold);
        const real_t ylength(std::fabs(Q_(2, 1) * length_) * real_t(0.5) +
                             radius_ + contact_threshold);
        const real_t zlength(std::fabs(Q_(2, 2) * length_) * real_t(0.5) +
                             radius_ + contact_threshold);
        aabb_[0] = gpos_[0] - xlength;
        aabb_[1] = gpos_[1] - ylength;
        aabb_[2] = gpos_[2] - zlength;
        aabb_[3] = gpos_[0] + xlength;
        aabb_[4] = gpos_[1] + ylength;
        aabb_[5] = gpos_[2] + zlength;

        ELASTICA_ASSERT(aabb_.is_valid(), "Invalid bounding box detected");
        ELASTICA_ASSERT(aabb_.contains(gpos_), "Invalid bounding box detected");
      }
      //************************************************************************

    }  // namespace detail

  }  // namespace collision

}  // namespace elastica