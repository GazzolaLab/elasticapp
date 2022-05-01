//==============================================================================
/*!
//  \file
//  \brief Batch container policy
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
#include <functional>

#include "Simulator/Environments/CollisionSystem/Contact/Types.hpp"
// I steal elements from Contacts
#include "Simulator/Environments/CollisionSystem/Contact/ContainerPolicy/Types.hpp"
//
#include "Simulator/Environments/CollisionSystem/Contact/ContainerPolicy/NonSequentialPolicy.hpp"
#include "Simulator/Environments/CollisionSystem/Contact/ContainerPolicy/SequentialPolicy.hpp"

namespace elastica {

  namespace collision {

    namespace batches {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Policy class that determines container type of a contact batch
       * \ingroup collision_system
       *
       * \tparam ExecutionPolicy_ The execution policy followed by the collision
       * system while using the Batch container
       */
      template <typename ExecutionPolicy_>
      struct BatchContainerTrait {
        using ExecutionPolicy = ExecutionPolicy_;
        using Batch = typename detail::ContactContainerPolicy<
            ExecutionPolicy>::template type<std::reference_wrapper<Contact>>;
        using type = typename detail::ContactContainerPolicy<
            ExecutionPolicy>::template type<Batch>;
      };
      //************************************************************************

    }  // namespace batches

  }  // namespace collision

}  // namespace elastica
