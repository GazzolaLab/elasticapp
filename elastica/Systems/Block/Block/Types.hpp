//==============================================================================
/*!
//  \file
//  \brief Block types
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

#include "Systems/Block/Block/Block.hpp"

namespace blocks {

  //////////////////////////////////////////////////////////////////////////////
  //
  // Forward declarations of block types
  //
  //////////////////////////////////////////////////////////////////////////////

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  template <typename /* Plugin */>
  class BlockSlice;

  template <typename /* Plugin */>
  class ConstBlockSlice;

  template <typename /*Plugin*/>
  class BlockFacade;

  template <typename /* Plugin */>
  class BlockView;

  template <typename /* Plugin */>
  class ConstBlockView;

  template <typename /*Plugin*/>
  class BlockIterator;

  template <typename /*Plugin*/>
  class ConstBlockIterator;
  /*! \endcond */
  //****************************************************************************

}  // namespace blocks
