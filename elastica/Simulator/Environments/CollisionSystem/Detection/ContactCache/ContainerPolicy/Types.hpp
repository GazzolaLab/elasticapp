//==============================================================================
/*!
//  \file
//  \brief Types of container policies for contact cache
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

  //////////////////////////////////////////////////////////////////////////////
  //
  // Forward declarations
  //
  //////////////////////////////////////////////////////////////////////////////

  //============================================================================
  //
  //  CLASS DEFINITION
  //
  //============================================================================

  //****************************************************************************
  /*!\brief Policy class that determines container type of PairwiseCache
   * \ingroup collision_system
   *
   * \tparam ExecutionPolicy_ The execution policy followed by the collision
   * system while using the PairwiseCache
   */
  template <typename ExecutionPolicy_>
  struct PairwiseCacheContainerPolicy;
  //****************************************************************************

}  // namespace elastica