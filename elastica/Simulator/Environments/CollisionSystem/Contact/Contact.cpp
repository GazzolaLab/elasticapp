//==============================================================================
/*!
//  \file
//  \brief Source file for the Contact class
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
//
//  This file contains implementation from the pe library with the following
//  copyrights
**
**  Project home: https://www.cs10.tf.fau.de/research/software/pe/
**
**  Copyright (C) 2009 Klaus Iglberger
**
**  This file is part of pe.
**
**  pe is free software: you can redistribute it and/or modify it under the
** terms of the GNU General Public License as published by the Free Software
** Foundation, either version 3 of the License, or (at your option) any later
** version.
**
**  pe is distributed in the hope that it will be useful, but WITHOUT ANY
** WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
** A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License along with
** pe. If not, see <http://www.gnu.org/licenses/>.
*/
//==============================================================================

#include "Contact.hpp"

#include <cmath>  // fabs

#include "Simulator/Materials/Materials.hpp"
#include "Utilities/Thresholds.hpp"
//
#include "ErrorHandling/Assert.hpp"
//
#include "Utilities/Logging.hpp"
#include "Utilities/Math/Dot.hpp"
#include "Utilities/Math/Normalize.hpp"
#include "Utilities/Math/SqrLength.hpp"

namespace elastica {

  namespace collision {

    //==========================================================================
    //
    //  DEFINITION AND INITIALIZATION OF THE STATIC MEMBER VARIABLES
    //
    //==========================================================================

    // TODO : Avoid static allocation
    // Contact::ContactPool Contact::contactPool_;

    //==========================================================================
    //
    //  CONSTRUCTORS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Contact constructor for a vertex/face contact.
     *
     * \param g1 The first contacting geometric primitive.
     * \param g2 The second contacting geometric primitive.
     * \param gpos The global position of the contact.
     * \param normal The global normal of the contact (running from body 2 to
     * body 1).
     * \param dist The distance between the surfaces of the contacting
     * rigid bodies.
     */
    Contact::Contact(GeomID g1, GeomID g2, Vec3 const& gpos, Vec3 const& normal,
                     real_t dist)
        : Parent(g1, g2, gpos, normal, dist) {
      using ::elastica::dot;

      ELASTICA_ASSERT(::elastica::is_normalized(normal),
                      "Invalid contact normal");
      ELASTICA_ASSERT(!(b1_->is_fixed() and b2_->is_fixed()),
                      "Invalid contact between two fixed rigid bodies");

      // Calculating the relative velocity
      Vec3 const rvel(
          b1_->velocity_from_world_FOR(gpos_) -
          b2_->velocity_from_world_FOR(gpos_));  // Relative velocity
      real_t const rvel_n(dot(normal_, rvel));   // Normal relative velocity
      real_t const rvel_t(
          dot(normal_,
              rvel - normal_ * rvel_n));  // Tangential relative velocity

      // Calculating the coefficient of restitution between the two colliding
      // (sub-)bodies In case the normal relative velocity between the two
      // colliding rigid bodies is smaller than the restitution threshold, a
      // coefficient of restitution of 0 is used to prevent an infinite number
      // of collisions during a single time step.
      if (std::fabs(rvel_n) > restitution_threshold) {
        restitution_ =
            Material::get_restitution(g1->get_material(), g2->get_material());
      }

      // Calculate the stiffness and damping parameters
      stiffness_ =
          Material::get_stiffness(g1->get_material(), g2->get_material());
      dampingN_ =
          Material::get_normal_damping(g1->get_material(), g2->get_material());
      dampingT_ = Material::get_tangential_damping(g1->get_material(),
                                                   g2->get_material());

      // Calculating the coefficient of friction
      // In case the tangential relative velocity between the two colliding
      // rigid bodies is smaller than the friction threshold, the coefficient of
      // static friction is used. Otherwise, the coefficient of dynamic friction
      // is used.
      if (std::fabs(rvel_t) > friction_threshold)
        friction_ = Material::get_dynamic_friction(g1->get_material(),
                                                   g2->get_material());
      else
        friction_ = Material::get_static_friction(g1->get_material(),
                                                  g2->get_material());

      // TODO Registering the contact with both attached rigid bodies, useful
      //      in creating batches
      //      if (not b1_->is_fixed())
      //        b1_->registerContact(this);
      //      if (not b2_->is_fixed())
      //        b2_->registerContact(this);

      // Merging the contact graph when both bodies are not fixed
      if (not(b1_->is_fixed() or b2_->is_fixed()))
        merge_nodes(b1_, b2_);
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Contact constructor for an edge/edge contact.
     *
     * \param g1 The first contacting geometric primitive.
     * \param g2 The second contacting geometric primitive.
     * \param gpos The global position of the contact.
     * \param normal The global normal of the contact (running from body 2 to
     * body 1).
     * \param e1 Edge direction of the colliding edge of the first colliding
     * rigid body. \param e2 Edge direction of the colliding edge of the second
     * colliding rigid body. \param dist The distance between the surfaces of
     * the contacting rigid bodies.
     */
    Contact::Contact(GeomID g1, GeomID g2, Vec3 const& gpos, Vec3 const& normal,
                     Vec3 const& e1, Vec3 const& e2, real_t dist)
        : Parent(g1, g2, gpos, normal, e1, e2,
                 dist)  // Initialization of the parent class
    {
      using ::elastica::dot;
      using ::elastica::sqrLength;

      ELASTICA_ASSERT(::elastica::is_normalized(normal),
                      "Invalid contact normal");
      // check for non-zero edges
      ELASTICA_ASSERT(sqrLength(e1) > real_t(0), "Invalid colliding edge");
      ELASTICA_ASSERT(sqrLength(e2) > real_t(0), "Invalid colliding edge");
      ELASTICA_ASSERT(!(Parent::b1_->is_fixed() and Parent::b2_->is_fixed()),
                      "Invalid contact between two fixed rigid bodies");

      // Calculating the relative velocity
      Vec3 const rvel(
          b1_->velocity_from_world_FOR(gpos_) -
          b2_->velocity_from_world_FOR(gpos_));  // Relative velocity
      real_t const rvel_n(dot(normal_, rvel));   // Normal relative velocity
      real_t const rvel_t(
          dot(normal_,
              rvel - normal_ * rvel_n));  // Tangential relative velocity

      // Calculating the coefficient of restitution between the two colliding
      // (sub-)bodies In case the normal relative velocity between the two
      // colliding rigid bodies is smaller than the restitution threshold, a
      // coefficient of restitution of 0 is used to prevent an infinite number
      // of collisions during a single time step.
      if (std::fabs(rvel_n) > restitution_threshold) {
        restitution_ =
            Material::get_restitution(g1->get_material(), g2->get_material());
      }

      // Calculating the coefficient of friction
      // In case the tangential relative velocity between the two colliding
      // rigid bodies is smaller than the friction threshold, the coefficient of
      // static friction is used. Otherwise, the coefficient of dynamic friction
      // is used.
      friction_ = (std::fabs(rvel_t) > friction_threshold)
                      ? Material::get_dynamic_friction(g1->get_material(),
                                                       g2->get_material())
                      : Material::get_static_friction(g1->get_material(),
                                                      g2->get_material());

      // // TODO Registering the contact with both attached rigid bodies
      //      if (!b1_->isFixed())
      //        b1_->registerContact(this);
      //      if (!b2_->isFixed())
      //        b2_->registerContact(this);

      // Merging the contact graph
      if (!b1_->is_fixed() and !b2_->is_fixed())
        merge_nodes(b1_, b2_);
    }
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica
