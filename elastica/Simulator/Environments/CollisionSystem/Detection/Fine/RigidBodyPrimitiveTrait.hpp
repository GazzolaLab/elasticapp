#pragma once

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
        /*!\brief Rigid body customization class for the fine collision
         * detection. \ingroup fine_collision_detection
         *
         * The fine detection module RigidBodyPrimitiveTrait class template is
         * used to adapt the RigidBody class to the used fine collision
         * detection algorithm. For instance, this class configures the
         * axis-aligned bounding box for rigid bodies depending on the
         * requirements of the selected fine collision detection algorithm.\n In
         * order to adapt the RigidBody class to a particular algorithm, the
         * base template needs to be specialized.
         */
        template <typename C>  // Type of the configuration
        class RigidBodyPrimitiveTrait {};
        //*************************************************************************************************
      }  // namespace fine

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica
