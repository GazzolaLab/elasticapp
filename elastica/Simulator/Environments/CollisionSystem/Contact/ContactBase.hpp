//==============================================================================
/*!
//  \file
//  \brief A baseclass for recording Contacts
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
**/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <cstddef>  // size_t

#include "Simulator/Environments/CollisionSystem/Configuration.hpp"
///
#include "Simulator/Environments/CollisionSystem/Contact/Types.hpp"
//
#include "Simulator/Environments/CollisionSystem/Contact/ContactType.hpp"
///
#include "Simulator/Environments/CollisionSystem/Batches/ContactTrait.hpp"
#include "Simulator/Environments/CollisionSystem/Detection/Coarse/ContactTrait.hpp"
#include "Simulator/Environments/CollisionSystem/Detection/Fine/ContactTrait.hpp"
#include "Simulator/Environments/CollisionSystem/Response/ContactTrait.hpp"
///
#include "Simulator/Environments/Primitives/GeomPrimitive.hpp"
#include "Simulator/Environments/Primitives/RigidBodyPrimitive.hpp"
//
#include "Utilities/DefineTypes.h"
///
#include "Utilities/Math/Dot.hpp"
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/Thresholds.hpp"

namespace elastica {

  namespace collision {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Base class for contacts between rigid bodies.
     * \ingroup contact
     *
     * The ContactBase class represents the base class for all types of contacts
     * between rigid bodies in the simulation system. It provides the basic
     * functionality of a contact and is the base class for all types of
     * contacts between rigid bodies in the simulation system.
     */
    class ContactBase : public detection::coarse::ContactTrait<CollisionConfig>,
                        public detection::fine::ContactTrait<CollisionConfig>,
                        public batches::ContactTrait<CollisionConfig>,
                        public response::ContactTrait<CollisionConfig>,
                        private NonCopyable {
     private:
      //**Type definitions******************************************************
      //! The coarse collision detection contact trait base class.
      /*! This base class configures the ContactBase class for the requirements
         of the selected coarse collision detection algorithm. */
      using CCDCT = detection::coarse::ContactTrait<CollisionConfig>;
      //! Fine collision detection contact trait base class.
      /*! This base class configures the ContactBase class for the requirements
         of the selected fine collision detection algorithm. */
      using FCDCT = detection::fine::ContactTrait<CollisionConfig>;
      //! Batch generation contact trait base.
      /*! This base class configures the ContactBase class for the requirements
         of the selected batch generation algorithm. */
      using BCT = batches::ContactTrait<CollisionConfig>;
      //! Collision response contact trait base.
      /*! This base class configures the ContactBase class for the requirements
         of the selected collision response algorithm. */
      using RCT = response::ContactTrait<CollisionConfig>;
      //************************************************************************

     protected:
      //**Constructors**********************************************************
      /*!\name Constructors */
      //@{
      inline ContactBase(GeomID g1, GeomID g2, Vec3 const& gpos,
                         Vec3 const& normal, real_t dist);
      inline ContactBase(GeomID g1, GeomID g2, Vec3 const& gpos,
                         Vec3 const& normal, Vec3 const& e1, Vec3 const& e2,
                         real_t dist);
      ContactBase(ContactBase&&) noexcept = default;
      //@}
      //************************************************************************

      //**Destructor************************************************************
      /*!\name Destructor */
      //@{
      inline ~ContactBase();
      //@}
      //************************************************************************

     public:
      //**Get functions*********************************************************
      /*!\name Get functions */
      //@{
      //    inline bool isActive() const;
      inline bool is_penetrating() const;
      inline bool is_contacting(ConstBodyID body) const;

      // inline id_t get_ID() const;
      inline std::size_t get_index() const;
      inline BodyID get_first_body() const;
      inline BodyID get_second_body() const;
      inline const Vec3& get_position() const;
      inline const Vec3& get_normal() const;
      inline const Vec3 get_normal_temporal_derivative() const;
      inline real_t get_distance() const;
      inline real_t get_restitution() const;
      inline real_t get_stiffness() const;
      inline real_t get_normal_damping() const;
      inline real_t get_tangential_damping() const;
      //    inline real_t getCorParam() const;
      inline real_t get_friction() const;
      inline ContactType get_contact_type() const;

      inline real_t get_normal_relative_velocity() const;
      inline Vec3 get_relative_velocity() const;
      real_t get_normal_relative_acceleration() const;
      real_t get_normal_relative_force() const;
      //@}
      //************************************************************************

      //**Set functions*********************************************************
      /*!\name Set functions */
      //@{
      inline void set_index(std::size_t index);
      //@}
      //************************************************************************

     protected:
      //**Member variables******************************************************
      /*!\name Member variables */
      //@{
      /*!< If set to \a true, the contact is a vertex/face contact. If set
           to \a false, the contact is an edge/edge contact. */
      //! Vertex/face flag.
      bool vf_;
      //! User-specific contact ID, can be used for hashing in stiction
      // id_t id_;
      /*  During the setup of a contact graph, the contact is assigned to
       *  a batch of contacts. The batch index represents the index within
       *  this contact batch. The index is first assigned during the batch
       *  setup.
       */
      //! The current batch index of the contact.
      std::size_t index_;
      //! The first contacting rigid body.
      BodyID b1_;
      //! The second contacting rigid body.
      BodyID b2_;
      //! The global position of the contact.
      Vec3 gpos_;
      //! Normal of the contact.
      Vec3 normal_;
      /* The normal is defined within the global world frame and points
         from body 2 to body 1. */
      //! Edge direction of the colliding edge of body 1.
      Vec3 e1_;
      //! Edge direction of the colliding edge of body 2.
      Vec3 e2_;
      /* A negative distance means penetration of the two bodies. */
      //! Distance between the surfaces of the contacting rigid bodies.
      real_t dist_;
      //! Coefficient of restitution of the contact.
      real_t restitution_;
      //! Stiffness in normal direction of the contact region.
      real_t stiffness_;
      //! Damping coefficient in normal direction of the contact region.
      real_t dampingN_;
      //! Damping coefficient in tangential direction of the contact region.
      real_t dampingT_;
      //! Coefficient of friction of the contact.
      real_t friction_;
      //! Contact counter.
      // static id_t nextContact_;
      //@}
      //************************************************************************
    };
    //**************************************************************************

    //==========================================================================
    //
    //  CONSTRUCTORS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief ContactBase constructor for a vertex/face contact.
     *
     * \param g1 The first contacting geometric primitive.
     * \param g2 The second contacting geometric primitive.
     * \param gpos The global position of the contact.
     * \param normal The global normal of the contact (running from body 2 to
     * body 1).
     * \param dist The distance between the surfaces of the contacting
     * rigid bodies.
     */
    inline ContactBase::ContactBase(GeomID g1, GeomID g2, Vec3 const& gpos,
                                    Vec3 const& normal, real_t dist)
        : CCDCT(),  // Initialization of the coarse collision detection contact
                    // trait base class
          FCDCT(),  // Initialization of the fine collision detection contact
                    // trait base class
          BCT(),    // Initialization of the batch generation contact trait base
                    // class
          RCT(g1, g2, gpos, normal),  // Initialization of the collision
                                      // response contact trait base class
          vf_(true),                  // Vertex/face flag
          // id_(nextContact_++),        // User-specific contact ID
          index_(0),  // The current batch index of the contact
          b1_(static_cast<BodyID>(g1)),  // The first contacting rigid body
          b2_(static_cast<BodyID>(g2)),  // The second contacting rigid body
          gpos_(gpos),                   // Global position
          normal_(normal),               // Normal of the contact
          e1_(),        // Edge direction of the colliding edge of body 1
          e2_(),        // Edge direction of the colliding edge of body 2
          dist_(dist),  // Distance between the surfaces of the contacting rigid
                        // bodies
          /* From this point on below is initialized in Contact.hpp */
          restitution_(0.0),  // Coefficient of restitution
          stiffness_(
              0.0),  // Stiffness in normal direction of the contact region
          dampingN_(0.0),  // Damping coefficient in normal direction of the
                           // contact region
          dampingT_(0.0),  // Damping coefficient in tangential direction of the
                           // contact region
          friction_(0.0)   // Coefficient of friction
    {}
    //**************************************************************************

    //**************************************************************************
    /*!\brief ContactBase constructor for an edge/edge contact.
     *
     * \param g1 The first contacting geometric primitive.
     * \param g2 The second contacting geometric primitive.
     * \param gpos The global position of the contact.
     * \param normal The global normal of the contact (running from body 2 to
     * body 1).
     * \param e1 Edge direction of the colliding edge of the first
     * colliding rigid body.
     * \param e2 Edge direction of the colliding edge of
     * the second colliding rigid body.
     * \param dist The distance between the
     * surfaces of the contacting rigid bodies.
     */
    inline ContactBase::ContactBase(GeomID g1, GeomID g2, Vec3 const& gpos,
                                    Vec3 const& normal, Vec3 const& e1,
                                    Vec3 const& e2, real_t dist)
        : CCDCT(),  // Initialization of the coarse collision detection contact
                    // trait base class
          FCDCT(),  // Initialization of the fine collision detection contact
                    // trait base class
          BCT(),    // Initialization of the batch generation contact trait base
                    // class
          RCT(g1, g2, gpos, normal),  // Initialization of the collision
                                      // response contact trait base class
          vf_(false),                 // Vertex/face flag
          // id_(nextContact_++),        // User-specific contact ID
          index_(0),  // The current batch index of the contact
          b1_(static_cast<BodyID>(g1)),  // The first contacting rigid body
          b2_(static_cast<BodyID>(g2)),  // The second contacting rigid body
          gpos_(gpos),                   // Global position
          normal_(normal),               // Normal of the contact
          e1_(e1),      // Edge direction of the colliding edge of body 1
          e2_(e2),      // Edge direction of the colliding edge of body 2
          dist_(dist),  // Distance between the surfaces of the contacting rigid
                        // bodies
          /* From this point on below is initialized in Contact.hpp */
          restitution_(0.0),  // Coefficient of restitution
          stiffness_(
              0.0),  // Stiffness in normal direction of the contact region
          dampingN_(0.0),  // Damping coefficient in normal direction of the
                           // contact region
          dampingT_(0.0),  // Damping coefficient in tangential direction of the
                           // contact region
          friction_(0.0)   // Coefficient of friction
    {}
    //**************************************************************************

    //==========================================================================
    //
    //  DESTRUCTOR
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Destructor for the ContactBase class.
     */
    inline ContactBase::~ContactBase() {
      /*
       * One needs to be careful here, and might want to decrement the global
       * nextContact_ counter, however as it presently stands UINT_MAX can
       * overflow and becomes 0, so the program is still technically correct
       * (its not UB : https://stackoverflow.com/a/16188846)
       */
    }
    //**************************************************************************

    //==========================================================================
    //
    //  GET FUNCTIONS
    //
    //==========================================================================

    //  //**********************************************************************
    //  /*!\brief Returns whether the contact is active or not.
    //   *
    //   * \return \a true if the contact is active, \a false if it inactive.
    //   *
    //   * This function returns whether the contact is active or not. A contact
    //   is
    //   * considered active at least one of the contacting rigid bodies is
    //   active.
    //   */
    //  inline bool ContactBase::isActive() const {
    //    return b1_->isActive() or b2_->isActive();
    //  }
    //  //*************************************************************************************************
    //
    //**************************************************************************
    /*!\brief Checks if the two contacting rigid bodies are penetrating each
     * other.
     *
     * \return \a true if the two rigid bodies are penetrating each other, \a
     * false if not.
     *
     * The tolerance level of the check is pe::contactThreshold.
     */
    inline bool ContactBase::is_penetrating() const {
      return dist_ < -contact_threshold;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Returns whether the given rigid body is involved in this contact.
     *
     * \return \a true if the rigid body is involved in this contact, \a false
     * if not.
     */
    inline bool ContactBase::is_contacting(ConstBodyID body) const {
      return (b1_ == body or b2_ == body);
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Returns the ID of the contact.
     *
     * \return The contact ID.
     */
    // inline id_t ContactBase::get_ID() const { return id_; }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Returns the current batch index of the contact.
     *
     * \return The current batch index.
     */
    inline std::size_t ContactBase::get_index() const { return index_; }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Returns the first constrained rigid body.
     *
     *\return The first constrained rigid body.
     */
    inline BodyID ContactBase::get_first_body() const { return b1_; }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Returns the second constrained rigid body.
     *
     *\return The second constrained rigid body.
     */
    inline BodyID ContactBase::get_second_body() const { return b2_; }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Returns the global position of the contact.
     *
     * \return Global position of the contact.
     */
    inline const Vec3& ContactBase::get_position() const { return gpos_; }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Returns the global normal of the contact.
     *
     * \return Global normal of the contact.
     */
    inline const Vec3& ContactBase::get_normal() const { return normal_; }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Returns the derivative of the normal vector.
     *
     * \return Derivative of the normal vector.
     */
    inline const Vec3 ContactBase::get_normal_temporal_derivative() const {
      // TODO : What magic is this? It only takes body2's omega to get n dot
      if (vf_) {
        return b2_->get_angular_velocity() % normal_;
      } else {
        Vec3 const e1dot(b1_->get_angular_velocity() % e1_);
        Vec3 const e2dot(b2_->get_angular_velocity() % e2_);
        Vec3 const z(e1dot % e2_ + e1_ % e2dot);

        return z - ((z % normal_) % normal_);
      }
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Returns the distance between the surfaces of the contacting rigid
     * bodies.
     *
     * \return Distance between the surfaces of the contacting rigid bodies.
     */
    inline real_t ContactBase::get_distance() const { return dist_; }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Returns the restitution of the contact.
     *
     * \return Restitution of the contact.
     */
    inline real_t ContactBase::get_restitution() const { return restitution_; }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Returns the stiffness in normal direction of the contact.
     *
     * \return Stiffness in normal direction of the contact.
     */
    inline real_t ContactBase::get_stiffness() const { return stiffness_; }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Returns the damping coefficient in normal direction of the
     * contact.
     *
     * \return Damping coefficient in normal direction of the contact.
     */
    inline real_t ContactBase::get_normal_damping() const { return dampingN_; }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Returns the damping coefficient in tangential direction of the
     * contact.
     *
     * \return Damping coefficient in tangential direction of the contact.
     */
    inline real_t ContactBase::get_tangential_damping() const {
      return dampingT_;
    }
    //**************************************************************************

    //  //**********************************************************************
    //  /*!\brief Returns the error correction parameter.
    //   *
    //   * \return The error correction parameter.
    //   */
    //  inline real_t ContactBase::getCorParam() const {
    //    return (pe::errCorContact / TimeStep::size());
    //  }
    //  //**********************************************************************
    //
    //**************************************************************************
    /*!\brief Returns the coefficient of friction of the contact.
     *
     * \return Coefficient of friction of the contact.
     */
    inline real_t ContactBase::get_friction() const { return friction_; }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Returns the type of the contact.
     *
     * \return Type of the contact.
     *
     * This function returns the type of the contact: separating, resting or
     * colliding. The contact is classified according to the relative velocity
     * of the two touching rigid bodies in the contact:
     *
     *  - \f$ v_{rel} \f$ > 0: separating contact
     *  - \f$ v_{rel} \f$ = 0: resting contact
     *  - \f$ v_{rel} \f$ < 0: colliding contact\n
     *
     * (in order to classify the contact, pe::collision_threshold is used as
     * tolerance level).
     */
    inline ContactType ContactBase::get_contact_type() const {
      //      real_t vrel =
      //          trans(normal_) * (b1_->velocity_from_world_FOR(gpos_) -
      //          b2_->velocity_from_world_FOR(gpos_));
      real_t const vrel = get_normal_relative_velocity();

      if (vrel > collision_threshold)
        return ContactType::separating;
      else if (vrel > -collision_threshold)
        return ContactType::resting;
      else
        return ContactType::colliding;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Calculation of the normal relative velocity between the two
     * contacting rigid bodies.
     *
     * \return The relative velocity in normal direction.
     */
    inline real_t ContactBase::get_normal_relative_velocity() const {
      //      return trans(normal_) * (b1_->velocity_from_world_FOR(gpos_) -
      //      b2_->velocity_from_world_FOR(gpos_));
      return dot(normal_, get_relative_velocity());
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Calculation of the relative velocity between the two contacting
     * rigid bodies.
     *
     * \return The relative velocity.
     */
    inline Vec3 ContactBase::get_relative_velocity() const {
      return b1_->velocity_from_world_FOR(gpos_) -
             b2_->velocity_from_world_FOR(gpos_);
    }
    //**************************************************************************

    //==========================================================================
    //
    //  SET FUNCTIONS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Setting the batch index of the contact.
     *
     * \param index The new batch index of the contact.
     * \return void
     *
     * This function sets the batch index of the contact, i.e. the
     * identification number of the contact within a batch.
     */
    inline void ContactBase::set_index(std::size_t index) { index_ = index; }
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica
