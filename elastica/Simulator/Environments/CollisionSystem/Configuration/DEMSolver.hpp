#pragma once

#include "Default.hpp"
#include "elastica/ModuleSettings/DEMSolver.hpp"

namespace elastica {

  namespace collision {

    //*************************************************************************************************
    // Forward Declarations
    //*************************************************************************************************
    class Contact;
    class RigidBodyPrimitive;
    class SpherePrimitive;

    //==========================================================================
    //
    //  SPECIALIZATION FOR THE DEM SOLVER
    //
    //==========================================================================

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of the configuration for the DEM solver.
     * \ingroup core
     *
     * This specialization of the Configuration class handles the special case
     * that the DEM solver doesn't require a batch generator. Therefore the
     * SingleBatch algorithm is specifically chosen as batch generation
     * algorithm, since this algorithm does not impose additional data members
     * or functionality to both primitives and contacts.
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
      using CollisionConfig = CollisionSystemConfiguration<
          CD, FD, batches::SingleBatch,
          response::DEMSolver>;  //!< Type of this configuration.

      using BodyType = RigidBodyPrimitive;  //!< Type of the rigid bodies.
      using BodyID = RigidBodyPrimitive*;   //!< Handle for a rigid body.
      using ConstBodyID =
      const RigidBodyPrimitive*;  //!< Handle for a constant rigid body.

      using SphereType =
      SpherePrimitive;  //!< Type of the sphere geometric primitive.
      using SphereID = SpherePrimitive*;  //!< Handle for a sphere primitive.
      using ConstSphereID =
      const SpherePrimitive*;  //!< Handle for a constant sphere primitive.

      using ContactType = Contact;  //!< Type of the contacts.
      using ContactID = Contact*;   //!< Handle for a contact.
      using ConstContactID =
      const Contact*;  //!< Handle for a constant contact.

      //! Template signature of the coarse collision detection algorithm.
      /*! The type of the coarse collision detection algorithm is selected by
         the setting of the ELASTICA::ELASTICA_COARSE_COLLISION_DETECTOR macro.
       */
      template <typename C>
      using CoarseDetectorFamily = CD<C>;

      //! Type of the fine collision detector.
      /*! The type of the fine collision detection algorithm is selected by the
         setting of the ELASTICA::ELASTICA_FINE_COLLISION_DETECTOR macro. */
      using FineDetector = FD;

      //! Type of the batch generation algorithm.
      /*! The type of the batch generation algorithm is selected by the setting
         of the ELASTICA_BATCH_GENERATOR macro. */
      using BatchGenerator = BG<CollisionConfig>;


      //! Type of the force model to be employed, specialized for the DEM solver
      /*! The type of the force model algorithm is selected by setting
         the ELASTICA_DEM_DEFAULT_FORCE_RESPONSE_MODEL macro. This can be overriden
         with another appropriate model from a user-defined client file*/
      using ForceModel = ELASTICA_DEFAULT_DEM_RESPONSE_MODEL;
    };

  }  // namespace collision
}  // namespace elastica