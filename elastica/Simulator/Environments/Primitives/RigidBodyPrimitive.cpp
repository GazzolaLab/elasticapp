#include "RigidBodyPrimitive.hpp"

#include "elastica/Utilities/MakeSignalingNan.hpp"

namespace elastica {

  namespace collision {
    //=================================================================================================
    //
    //  CONSTRUCTORS
    //
    //=================================================================================================

    //*************************************************************************************************
    /*!\brief Constructor for the RigidBodyPrimitive class.
     *
     * \param body The body ID of this rigid body.
     */
    RigidBodyPrimitive::RigidBodyPrimitive(BodyID body, bool finite, id_t sid,
                                           id_t uid)
        : CCDBT(body),  // Initialization of the coarse collision detection body
                        // trait base class
          FCDBT(),  // Initialization of the fine collision detection body trait
                    // base class
          BGBT(),   // Initialization of the batch generation body trait base
                    // class
          CRBT(),   // Initialization of the collision response body trait base
                    // class
          fixed_(false),
#ifdef ELASTICA_DEBUG
          mass_(make_signaling_NaN<real_t>()),  // Total mass of the rigid body
          gpos_(make_signaling_NaN<real_t>()),  // Global position of the center
                                                // of mass
          //          rot_mat_(make_signaling_NaN<real_t>()),  // Rotation
          //          matrix
          rot_mat_(real_t(0.0)),  // Rotation matrix
          v_(make_signaling_NaN<real_t>()),        // Linear velocity
          w_(make_signaling_NaN<real_t>()),        // Angular velocity
          // Forces and torques are always transferred to the final rigid body
          // so make sure they aren't nan-initialized
          force_(real_t(0.0)),   // Total force
          torque_(real_t(0.0)),  // Total torque
#else
          mass_(0),    // Total mass of the rigid body
          gpos_(),     // Global position of the center of mass
          rot_mat_(),  // Rotation matrix
          v_(),        // Linear velocity
          w_(),        // Angular velocity
          force_(),    // Total force
          torque_(),   // Total torque
#endif
          finite_(finite),
          sid_(sid),
          uid_(uid) {
    }

    //=================================================================================================
    //
    //  DESTRUCTOR
    //
    //=================================================================================================

    //*************************************************************************************************
    /*!\brief Destructor for the RigidBodyBase class.
     */
    RigidBodyPrimitive::~RigidBodyPrimitive() = default;
    //*************************************************************************************************

  }  // namespace collision

}  // namespace elastica
