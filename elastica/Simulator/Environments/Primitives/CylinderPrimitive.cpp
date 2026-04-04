//==============================================================================
/*!
//  \file
//  \brief Source file for CylinderPrimitive
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

#include "Simulator/Environments/Primitives/CylinderPrimitive.hpp"

#include "Utilities/MakeSignalingNan.hpp"
#include "Utilities/Math/Orthogonal.hpp"

namespace elastica {

  namespace collision {

    //==========================================================================
    //
    //  CONSTRUCTOR
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Constructor for the CylinderPrimitive class.
     *
     * \param sid Unique system-specific ID for the cylinder.
     * \param uid User-specific ID for the cylinder.
     * \param finite Whether the primitive participates in collision as finite.
     * \param material The material of the cylinder.
     */
    CylinderPrimitive::CylinderPrimitive(id_t sid, id_t uid, bool finite,
                                         MaterialID material)
        : GeomPrimitive(finite, sid, uid, material),
#ifdef ELASTICA_DEBUG
          radius_(make_signaling_NaN<real_t>()),  // Radius of the cylinder
          length_(make_signaling_NaN<real_t>())   // Length of the cylinder
#else
          radius_(0.0),  // Radius of the cylinder
          length_(0.0)   // Length of the cylinder
#endif
    {
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Constructor for the CylinderPrimitive class.
     *
     * \param sid Unique system-specific ID for the cylinder.
     * \param uid User-specific ID for the cylinder.
     * \param finite Whether the primitive participates in collision as finite.
     * \param gpos Global geometric center of the cylinder.
     * \param axis_direction Direction of centerline axis of the cylinder.
     * \param radius The radius of the  \f$ (0..\infty) \f$.
     * \param length The length of the cylinder \f$ (0..\infty) \f$.
     * \param material The material of the cylinder.
     */
    CylinderPrimitive::CylinderPrimitive(id_t sid, id_t uid, bool finite,
                                         const Vec3& gpos,
                                         const Vec3& axis_direction,
                                         real_t radius, real_t length,
                                         MaterialID material)
        : CylinderPrimitive(sid, uid, finite, gpos,
                            make_orthogonal_bases_from_tangent(axis_direction),
                            radius, length, material) {}
    //**************************************************************************

    //**************************************************************************
    /*!\brief Constructor for the CylinderPrimitive class.
     *
     * \param sid Unique system-specific ID for the cylinder.
     * \param uid User-specific ID for the cylinder.
     * \param finite Whether the primitive participates in collision as finite.
     * \param gpos Global geometric center of the cylinder.
     * \param rot The rotation matrix of the cylinder.
     * \param radius The radius of the  \f$ (0..\infty) \f$.
     * \param length The length of the cylinder \f$ (0..\infty) \f$.
     * \param material The material of the cylinder.
     */
    CylinderPrimitive::CylinderPrimitive(id_t sid, id_t uid, bool finite,
                                         const Vec3& gpos, const Rot3& rot,
                                         real_t radius, real_t length,
                                         MaterialID material)
        : GeomPrimitive(finite, sid, uid, material),
          radius_(radius),
          length_(length) {
      ELASTICA_ASSERT(radius > real_t(0), "Invalid cylinder radius");
      ELASTICA_ASSERT(length > real_t(0), "Invalid cylinder radius");

      // Setting the center of the cylinder
      gpos_ = gpos;
      Q_ = rot;

      // Setting the axis-aligned bounding box
      CylinderPrimitive::calculate_bounding_box();
    }
    //**************************************************************************

    //    CylinderPrimitive::CylinderPrimitive(CylinderPrimitive&& other)
    //    noexcept
    //        : GeomPrimitive(std::move(other)) {}

    //==========================================================================
    //
    //  DESTRUCTOR
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Destructor for the CylinderPrimitive class.
     */
    CylinderPrimitive::~CylinderPrimitive() noexcept = default;
    //**************************************************************************

    //==========================================================================
    //
    //  UTILITY FUNCTIONS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Calculation of the bounding box of the cylinder.
     *
     * \return void
     *
     * This function updates the axis-aligned bounding box of the cylinder
     * primitive according to the current position and orientation of the
     * cylinder. Note that the bounding box is increased in all dimensions by
     * elastica::contact_threshold to guarantee that rigid bodies in close
     * proximity of the cylinder are also considered during the collision
     * detection process.
     */
    void CylinderPrimitive::calculate_bounding_box() {
      using std::fabs;
      // An exact bounding box for a cylinder can be calculated by projecting
      // the center line on the
      // x-, y-, and z-axes and adding the projection of the two cap circles.
      // Knowing the two cylinder end points A and B and the radius r, the
      // bounding box offset from the center of mass is calculated by:
      //
      //   dx = (1/2)*fabs( Q_[6] )*length_ + kx * radius;
      //   dy = (1/2)*fabs( Q_[7] )*length_ + ky * radius;
      //   dz = (1/2)*fabs( Q_[8] )*length_ + kz * radius;
      //
      // where
      //
      //   kx = sqrt( ( (A[1]-B[1])^2 + (A[2]-B[2])^2 ) / ( (A[0]-B[0])^2 +
      //   (A[1]-B[1])^2 + (A[2]-B[2])^2 ) )
      //
      //   ky = sqrt( ( (A[0]-B[0])^2 +
      //   (A[2]-B[2])^2 ) / ( (A[0]-B[0])^2 + (A[1]-B[1])^2 + (A[2]-B[2])^2 ) )
      //
      //   kz = sqrt( ( (A[0]-B[0])^2 + (A[1]-B[1])^2 ) / ( (A[0]-B[0])^2 +
      //   (A[1]-B[1])^2 + (A[2]-B[2])^2 ) )
      //
      // Alternatively, an exact bounding box can be calculated using the
      // rotation matrix:
      //
      //  dx = (1/2)*fabs( Q_[6] )*length_ + sqrt( Q_[0]^2 + Q_[3]^2 )*radius_
      //  dy = (1/2)*fabs( Q_[7] )*length_ + sqrt( Q_[1]^2 + Q_[4]^2 )*radius_
      //  dz = (1/2)*fabs( Q_[8] )*length_ + sqrt( Q_[2]^2 + Q_[5]^2 )*radius_
      //
      // However, in order to avoid the three costly square root calculations
      // (in both formulations), a loosely fitting bounding box is calculated
      // instead by the three following equations:
      //
      //  dx = (1/2)*fabs( Q_[6] )*length_ + fabs( Q_[0] )*radius_ + fabs( Q_[3]
      //  )*radius_
      //  dy = (1/2)*fabs( Q_[7] )*length_ + fabs( Q_[1] )*radius_ + fabs( Q_[4]
      //  )*radius_
      //  dz = (1/2)*fabs( Q_[8] )*length_ + fabs( Q_[2] )*radius_ +
      //  fabs( Q_[5] )*radius_

      constexpr auto binormal = cast_along(LagrangianFrame::Binormal);
      constexpr auto normal = cast_along(LagrangianFrame::Normal);
      constexpr auto tangent = cast_along(LagrangianFrame::Tangent);
      constexpr auto x = cast_along(EulerianFrame::X);
      constexpr auto y = cast_along(EulerianFrame::Y);
      constexpr auto z = cast_along(EulerianFrame::Z);

      const real_t xlength(real_t(0.5) * fabs(Q_(tangent, x)) * length_ +
                           (fabs(Q_(normal, x)) + fabs(Q_(binormal, x))) *
                               radius_ +
                           contact_threshold);
      const real_t ylength(real_t(0.5) * fabs(Q_(tangent, y)) * length_ +
                           (fabs(Q_(normal, y)) + fabs(Q_(binormal, y))) *
                               radius_ +
                           contact_threshold);
      const real_t zlength(real_t(0.5) * fabs(Q_(tangent, z)) * length_ +
                           (fabs(Q_(normal, z)) + fabs(Q_(binormal, z))) *
                               radius_ +
                           contact_threshold);

      aabb_[0] = gpos_[0] - xlength;
      aabb_[1] = gpos_[1] - ylength;
      aabb_[2] = gpos_[2] - zlength;
      aabb_[3] = gpos_[0] + xlength;
      aabb_[4] = gpos_[1] + ylength;
      aabb_[5] = gpos_[2] + zlength;

      ELASTICA_ASSERT(aabb_.is_valid(), "Invalid bounding box detected");
      ELASTICA_ASSERT(aabb_.contains(gpos_), "Invalid bounding box detected");
    }
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica
