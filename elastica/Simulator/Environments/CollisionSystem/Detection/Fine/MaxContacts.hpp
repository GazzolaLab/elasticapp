//==============================================================================
/*!
//  \file
//  \brief Implementation of the 'MaxContacts' fine collision detection
//  algorithm
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

// Primitives
#include "Simulator/Environments/Primitives.hpp"
// Kernels
#include "Simulator/Environments/CollisionSystem/Detection/Fine/Kernels.hpp"
// Utilities
#include "Utilities/ForceInline.hpp"
#include "Utilities/NonCreatable.hpp"
#include "Utilities/Requires.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace collision {

    namespace detection {

      namespace fine {

        //======================================================================
        //
        //  CLASS DEFINITION
        //
        //======================================================================

        //**********************************************************************
        /*!\brief Contact generation module.
         * \ingroup fine_collision_detection
         *
         * \details
         * The MaxContacts class is in essence the fine collision detection
         * module and provides the functionality to create contacts between
         * colliding rigid bodies. The primary focus of this class is physical
         * accuracy and therefore it creates the maximal number of contact
         * points necessary to handle collisions between rigid bodies physically
         * accurate (hence the name of the class).
         *
         * Developer note:
         * This class is not OCP friendly, as it cannot be really extended. This
         * is by design as we don't have many primitives for which an exact,
         * geometrical fine contact detection algorithm can be coded. Hence
         * extending this class probably won't be the right fit.
         */
        class MaxContacts : private NonCreatable {
         public:
          //**Type definitions**************************************************
          /* To avoid repeated functions for symmetry, we sort the input
           * typelist by giving a ordering here. We are then assured that
           * the primitives to the collide function arrive from left to right
           * order as specified
           */
          //! A typelist of all primitives in \elastica with ordering
          using PrimitiveOrdering =
              tmpl::list<SpherePrimitive, CylinderPrimitive, CapsulePrimitive,
                         CapsuleFromRodPrimitive, PlanePrimitive,
                         HalfSpacePrimitive>;
          //********************************************************************

          //**Contact generation functions**************************************
          /*!\name Contact generation functions */
          //@{

          //********************************************************************
          /*!\brief Contact generation between two colliding rigid primitives.
           * \ingroup fine_collision_detection
           *
           * \param first The first colliding primitive.
           * \param second The second colliding primitive.
           * \param contacts Contact container for the generated contacts.
           * \return void
           *
           * Calculation of all necessary contact points between two rigid
           * primitives. This function works as a dispatch to specialized
           * contact functions based on the actual type of the rigid bodies.
           *
           * \note
           * This function assumes that the two given rigid bodies are
           * close to each other and therefore potentially colliding. No further
           * coarse collision detection test (like checking the axis-aligned
           * bounding boxes of the two bodies) is applied!
           */
          template <typename FirstPrimitive,   // Type of the first primitive
                    typename SecondPrimitive,  // Type of the second primitive
                    typename CC,               // Type of the contact container
                    typename Enabler = std::nullptr_t>
          static ELASTICA_ALWAYS_INLINE auto collide(FirstPrimitive& first,
                                                     SecondPrimitive& second,
                                                     CC& contacts) noexcept
              -> void {
            do_collide(first, second, contacts);
          }
          //********************************************************************

          //********************************************************************
          /*! \cond ELASTICA_INTERNAL */
          /*!\brief Contact generation between two colliding rigid primitives.
           * \ingroup fine_collision_detection
           *
           * Overload for mis-ordered primitives (does not occur, so why is this
           * needed?)
           */
          template <
              typename FirstPrimitive,   // Type of the first primitive
              typename SecondPrimitive,  // Type of the second primitive
              typename CC,               // Type of the contact container
              Requires<
                  (tmpl::index_of<PrimitiveOrdering, FirstPrimitive>::value >
                   tmpl::index_of<PrimitiveOrdering, SecondPrimitive>::value)>>
          static ELASTICA_ALWAYS_INLINE auto collide(FirstPrimitive& first,
                                                     SecondPrimitive& second,
                                                     CC& contacts) noexcept
              -> void {
            do_collide(second, first, contacts);
          }
          /*! \endcond */
          //********************************************************************

          //@}
          //********************************************************************
        };

        //**********************************************************************
        /*!\brief Contact generation between two colliding approximate cylinder
         * primitives.
         * \ingroup fine_detection
         *
         * \param c1 The first colliding approximate cylinder.
         * \param c2 The second colliding approximate cylinder.
         * \param contacts Contact container for the generated contacts.
         * \return void
         *
         * In case of two colliding approximate cylinders one or two contact
         * points may be generated. In order to estimate if the two approximate
         * cylinders are colliding, the closest points on both centerlines are
         * calculated (see elastica::get_closest_line_segment_points). If the
         * distance of these two points is smaller than the sum of their radii,
         * the two approximate cylinders are colliding. In case they are lying
         * parallel to each other and touch each other along their cylindrical
         * part, two contact points are generated. In all other cases, a single
         * contact point between the calculated closest points is generated.
         *
         * DEPRECATED!
        template <typename CC>  // Type of the contact container
        inline void MaxContacts::do_collide(ApproximateCylinderPrimitive& c1,
                                            ApproximateCylinderPrimitive& c2,
                                            CC& contacts) noexcept {
          const Vec3& c1pos(c1.get_position());
          const Vec3& c2pos(c2.get_position());

          const Rot3& Q1(c1.get_rotation_matrix());
          const Rot3& Q2(c2.get_rotation_matrix());

          // Calculating the "up" direction for both approx. cylinder that
          // points from the center of mass to the center of the upper disc of
          // each approx. cylinder.
          const Vec3 c1_up_dir{Q1(2, 0), Q1(2, 1), Q1(2, 2)};
          const Vec3 c2_up_dir{Q2(2, 0), Q2(2, 1), Q2(2, 2)};

          // Calculating the displacement of the center of the upper cap-spheres
          // in world space coordinates
          const Vec3 c1_up(real_t(0.5) * c1.get_length() * c1_up_dir);
          const Vec3 c2_up(real_t(0.5) * c2.get_length() * c2_up_dir);

          // calculate the closest points of the two center lines
          Vec3 cp1, cp2;
          get_closest_line_segment_points(c1pos + c1_up, c1pos - c1_up,
                                          c2pos + c2_up, c2pos - c2_up, cp1,
                                          cp2);

          Vec3 normal(cp1 - cp2);
          const real_t dist(length(normal) - c1.get_radius() - c2.get_radius());
          if (dist < contact_threshold) {
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
              const real_t k(c1.get_radius() + real_t(0.5) * dist);
              const real_t half_length_1(real_t(0.5) * c1.get_length());
              const real_t half_length_2(real_t(0.5) * c2.get_length());

              LOG_DEBUG_SECTION(log) {
                log << "      First contact created between approximate "
                       "cylinder "
                    << c1.getID() << " and approximate cylinder " << c2.getID()
                    << " (dist=" << dist << ")";
              }

              // Creating the "upper" contact point in world coordinates
              if (half_length_1 < c2x + half_length_2) {
                contacts.add_vertex_face_contact(
                    &c1, &c2, c1pos + c1_up - k * normal, normal, dist);
              } else {
                // Cannot reuse c2_up as c2_up_dir and c1_up_dir can be
                // antiparallel (if parallel there are no issues)
                contacts.add_vertex_face_contact(
                    &c1, &c2, c2pos + half_length_2 * c1_up_dir + k * normal,
                    normal, dist);
              }

              LOG_DEBUG_SECTION(log) {
                log << "      Second contact created between approximate "
                       "cylinder "
                    << c1.getID() << " and approximate cylinder " << c2.getID()
                    << " (dist=" << dist << ")";
              }

              // Creating the "lower" contact point in world coordinates
              if (-half_length_1 > c2x - half_length_2) {
                contacts.add_vertex_face_contact(
                    &c1, &c2, c1pos - c1_up - k * normal, normal, dist);
              } else {
                contacts.add_vertex_face_contact(
                    &c1, &c2, c2pos - half_length_2 * c1_up_dir + k * normal,
                    normal, dist);
              }

            }
            // Creating a single contact point
            else {
              const real_t k(c2.get_radius() + real_t(0.5) * dist);
              const Vec3 gPos(cp2 + normal * k);

              LOG_DEBUG_SECTION(log) {
                log << "      Single contact created between approximate "
                       "cylinder "
                    << c1.getID() << " and approximate cylinder " << c2.getID()
                    << " (dist=" << dist << ")";
              }

              contacts.add_vertex_face_contact(&c1, &c2, gPos, normal, dist);
            }
          }
        }
        */
        //**********************************************************************

        //**********************************************************************
        /*!\brief Contact generation between an approximate cylinder and
         * sphere \ingroup fine_detection
         *
         * \param c The colliding approximate cylinder.
         * \param s The colliding sphere.
         * \param contacts Contact container for the generated contacts.
         * \return void
         *
         * In case of a contact between a sphere and an approximate cylinder,
         * a single contact point is generated.
         * This contact point is calculated by first estimating a sphere
         * within the capsule whose center of mass is on the centerline of the
         * capsule and closest to the center of mass of the colliding sphere.
         * After this, a sphere-sphere collision is performed between the
         * capsule representation and the colliding sphere.
         *
         * \image html collideSphereSphere.png
         * \image latex collideSphereSphere.eps "Collision between two
         * spheres" width=236pt
         *
         * DEPRECATED
         *
        template <typename CC>  // Type of the contact container
        inline void MaxContacts::do_collide(ApproximateCylinderPrimitive& c,
                                            SpherePrimitive& s,
                                            CC& contacts) noexcept {
          //          // Force a defined order of collision detection across
          //          processes if (s2->getSystemID() < s1->getSystemID())
          //            std::swap(s1, s2);

          // Half cylinder length
          const real_t half_cyl_length(real_t(0.5) * c.get_length());
          // Global position of the sphere
          const Vec3& spos(s.get_position());
          // Global position of the approximate cylinder
          const Vec3& cpos(c.get_position());
          // Rotation of the approximate cylinder
          const Rot3& Q(c.get_rotation_matrix());
          //          const Vec3 rp(Q * (spos - cpos));
          const real_t max_allowed_length(s.get_radius() + half_cyl_length -
                                          contact_threshold);

          // Calculating the relative position of the sphere along
          // the tangent  in the frame of the approximate cylinder
          //          const real_t rp_tangent = rp[2];
          const real_t rp_tangent(Q(2, 0) * (spos[0] - cpos[0]) +
                                  Q(2, 1) * (spos[1] - cpos[1]) +
                                  Q(2, 2) * (spos[2] - cpos[2]));

          // If the relative position along the tangent is > max length then
          // no contact possible
          if (std::fabs(rp_tangent) > max_allowed_length)
            return;

          // Compute the point of closest distance along centerline
          const Vec3 shifted_pos{rp_tangent * Q(2, 0) + cpos[0],
                                 rp_tangent * Q(2, 1) + cpos[1],
                                 rp_tangent * Q(2, 2) + cpos[2]};

          // Ordering : Normal from 2->1 always
          Vec3 normal(-(spos - shifted_pos));
          //          Vec3 normal{-rp[0], -rp[1], 0.0};
          const real_t dist(length(normal) - s.get_radius() - c.get_radius());

          if (dist < contact_threshold) {
            normal = normalize(normal);
            const real_t k(s.get_radius() + real_t(0.5) * dist);

            // We can use s.get_position() + normal * k
            // but the problem is that the normal as done before was in the
            // rotated FOR and not world frame
            const Vec3 gPos(spos + normal * k);

            LOG_DEBUG_SECTION(log) {
              log << "      Contact created between approximate cylinder "
                  << c.getID() << " and sphere " << s.getID()
                  << " (dist=" << dist << ")";
            }

            contacts.add_vertex_face_contact(&c, &s, gPos, normal, dist);
          }
        }
         */
        //**********************************************************************

      }  // namespace fine

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica
