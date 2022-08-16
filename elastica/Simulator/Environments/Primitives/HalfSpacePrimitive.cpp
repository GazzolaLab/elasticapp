//==============================================================================
/*!
//  \file
//  \brief Source file for HalfSpacePrimitive
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
#include "HalfSpacePrimitive.hpp"

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
    HalfSpacePrimitive::HalfSpacePrimitive(id_t sid, id_t uid, bool finite,
                                           MaterialID material)
        :  // Initializing the base object
          GeomPrimitive(finite, sid, uid, material) {}
    //**************************************************************************

    //**************************************************************************
    /*!\brief Constructor for the HalfSpacePrimitive class.
     *
     * \param sid Unique system-specific ID for the plane.
     * \param uid User-specific ID for the plane.
     * \param anchor_pos Global geometric center of the plane.
     * \param normal The normal direction of the plane
     * \param material The material of the plane.
     */
    HalfSpacePrimitive::HalfSpacePrimitive(id_t sid, id_t uid,
                                           const Vec3& anchor_pos,
                                           const Vec3& normal,
                                           MaterialID material)
        : HalfSpacePrimitive(sid, uid, true, anchor_pos,
                             make_orthogonal_bases_from_normal(normal),
                             material) {}
    //**************************************************************************

    //**************************************************************************
    /*!\brief Constructor for the HalfSpacePrimitive class.
     *
     * \param sid Unique system-specific ID for the plane.
     * \param uid User-specific ID for the plane.
     * \param anchor_pos Global geometric center of the plane.
     * \param rot The rotation matrix of the plane \f$ (0..\infty) \f$.
     * \param spans The spans of the plane \f$ (0..\infty) x (0..\infty) \f$.
     * \param material The material of the plane.
     */
    HalfSpacePrimitive::HalfSpacePrimitive(id_t sid, id_t uid, bool finite,
                                           const Vec3& anchor_pos,
                                           const Rot3& rot, MaterialID material)
        : GeomPrimitive(finite, sid, uid, material) {
      bool is_rot_orthogonal(is_orthogonal(rot));

      if (!is_rot_orthogonal) {
        using namespace ::elastica::logging;
        log<warning>([](const auto& log) {
          log << "Input rotation matrix is non-orthogonal";
        });
      }

      Q_ = is_rot_orthogonal ? rot : orthogonalize(rot);

      // update gpos properly
      gpos_ = anchor_pos - 0.5 * get_span() * get_normal();

      // Here set plane values to numbers that make sense
      v_ = 0.0;
      w_ = 0.0;

      HalfSpacePrimitive::calculate_bounding_box();
    }

    //==========================================================================
    //
    //  DESTRUCTOR
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Destructor for the HalfSpacePrimitive class.
     */
    HalfSpacePrimitive::~HalfSpacePrimitive() noexcept = default;
    //**************************************************************************

    //==========================================================================
    //
    //  UTILITY FUNCTIONS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Get span of the half-space primitive
     *
     */
    real_t HalfSpacePrimitive::get_span() noexcept {
      // Half shifting here because the contact threshold only has half the
      // amount of digits, and we should always have enough precision to
      // represent it. It maybe wise to refactor both contact threshold and span
      // to use some variant of digits10.
      return real_t(std::pow(10, std::numeric_limits<real_t>::digits10 >> 1));
    }
    //**************************************************************************

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
    void HalfSpacePrimitive::calculate_bounding_box() {
      // d_3 and d_1 are along spans, d_2 is the normal
      // TODO : Check indexing
      // TODO don't access one by one, rather get buffer?
      using std::fabs;

      constexpr auto binormal = cast_along(LagrangianFrame::Binormal);
      constexpr auto normal = cast_along(LagrangianFrame::Normal);
      constexpr auto tangent = cast_along(LagrangianFrame::Tangent);
      constexpr auto x = cast_along(EulerianFrame::X);
      constexpr auto y = cast_along(EulerianFrame::Y);
      constexpr auto z = cast_along(EulerianFrame::Z);

      const real_t xlength(real_t(0.5) * (fabs(Q_(binormal, x) * get_span()) +
                                          fabs(Q_(normal, x) * get_span()) +
                                          fabs(Q_(tangent, x) * get_span())) +
                           contact_threshold);
      const real_t ylength(real_t(0.5) * (fabs(Q_(binormal, y) * get_span()) +
                                          fabs(Q_(normal, y) * get_span()) +
                                          fabs(Q_(tangent, y) * get_span())) +
                           contact_threshold);
      const real_t zlength(real_t(0.5) * (fabs(Q_(binormal, z) * get_span()) +
                                          fabs(Q_(normal, z) * get_span()) +
                                          fabs(Q_(tangent, z) * get_span())) +
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
