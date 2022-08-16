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
#include "Simulator/Environments/Primitives/SpherePrimitive.hpp"
//
#include "Utilities/DefineTypes.h"
#include "Utilities/Logging.hpp"
#include "Utilities/Thresholds.hpp"
//
#include "Utilities/Math/Normalize.hpp"
#include "Utilities/Math/Vec3.hpp"

namespace elastica {

  namespace collision {

    namespace detection {

      namespace fine {

        //**********************************************************************
        /*!\brief Contact generation between two colliding Sphere primitives.
         * \ingroup fine_collision_detection
         *
         * \details
         * In the case of a collision between two spheres, only a single
         * contact is generated. This contact point lies on the connection
         * between both centers of mass, relative to the radii of the spheres.
         *
         * \param s1 The first colliding sphere.
         * \param s2 The second colliding sphere.
         * \param contacts Contact container for the generated contacts.
         * \return void
         */
        template <typename CC>  // Type of the contact container
        void do_collide(SpherePrimitive& s1, SpherePrimitive& s2,
                        CC& contacts) noexcept {
          // Ordering : Normal from 2->1 always
          Vec3 normal(s1.get_position() - s2.get_position());
          const real_t dist(length(normal) - s1.get_radius() - s2.get_radius());

          if (dist < contact_threshold) {
            normal = normalize(normal);
            const real_t k(s2.get_radius() + real_t(0.5) * dist);
            const Vec3 gPos(s2.get_position() + normal * k);

            using namespace ::elastica::logging;
            log<debug>([&](auto& log) {
              log << "      Contact created between sphere " << s1.getID()
                  << " and sphere " << s2.getID() << " (dist=" << dist << ")";
            });

            contacts.add_vertex_face_contact(&s1, &s2, gPos, normal, dist);
          }
        }
        //**********************************************************************

      }  // namespace fine

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica
