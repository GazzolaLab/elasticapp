#pragma once

#include <cmath>

//////// Types first
#include "elastica/Simulator/Environments/CollisionSystem/Types.hpp"
#include "elastica/Simulator/Environments/Primitives/Types.hpp"
#include "elastica/Utilities/DefineTypes.h"
////////

#include "elastica/ErrorHandling/Assert.hpp"
#include "elastica/Simulator/Environments/Primitives/GeomPrimitive.hpp"
#include "elastica/Simulator/Environments/Primitives/Protocols.hpp"
#include "elastica/Simulator/Thresholds.hpp"
#include "elastica/Utilities/Math/Equal.hpp"
#include "elastica/Utilities/Math/SqrLength.hpp"
#include "elastica/Utilities/Math/Vec3.hpp"
#include "elastica/Utilities/ProtocolHelpers.hpp"

namespace elastica {

  namespace collision {

    class ApproximateCylinderPrimitive : public GeomPrimitive {
     public:
      //**Constructor***********************************************************
      /*!\name Constructor */
      //@{
      explicit ApproximateCylinderPrimitive(id_t sid, id_t uid, bool finite,
                                            MaterialID material);
      ApproximateCylinderPrimitive(id_t sid, id_t uid, bool finite,
                                   const Vec3& gpos, real_t radius,
                                   real_t length, MaterialID material);
      ApproximateCylinderPrimitive(ApproximateCylinderPrimitive&&) noexcept;
      //@}
      //************************************************************************

      //**Destructor************************************************************
      /*!\name Destructor */
      //@{
      ~ApproximateCylinderPrimitive() noexcept override;
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

      //!< Calculation of the axis-aligned bounding box
      inline void calculate_bounding_box();

      template <typename Vec3Convertible, typename Rot3Convertible>
      inline void transfer(real_t radius, real_t length,
                           Vec3Convertible const& gpos,
                           Rot3Convertible const& rot_mat,
                           Vec3Convertible const& velocity,
                           Vec3Convertible const& omega) noexcept;

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
    inline real_t ApproximateCylinderPrimitive::get_radius() const noexcept {
      return radius_;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Returns the length of the cylinder
     *
     * \return The length of the cylinder .
     */
    inline real_t ApproximateCylinderPrimitive::get_length() const noexcept {
      return length_;
    }
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
    inline void ApproximateCylinderPrimitive::calculate_bounding_box() {
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
      const real_t xlength(real_t(0.5) * fabs(Q_(2, 0)) * length_ +
                           (fabs(Q_(0, 0)) + fabs(Q_(1, 0))) * radius_ +
                           contact_threshold);
      const real_t ylength(real_t(0.5) * fabs(Q_(2, 1)) * length_ +
                           (fabs(Q_(0, 1)) + fabs(Q_(1, 1))) * radius_ +
                           contact_threshold);
      const real_t zlength(real_t(0.5) * fabs(Q_(2, 2)) * length_ +
                           (fabs(Q_(0, 2)) + fabs(Q_(1, 2))) * radius_ +
                           contact_threshold);

      aabb_[0] = gpos_[0] - xlength;
      aabb_[1] = gpos_[1] - xlength;
      aabb_[2] = gpos_[2] - ylength;
      aabb_[3] = gpos_[0] + ylength;
      aabb_[4] = gpos_[1] + zlength;
      aabb_[5] = gpos_[2] + zlength;

      ELASTICA_ASSERT(aabb_.isValid(), "Invalid bounding box detected");
      ELASTICA_ASSERT(aabb_.contains(gpos_), "Invalid bounding box detected");
    }

    //**************************************************************************
    /*!\brief Transfers information from the super-ordinate entitry to the
     * primitive
     *
     * TODO Fill in
     */
    template <typename Vec3Convertible, typename Rot3Convertible>
    void ApproximateCylinderPrimitive::transfer(real_t radius, real_t length,
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

      gpos_ = gpos;
      Q_ = rot_mat;
      v_ = velocity;
      w_ = omega;

      // Setting the axis-aligned bounding box
      ApproximateCylinderPrimitive::calculate_bounding_box();

      // We can reset the forces and torques to be zero here, but its not SRP
      // so shifted the functionality in the TransferHelper class
    }
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica
