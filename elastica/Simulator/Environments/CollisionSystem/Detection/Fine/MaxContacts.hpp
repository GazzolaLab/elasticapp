//=================================================================================================
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
//=================================================================================================

#pragma once

//#include "EPA.hpp"
//#include "GJK.hpp"
#include "elastica/Simulator/Thresholds.hpp"
#include "elastica/Utilities/DefineTypes.h"
#include "elastica/Utilities/Logging.hpp"
#include "elastica/Utilities/Math/Dot.hpp"
#include "elastica/Utilities/Math/Normalize.hpp"
#include "elastica/Utilities/Math/SqrLength.hpp"
#include "elastica/Utilities/Math/Trans.hpp"
#include "elastica/Utilities/Math/Vec3.hpp"
#include "elastica/Utilities/NonCreatable.hpp"
#include "elastica/Utilities/TMPL.hpp"

// Include list of primitives here
#include "elastica/Simulator/Environments/Primitives.hpp"

/*
 * TODO : Compile time checking of all functions based on given primtive
 * ordering
 *
 * NOTE : This is similar to "multiple dispatch", but achieved statically
 */

namespace elastica {

  namespace collision {

    namespace detection {

      namespace fine {

        //=================================================================================================
        //
        //  CLASS DEFINITION
        //
        //=================================================================================================

        //*************************************************************************************************
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
          // order as specified here
          using PrimitiveOrdering = tmpl::list<SpherePrimitive, PlanePrimitive>;

          template <typename CC>
          static inline void collide(SpherePrimitive& s1, SpherePrimitive& s2,
                                     CC& contacts) noexcept;
          template <typename CC>
          static inline void collide(SpherePrimitive& s, PlanePrimitive& p,
                                     CC& contacts) noexcept;
          //          template <typename CC>
          //          static void collide(SpherePrimitive& s, CylinderPrimitive&
          //          c,
          //                              CC& contacts);
          //          template <typename CC>
          //          static void collide(CylinderPrimitive& c1,
          //          CylidnerPrimitive& c2,
          //                              CC& contacts);

         protected:
          //**Contact generation utility
          // functions********************************************************
          /*!\name Contact generation utility functions */
          //@{
          //          template <typename Type1, typename Type2>
          //          static inline bool gjkEPAcollideHybrid(Type1 geom1, Type2
          //          geom2,
          //                                                 Vec3& normal,
          //                                                 Vec3& contactPoint,
          //                                                 real&
          //                                                 penetrationDepth);
          //
          //          template <typename Type1, typename Type2>
          //          static inline bool gjkEPAcollide(Type1 geom1, Type2 geom2,
          //                                           Vec3& normal, Vec3&
          //                                           contactPoint, real&
          //                                           penetrationDepth);

          //@}
          //**********************************************************************************************
        };

        //================================================================================================
        //
        // CONTACT GENERATION UTILITY FUNCTIONS
        //
        //================================================================================================

        //*************************************************************************************************
        /*!\brief TODO
         */
        //        template <typename Type1, typename Type2>
        //        inline bool MaxContacts::gjkEPAcollideHybrid(Type1 geom1,
        //        Type2 geom2,
        //                                                     Vec3& normal,
        //                                                     Vec3&
        //                                                     contactPoint,
        //                                                     real&
        //                                                     penetrationDepth)
        //                                                     {
        //          // For more information on hybrid GJK/EPA see page 166 in
        //          "Collision
        //          // Detecton in Interactive 3D Environments" by Gino van den
        //          Bergen.
        //
        //          GJK gjk;
        //          penetrationDepth =
        //              gjk.doGJK<Type1, Type2>(geom1, geom2, normal,
        //              contactPoint);
        //          if (penetrationDepth > contactThreshold) {
        //            // not close enough create no contact
        //            return false;
        //          } else if (penetrationDepth < 0.01 * contactThreshold) {
        //            // objects are quite close use GJKcontactTrashold +
        //            // EPAcontactTrashold to calc distance
        //            if (gjk.doGJKcontactThreshold<Type1, Type2>(geom1, geom2))
        //            {
        //              // possible penetration
        //              EPA epa;
        //              return epa.doEPAcontactThreshold<Type1, Type2>(
        //                  geom1, geom2, gjk, normal, contactPoint,
        //                  penetrationDepth);
        //            }
        //          } else {
        //            // objects are close but separated use data calculated by
        //            GJK return true;
        //          }
        //          // never to be reached
        //          return false;
        //        }
        //*************************************************************************************************

        //*************************************************************************************************
        /*!\brief TODO
         */
        //        template <typename Type1, typename Type2>  // ID-Type of the
        //        geometry inline bool MaxContacts::gjkEPAcollide(Type1 geom1,
        //        Type2 geom2,
        //                                               Vec3& normal, Vec3&
        //                                               contactPoint, real&
        //                                               penetrationDepth) {
        //          GJK gjk;
        //          if (gjk.doGJKcontactThreshold<Type1, Type2>(geom1, geom2)) {
        //            // possible penetration
        //            EPA epa;
        //            return epa.doEPAcontactThreshold<Type1, Type2>(
        //                geom1, geom2, gjk, normal, contactPoint,
        //                penetrationDepth);
        //          }
        //
        //          return false;
        //        }
        //*************************************************************************************************

        //*************************************************************************************************
        /*!\brief Contact generation between two colliding Sphere primitives.
         * \ingroup contact_generation
         *
         * \param s1 The first colliding sphere.
         * \param s2 The second colliding sphere.
         * \param contacts Contact container for the generated contacts.
         * \return void
         *
         * In the case of a collision between two spheres, only a single contact
         * is generated. This contact point lies on the connection between both
         * centers of mass, relative to the radii of the spheres.
         *
         * \image html collideSphereSphere.png
         * \image latex collideSphereSphere.eps "Collision between two spheres"
         * width=236pt
         */
        template <typename CC>  // Type of the contact container
        inline void MaxContacts::collide(SpherePrimitive& s1,
                                         SpherePrimitive& s2,
                                         CC& contacts) noexcept {
          //          // Force a defined order of collision detection across
          //          processes if (s2->getSystemID() < s1->getSystemID())
          //            std::swap(s1, s2);

          Vec3 normal(s1.get_position() - s2.get_position());
          const real_t dist(sqrLength(normal) - s1.get_radius() -
                            s2.get_radius());

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
        //*************************************************************************************************

        //*************************************************************************************************
        /*!\brief Contact generation between a Sphere and a Plane.
         * \ingroup contact_generation
         *
         * \param s The colliding sphere.
         * \param p The colliding plane.
         * \param contacts Contact container for the generated contacts.
         * \return void
         *
         * In the case of a contact between a sphere and a plane, only a single
         * contact point is generated. The contact point is calculated with a
         * projection of the global position of the sphere's center of mass and
         * the plane's normal, from which follows the distance between the
         * sphere and the plane.
         */
        template <typename CC>  // Type of the contact container
        inline void MaxContacts::collide(SpherePrimitive& s, PlanePrimitive& p,
                                         CC& contacts) noexcept {
          // Project distance between the sphere and plane on the plane's normal
          Vec3 normal(p.get_normal());
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

      }  // namespace fine

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica
