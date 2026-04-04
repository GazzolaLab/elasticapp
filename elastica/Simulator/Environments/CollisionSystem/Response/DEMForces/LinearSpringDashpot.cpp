//==============================================================================
/*!
//  \file
//  \brief Definition of LinearSpringDashpot
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#include "Simulator/Environments/CollisionSystem/Response/DEMForces/LinearSpringDashpot.hpp"
//
#include <algorithm>  // min and max
//
#include "ErrorHandling/Assert.hpp"
//
#include "Simulator/Environments/CollisionSystem/Contact/Contact.hpp"  // Contact&
//
#include "Utilities/Logging.hpp"
#include "Utilities/Math/Length.hpp"
#include "Utilities/Math/Normalize.hpp"
#include "Utilities/Math/Vec3.hpp"

namespace elastica {

  namespace collision {

    namespace response {

      real_t LinearSpringDashpot::operator()(
          Contact const& contact) const noexcept {
        BodyID b1(contact.get_first_body());
        BodyID b2(contact.get_second_body());

        if (not contact.is_penetrating()) {
          // Skip separated contacts
          using namespace ::elastica::logging;
          log<debug>(
              [](auto& log) { log << "Rejecting separated contact.\n"; });
          return 0;
        }

        ELASTICA_ASSERT(!b1->is_fixed() or !b2->is_fixed(),
                        "Invalid contact between two fixed objects.");

        // The absolute value of the penetration length
        real_t delta(-contact.get_distance());

        // Calculating the relative velocity in normal and tangential
        // direction The negative signs result from the different definition
        // of the relative normal velocity of the pe (see Contact::getType)
        const real_t relVelN(-contact.get_normal_relative_velocity());
        const Vec3 relVel(-contact.get_relative_velocity());
        const Vec3 relVelT(relVel - (relVelN * contact.get_normal()));

        // cannot add a negative force here, so threshold with 0
        // this is a ReLU operation!
        real_t const fNabs =
            std::max(real_t(0), contact.get_stiffness() * delta +
                                    contact.get_normal_damping() * relVelN);

        Vec3 fN(fNabs * contact.get_normal());

        // Calculating the tangential force based on the model by Haff and
        // Werner
        const real_t fTabs(
            std::min(contact.get_tangential_damping() * length(relVelT),
                     contact.get_friction() * fNabs));
        const Vec3 fT(fTabs * normalize(relVelT));

        // Add normal force at contact point
        Vec3 net_force = (fN + fT);
        // No point in moving
        b2->add_force_at_position(-net_force, contact.get_position());
        // No point in moving
        b1->add_force_at_position(net_force, contact.get_position());

        //        // Add tangential force at contact point
        //        b1->addForceAtPos(fT, gpos);
        //        b2->addForceAtPos(-fT, gpos);

        return delta;
      }

    }  // namespace response

  }  // namespace collision

}  // namespace elastica
