//==============================================================================
/*!
//  \file
//  \brief Types of container policies for contact vector
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

namespace elastica {

  namespace collision {

    namespace detail {

      //==========================================================================
      //
      //  CLASS DEFINITION
      //
      //==========================================================================

      //**************************************************************************
      /*!\brief Policy class that determines container type of a contact vector
       * \ingroup collision_system
       *
       * \tparam ExecutionPolicy_ The execution policy followed by the collision
       * system while using the Contact continer
       */
      template <typename ExecutionPolicy_>
      struct ContactContainerPolicy;
      //**************************************************************************

    }  // namespace detail

  }  // namespace collision

}  // namespace elastica
