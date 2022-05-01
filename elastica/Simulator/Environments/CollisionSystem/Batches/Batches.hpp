//==============================================================================
/*!
//  \file
//  \brief Inclusion of all batch generation components
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

///
#include "Simulator/Environments/CollisionSystem/Batches/Types.hpp"
///
#include "Simulator/Environments/CollisionSystem/Batches/BatchContainerTrait.hpp"
#include "Simulator/Environments/CollisionSystem/Batches/BatchGenerators.hpp"

//==============================================================================
//
//  DOXYGEN DOCUMENTATION
//
//==============================================================================

//******************************************************************************
/*!\defgroup batch_generation Batch Generation
 * \ingroup collision_system
 * \brief Functionality to generate batches of contacts for independent
 * collision resolution.
 */
//******************************************************************************

namespace elastica {

  namespace collision {

    //**************************************************************************
    /*!\brief Namespace for batch generation
     * \ingroup batch_generation
     */
    namespace batches {}
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica