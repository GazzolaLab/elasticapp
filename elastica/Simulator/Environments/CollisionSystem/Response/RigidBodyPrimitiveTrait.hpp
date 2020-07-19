#pragma once

namespace elastica {
  namespace collision {
    namespace response {

      //=================================================================================================
      //
      //  CLASS DEFINITION
      //
      //=================================================================================================

      //*************************************************************************************************
      /*!\brief Rigid body customization class for the collision response.
       * \ingroup collision_response
       *
       * The RigidBodyPrimitiveTrait class template is used to adapt the RigidBody class to
       * the used collision response algorithm. Depending on the used algorithm,
       * a body may require additional data or functionality to efficiently
       * support the collision response calculations.\n In order to adapt the
       * RigidBody class to a particular algorithm, the base template needs to
       * be specialized.
       */
      template <typename C>  // Type of the configuration
      class RigidBodyPrimitiveTrait {
       protected:
        //**Constructor*********************************************************************************
        /*!\name Constructor */
        //@{
        explicit inline RigidBodyPrimitiveTrait();
        //@}
        //**********************************************************************************************

        //**Destructor**********************************************************************************
        /*!\name Destructor */
        //@{
        virtual ~RigidBodyPrimitiveTrait();
        //@}
        //**********************************************************************************************
      };
      //*************************************************************************************************

      //*************************************************************************************************
      /*!\brief Default implementation of the RigidBodyPrimitiveTrait constructor.
       */
      template <typename C>  // Type of the configuration
      inline RigidBodyPrimitiveTrait<C>::RigidBodyPrimitiveTrait() {}
      //*************************************************************************************************

      //*************************************************************************************************
      /*!\brief Default implementation of the RigidBodyPrimitiveTrait destructor.
       */
      template <typename C>  // Type of the configuration
      RigidBodyPrimitiveTrait<C>::~RigidBodyPrimitiveTrait() {}
      //*************************************************************************************************

    }  // namespace response
  }    // namespace collision
}  // namespace elastica
