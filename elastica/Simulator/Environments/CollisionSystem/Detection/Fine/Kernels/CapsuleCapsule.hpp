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
#include "Simulator/Environments/Primitives/detail/CapsulePrimitiveBase.hpp"
//
#include "Utilities/DefineTypes.h"
#include "Utilities/Logging.hpp"
#include "Utilities/Thresholds.hpp"
//
#include "Utilities/Math/Dot.hpp"
#include "Utilities/Math/Length.hpp"
#include "Utilities/Math/Normalize.hpp"
#include "Utilities/Math/Rot3.hpp"
#include "Utilities/Math/Vec3.hpp"
//
#include "Utilities/Geometry/GetClosestLineSegmentPoints.hpp"
//
#include <cmath>

namespace elastica {

  namespace collision {

    namespace detection {

      namespace fine {

        // TODO : Confirm change from pe (the definition of k is changed)
        //**********************************************************************
        /*!\brief Contact generation between two colliding capsule
         * primitives.
         * \ingroup fine_detection
         *
         * \details
         * In case of two colliding capsules one or two contact points may be
         * generated. In order to estimate if the two capsules are colliding,
         * the closest points on both centerlines are calculated (see
         * elastica::get_closest_line_segment_points). If the distance of these
         * two points is smaller than the sum of their radii, the two capsules
         * are colliding. In case they are lying parallel to each other and
         * touch each other along their cylindrical part, two contact points are
         * generated. In all other cases, a single contact point between the
         * calculated closest points is generated.
         *
         * \param c1 The first colliding capsule.
         * \param c2 The second colliding capsule.
         * \param contacts Contact container for the generated contacts.
         * \return void
         */
        template <typename CC>  // Type of the contact container
        void do_collide_with_capsule(detail::CapsulePrimitiveBase& c1,
                                     detail::CapsulePrimitiveBase& c2,
                                     CC& contacts) noexcept {
          const Vec3& c1pos(c1.get_position());
          const Vec3& c2pos(c2.get_position());

          const Rot3& Q1(c1.get_rotation_matrix());
          const Rot3& Q2(c2.get_rotation_matrix());

          // Calculating the "up" direction for both capsules that
          // points from the center of mass to the center of the upper disc of
          // each capsule
          const Vec3 c1_up_dir{Q1(2, 0), Q1(2, 1), Q1(2, 2)};
          const Vec3 c2_up_dir{Q2(2, 0), Q2(2, 1), Q2(2, 2)};

          // Calculating the displacement of the center of the upper cap-spheres
          // in world space coordinates
          const Vec3 c1_up(real_t(0.5) * c1.get_length() * c1_up_dir);
          const Vec3 c2_up(real_t(0.5) * c2.get_length() * c2_up_dir);

          // calculate the closest points of the two center lines
          Vec3 cp1, cp2;
          ::elastica::geometry::get_closest_line_segment_points(
              c1pos + c1_up, c1pos - c1_up, c2pos + c2_up, c2pos - c2_up, cp1,
              cp2);

          Vec3 normal(cp1 - cp2);
          const real_t dist(length(normal) - c1.get_radius() - c2.get_radius());
          if (dist < contact_threshold) {
            // if dist is 0 then the capsules are identically colliding, i.e.
            // they either are the same capsule or they are adjacent elements of
            // a Cosserat rod. In this case, we skip contacts as the former
            // situation is impossible and the latter situation is commonly
            // encountered but we skip it.

            normal = normalize(normal);

            // Calculating the relative x-position of the second capsule in the
            // frame of the first capsule
            const real_t c2x(dot(c1_up_dir, (c2pos - c1pos)));

            // Creating two contact points if the capsules are parallel to each
            // other
            // No need to check for < parallel_threshold as its from the
            // rotation matrix
            if ((std::fabs(dot(c1_up_dir, c2_up_dir)) - real_t(1)) >
                    -parallel_threshold and
                std::fabs(c2x) < c1.get_length()) {
              const real_t half_length_1(real_t(0.5) * c1.get_length());
              const real_t half_length_2(real_t(0.5) * c2.get_length());

              using namespace ::elastica::logging;
              log<debug>([&](auto& log) {
                log << "      First contact created between capsule "
                    << c1.getID() << " and capsule " << c2.getID()
                    << " (dist=" << dist << ")";
              });

              // Project everything onto the 1-dimensional parallel axis
              // do intersection tests there
              // and then reproject to global

              // Creating the "upper" contact point in world coordinates
              if (half_length_1 < c2x + half_length_2) {
                const real_t k(c1.get_radius() + real_t(0.5) * dist);
                contacts.add_vertex_face_contact(
                    &c1, &c2, c1pos + c1_up - k * normal, normal, dist);
              } else {
                // Cannot reuse c2_up as c2_up_dir and c1_up_dir can be
                // antiparallel (if parallel there are no issues)
                const real_t k(c2.get_radius() + real_t(0.5) * dist);
                contacts.add_vertex_face_contact(
                    &c1, &c2, c2pos + half_length_2 * c1_up_dir + k * normal,
                    normal, dist);
              }

              using namespace ::elastica::logging;
              log<debug>([&](auto& log) {
                log << "      Second contact created between capsule "
                    << c1.getID() << " and capsule " << c2.getID()
                    << " (dist=" << dist << ")";
              });

              // Creating the "lower" contact point in world coordinates
              if (-half_length_1 > c2x - half_length_2) {
                const real_t k(c1.get_radius() + real_t(0.5) * dist);
                contacts.add_vertex_face_contact(
                    &c1, &c2, c1pos - c1_up - k * normal, normal, dist);
              } else {
                const real_t k(c2.get_radius() + real_t(0.5) * dist);
                contacts.add_vertex_face_contact(
                    &c1, &c2, c2pos - half_length_2 * c1_up_dir + k * normal,
                    normal, dist);
              }

            }
            // Creating a single contact point
            else {
              const real_t k(c2.get_radius() + real_t(0.5) * dist);
              const Vec3 gPos(cp2 + normal * k);

              using namespace ::elastica::logging;
              log<debug>([&](auto& log) {
                log << "      Single contact created between capsule "
                    << c1.getID() << " and capsule " << c2.getID()
                    << " (dist=" << dist << ")";
              });

              contacts.add_vertex_face_contact(&c1, &c2, gPos, normal, dist);
            }
          }
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Contact generation between two colliding capsule
         * primitives.
         * \ingroup fine_detection
         *
         * \details
         * In case of two colliding capsules one or two contact points may be
         * generated. In order to estimate if the two capsules are colliding,
         * the closest points on both centerlines are calculated (see
         * elastica::get_closest_line_segment_points). If the distance of these
         * two points is smaller than the sum of their radii, the two capsules
         * are colliding. In case they are lying parallel to each other and
         * touch each other along their cylindrical part, two contact points are
         * generated. In all other cases, a single contact point between the
         * calculated closest points is generated.
         *
         * \param c1 The first colliding capsule.
         * \param c2 The second colliding capsule.
         * \param contacts Contact container for the generated contacts.
         * \return void
         */
        template <typename CC>
        void do_collide(CapsulePrimitive& c1, CapsulePrimitive& c2,
                        CC& contacts) noexcept {
          do_collide_with_capsule(
              static_cast<detail::CapsulePrimitiveBase&>(c1),
              static_cast<detail::CapsulePrimitiveBase&>(c2), contacts);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Contact generation between colliding capsule primitives, one
         * from rigid bodies and one from Cosserat rods
         * \ingroup fine_detection
         *
         * \details
         * In case of two colliding capsules one or two contact points may be
         * generated. In order to estimate if the two capsules are colliding,
         * the closest points on both centerlines are calculated (see
         * elastica::get_closest_line_segment_points). If the distance of these
         * two points is smaller than the sum of their radii, the two capsules
         * are colliding. In case they are lying parallel to each other and
         * touch each other along their cylindrical part, two contact points are
         * generated. In all other cases, a single contact point between the
         * calculated closest points is generated.
         *
         * \param c1 The first colliding capsule.
         * \param c2 The second colliding capsule.
         * \param contacts Contact container for the generated contacts.
         * \return void
         */
        template <typename CC>
        void do_collide(CapsulePrimitive& c1, CapsuleFromRodPrimitive& c2,
                        CC& contacts) noexcept {
          do_collide_with_capsule(
              static_cast<detail::CapsulePrimitiveBase&>(c1),
              static_cast<detail::CapsulePrimitiveBase&>(c2), contacts);
        }
        //**********************************************************************

      }  // namespace fine

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica
