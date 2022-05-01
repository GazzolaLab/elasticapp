//==============================================================================
/*!
//  \file
//  \brief Types of contact cache
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
#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"  // doesnt have types
#include "Simulator/Environments/CollisionSystem/Detection/ContactCache/ContainerPolicy/Types.hpp"

namespace elastica {

  //////////////////////////////////////////////////////////////////////////////
  //
  // Forward declarations
  //
  //////////////////////////////////////////////////////////////////////////////

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  template <typename L,
            typename ContainerPolicy_ = PairwiseCacheContainerPolicy<
                ::elastica::configuration::sequential_policy>>
  class ContactCache;
  /*! \endcond */
  //****************************************************************************

}  // namespace elastica