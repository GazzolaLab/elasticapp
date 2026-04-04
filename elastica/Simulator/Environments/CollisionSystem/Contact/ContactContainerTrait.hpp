//==============================================================================
/*!
//  \file
//  \brief Contact container policy
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include "Simulator/Environments/CollisionSystem/Contact/ContactVectorAdapter.hpp"
#include "Simulator/Environments/CollisionSystem/Contact/Types.hpp"
//
#include "Simulator/Environments/CollisionSystem/Contact/ContainerPolicy/Types.hpp"
//
#include "Simulator/Environments/CollisionSystem/Contact/ContainerPolicy/NonSequentialPolicy.hpp"
#include "Simulator/Environments/CollisionSystem/Contact/ContainerPolicy/SequentialPolicy.hpp"

namespace elastica {

  namespace collision {

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
    struct ContactContainerTrait {
      using ExecutionPolicy = ExecutionPolicy_;
      using type =
          ContactVectorAdapter<Contact,
                               typename detail::ContactContainerPolicy<
                                   ExecutionPolicy>::template type<Contact>>;
    };
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica