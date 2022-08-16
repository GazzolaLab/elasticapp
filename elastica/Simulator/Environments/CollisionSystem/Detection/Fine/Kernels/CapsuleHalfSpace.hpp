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
#include "Simulator/Environments/Primitives/CapsuleFromRodPrimitive.hpp"
#include "Simulator/Environments/Primitives/CapsulePrimitive.hpp"
#include "Simulator/Environments/Primitives/HalfSpacePrimitive.hpp"
#include "Simulator/Environments/Primitives/detail/CapsulePrimitiveBase.hpp"
//
#include "Utilities/DefineTypes.h"
#include "Utilities/Logging.hpp"
#include "Utilities/Thresholds.hpp"
//
#include "Utilities/Math/Dot.hpp"
#include "Utilities/Math/Vec3.hpp"
//
#include <cmath>

namespace elastica {

  namespace collision {

    namespace detection {

      namespace fine {

        //**********************************************************************
        /*!\brief Contact generation between a Capsule and a HalfSpace.
         * \ingroup fine_collision_detection
         *
         * \param c The colliding capsule.
         * \param p The colliding half-space.
         * \param contacts Contact container for the generated contacts.
         * \return void
         *
         * The collision between a capsule and a half-space is handled
         * as a single collision between the capsule axis and the plane defining
         * the half-space.
         */
        template <typename CC>  // Type of the contact container
        void do_collide_with_capsule(detail::CapsulePrimitiveBase& c,
                                     HalfSpacePrimitive& p,
                                     CC& contacts) noexcept {
          const Vec3& cpos(c.get_position());
          const Rot3& Q(c.get_rotation_matrix());
          const auto r(c.get_radius());

#ifndef _ELASTICA_CAPSULE_HALFSPACE_SPHERE_CHECK
          // Calculating the displacement of the center of the upper cap-spheres
          // in world space coordinates
          const Vec3 c_axis{Q(2, 0), Q(2, 1), Q(2, 2)};

          const Vec3& normal(p.get_normal());
          const real_t cosa = dot(c_axis, normal);

          if (std::abs(cosa) < contact_threshold) {
            // its almost parallel, so generate one point of contact at the
            // center of mass of the capsule

            // 1. Calculate centerline distance
            const real_t dist(dot(normal, cpos - p.get_position()) -
                              0.5 * p.get_span() - r);

            if (dist < contact_threshold) {
              using namespace ::elastica::logging;
              log<debug>([&](auto& log) {
                log << "      Contact created between capsule " << c.getID()
                    << " and half-space " << p.getID() << " (dist=" << dist
                    << ")";
              });

              // 2. Add contact
              contacts.add_vertex_face_contact(
                  &c, &p, cpos - (r + real_t(0.5) * dist) * normal, normal,
                  dist);
            }
          } else {
            // cosa > 0 : tail is closer, else head is closer
            int sign = (cosa > 0) ? -1 : 1;
            const Vec3 closest_point(cpos + sign * real_t(0.5) *
                                                c.get_length() * c_axis);
            // Calculate distance
            const real_t dist(dot(normal, closest_point - p.get_position()) -
                              0.5 * p.get_span() - r);

            if (dist < contact_threshold) {
              using namespace ::elastica::logging;
              log<debug>([&](auto& log) {
                log << "      Contact created between capsule " << c.getID()
                    << " and half-space " << p.getID() << " (dist=" << dist
                    << ")";
              });

              // 2. Add contact
              contacts.add_vertex_face_contact(
                  &c, &p, closest_point - (r + real_t(0.5) * dist) * normal,
                  normal, dist);
            }
          }

#else
          // Generates two collisions
          const Vec3 pos_upper_sphere(cpos + c_up);
          const Vec3 pos_lower_sphere(cpos - c_up);
          const Vec3 c_up(real_t(0.5) * c.get_length() * c_axis);

          const real_t dist_upper_sphere(
              std::abs(dot(normal, pos_upper_sphere - p.get_position())) - r);
          const real_t dist_lower_sphere(
              std::abs(dot(normal, pos_lower_sphere - p.get_position())) - r);

          auto const register_collision = [&](Vec3 const& pos,
                                              real_t dist) noexcept {
            if (dist < -r) {
              // do something
              contacts.add_vertex_face_contact(
                  &c, &p, pos - (r + dist) * normal, normal, dist);
            } else if (dist < contact_threshold) {
              using namespace ::elastica::logging;
              log<debug>([&](auto& log) {
                log << "      Contact created between capsule " << c.getID()
                    << " and half-space " << p.getID() << " (dist=" << dist
                    << ")";
              });

              contacts.add_vertex_face_contact(
                  &c, &p, pos - (r + dist) * normal, normal, dist);
            }
          };

          // Collision of the upper sphere with the half-space
          register_collision(pos_upper_sphere, dist_upper_sphere);
          // Collision of the lower sphere with the half-space
          register_collision(pos_lower_sphere, dist_lower_sphere);
#endif
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Contact generation between a Capsule and a HalfSpace.
         * \ingroup fine_collision_detection
         *
         * \param c The colliding capsule.
         * \param p The colliding half-space.
         * \param contacts Contact container for the generated contacts.
         * \return void
         *
         * The collision between a capsule and a half-space is handled as two
         * collisions between the two cap spheres and the half-space: the
         * contact points are calculated with a projection of the global
         * positions of the spheres' center of mass on the half-space's normal.
         * If the length of the projection is smaller than the radius of the cap
         * sphere, a contact point is generated, which results in a maximum of
         * two contact points.
         */
        template <typename CC>
        inline void do_collide(CapsulePrimitive& c, HalfSpacePrimitive& p,
                               CC& contacts) noexcept {
          do_collide_with_capsule(static_cast<detail::CapsulePrimitiveBase&>(c),
                                  p, contacts);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Contact generation between a RodCapsule and a HalfSpace.
         * \ingroup fine_collision_detection
         *
         * \param c The colliding rod_capsule.
         * \param p The colliding half-space.
         * \param contacts Contact container for the generated contacts.
         * \return void
         *
         * The collision between a rod_capsule and a half-space is handled as
         * two collisions between the two cap spheres and the half-space: the
         * contact points are calculated with a projection of the global
         * positions of the spheres' center of mass on the half-space's normal.
         * If the length of the projection is smaller than the radius of the cap
         * sphere, a contact point is generated, which results in a maximum of
         * two contact points.
         */
        template <typename CC>
        inline void do_collide(CapsuleFromRodPrimitive& c,
                               HalfSpacePrimitive& p, CC& contacts) noexcept {
          do_collide_with_capsule(static_cast<detail::CapsulePrimitiveBase&>(c),
                                  p, contacts);
        }
        //**********************************************************************

      }  // namespace fine

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica
