//==============================================================================
/*!
 *  \file pe/core/detection/fine/MaxContacts.h
 *  \brief Contact generation module
 *
 *  Copyright (C) 2009 Klaus Iglberger
 *                2013-2014 Tobias Scharpff
 *
 *  This file is part of pe.
 *
 *  pe is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 *  pe is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with
 * pe. If not, see <http://www.gnu.org/licenses/>.
 */
//==============================================================================

#pragma once

#include "elastica/Simulator/Environments/CollisionSystem/Detection/Fine/EPA.hpp"
#include "elastica/Simulator/Environments/CollisionSystem/Detection/Fine/GJK.hpp"
#include "elastica/Simulator/Thresholds.hpp"
#include "elastica/Utilities/DefineTypes.h"
#include "elastica/Utilities/ForceInline.hpp"
#include "elastica/Utilities/Logging.hpp"
#include "elastica/Utilities/Math/Dot.hpp"
#include "elastica/Utilities/Math/Length.hpp"
#include "elastica/Utilities/Math/Normalize.hpp"
#include "elastica/Utilities/Math/SqrLength.hpp"
#include "elastica/Utilities/Math/Trans.hpp"
#include "elastica/Utilities/Math/Vec3.hpp"
#include "elastica/Utilities/NonCreatable.hpp"
#include "elastica/Utilities/ProtocolHelpers.hpp"
#include "elastica/Utilities/Requires.hpp"
#include "elastica/Utilities/TMPL.hpp"
#include "elastica/Utilities/TypeTraits.hpp"

// Include list of primitives here
#include "elastica/Simulator/Environments/Primitives.hpp"
#include "elastica/Simulator/Environments/Primitives/GeomTools.hpp"
#include "elastica/Simulator/Environments/Primitives/Protocols.hpp"
#include "elastica/Simulator/Environments/Primitives/TypeTraits.hpp"

/*
 * TODO : Compile time checking of all functions based on given primitive
 * ordering
 *
 * NOTE : This is similar to "multiple dispatch", but achieved statically
 */

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
         * The MaxContacts class is part of the fine collision detection module
         * of the \b pe physics engine and provides the functionality to create
         * contacts between colliding rigid bodies. The primary focus of this
         * class is physical accuracy and therefore it creates the maximal
         * number of contact points necessary to handle collisions between rigid
         * bodies physically accurate (hence the name of the class).
         */
        class MaxContacts : private NonCreatable {
         public:
          // To avoid repeated functions for symmetry, we sort the input
          // typelist by giving a ordering here. We are then assured that
          // the primitives to the collide function arrive from left to right
          // order as specified
          // !NOTE : This is no longer necessary given our symmetric collide
          // functions below
          using PrimitiveOrdering =
              tmpl::list<ApproximateCylinderPrimitive, SpherePrimitive,
                         CylinderPrimitive, CapsulePrimitive, PlanePrimitive>;

          //**Contact interface functions***********************************
          /*!\name Contact interface functions */
          //@{

          // Default, FirstPrimitive < SecondPrimitive, so the primitives arrive
          // in the correct order
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

          // FirstPrimitive can be found further down the typelist than
          // SecondPrimitive, so invert the order of arguments
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
          //@}
          //********************************************************************

         protected:
          //**Contact generation functions**************************************
          /*!\name Contact generation functions */
          //@{

          // Specializations
          template <typename CC>  // Type of the contact container
          static inline void do_collide(ApproximateCylinderPrimitive& c1,
                                        ApproximateCylinderPrimitive& c2,
                                        CC& contacts) noexcept;
          template <typename CC>  // Type of the contact container
          static inline void do_collide(ApproximateCylinderPrimitive& c,
                                        SpherePrimitive& s,
                                        CC& contacts) noexcept;
          /* GJK collision
          template <typename CC>  // Type of the contact container
          static inline void do_collide(ApproximateCylinderPrimitive& c1,
                                        CylinderPrimitive& c2,
                                        CC& contacts) noexcept;
                                        */
          /* GJK collision, its not clear how the end points of the
           * approximate cylinder should behave. For now we treat
           * the approximate cylinder as a capsule : this is probably
           * the right thing to do since the tip of the rods are
           * capsulish?
          template <typename CC>  // Type of the contact container
          static inline void do_collide(ApproximateCylinderPrimitive& c1,
                                        CapsulePrimitive& c2,
                                        CC& contacts) noexcept;
                                        */
          template <typename CC>  // Type of the contact container
          static inline void do_collide(SpherePrimitive& s1,
                                        SpherePrimitive& s2,
                                        CC& contacts) noexcept;
          template <typename CC>  // Type of the contact container
          static inline void do_collide(SpherePrimitive& s, PlanePrimitive& p,
                                        CC& contacts) noexcept;

          // Default case, not marked noexcept because GJK and EPA can throw
          template <
              typename FirstPrimitive,   // Type of the first primitive
              typename SecondPrimitive,  // Type of the second primitive
              typename CC,               // Type of the contact container
              Requires<cpp17::conjunction_v<
                  tt::is_rigid_primitive_v<FirstPrimitive>,
                  ::tt::conforms_to_v<FirstPrimitive,
                                      protocols::ConvexPrimitive>,
                  tt::is_rigid_primitive_v<SecondPrimitive>,
                  ::tt::conforms_to_v<SecondPrimitive,
                                      protocols::ConvexPrimitive>>> = nullptr>
          static inline void do_collide(FirstPrimitive& f, SecondPrimitive& s,
                                        CC& contacts);
          //@}
          //********************************************************************
          //          template <typename CC>
          //          static void collide(SpherePrimitive& s, CylinderPrimitive&
          //          c,
          //                              CC& contacts);
          //          template <typename CC>
          //          static void collide(CylinderPrimitive& c1,
          //          CylinderPrimitive& c2,
          //                              CC& contacts);

          //**Contact generation utility functions******************************
          /*!\name Contact generation utility functions */
          //@{
          template <typename FirstPrimitive,   // Type of the first primitive
                    typename SecondPrimitive>  // Type of the second primitive
          static inline bool do_hybrid_GJK_EPA_collide(
              FirstPrimitive& first, SecondPrimitive& second, Vec3& normal,
              Vec3& contact_point, real_t& penetration_depth);

          template <typename FirstPrimitive,   // Type of the first primitive
                    typename SecondPrimitive>  // Type of the second primitive
          static inline bool do_GJK_EPA_collide(FirstPrimitive& first,
                                                SecondPrimitive& second,
                                                Vec3& normal,
                                                Vec3& contact_point,
                                                real_t& penetration_depth);

          //@}
          //********************************************************************
        };

        //======================================================================
        //
        // CONTACT GENERATION UTILITY FUNCTIONS
        //
        //======================================================================

        //**********************************************************************
        /*!\brief TODO
         */
        template <typename FirstPrimitive,   // Type of the first primitive
                  typename SecondPrimitive>  // Type of the second primitive
        inline bool MaxContacts::do_hybrid_GJK_EPA_collide(
            FirstPrimitive& first, SecondPrimitive& second, Vec3& normal,
            Vec3& contact_point, real_t& penetration_depth) {
          /*
           * For more information on hybrid GJK/EPA see page 166 in
           * "Collision Detecton in Interactive 3D Environments" by Gino van den
           * Bergen.
           */

          GJK gjk;
          penetration_depth = gjk.doGJK<FirstPrimitive, SecondPrimitive>(
              first, second, normal, contact_point);
          if (penetration_depth > contact_threshold) {
            // not close enough create no contact
            return false;
          } else if (penetration_depth < 0.01 * contact_threshold) {
            // objects are quite close use GJKcontactTrashold +
            // EPAcontactTrashold to calc distance
            if (gjk.do_contact_threshold<FirstPrimitive, SecondPrimitive>(
                    first, second)) {
              // possible penetration
              EPA epa;
              return epa.do_contact_threshold<FirstPrimitive, SecondPrimitive>(
                  first, second, gjk, normal, contact_point, penetration_depth);
            }
          } else {
            // objects are close but separated use data calculated by GJK
            return true;
          }
          // never to be reached
          return false;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief TODO
         */
        template <typename FirstPrimitive,   // Type of the first primitive
                  typename SecondPrimitive>  // Type of the second primitive
        inline bool MaxContacts::do_GJK_EPA_collide(FirstPrimitive& first,
                                                    SecondPrimitive& second,
                                                    Vec3& normal,
                                                    Vec3& contact_point,
                                                    real_t& penetration_depth) {
          GJK gjk;
          if (gjk.do_contact_threshold<FirstPrimitive, SecondPrimitive>(
                  first, second)) {
            // possible penetration
            EPA epa;
            return epa.do_contact_threshold<FirstPrimitive, SecondPrimitive>(
                first, second, gjk, normal, contact_point, penetration_depth);
          }

          return false;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Contact generation between two colliding approximate cylinder
         * primitives.
         * \ingroup contact_generation
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
         */
        template <typename CC>  // Type of the contact container
        inline void MaxContacts::do_collide(ApproximateCylinderPrimitive& c1,
                                            ApproximateCylinderPrimitive& c2,
                                            CC& contacts) noexcept {
          const Vec3& c1pos(c1.get_position());
          const Vec3& c2pos(c2.get_position());

          const Rot3& Q1(c1.get_rotation());
          const Rot3& Q2(c2.get_rotation());

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
                contacts.addVertexFaceContact(
                    &c1, &c2, c1pos + c1_up - k * normal, normal, dist);
              } else {
                // Cannot reuse c2_up as c2_up_dir and c1_up_dir can be
                // antiparallel (if parallel there are no issues)
                contacts.addVertexFaceContact(
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
                contacts.addVertexFaceContact(
                    &c1, &c2, c1pos - c1_up - k * normal, normal, dist);
              } else {
                contacts.addVertexFaceContact(
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

              contacts.addVertexFaceContact(&c1, &c2, gPos, normal, dist);
            }
          }
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Contact generation between an approximate cylinder and
         * sphere \ingroup contact_generation
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
         */
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
          const Rot3& Q(c.get_rotation());
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

            contacts.addVertexFaceContact(&c, &s, gPos, normal, dist);
          }
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Contact generation between two colliding Sphere primitives.
         * \ingroup contact_generation
         *
         * \param s1 The first colliding sphere.
         * \param s2 The second colliding sphere.
         * \param contacts Contact container for the generated contacts.
         * \return void
         *
         * In the case of a collision between two spheres, only a single
         * contact is generated. This contact point lies on the connection
         * between both centers of mass, relative to the radii of the spheres.
         *
         * \image html collideSphereSphere.png
         * \image latex collideSphereSphere.eps "Collision between two
         * spheres" width=236pt
         */
        template <typename CC>  // Type of the contact container
        inline void MaxContacts::do_collide(SpherePrimitive& s1,
                                            SpherePrimitive& s2,
                                            CC& contacts) noexcept {
          //          // Force a defined order of collision detection across
          //          processes if (s2->getSystemID() < s1->getSystemID())
          //            std::swap(s1, s2);

          // Ordering : Normal from 2->1 always
          Vec3 normal(s1.get_position() - s2.get_position());
          const real_t dist(length(normal) - s1.get_radius() - s2.get_radius());

          if (dist < contact_threshold) {
            normal = normalize(normal);
            const real_t k(s2.get_radius() + real_t(0.5) * dist);
            const Vec3 gPos(s2.get_position() + normal * k);

            LOG_DEBUG_SECTION(log) {
              log << "      Contact created between sphere " << s1.getID()
                  << " and sphere " << s2.getID() << " (dist=" << dist << ")";
            }

            contacts.addVertexFaceContact(&s1, &s2, gPos, normal, dist);
          }
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Contact generation between a Sphere and a Plane.
         * \ingroup contact_generation
         *
         * \param s The colliding sphere.
         * \param p The colliding plane.
         * \param contacts Contact container for the generated contacts.
         * \return void
         *
         * In the case of a contact between a sphere and a plane, only a
         * single contact point is generated. The contact point is calculated
         * with a projection of the global position of the sphere's center of
         * mass and the plane's normal, from which follows the distance
         * between the sphere and the plane.
         */
        template <typename CC>  // Type of the contact container
        inline void MaxContacts::do_collide(SpherePrimitive& s,
                                            PlanePrimitive& p,
                                            CC& contacts) noexcept {
          // Project distance between the sphere and plane on the plane's
          // normal

          // Ordering : Normal from 2->1 always
          const Vec3 normal(p.get_normal());
          //          std::cout << "Normal is" << normal << std::endl;
          const real_t dist(
              std::abs(dot(normal, s.get_position() - p.get_position())) -
              s.get_radius());

          if (dist < contact_threshold) {
            // Cannot use plane's center here as plane is wide af
            const Vec3 gPos(s.get_position() -
                            (s.get_radius() + dist) * normal);

            LOG_DEBUG_SECTION(log) {
              log << "      Contact created between sphere " << s.getID()
                  << " and plane " << p.getID() << " (dist=" << dist << ")";
            }

            contacts.addVertexFaceContact(&s, &p, gPos, normal, dist);
          }
        }

        template <
            typename FirstPrimitive, typename SecondPrimitive, typename CC,
            Requires<cpp17::conjunction_v<
                tt::is_rigid_primitive_v<FirstPrimitive>,
                ::tt::conforms_to_v<FirstPrimitive, protocols::ConvexPrimitive>,
                tt::is_rigid_primitive_v<SecondPrimitive>,
                ::tt::conforms_to_v<SecondPrimitive,
                                    protocols::ConvexPrimitive>>>>
        void MaxContacts::do_collide(FirstPrimitive& f, SecondPrimitive& s,
                                     CC& contacts) {
          Vec3 normal;
          Vec3 contact_point;
          real_t penetration_depth;
          if (do_hybrid_GJK_EPA_collide<FirstPrimitive, SecondPrimitive>(
                  f, s, normal, contact_point, penetration_depth)) {
            // bodys possibly overlap
            // normal points form object2 (m) to object1 (b)
            contacts.addVertexFaceContact(&f, &s, contact_point, normal,
                                          penetration_depth);
            LOG_DEBUG_SECTION(log) {
              log << "      Contact created between box " << f.getID()
                  << " and triangle mesh " << s.getID()
                  << " (dist=" << penetration_depth << ")";
            }
          }
        }

      }  // namespace fine

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica
