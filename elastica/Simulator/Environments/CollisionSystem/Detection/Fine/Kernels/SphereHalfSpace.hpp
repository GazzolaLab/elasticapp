//==============================================================================
/*!
//  \file
//  \brief Implementation of a collision kernel.
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
//
#include "Simulator/Environments/Primitives/HalfSpacePrimitive.hpp"
#include "Simulator/Environments/Primitives/SpherePrimitive.hpp"
//
#include "Utilities/DefineTypes.h"
#include "Utilities/Logging.hpp"
#include "Utilities/Thresholds.hpp"
//
#include "Utilities/Math/Dot.hpp"
#include "Utilities/Math/Vec3.hpp"

namespace elastica {

  namespace collision {

    namespace detection {

      namespace fine {

        //**********************************************************************
        /*!\brief Contact generation between a Sphere and a HalfSpace.
         * \ingroup fine_collision_detection
         *
         * \details
         * In the case of a contact between a sphere and a half-space, only a
         * single contact point is generated. The contact point is calculated
         * with a projection of the global position of the sphere's center of
         * mass and the half-space's normal, from which follows the distance
         * between the sphere and the half-space.
         *
         * \param s The colliding sphere.
         * \param p The colliding half-space.
         * \param contacts Contact container for the generated contacts.
         * \return void
         */
        template <typename CC>  // Type of the contact container
        void do_collide(SpherePrimitive& s, HalfSpacePrimitive& p,
                        CC& contacts) noexcept {
          // Project distance between the sphere and half-space on the normal
          // to the plane that defines the half-space

          // Ordering : Normal from 2->1 always
          const Vec3 normal(p.get_normal());
          auto const r(s.get_radius());
          real_t const dist(dot(normal, s.get_position() - p.get_position()) -
                            0.5 * p.get_span() - r);
          if (dist < real_t(-2.0) * r) {
            // Special treatment if the sphere is entiurely inside the
            // half-plane.
            // 2r limit is such that the contact position is always a continous
            // function of the penetration depth, so jerks are avoided.
            // In this case, a contact at the sphere's center of mass with a
            // normal away from the half-space plane is generated.
            contacts.add_vertex_face_contact(&s, &p, s.get_position(), normal,
                                             dist);
          } else if (dist < contact_threshold) {
            // Cannot use half-space's center here as half-space is wide af
            // Added a 0.5 since half-space has finite width
            const Vec3 gPos(s.get_position() -
                            (r + real_t(0.5) * dist) * normal);

            using namespace ::elastica::logging;
            log<debug>([&](auto& log) {
              log << "      Contact created between sphere " << s.getID()
                  << " and half-space " << p.getID() << " (dist=" << dist
                  << ")";
            });

            contacts.add_vertex_face_contact(&s, &p, gPos, normal, dist);
          }
        }
        //**********************************************************************

      }  // namespace fine

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica
