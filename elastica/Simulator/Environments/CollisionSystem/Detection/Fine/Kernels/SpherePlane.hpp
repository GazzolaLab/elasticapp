//==============================================================================
/*!
//  \file
//  \brief Implementation of a collision kernel.
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
//
#include "Simulator/Environments/Primitives/PlanePrimitive.hpp"
#include "Simulator/Environments/Primitives/SpherePrimitive.hpp"
//
#include "Utilities/DefineTypes.h"
#include "Utilities/Logging.hpp"
#include "Utilities/Thresholds.hpp"
//
#include "Utilities/Math/Dot.hpp"
#include "Utilities/Math/Vec3.hpp"
//
#include <cmath>  // abs

namespace elastica {

  namespace collision {

    namespace detection {

      namespace fine {

        //**********************************************************************
        /*!\brief Contact generation between a Sphere and a Plane.
         * \ingroup fine_collision_detection
         *
         * \details
         * In the case of a contact between a sphere and a plane, only a
         * single contact point is generated. The contact point is calculated
         * with a projection of the global position of the sphere's center of
         * mass and the plane's normal, from which follows the distance
         * between the sphere and the plane.
         *
         * \param s The colliding sphere.
         * \param p The colliding plane.
         * \param contacts Contact container for the generated contacts.
         * \return void
         */
        template <typename CC>  // Type of the contact container
        void do_collide(SpherePrimitive& s, PlanePrimitive& p,
                        CC& contacts) noexcept {
          // Project distance between the sphere and plane on the plane's
          // normal

          // Ordering : Normal from 2->1 always
          const Vec3 normal(p.get_normal());
          const real_t dist(
              std::abs(dot(normal, s.get_position() - p.get_position())) -
              s.get_radius());

          if (dist < contact_threshold) {
            // Cannot use plane's center here as plane is wide af
            const Vec3 gPos(s.get_position() -
                            (s.get_radius() + dist) * normal);

            using namespace ::elastica::logging;
            log<debug>([&](auto& log) {
              log << "      Contact created between sphere " << s.getID()
                  << " and plane " << p.getID() << " (dist=" << dist << ")";
            });

            contacts.add_vertex_face_contact(&s, &p, gPos, normal, dist);
          }
        }
        //**********************************************************************

      }  // namespace fine

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica
