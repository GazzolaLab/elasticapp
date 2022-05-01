#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include "Simulator/Environments/CollisionSystem/Configuration/Types.hpp"
#include "Simulator/Environments/CollisionSystem/Contact/Types.hpp"
#include "Simulator/Environments/CollisionSystem/Response/Types.hpp"
#include "Simulator/Environments/Primitives/Types.hpp"

namespace elastica {

  namespace collision {

    //**************************************************************************
    // Forward Declarations
    //**************************************************************************
    class Contact;
    class RigidBodyPrimitive;

    //==========================================================================
    //
    //  SPECIALIZATION FOR THE DEM SOLVER
    //
    //==========================================================================

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of the configuration for the DEM solver.
     * \ingroup collision_system
     *
     * This specialization of the Configuration class handles the special case
     * of the DEM solver.
     *
     * The type of the force model algorithm is selected by setting the
     * ELASTICA_DEM_DEFAULT_FORCE_RESPONSE_MODEL macro. This can be overriden
     * with another appropriate model from a user-defined client file
     */
    template <
        // Type of the coarse collision detection algorithm
        template <typename> class CD,
        // Type of the fine collision detection algorithm
        typename FD,
        // Type of the batch generation algorithm
        template <typename> class BG>
    struct CollisionSystemConfiguration<CD, FD, BG, response::DEMSolver> {
     public:
      //**Type definitions******************************************************
      //! Type of this configuration
      using CollisionConfig =
          CollisionSystemConfiguration<CD, FD, BG, response::DEMSolver>;
      //! Type of the rigid bodies
      using BodyType = RigidBodyPrimitive;
      //! Handle to a rigid body
      using BodyID = RigidBodyPrimitive*;
      //! Handle to a constant rigid body
      using ConstBodyID = const RigidBodyPrimitive*;
      //! Type of the contacts
      using Contact = Contact;
      //! Handle to a contact
      using ContactID = Contact*;
      //!< Handle to a constant contact
      using ConstContactID = const Contact*;
      //! Template signature of the coarse collision detection algorithm.
      template <typename C>
      using CoarseDetectorFamily = CD<C>;
      //! Type of the fine collision detector
      using FineDetector = FD;
      //! Type of the batch generation algorithm
      template <typename C>
      using BatchGenerator = BG<C>;
      // //! Type of the force model to be employed, specialized for the DEM
      // solver using ForceModel = ELASTICA_DEFAULT_DEM_RESPONSE_MODEL;
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica