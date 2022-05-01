//==============================================================================
/*!
//  \file
//  \brief Default implementation of \elastica collision configuration
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include "Simulator/Environments/CollisionSystem/Configuration/Types.hpp"
#include "Simulator/Environments/CollisionSystem/Contact/Types.hpp"
#include "Simulator/Environments/Primitives/Types.hpp"

namespace elastica {

  namespace collision {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Configuration of the collision module
     * \ingroup collision_system
     *
     * The CollisionSystemConfiguration class represents the actual
     * configuration of the collision engine. The type of the configuration is
     * depending on the selected coarse collision detection algorithm (see \ref
     * ELASTICA_COARSE_COLLISION_DETECTOR), the fine collision
     * detection (see \ref ELASTICA_FINE_COLLISION_DETECTOR), the batch
     * generator (see \ref ELASTICA_BATCH_GENERATOR) and the collision
     * response algorithm (see \ref ELASTICA_CONSTRAINT_SOLVER). The type
     * of the actual configuration is used to instantiate configuration
     * dependent classes, as for instance the CollisionSystem class and all
     * necessary base classes for RigidBodyPrimitive and Contact.
     */
    template <
        // Type of the coarse collision detection algorithm
        template <typename> class CD,
        // Type of the fine collision detection algorithm
        typename FD,
        // Type of the batch generation algorithm
        template <typename> class BG,
        // Type of the collision response algorithm
        template <typename, typename, typename> class CR>
    struct CollisionSystemConfiguration {
     public:
      //**Type definitions******************************************************
      //! Type of this configuration
      using CollisionConfig = CollisionSystemConfiguration<CD, FD, BG, CR>;
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
      //! Handle to a constant contact
      using ConstContactID = const Contact*;
      //! Template signature of the coarse collision detection algorithm.
      template <typename C>
      using CoarseDetectorFamily = CD<C>;
      //! Type of the fine collision detector.
      using FineDetector = FD;
      //! Type of the batch generation algorithm.
      template <typename C>
      using BatchGenerator = BG<C>;
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica
