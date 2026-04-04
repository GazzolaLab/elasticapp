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
#include "Simulator/Environments/Primitives/CapsuleFromRodPrimitive.hpp"
#include "Simulator/Environments/Primitives/CapsulePrimitive.hpp"
#include "Simulator/Environments/Primitives/PlanePrimitive.hpp"
#include "Simulator/Environments/Primitives/detail/CapsulePrimitiveBase.hpp"
//
#include "Utilities/DefineTypes.h"
#include "Utilities/Logging.hpp"
#include "Utilities/Thresholds.hpp"
//
#include "Utilities/Math/Dot.hpp"
#include "Utilities/Math/Rot3.hpp"
#include "Utilities/Math/Vec3.hpp"
//
#include <cmath>

namespace elastica {

  namespace collision {

    namespace detection {

      namespace fine {

        //**********************************************************************
        /*!\brief Contact generation between a Capsule and a Plane.
         * \ingroup fine_collision_detection
         *
         * \param c The colliding capsule.
         * \param p The colliding plane.
         * \param contacts Contact container for the generated contacts.
         * \return void
         *
         * The collision between a capsule and a plane is handled as two
         * collisions between the two cap spheres and the plane: the contact
         * points are calculated with a projection of the global positions of
         * the spheres' center of mass on the plane's normal. If the length of
         * the projection is smaller than the radius of the cap sphere, a
         * contact point is generated, which results in a maximum of two contact
         * points.
         */
        template <typename CC>  // Type of the contact container
        void do_collide_with_capsule(detail::CapsulePrimitiveBase& c,
                                     PlanePrimitive& p, CC& contacts) noexcept {
          const Vec3& cpos(c.get_position());
          const Rot3& Q(c.get_rotation_matrix());

          // Calculating the displacement of the center of the upper cap-spheres
          // in world space coordinates
          const Vec3 c_up(real_t(0.5) * c.get_length() *
                          Vec3{Q(2, 0), Q(2, 1), Q(2, 2)});
          const Vec3 pos_upper_sphere(cpos + c_up);
          const Vec3 pos_lower_sphere(cpos - c_up);

          // Computing the distance between the sphere caps and the plane
          const Vec3& normal(p.get_normal());
          const real_t dist_upper_sphere(
              std::abs(dot(normal, pos_upper_sphere - p.get_position())) -
              c.get_radius());
          const real_t dist_lower_sphere(
              std::abs(dot(normal, pos_lower_sphere - p.get_position())) -
              c.get_radius());

          // Collision of the upper sphere with the plane
          if (dist_upper_sphere < contact_threshold) {
            using namespace ::elastica::logging;
            log<debug>([&](auto& log) {
              log << "      Contact created between capsule " << c.getID()
                  << " and plane " << p.getID()
                  << " (dist=" << dist_upper_sphere << ")";
            });

            contacts.add_vertex_face_contact(
                &c, &p,
                pos_upper_sphere -
                    (c.get_radius() + dist_upper_sphere) * normal,
                normal, dist_upper_sphere);
          }

          // Collision of the lower sphere with the plane
          if (dist_lower_sphere < contact_threshold) {
            using namespace ::elastica::logging;
            log<debug>([&](auto& log) {
              log << "      Contact created between capsule " << c.getID()
                  << " and plane " << p.getID()
                  << " (dist=" << dist_lower_sphere << ")";
            });

            contacts.add_vertex_face_contact(
                &c, &p,
                pos_lower_sphere -
                    (c.get_radius() + dist_lower_sphere) * normal,
                normal, dist_lower_sphere);
          }
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Contact generation between a Capsule and a Plane.
         * \ingroup fine_collision_detection
         *
         * \param c The colliding capsule.
         * \param p The colliding plane.
         * \param contacts Contact container for the generated contacts.
         * \return void
         *
         * The collision between a capsule and a plane is handled as two
         * collisions between the two cap spheres and the plane: the contact
         * points are calculated with a projection of the global positions of
         * the spheres' center of mass on the plane's normal. If the length of
         * the projection is smaller than the radius of the cap sphere, a
         * contact point is generated, which results in a maximum of two contact
         * points.
         */
        template <typename CC>
        void do_collide(CapsulePrimitive& c, PlanePrimitive& p,
                        CC& contacts) noexcept {
          do_collide_with_capsule(static_cast<detail::CapsulePrimitiveBase&>(c),
                                  p, contacts);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Contact generation between a RodCapsule and a Plane.
         * \ingroup fine_collision_detection
         *
         * \param c The colliding rod_capsule.
         * \param p The colliding plane.
         * \param contacts Contact container for the generated contacts.
         * \return void
         *
         * The collision between a rod_capsule and a plane is handled as two
         * collisions between the two cap spheres and the plane: the contact
         * points are calculated with a projection of the global positions of
         * the spheres' center of mass on the plane's normal. If the length of
         * the projection is smaller than the radius of the cap sphere, a
         * contact point is generated, which results in a maximum of two contact
         * points.
         */
        template <typename CC>
        void do_collide(CapsuleFromRodPrimitive& c, PlanePrimitive& p,
                        CC& contacts) noexcept {
          do_collide_with_capsule(static_cast<detail::CapsulePrimitiveBase&>(c),
                                  p, contacts);
        }
        //**********************************************************************

      }  // namespace fine

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica
