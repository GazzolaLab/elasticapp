//==============================================================================
/*!
//  \file
//  \brief Customization of rigid-body class based on the collision response
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include "Simulator/Environments/CollisionSystem/Response/Types.hpp"

namespace elastica {

  namespace collision {

    namespace response {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Rigid body customization class for the collision response.
       * \ingroup collision_response
       *
       * The RigidBodyPrimitiveTrait class template is used to adapt the
       * RigidBody class to the used collision response algorithm. Depending on
       * the used algorithm, a body may require additional data or functionality
       * to efficiently support the collision response calculations.
       *
       * In order to adapt the RigidBody class to a particular algorithm, the
       * base template needs to be specialized.
       *
       * \tparam C The configuration of collision detection
       */
      template <typename C>
      class RigidBodyPrimitiveTrait {
       protected:
        //**Constructor*********************************************************
        /*!\name Constructor */
        //@{

        //**********************************************************************
        /*!\brief Default constructor.
         */
        explicit inline RigidBodyPrimitiveTrait() = default;
        //**********************************************************************

        //@}
        //**********************************************************************

        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{
        /*virtual*/ ~RigidBodyPrimitiveTrait() = default;
        //@}
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace response

  }  // namespace collision

}  // namespace elastica
