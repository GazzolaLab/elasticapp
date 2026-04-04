//==============================================================================
/*!
//  \file
//  \brief Block concept types
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

namespace blocks {

  //////////////////////////////////////////////////////////////////////////////
  //
  // Forward declarations of block concepts
  //
  //////////////////////////////////////////////////////////////////////////////

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  template <typename /*Derived*/>
  class Gettable;

  template <typename /*Derived*/>
  class Sliceable;

  template <typename /*Derived*/>
  class Spannable;

  template <typename /*Derived*/>
  class Subdividable;

  template <typename /*Derived*/>
  class Viewable;
  /*! \endcond */
  //****************************************************************************

}  // namespace blocks
