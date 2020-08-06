#pragma once

#include <vector>

#include "Simulator/Environments/CollisionSystem/Batches/RigidBodyPrimitiveTrait.hpp"
#include "Simulator/Environments/CollisionSystem/Configuration.hpp"
#include "Simulator/Environments/CollisionSystem/Detection/Coarse/RigidBodyPrimitiveTrait.hpp"
#include "Simulator/Environments/CollisionSystem/Detection/Fine/RigidBodyPrimitiveTrait.hpp"
#include "Simulator/Environments/CollisionSystem/Response/RigidBodyPrimitiveTrait.hpp"
#include "Simulator/Environments/CollisionSystem/Types.hpp"
#include "elastica/Utilities/DefineTypes.h"
#include "elastica/Utilities/Math/Rot3.hpp"
#include "elastica/Utilities/Math/Vec3.hpp"
#include "elastica/Utilities/NonCopyable.hpp"

namespace elastica {

  namespace collision {

    class RigidBodyPrimitive
        : public detection::coarse::RigidBodyPrimitiveTrait<CollisionConfig>,
          public detection::fine::RigidBodyPrimitiveTrait<CollisionConfig>,
          public batches::RigidBodyPrimitiveTrait<CollisionConfig>,
          public response::RigidBodyPrimitiveTrait<CollisionConfig>,
          private NonCopyable {
     private:
      // TODO : Customize for parallel operations using tbb::concurrent vector?
      // TODO : Rename Traist to policy
      //      using Contacts =
      //          std::vector<Contact*>;  //!< Vector for attached contacts.
     protected:
      //**Type
      // definitions****************************************************************************
      //! Abbreviation for the coarse collision detection body trait base class.
      /*! This base class configures the type of the axis-aligned bounding box
       * of the rigid body. */
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
      /*! This base class adapts the rigid body to the requirements of the
         selected collision response algorithm. */
      using CRBT = response::RigidBodyPrimitiveTrait<CollisionConfig>;

      //!< Bounding volume of the rigid body.
      using AABB = typename CCDBT::AABB;

     protected:
      //**Constructor*********************************************************************************
      /*!\name Constructor */
      //@{
      explicit RigidBodyPrimitive(BodyID body, bool finite, id_t sid, id_t uid);
      //@}
      //**********************************************************************************************

      //**Destructor**********************************************************************************
      /*!\name Destructor */
      //@{
      virtual ~RigidBodyPrimitive() = 0;
      //@}
      //**********************************************************************************************
     public:
      inline bool is_fixed() const;
      inline bool is_finite() const;
      //      inline void registerContact(ContactID contact);

      //**Get
      // functions*******************************************************************************
      /*!\name Get functions */
      //@{

      inline auto getSystemID() const noexcept -> id_t;
      inline auto getID() const noexcept -> id_t;
      inline auto get_position() const noexcept -> const Vec3&;
      inline auto get_AABB() const noexcept -> const AABB&;
      inline auto get_force() const noexcept -> const Vec3&;
      inline auto get_torque() const noexcept -> const Vec3&;

      inline auto get_angular_velocity() const noexcept -> const Vec3&;

      // TODO : noexcept?
      inline auto velocity_from_world_FOR(real_t px, real_t py, real_t pz) const
          -> Vec3;
      inline auto velocity_from_world_FOR(const Vec3& gpos) const -> Vec3;
      //      inline auto accFromWF(real_t px, real_t py, real_t pz) const ->
      //      Vec3; inline auto accFromWF(const Vec3& gpos) const -> Vec3;

      //@}
      //**********************************************************************************************

      // For
      // TODO : Move overloads if possible, wont help much
      inline void add_force_at_position(real_t fx, real_t fy, real_t fz,
                                        real_t px, real_t py, real_t pz);
      inline void add_force_at_position(const Vec3& f, const Vec3& gpos);
      inline void add_force_at_position(Vec3&& f, const Vec3& gpos);
      inline void add_force_at_position(Vec3&& f, Vec3&& gpos);

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
      //**Member
      // variables****************************************************************************
      /*!\name Member variables */
      //@{
      bool fixed_;  //!< Fixation flag.
      /*!< The flag value indicates if the rigid body is fixed/stationary
           (fixed = \a true). */

      // TODO : Needed?
      real_t mass_;  //!< The total mass of the rigid body.
      Vec3 gpos_;  //!< The global position of the center of mass of this rigid
                   //!< body.
      Rot3 rot_mat_;    //!< The rotation in reference to the global frame of
                        //!< reference.
      mutable Vec3 v_;  //!< The linear velocity of this rigid body.
      mutable Vec3 w_;  //!< Angular velocity of this rigid body.
      Vec3 force_;      //!< Total force (external+contact) acting in the body's
                        //!< center of mass.
      Vec3 torque_;  //!< Total torque (external+contact) acting in the body's
                     //!< center of mass.
      //**********************************************************************************************

      bool finite_;  //!< Finiteness flag.
      /*!< The flag value indicates if the rigid body is finite (\a true)
           or infinite (\a false). */
      id_t sid_;  //!< The unique system-specific body ID.
      id_t uid_;  //!< The user-specific body ID.

      // TODO: determine if needed?
      //      Contacts contacts_;  //!< Vector for the currently attached
      //      contacts.
    };
    //=================================================================================================
    //
    //  CONTACT FUNCTIONS
    //
    //=================================================================================================

    //*************************************************************************************************
    /*!\brief Registering a single attached contact with the rigid body.
     *
     * \param contact The contact to be registered with the rigid body.
     * \return void
     */
    //    inline void RigidBodyPrimitive::registerContact(ContactID contact) {
    //      contacts_.push_back(contact);
    //    }
    //*************************************************************************************************

    //=================================================================================================
    //
    //  GET FUNCTIONS
    //
    //=================================================================================================

    //*************************************************************************************************
    /*!\brief Returns whether the rigid body's position is fixed or not.
     *
     * \return \a true in case of a fixed/stationary rigid body, \a false in
     * case of a free body.
     */
    inline bool RigidBodyPrimitive::is_fixed() const { return fixed_; }
    //*************************************************************************************************

    //*************************************************************************************************
    /*!\brief Returns whether the rigid body is finite or not.
     *
     * \return \a true in case of a finite rigid body, \a false in case of a
     * infinite rigid body.
     */
    inline bool RigidBodyPrimitive::is_finite() const { return finite_; }
    //*************************************************************************************************

    //*************************************************************************************************
    /*!\brief TODO Returns whether the rigid body's position is fixed or not.
     *
     * \return \a true in case of a fixed/stationary rigid body, \a false in
     * case of a free body.
     */
    inline auto RigidBodyPrimitive::getSystemID() const noexcept -> id_t {
      return sid_;
    }
    //*************************************************************************************************

    //*************************************************************************************************
    /*!\brief TODO Returns whether the rigid body's position is fixed or not.
     *
     * \return \a true in case of a fixed/stationary rigid body, \a false in
     * case of a free body.
     */
    inline auto RigidBodyPrimitive::getID() const noexcept -> id_t {
      return uid_;
    }
    //*************************************************************************************************

    //*************************************************************************************************
    /*!\brief Returns the global position of the center of mass of the rigid
     * body.
     *
     * \return The global position of the center of mass.
     */
    inline const Vec3& RigidBodyPrimitive::get_position() const noexcept {
      return gpos_;
    }
    //*************************************************************************************************

    //*************************************************************************************************
    /*!\brief Returns the axis-aligned bounding box of the rigid body.
     *
     * \return The axis-aligned bounding box of the rigid body.
     */
    inline const RigidBodyPrimitive::AABB& RigidBodyPrimitive::get_AABB()
        const noexcept {
      return aabb_;
    }
    //*************************************************************************************************

    //*************************************************************************************************
    /*!\brief Returns the global angular velocity.
     *
     * \return The global angular velocity.
     *
     * This function returns the angluar velocity of the center of mass in
     * reference to the global world frame.
     */
    inline const Vec3& RigidBodyPrimitive::get_angular_velocity()
        const noexcept {
      return w_;
    }
    //*************************************************************************************************

    //*************************************************************************************************
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
    //*************************************************************************************************

    //*************************************************************************************************
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
      return v_ + w_ % (gpos - gpos_);
    }
    //*************************************************************************************************

    inline void RigidBodyPrimitive::add_force_at_position(real_t fx, real_t fy,
                                                          real_t fz, real_t px,
                                                          real_t py,
                                                          real_t pz) {
      add_force_at_position(Vec3{fx, fy, fz}, Vec3{px, py, pz});
    }

    inline void RigidBodyPrimitive::add_force_at_position(const Vec3& f,
                                                          const Vec3& gpos) {
      // Increasing the force and torque on this rigid body
      force_ += f;
      torque_ += (gpos - gpos_) % f;
    }

    inline void RigidBodyPrimitive::add_force_at_position(Vec3&& f,
                                                          const Vec3& gpos) {
      // Increasing the force and torque on this rigid body
      force_ += f;
      torque_ += (gpos - gpos_) % f;
    }

    inline void RigidBodyPrimitive::add_force_at_position(Vec3&& f,
                                                          Vec3&& gpos) {
      // Increasing the force and torque on this rigid body
      force_ += f;
      torque_ += (gpos - gpos_) % f;
    }

    auto RigidBodyPrimitive::get_force() const noexcept -> const Vec3& {
      return force_;
    }

    auto RigidBodyPrimitive::get_torque() const noexcept -> const Vec3& {
      return torque_;
    }

    //    auto RigidBodyPrimitive::accFromWF(real_t px, real_t py, real_t pz)
    //    const
    //        -> Vec3 {
    //      return accFromWF(Vec3{px, py, pz});
    //    }
    //
    //    auto RigidBodyPrimitive::accFromWF(const Vec3& gpos) const -> Vec3 {
    //      return elastica::Vec3();
    //    }
    //
  }  // namespace collision

}  // namespace elastica
