//==============================================================================
/*!
//  \file
//  \brief Source file for CylinderPrimitive
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
#include "PlanePrimitive.hpp"

#include <cmath>

#include "ErrorHandling/Assert.hpp"
#include "Simulator/Frames.hpp"
#include "Utilities/Logging.hpp"
#include "Utilities/MakeArray.hpp"
#include "Utilities/MakeSignalingNan.hpp"
#include "Utilities/Math/Infinity.hpp"
#include "Utilities/Math/Orthogonal.hpp"
#include "Utilities/Thresholds.hpp"

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
     * \param sid Unique system-specific ID for the plane.
     * \param uid User-specific ID for the plane.
     * \param material The material of the plane.
     */
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
    //**************************************************************************

    //**************************************************************************
    /*!\brief Constructor for the PlanePrimitive class.
     *
     * \param sid Unique system-specific ID for the plane.
     * \param uid User-specific ID for the plane.
     * \param gpos Global geometric center of the plane.
     * \param normal The normal direction of the plane
     * \param material The material of the plane.
     */
    PlanePrimitive::PlanePrimitive(id_t sid, id_t uid, const Vec3& gpos,
                                   const Vec3& normal, MaterialID material)
        // putting spans = inf causes the bounding box routine to error out
        // so I put a high number
        : PlanePrimitive(sid, uid, false, gpos, normal, {1e40, 1e40},
                         material) {}
    //**************************************************************************

    //**************************************************************************
    /*!\brief Constructor for the PlanePrimitive class.
     *
     * \param sid Unique system-specific ID for the plane.
     * \param uid User-specific ID for the plane.
     * \param gpos Global geometric center of the plane.
     * \param normal The normal direction of the plane
     * \param spans The spans of the plane \f$ (0..\infty) x (0..\infty) \f$.
     * \param material The material of the plane.
     */
    PlanePrimitive::PlanePrimitive(id_t sid, id_t uid, bool finite,
                                   const Vec3& gpos, const Vec3& normal,
                                   const Spans& spans, MaterialID material)
        : PlanePrimitive(sid, uid, finite, gpos,
                         make_orthogonal_bases_from_normal(normal), spans,
                         material) {}
    //**************************************************************************

    //**************************************************************************
    /*!\brief Constructor for the PlanePrimitive class.
     *
     * \param sid Unique system-specific ID for the plane.
     * \param uid User-specific ID for the plane.
     * \param gpos Global geometric center of the plane.
     * \param rot The rotation matrix of the plane \f$ (0..\infty) \f$.
     * \param spans The spans of the plane \f$ (0..\infty) x (0..\infty) \f$.
     * \param material The material of the plane.
     */
    PlanePrimitive::PlanePrimitive(id_t sid, id_t uid, bool finite,
                                   const Vec3& gpos, const Rot3& rot,
                                   const Spans& spans, MaterialID material)
        : GeomPrimitive(finite, sid, uid, material), spans_(spans) {
      gpos_ = gpos;

      bool is_rot_orthogonal(is_orthogonal(rot));

      if (!is_rot_orthogonal) {
        using namespace ::elastica::logging;
        log<warning>([](const auto& log) {
          log << "Input rotation matrix is non-orthogonal";
        });
      }

      Q_ = is_rot_orthogonal ? rot : orthogonalize(rot);

      // Here set plane values to numbers that make sense
      v_ = 0.0;
      w_ = 0.0;

      PlanePrimitive::calculate_bounding_box();
    }

    //    PlanePrimitive::PlanePrimitive(PlanePrimitive&& other) noexcept
    //        : GeomPrimitive(other.finite_, other.sid_, other.uid_,
    //                        other.material_) {
    //      spans_[0UL] = other.spans_[0UL];
    //      spans_[1UL] = other.spans_[1UL];
    //      gpos_ = other.gpos_;
    //      Q_ = other.Q_;
    //
    //      // std::cout << "Spans :" << spans_[0] << " " << spans_[1] <<
    //      std::endl;
    //      // std::cout << blaze::row<1UL>(Q_) << std::endl;
    //
    //      // For now put construction of AABB here
    //      PlanePrimitive::calculate_bounding_box();
    //    }
    //    //**************************************************************************

    //==========================================================================
    //
    //  DESTRUCTOR
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Destructor for the PlanePrimitive class.
     */
    PlanePrimitive::~PlanePrimitive() noexcept = default;
    //**************************************************************************

    //==========================================================================
    //
    //  UTILITY FUNCTIONS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Calculation of the bounding box of the plane.
     *
     * \return void
     *
     * This function updates the axis-aligned bounding box of the plane
     * primitive according to the current position and orientation of the plane.
     * Note that the bounding box is increased in all dimensions by
     * elastica::contact_threshold to guarantee that rigid bodies in close
     * proximity of the plane are also considered during the collision detection
     * process.
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
      using std::fabs;

      constexpr auto binormal = cast_along(LagrangianFrame::Binormal);
      constexpr auto tangent = cast_along(LagrangianFrame::Tangent);
      constexpr auto x = cast_along(EulerianFrame::X);
      constexpr auto y = cast_along(EulerianFrame::Y);
      constexpr auto z = cast_along(EulerianFrame::Z);

      const real_t xlength(real_t(0.5) * (fabs(Q_(binormal, x) * spans_[1]) +
                                          fabs(Q_(tangent, x) * spans_[0])) +
                           contact_threshold);
      const real_t ylength(real_t(0.5) * (fabs(Q_(binormal, y) * spans_[1]) +
                                          fabs(Q_(tangent, y) * spans_[0])) +
                           contact_threshold);
      const real_t zlength(real_t(0.5) * (fabs(Q_(binormal, z) * spans_[1]) +
                                          fabs(Q_(tangent, z) * spans_[0])) +
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
