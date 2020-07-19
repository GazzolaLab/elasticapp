//=================================================================================================
/*!
 *  \file pe/core/configuration/Default.h
 *  \brief Header file for the default implementation of the Configuration class
 * template.
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

namespace elastica {

  namespace collision {

    //*************************************************************************************************
    // Forward Declarations
    //*************************************************************************************************
    class Contact;
    class RigidBodyPrimitive;
    class SpherePrimitive;

    //=================================================================================================
    //
    //  CLASS DEFINITION
    //
    //=================================================================================================

    //*************************************************************************************************
    /*!\brief Configuration of the collision module.
     * \ingroup core
     *
     * The CollisionSystemConfiguration class represents the actual
     * configuration of the collision engine. The type of the configuration is
     * depending on the selected coarse collision detection algorithm (see
     * ELASTICA::ELASTICA_COARSE_COLLISION_DETECTOR), the fine collision
     * detection (see ELASTICA::ELASTICA_FINE_COLLISION_DETECTOR), the batch
     * generator (see ELASTICA::ELASTICA_BATCH_GENERATOR) and the collision
     * response algorithm (see ELASTICA::ELASTICA_CONSTRAINT_SOLVER). The type
     * of the actual configuration is used to instantiate configuration
     * dependent classes, as for instance the CollisionSystem class and all
     * necessary base classes for RigidBody and Contact.
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
      using CollisionConfig =
          CollisionSystemConfiguration<CD, FD, BG,
                                       CR>;  //!< Type of this configuration.

      using BodyType = RigidBodyPrimitive;  //!< Type of the rigid bodies.
      using BodyID = RigidBodyPrimitive*;   //!< Handle for a rigid body.
      using ConstBodyID =
          const RigidBodyPrimitive*;  //!< Handle for a constant rigid body.
      //
      //      typedef Box BoxType;            //!< Type of the box geometric
      //      primitive. typedef Box* BoxID;             //!< Handle for a box
      //      primitive. typedef const Box* ConstBoxID;  //!< Handle for a
      //      constant box primitive.
      //
      //      typedef Capsule
      //          CapsuleType;             //!< Type of the capsule geometric
      //          primitive.
      //      typedef Capsule* CapsuleID;  //!< Handle for a capsule primitive.
      //      typedef const Capsule*
      //          ConstCapsuleID;  //!< Handle for a constant capsule primitive.
      //
      //      typedef Cylinder
      //          CylinderType;  //!< Type of the cylinder geometric primitive.
      //      typedef Cylinder* CylinderID;  //!< Handle for a cylinder
      //      primitive. typedef const Cylinder*
      //          ConstCylinderID;  //!< Handle for a constant cylinder
      //          primitive.
      //
      //      typedef Plane PlaneType;  //!< Type of the plane geometric
      //      primitive. typedef Plane* PlaneID;   //!< Handle for a plane
      //      primitive. typedef const Plane*
      //          ConstPlaneID;  //!< Handle for a constant plane primitive.
      //

      using SphereType =
          SpherePrimitive;  //!< Type of the sphere geometric primitive.
      using SphereID = SpherePrimitive*;  //!< Handle for a sphere primitive.
      using ConstSphereID =
          const SpherePrimitive*;  //!< Handle for a constant sphere primitive.

      //
      //      typedef TriangleMesh
      //          MeshType;  //!< Type of the triangle mesh geometric primitive.
      //      typedef TriangleMesh* MeshID;  //!< Handle for a triangle mesh
      //      primitive. typedef const TriangleMesh*
      //          ConstMeshID;  //!< Handle for a constant triangle mesh
      //          primitive.
      //
      //      typedef Union UnionType;  //!< Type of the union compound
      //      geometry. typedef Union* UnionID;   //!< Handle for a union.
      //      typedef const Union* ConstUnionID;  //!< Handle for a constant
      //      union.
      //
      //      typedef Attachable AttachableType;  //!< Type of the attachables.
      //      typedef Attachable* AttachableID;   //!< Handle for an attachable.
      //      typedef const Attachable*
      //          ConstAttachableID;  //!< Handle for a constant attachable.
      //
      //      typedef Gravity GravityType;  //!< Type of the gravity force
      //      generators. typedef Gravity* GravityID;   //!< Handle for a
      //      gravity force generator. typedef const Gravity*
      //          ConstGravityID;  //!< Handle for a constant gravity force
      //          generator.
      //
      //      typedef Spring SpringType;  //!< Type of the spring force
      //      generators. typedef Spring* SpringID;   //!< Handle for a spring
      //      force generator. typedef const Spring*
      //          ConstSpringID;  //!< Handle for a constant spring force
      //          generator.
      //
      using ContactType = Contact;  //!< Type of the contacts.
      using ContactID = Contact*;   //!< Handle for a contact.
      using ConstContactID =
          const Contact*;  //!< Handle for a constant contact.
      //
      //      typedef Joint JointType;            //!< Type of the joints.
      //      typedef Joint* JointID;             //!< Handle for a joint.
      //      typedef const Joint* ConstJointID;  //!< Handle for a constant
      //      joint.
      //
      //      typedef SliderJoint SliderJointType;  //!< Type of the slider
      //      joint. typedef SliderJoint* SldierJointID;   //!< Handle for a
      //      slider joint. typedef const SliderJoint*
      //          ConstSliderJointID;  //!< Handle for a constant slider joint.
      //
      //      typedef HingeJoint HingeJointType;  //!< Type of the hinge joint.
      //      typedef HingeJoint* HingeJointID;   //!< Handle for a hinge joint.
      //      typedef const HingeJoint*
      //          ConstHingeJointID;  //!< Handle for a constant hinge joint.
      //
      //      typedef BallJoint BallJointType;  //!< Type of the ball joint.
      //      typedef BallJoint* BallJointID;   //!< Handle for a ball joint.
      //      typedef const BallJoint*
      //          ConstBallJointID;  //!< Handle for a constant ball joint.
      //
      //      typedef Process ProcessType;  //!< Type of the remote processes.
      //      typedef Process* ProcessID;   //!< Handle for a remote process.
      //      typedef const Process*
      //          ConstProcessID;  //!< Handle for a constant remote process.

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

      //************************************************************************
    };
    //*************************************************************************************************

  }  // namespace collision

}  // namespace elastica
