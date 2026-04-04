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
#include "Simulator/Environments/Primitives/SpherePrimitive.hpp"
// Utilities
#include "Utilities/DefineTypes.h"
#include "Utilities/Logging.hpp"
#include "Utilities/Thresholds.hpp"
//
#include "Utilities/Math/Length.hpp"
#include "Utilities/Math/Normalize.hpp"
#include "Utilities/Math/Rot3.hpp"
#include "Utilities/Math/Vec3.hpp"

namespace elastica {

  namespace collision {

    namespace detection {

      namespace fine {

        //**********************************************************************
        /*!\brief Contact generation between a Sphere and a Capsule.
         * \ingroup fine_collision_detection
         *
         * \details
         * In case of a contact between a sphere and a capsule, a single contact
         * point is generated. This contact point is calculated by first
         * estimating a sphere within the capsule whose center of mass is on the
         * centerline of the capsule and closest to the center of mass of the
         * colliding sphere. After this, a sphere-sphere collision is performed
         * between the capsule representation and the colliding sphere.
         *
         * \param s The colliding sphere.
         * \param c The colliding capsule.
         * \param contacts Contact container for the generated contacts.
         * \return void
         */
        template <typename CC>  // Type of the contact container
        void do_collide_with_capsule(SpherePrimitive& s,
                                     detail::CapsulePrimitiveBase& c,
                                     CC& contacts) noexcept {
          const real_t half_capsule_length(
              real_t(0.5) * c.get_length());   // Half cylinder length
          const Vec3& spos(s.get_position());  // Global position of the sphere
          const Vec3& cpos(c.get_position());  // Global position of the capsule
          const Rot3& Q(c.get_rotation_matrix());  // Rotation of the capsule

          // Calculating the relative tangent position of the sphere in the
          // frame of the capsule
          real_t rp_tangent(Q(2, 0) * (spos[0] - cpos[0]) +
                            Q(2, 1) * (spos[1] - cpos[1]) +
                            Q(2, 2) * (spos[2] - cpos[2]));

          // Calculation the center of the sphere representing the capsule
          rp_tangent = std::clamp(rp_tangent, -half_capsule_length,
                                    half_capsule_length);

          // Compute the point of closest distance along centerline
          const Vec3 shifted_pos{rp_tangent * Q(2, 0) + cpos[0],
                                 rp_tangent * Q(2, 1) + cpos[1],
                                 rp_tangent * Q(2, 2) + cpos[2]};

          // Performing a sphere-sphere collision between the colliding sphere
          // and the capsule representation

          // Ordering : Normal from 2->1 always
          Vec3 normal(spos - shifted_pos);
          const real_t dist(length(normal) - s.get_radius() - c.get_radius());

          if (dist < contact_threshold) {
            normal = normalize(normal);
            const real_t k(c.get_radius() + real_t(0.5) * dist);
            const Vec3 gPos(shifted_pos + normal * k);

            using namespace ::elastica::logging;
            log<debug>([&](auto& log) {
              log << "      Contact created between sphere " << s.getID()
                  << " and capsule " << c.getID() << " (dist=" << dist << ")";
            });

            contacts.add_vertex_face_contact(&s, &c, gPos, normal, dist);
          }
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Contact generation between a Sphere and a Capsule.
         * \ingroup fine_collision_detection
         *
         * \details
         * In case of a contact between a sphere and a capsule, a single contact
         * point is generated. This contact point is calculated by first
         * estimating a sphere within the capsule whose center of mass is on the
         * centerline of the capsule and closest to the center of mass of the
         * colliding sphere. After this, a sphere-sphere collision is performed
         * between the capsule representation and the colliding sphere.
         *
         * \param s The colliding sphere.
         * \param p The colliding capsule.
         * \param contacts Contact container for the generated contacts.
         * \return void
         */
        template <typename CC>
        void do_collide(SpherePrimitive& s, CapsulePrimitive& p,
                        CC& contacts) noexcept {
          do_collide_with_capsule(
              s, static_cast<detail::CapsulePrimitiveBase&>(p), contacts);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Contact generation between a Sphere and a Capsule.
         * \ingroup fine_collision_detection
         *
         * \details
         * In case of a contact between a sphere and a capsule, a single contact
         * point is generated. This contact point is calculated by first
         * estimating a sphere within the capsule whose center of mass is on the
         * centerline of the capsule and closest to the center of mass of the
         * colliding sphere. After this, a sphere-sphere collision is performed
         * between the capsule representation and the colliding sphere.
         *
         * \param s The colliding sphere.
         * \param p The colliding capsule (from-rod specialization).
         * \param contacts Contact container for the generated contacts.
         * \return void
         */
        template <typename CC>
        void do_collide(SpherePrimitive& s, CapsuleFromRodPrimitive& p,
                        CC& contacts) noexcept {
          do_collide_with_capsule(
              s, static_cast<detail::CapsulePrimitiveBase&>(p), contacts);
        }
        //**********************************************************************

      }  // namespace fine

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica
