#include "PlanePrimitive.hpp"

#include "elastica/ErrorHandling/Assert.hpp"
#include "elastica/Simulator/Thresholds.hpp"
#include "elastica/Utilities/Logging.hpp"
#include "elastica/Utilities/MakeArray.hpp"
#include "elastica/Utilities/MakeSignalingNan.hpp"
#include "elastica/Utilities/Math/Infinity.hpp"
#include "elastica/Utilities/Math/Orthogonal.hpp"

#include <iostream>

namespace elastica {

  namespace collision {

    //=================================================================================================
    //
    //  CONSTRUCTOR
    //
    //=================================================================================================
    PlanePrimitive::PlanePrimitive(id_t sid, id_t uid, bool finite,
                                   MaterialID material)
        :  // Initializing the base object
          GeomPrimitive(finite, sid, uid, material),
#ifdef ELASTICA_DEBUG
          spans_(make_array<std::tuple_size<Spans>::value>(
              make_signaling_NaN<real_t>()))
#else
          spans_(make_array<std::tuple_size<Spans>::value>(0.0))
#endif
    {
    }
    //*************************************************************************************************

    PlanePrimitive::PlanePrimitive(id_t sid, id_t uid, bool finite,
                                   const Vec3& gpos, const Vec3& normal,
                                   const Spans& spans, MaterialID material)
        : GeomPrimitive(finite, sid, uid, material), spans_(spans) {
      gpos_ = gpos;
      Q_ = make_orthogonal_bases(normal);
      std::cout << Q_ << std::endl;
      std::cout << blaze::row<1UL>(Q_) << std::endl;
      std::cout << "Spans :" << spans_[0] << " " << spans_[1] << std::endl;
      PlanePrimitive::calculate_bounding_box();

      // For now explicitly set velocities etc. for contact
      v_ = 0.0;
      w_ = 0.0;
    }

    PlanePrimitive::PlanePrimitive(id_t sid, id_t uid, bool finite,
                                   const Vec3& gpos, const Rot3& rot,
                                   const Spans& spans, MaterialID material)
        : GeomPrimitive(finite, sid, uid, material), spans_(spans) {
      gpos_ = gpos;

      // DONE normalize every row of the matrix and log a warning
      bool is_rot_orthogonal(is_orthogonal(rot));
      if (!is_rot_orthogonal) {
        LOG_WARNING_SECTION(log) {
          log << "Input rotation matrix is non-orthogonal\n";
        }
      }
      Q_ = is_rot_orthogonal ? rot : orthogonalize(rot);
      PlanePrimitive::calculate_bounding_box();
    }

    // In this case, since we access it ourselves we need to call the
    // appropriate move constructor including setting the parent variables
    // Hacky but okay for now

    //    PlanePrimitive::PlanePrimitive(PlanePrimitive&& other) noexcept
    //        : GeomPrimitive(std::move(other)),
    //#ifdef ELASTICA_DEBUG
    //          spans_(make_array<std::tuple_size<Spans>::value>(
    //              make_signaling_NaN<real_t>()))
    //#else
    //          spans_(make_array<std::tuple_size<Spans>::value>(0.0))
    //#endif
    //    {
    //    }

    PlanePrimitive::PlanePrimitive(PlanePrimitive&& other) noexcept
        : GeomPrimitive(other.finite_, other.sid_, other.uid_,
                        other.material_) {
      spans_[0UL] = other.spans_[0UL];
      spans_[1UL] = other.spans_[1UL];
      gpos_ = other.gpos_;
      Q_ = other.Q_;

      std::cout << "Spans :" << spans_[0] << " " << spans_[1] << std::endl;
      std::cout << blaze::row<1UL>(Q_) << std::endl;

      // For now put construction of AABB here
      PlanePrimitive::calculate_bounding_box();


      // For now explicitly set velocities etc.
      v_ = 0.0;
      w_ = 0.0;
    }


    //=================================================================================================
    //
    //  DESTRUCTOR
    //
    //=================================================================================================

    //*************************************************************************************************
    /*!\brief Destructor for the SphereBase class.
     */
    PlanePrimitive::~PlanePrimitive() noexcept {}
    //*************************************************************************************************

    //=================================================================================================
    //
    //  UTILITY FUNCTIONS
    //
    //=================================================================================================

    //*************************************************************************************************
    /*!\brief Calculation of the bounding box of the plane.
     *
     * \return void
     *
     * This function updates the axis-aligned bounding box of the plane
     * primitive according to the current position and orientation of the plane.
     * Note that the bounding box is increased in all dimensions by
     * pe::contact_threshold to guarantee that rigid bodies in close proximity
     * of the plane are also considered during the collision detection process.
     */
    void PlanePrimitive::calculate_bounding_box() {
      /*
      aabb_[0] = (normal_[0] < real_t(0) and normal_[1] == real_t(0) and
                  normal_[2] == real_t(0))
                     ? (-d_ - contact_threshold)
                     : (-inf);
      aabb_[1] = (normal_[0] == real_t(0) and normal_[1] < real_t(0) and
                  normal_[2] == real_t(0))
                     ? (-d_ - contact_threshold)
                     : (-inf);
      aabb_[2] = (normal_[0] == real_t(0) and normal_[1] == real_t(0) and
                  normal_[2] < real_t(0))
                     ? (-d_ - contact_threshold)
                     : (-inf);
      aabb_[3] = (normal_[0] > real_t(0) and normal_[1] == real_t(0) and
                  normal_[2] == real_t(0))
                     ? (d_ + contact_threshold)
                     : (inf);
      aabb_[4] = (normal_[0] == real_t(0) and normal_[1] > real_t(0) and
                  normal_[2] == real_t(0))
                     ? (d_ + contact_threshold)
                     : (inf);
      aabb_[5] = (normal_[0] == real_t(0) and normal_[1] == real_t(0) and
                  normal_[2] > real_t(0))
                     ? (d_ + contact_threshold)
                     : (inf);
                     */

      // d_3 and d_1 are along spans, d_2 is the normal
      // TODO : Check indexing
      // TODO don't access one by one, rather get buffer?
      const real_t xlength(real_t(0.5) * (fabs(Q_(0, 0) * spans_[1]) +
                                          fabs(Q_(2, 0) * spans_[0])) +
                           contact_threshold);
      const real_t ylength(real_t(0.5) * (fabs(Q_(0, 1) * spans_[1]) +
                                          fabs(Q_(2, 1) * spans_[0])) +
                           contact_threshold);
      const real_t zlength(real_t(0.5) * (fabs(Q_(0, 2) * spans_[1]) +
                                          fabs(Q_(2, 2) * spans_[0])) +
                           contact_threshold);

      aabb_[0] = gpos_[0] - xlength;
      aabb_[1] = gpos_[1] - ylength;
      aabb_[2] = gpos_[2] - zlength;
      aabb_[3] = gpos_[0] + xlength;
      aabb_[4] = gpos_[1] + ylength;
      aabb_[5] = gpos_[2] + zlength;

      ELASTICA_ASSERT(aabb_.isValid(), "Invalid bounding box detected");
      ELASTICA_ASSERT(aabb_.contains(gpos_), "Invalid bounding box detected");
    }
    //*************************************************************************************************

  }  // namespace collision

}  // namespace elastica
