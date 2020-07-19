#include "elastica/Simulator/Environments/CollisionSystem/Response/DEMForces/LinearSpringDashpot.hpp"

#include <cmath>

#include "elastica/ErrorHandling/Assert.hpp"
#include "elastica/Simulator/Environments/CollisionSystem/Contact/Contact.hpp"
#include "elastica/Simulator/Environments/Primitives/RigidBodyPrimitive.hpp"
#include "elastica/Utilities/Logging.hpp"
#include "elastica/Utilities/Math/Normalize.hpp"
#include "elastica/Utilities/Math/SqrLength.hpp"
#include "elastica/Utilities/Math/Vec3.hpp"

namespace elastica {

  namespace collision {

    namespace response {

      // TODO Refactor to use a reference to prevent aliasing
      //      template<>
      //      real_t LinearSpringDashpot::operator()<ContactID>(ContactID c)
      //      const noexcept {

      real_t LinearSpringDashpot::operator()(ContactID c) const noexcept {
        BodyID b1(c->getBody1());
        BodyID b2(c->getBody2());

        if (!c->is_penetrating()) {
          // Skip separated contacts
          LOG_DEBUG_SECTION(log) { log << "Rejecting separated contact.\n"; }
          return 0;
        }

        ELASTICA_ASSERT(!b1->is_fixed() || !b2->is_fixed(),
                        "Invalid contact between two fixed objects.");

        // Global position of contact
        const Vec3& gpos(c->get_position());

        // The absolute value of the penetration length
        real_t delta(-c->get_distance());

        // Calculating the relative velocity in normal and tangential
        // direction The negative signs result from the different definition
        // of the relative normal velocity of the pe (see Contact::getType)
        const real_t relVelN(-c->get_normal_relative_velocity());
        const Vec3 relVel(-c->get_relative_velocity());
        const Vec3 relVelT(relVel - (relVelN * c->get_normal()));

        real_t fNabs(0);

        fNabs = c->get_stiffness() * delta + c->get_normal_damping() * relVelN;
        fNabs = std::max(real_t(0), fNabs);
        //          if (fNabs < real_t(0))
        //            fNabs = real_t(0);

        Vec3 fN(fNabs * c->get_normal());

        // Calculating the tangential force based on the model by Haff and
        // Werner
        const real_t fTabs(std::min(c->get_tangential_damping() * sqrLength(relVelT),
                                    c->get_friction() * fNabs));
        const Vec3 fT(fTabs * normalize(relVelT));

        // TODO Careful here, needs mutexing or some other mechanism to prevent
        //      data races if parallel
        // Add normal force at contact point
        Vec3 net_force = (fN + fT);
        // should automatically move
        b2->add_force_at_position(-net_force, gpos);
        // Force move No point
        b1->add_force_at_position(std::move(net_force), gpos);

        //        // Add tangential force at contact point
        //        b1->addForceAtPos(fT, gpos);
        //        b2->addForceAtPos(-fT, gpos);

        return delta;
      }

      //      real_t LinearSpringDashpot::operator()(int c) const noexcept {
      //        return real_t (c);
      //      }
      //
    }  // namespace response

  }  // namespace collision

}  // namespace elastica
