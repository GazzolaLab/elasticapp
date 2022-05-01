//==============================================================================
/*!
//  \file
//  \brief Base class for rigid environment primitives
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

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <vector>

#include "Simulator/Environments/CollisionSystem/Configuration.hpp"
#include "Simulator/Environments/Primitives/Types.hpp"
//
#include "Simulator/Environments/CollisionSystem/Batches/RigidBodyPrimitiveTrait.hpp"
#include "Simulator/Environments/CollisionSystem/Detection/Coarse/RigidBodyPrimitiveTrait.hpp"
#include "Simulator/Environments/CollisionSystem/Detection/Fine/RigidBodyPrimitiveTrait.hpp"
#include "Simulator/Environments/CollisionSystem/Response/RigidBodyPrimitiveTrait.hpp"
//
#include "Utilities/DefineTypes.h"
#include "Utilities/Math/Rot3.hpp"
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/NonCopyable.hpp"

namespace elastica {

  namespace collision {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Base class for all rigid primitives
     * \ingroup primitives
     *
     * The RigidBody class is the base class for all rigid primitives of the
     * \elastica physics engine. The class provides the common data members and
     * the common functionality for all rigid bodies. It is NOT declared as
     * an abstract base class, as runtime polymorphic use is NOT intended.
     * Rather we achieve compile-time polymorphism via template meta-programming
     */
    class RigidBodyPrimitive
        : public detection::coarse::RigidBodyPrimitiveTrait<CollisionConfig>,
          public detection::fine::RigidBodyPrimitiveTrait<CollisionConfig>,
          public batches::RigidBodyPrimitiveTrait<CollisionConfig>,
          public response::RigidBodyPrimitiveTrait<CollisionConfig>,
          private NonCopyable {
     private:
     protected:
      //**Type definitions******************************************************
      //! Abbreviation for the coarse collision detection body trait base class.
      /*! This base class configures the type of the axis-aligned bounding box
       * of the rigid primitive. */
      using CCDBT = detection::coarse::RigidBodyPrimitiveTrait<CollisionConfig>;
      //! Abbreviation for the fine collision detection body trait base class.
      /*! This base class configures rigid bodies for the requirements of the
         selected fine collision detection algorithm. */
      using FCDBT = detection::fine::RigidBodyPrimitiveTrait<CollisionConfig>;
      //! Abbreviation for the batch generation body trait base class.
      /*! This base class configures rigid bodies for the requirements of the
         selected batch generation algorithm. */
      using BGBT = batches::RigidBodyPrimitiveTrait<CollisionConfig>;
      //! Abbreviation for the collision response body trait base class.
      /*! This base class adapts the rigid primitive to the requirements of the
         selected collision response algorithm. */
      using CRBT = response::RigidBodyPrimitiveTrait<CollisionConfig>;
      // TODO: based on whether parallelism is needed for a DEM force
      // TODO : Customize for parallel operations using tbb::concurrent vector?
      // TODO : Rename Traist to policy
      //! Handle to contacts
      using ContactID = typename CollisionConfig::ContactID;
      //! Handle to body
      using BodyID = typename CollisionConfig::BodyID;
      //! Vector for attached contacts.
      using Contacts = std::vector<ContactID>;
      //************************************************************************

     protected:
      //**Constructor***********************************************************
      /*!\name Constructor */
      //@{
      explicit RigidBodyPrimitive(BodyID body, bool finite, id_t sid, id_t uid);
      RigidBodyPrimitive(RigidBodyPrimitive&&) noexcept = default;
      //@}
      //************************************************************************

      //**Destructor************************************************************
      /*!\name Destructor */
      //@{
      ~RigidBodyPrimitive() /*override = 0*/;
      //@}
      //************************************************************************
     public:
      //**Type definitions******************************************************
      //! Bounding volume of the rigid primitive.
      using AABB = typename CCDBT::AABB;
      //************************************************************************

      inline bool is_fixed() const;
      inline bool is_finite() const;

      //**Contact functions*****************************************************
      /*!\name Contact functions */
      //@{
      inline void register_contact(ContactID contact);
      //@}
      //************************************************************************

      //**Get functions*********************************************************
      /*!\name Get functions */
      //@{

      inline auto getSystemID() const noexcept -> id_t;
      inline auto getID() const noexcept -> id_t;
      inline auto get_position() const noexcept -> const Vec3&;
      inline auto get_rotation_matrix() const noexcept -> const Rot3&;
      inline auto get_AABB() const noexcept -> const AABB&;
      inline auto get_force() const noexcept -> const Vec3&;
      inline auto get_torque() const noexcept -> const Vec3&;
      inline auto get_angular_velocity() const noexcept -> const Vec3&;
      inline auto get_velocity() const noexcept -> const Vec3&;
      inline auto from_body_FOR_to_world_FOR(real_t vx, real_t vy,
                                             real_t vz) const -> Vec3;
      inline auto from_body_FOR_to_world_FOR(const Vec3& v) const -> Vec3;
      inline auto from_world_FOR_to_body_FOR(real_t vx, real_t vy,
                                             real_t vz) const -> Vec3;
      inline auto from_world_FOR_to_body_FOR(const Vec3& v) const -> Vec3;
      // TODO : noexcept?
      inline auto velocity_from_world_FOR(real_t px, real_t py, real_t pz) const
          -> Vec3;
      inline auto velocity_from_world_FOR(const Vec3& gpos) const -> Vec3;
      inline auto acceleration_from_world_FOR(real_t px, real_t py,
                                              real_t pz) const -> Vec3;
      inline auto acceleration_from_world_FOR(const Vec3& gpos) const -> Vec3;
      //@}
      //************************************************************************

      //**Force functions*******************************************************
      /*!\name Force functions */
      //@{
      // TODO : Move overloads if possible, wont help much
      inline void add_force_at_position(real_t fx, real_t fy, real_t fz,
                                        real_t px, real_t py, real_t pz);
      inline void add_force_at_position(const Vec3& f, const Vec3& gpos);
      inline void reset_forces_and_torques();
      //@}
      //************************************************************************

      //      template <typename RigidBodyType, typename
      //      RigidBodyPrimitiveIterator> friend void transfer(RigidBodyType
      //      const&, RigidBodyPrimitiveIterator,
      //                           RigidBodyPrimitiveIterator, std::true_type);
      //      template <typename RigidBodyType,
      //                typename ConstRigidBodyPrimitiveIterator>
      //      friend void transfer(ConstRigidBodyPrimitiveIterator,
      //                           ConstRigidBodyPrimitiveIterator,
      //                           RigidBodyType&, std::true_type);

     protected:
      //**Member variables******************************************************
      /*!\name Member variables */
      //@{
      /*!< The flag value indicates if the rigid primitive is fixed/stationary
       * (fixed = \a true). */
      //! Fixation flag. // TODO : Needed?
      bool fixed_;
      //! The total mass of the rigid primitive. // TODO : Needed?
      real_t mass_;
      //! The global position of the center of mass of this rigid primitive.
      Vec3 gpos_;
      //! The rotation matrix in reference to the global frame of reference.
      Rot3 Q_;
      //! The linear velocity of this rigid primitive.
      mutable Vec3 v_;
      //! Angular velocity of this rigid primitive.
      mutable Vec3 w_;
      //! Total force (external+contact) acting in the body's center of mass.
      Vec3 force_;
      //! Total torque (external+contact) acting in the body's center of mass.
      Vec3 torque_;
      /*!< The flag value indicates if the rigid primitive is finite (\a true)
       * or infinite (\a false). */
      //! Finiteness flag.
      bool finite_;
      //! The unique system-specific body ID.
      id_t sid_;
      //! The user-specific body ID.
      id_t uid_;
      //! Vector for the currently attached contacts.
      Contacts contacts_;
      //@}
      //************************************************************************
    };
    //**************************************************************************

    //==========================================================================
    //
    //  CONTACT FUNCTIONS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Registering a single attached contact with the rigid primitive.
     *
     * \param contact The contact to be registered with the rigid primitive.
     * \return void
     */
    inline void RigidBodyPrimitive::register_contact(ContactID contact) {
      contacts_.push_back(contact);
    }
    //**************************************************************************

    //==========================================================================
    //
    //  GET FUNCTIONS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Returns whether the rigid primitive's position is fixed or not.
     *
     * \return \a true in case of a fixed/stationary rigid primitive, \a false
     * in case of a free body.
     */
    inline bool RigidBodyPrimitive::is_fixed() const { return fixed_; }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Returns whether the rigid primitive is finite or not.
     *
     * \return \a true in case of a finite rigid primitive, \a false in case of
     * a infinite rigid primitive.
     */
    inline bool RigidBodyPrimitive::is_finite() const { return finite_; }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Returns the system ID of the primitive
     */
    inline auto RigidBodyPrimitive::getSystemID() const noexcept -> id_t {
      return sid_;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Returns the user ID of the primitive
     */
    inline auto RigidBodyPrimitive::getID() const noexcept -> id_t {
      return uid_;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Returns the global position of the center of mass of the rigid
     * body.
     *
     * \return The global position of the center of mass.
     */
    inline const Vec3& RigidBodyPrimitive::get_position() const noexcept {
      return gpos_;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Returns the rotation of the rigid primitive.
     *
     * \return The rotation of the rigid primitive.
     *
     * This function returns the rotation matrix of the rigid primitive, which
     * represents the rotation of the body in reference to the global world
     * frame.
     */
    inline const Rot3& RigidBodyPrimitive::get_rotation_matrix()
        const noexcept {
      return Q_;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Returns the axis-aligned bounding box of the rigid primitive.
     *
     * \return The axis-aligned bounding box of the rigid primitive.
     */
    inline const RigidBodyPrimitive::AABB& RigidBodyPrimitive::get_AABB()
        const noexcept {
      return aabb_;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Returns the force acting on the primitive
     *
     * \return The force.
     */
    auto RigidBodyPrimitive::get_force() const noexcept -> const Vec3& {
      return force_;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Returns the torque acting on the primitive
     *
     * \return The torque in the global coordinates.
     */
    auto RigidBodyPrimitive::get_torque() const noexcept -> const Vec3& {
      return torque_;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Returns the global angular velocity.
     *
     * \return The global angular velocity.
     *
     * This function returns the angular velocity of the center of mass in
     * reference to the global world frame.
     */
    inline const Vec3& RigidBodyPrimitive::get_angular_velocity()
        const noexcept {
      return w_;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Returns the global velocity.
     *
     * \return The global velocity.
     *
     * This function returns the velocity of the center of mass in
     * reference to the global world frame.
     */
    inline const Vec3& RigidBodyPrimitive::get_velocity() const noexcept {
      return v_;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Transformation from a relative to a global vector.
     *
     * \param vx The x-component of the relative vector.
     * \param vy The y-component of the relative vector.
     * \param vz The z-component of the relative vector.
     * \return The transformed global vector.
     *
     * The function calculates the transformation of a vector in body frame to a
     * vector in global world frame.
     */
    inline auto RigidBodyPrimitive::from_body_FOR_to_world_FOR(real_t vx,
                                                               real_t vy,
                                                               real_t vz) const
        -> Vec3 {
      return trans(Q_) * Vec3{vx, vy, vz};
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Transformation from a relative to a global vector.
     *
     * \param v the relative vector.
     * \return The transformed global vector.
     *
     * The function calculates the transformation of a vector in body frame to a
     * vector in global world frame.
     */
    inline auto RigidBodyPrimitive::from_body_FOR_to_world_FOR(
        const Vec3& v) const -> Vec3 {
      return trans(Q_) * v;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Transformation from a global to a relative vector.
     *
     * \param vx The x-component of the global vector.
     * \param vy The y-component of the global vector.
     * \param vz The z-component of the global vector.
     * \return The transformed relative vector.
     *
     * The function calculates the transformation of a vector in global world
     * frame to a vector in body frame.
     */
    inline auto RigidBodyPrimitive::from_world_FOR_to_body_FOR(real_t vx,
                                                               real_t vy,
                                                               real_t vz) const
        -> Vec3 {
      return Q_ * Vec3{vx, vy, vz};
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Transformation from a global to a relative vector.
     *
     * \param v The global vector.
     * \return The transformed relative vector.
     *
     * The function calculates the transformation of a vector in global world
     * frame to a vector in body frame.
     */
    inline auto RigidBodyPrimitive::from_world_FOR_to_body_FOR(
        const Vec3& v) const -> Vec3 {
      return Q_ * v;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Calculation of the global velocity of a point in global
     * coordinates.
     *
     * \param px The x-component of the global coordinate.
     * \param py The y-component of the global coordinate.
     * \param pz The z-component of the global coordinate.
     * \return The global velocity.
     *
     * The function calculates the global velocity of a point in global
     * coordinates.
     */
    inline Vec3 RigidBodyPrimitive::velocity_from_world_FOR(real_t px,
                                                            real_t py,
                                                            real_t pz) const {
      return velocity_from_world_FOR(Vec3{px, py, pz});
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Calculation of the global velocity of a point in global
     * coordinates.
     *
     * \param gpos The global coordinate.
     * \return The global velocity.
     *
     * The function calculates the global velocity of a point in global
     * coordinates.
     */
    inline Vec3 RigidBodyPrimitive::velocity_from_world_FOR(
        const Vec3& gpos) const {
      // Not noexcept because blaze's static vector constructors are not marked
      // noexcept
      return v_ + from_body_FOR_to_world_FOR(w_) % (gpos - gpos_);
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Adds force components at some position (in global coordinates).
     *
     * \param fx The x-component of the force.
     * \param fy The y-component of the force.
     * \param fz The z-component of the force.
     * \param px The x-component of the global coordinate to add the force.
     * \param py The y-component of the global coordinate to add the force.
     * \param pz The z-component of the global coordinate to add the force.
     */
    inline void RigidBodyPrimitive::add_force_at_position(real_t fx, real_t fy,
                                                          real_t fz, real_t px,
                                                          real_t py,
                                                          real_t pz) {
      add_force_at_position(Vec3{fx, fy, fz}, Vec3{px, py, pz});
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Adds force at some position in global coordinates.
     *
     * \param fx The x-component of the force.
     * \param fy The y-component of the force.
     * \param fz The z-component of the force.
     * \param px The x-component of the global coordinate to add the force.
     * \param py The y-component of the global coordinate to add the force.
     * \param pz The z-component of the global coordinate to add the force.
     */
    inline void RigidBodyPrimitive::add_force_at_position(const Vec3& f,
                                                          const Vec3& gpos) {
      // Increasing the force and torque on this rigid primitive
      force_ += f;
      torque_ += (gpos - gpos_) % f;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Resetting all acting forces and torques acting on the rigid
     * primitive.
     *
     * \return void
     */
    void RigidBodyPrimitive::reset_forces_and_torques() {
      force_ = real_t(0);
      torque_ = real_t(0);
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Calculation of the global acceleration of a point in global
     * coordinates.
     *
     * \param px The x-component of the global coordinate.
     * \param py The y-component of the global coordinate.
     * \param pz The z-component of the global coordinate.
     * \return The global acceleration.
     *
     * The function calculates the global acceleration of a point in global
     * coordinates.
     */
    auto RigidBodyPrimitive::acceleration_from_world_FOR(real_t px, real_t py,
                                                         real_t pz) const
        -> Vec3 {
      return acceleration_from_world_FOR(Vec3{px, py, pz});
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Calculation of the global acceleration of a point in global
     * coordinates.
     *
     * \param gpos The global coordinate.
     * \return The global acceleration.
     *
     * The function calculates the global acceleration of a point in global
     * coordinates.
     *
     * \note
     * The function is not implemented yet, its only given for completeness
     */
    auto RigidBodyPrimitive::acceleration_from_world_FOR(
        const Vec3& /*gpos*/) const -> Vec3 {
      return elastica::Vec3();
    }
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica
