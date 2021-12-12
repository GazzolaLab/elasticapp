//==============================================================================
/*!
//  \file
//  \brief Types belonging to the Materials module
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

#include <cstdint>
#include <vector> // This seems like an anti-pattern

namespace elastica {

  //////////////////////////////////////////////////////////////////////////////
  //
  // Forward declarations of material types
  //
  //////////////////////////////////////////////////////////////////////////////

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  class Material;
  /*! \endcond */
  //****************************************************************************

  //**Type definitions**********************************************************
  //! ID type for materials
  using MaterialID = std::uint32_t;
  //! Collection of materials
  using Materials = std::vector<Material>;
  //****************************************************************************

}