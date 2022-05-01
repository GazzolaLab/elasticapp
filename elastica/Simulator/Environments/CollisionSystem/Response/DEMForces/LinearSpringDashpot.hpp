//==============================================================================
/*!
//  \file
//  \brief A DEM force that models a linear spring with damping
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

#include "Simulator/Environments/CollisionSystem/Contact/Types.hpp"
//
#include "Simulator/Environments/CollisionSystem/Response/DEMForces/Protocols.hpp"
#include "Simulator/Environments/CollisionSystem/Response/DEMForces/Types.hpp"
//
#include "Utilities/DefineTypes.h"  // real_t
#include "Utilities/ProtocolHelpers.hpp"

namespace elastica {

  namespace collision {

    namespace response {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Response model of a linear spring connected serially with a
       * damper
       * \ingroup collision_response
       *
       * \details
       * A DEM forcing model implementing a
       * TODO : Haff and Werner, other models?
       */
      struct LinearSpringDashpot
          : public ::tt::ConformsTo<
                ::elastica::collision::protocols::DEMForce> {
        //**Constructor*********************************************************
        /*!\name Constructor */
        //@{
        inline LinearSpringDashpot() = default;
        //@}
        //**********************************************************************

        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{
        ~LinearSpringDashpot() = default;
        //@}
        //**********************************************************************
        // Prevents implicit conversions
        // template <class U>
        // real_t operator()(U) const noexcept= delete;
        // template <>
        // real_t operator()<ContactID>(ContactID c) const noexcept;
        // In case the template isnt specified

        //**********************************************************************
        /*!\name Utility operator*/
        //@{

        //**********************************************************************
        /*!\brief Implements the LinearSpringDashpot model
         *
         * \details
         * Resolves forces and torques based on the LinearSpringDashpot model
         *
         * \param contact The contact being processed
         * \return The penetration depth of the current contact
         */
        auto operator()(Contact const& contact) const noexcept -> real_t;
        //**********************************************************************

        //@}
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace response

  }  // namespace collision

}  // namespace elastica
