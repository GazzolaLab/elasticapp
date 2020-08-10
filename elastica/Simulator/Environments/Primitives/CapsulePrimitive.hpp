#pragma once

#include <cmath>

//////// Types first
#include "elastica/Simulator/Environments/CollisionSystem/Types.hpp"
#include "elastica/Simulator/Environments/Primitives/Types.hpp"
#include "elastica/Utilities/DefineTypes.h"
////////

#include "elastica/ErrorHandling/Assert.hpp"
#include "elastica/Simulator/Environments/Primitives/ApproximateCylinderPrimitive.hpp"
#include "elastica/Simulator/Environments/Primitives/Protocols.hpp"
#include "elastica/Simulator/Thresholds.hpp"
#include "elastica/Utilities/Math/Equal.hpp"
#include "elastica/Utilities/Math/SqrLength.hpp"
#include "elastica/Utilities/Math/Vec3.hpp"
#include "elastica/Utilities/ProtocolHelpers.hpp"

namespace elastica {

  namespace collision {

    class CapsulePrimitive final
        : public ApproximateCylinderPrimitive,
          ::tt::ConformsTo<protocols::ConvexPrimitive> {
      //**Constructor***********************************************************
      /*!\name Constructor */
      //@{
      // Note : these are all purely forwarding constructors
      explicit CapsulePrimitive(id_t sid, id_t uid, bool finite,
                                MaterialID material);
      CapsulePrimitive(id_t sid, id_t uid, bool finite, const Vec3& gpos,
                       real_t radius, real_t length, MaterialID material);
      CapsulePrimitive(CapsulePrimitive&&) noexcept;
      //@}
      //************************************************************************

      //**Destructor************************************************************
      /*!\name Destructor */
      //@{
      ~CapsulePrimitive() noexcept final;
      //@}
      //************************************************************************

     public:
      //**Utility functions*****************************************************
      /*!\name Utility functions */
      //@{
      //!< support functions for protocol conformance
      inline Vec3 support(const Vec3& d) const;
      inline Vec3 support_contact_threshold(const Vec3& d) const;
      //@}
      //************************************************************************
    };

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
    inline Vec3 CapsulePrimitive::support(const Vec3& d) const {
      ELASTICA_ASSERT(!elastica::equal(elastica::sqrLength(d), 0.0),
                      "Zero length search direction");
      ELASTICA_ASSERT(
          !elastica::equal(std::abs(1.0 - elastica::sqrLength(d)), 0.0),
          "Search direction is not normalised");

      // d in body frame coordinates
      const Vec3 bfD(from_world_FOR_to_body_FOR(d));

      // Assume d_3 is along tangent and hence the length
      // so use d[2] to compute supportSegment
      const Vec3 support_segment = Vec3{
          0.0, 0.0,
          // sign (u, v)
          ((real_t(0.0) < bfD[2]) - (bfD[2] < real_t(0.0))) * length_ * 0.5};

      const Vec3 support_sphere = radius_ * d;

      return gpos_ + from_body_FOR_to_world_FOR(support_segment) +
             support_sphere;
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
    inline Vec3 CapsulePrimitive::support_contact_threshold(
        const Vec3& d) const {
      ELASTICA_ASSERT(!elastica::equal(elastica::sqrLength(d), 0.0),
                      "Zero length search direction");
      ELASTICA_ASSERT(
          !elastica::equal(std::abs(1.0 - elastica::sqrLength(d)), 0.0),
          "Search direction is not normalised");
      return support(d) + d * contact_threshold;
    }
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica
