//==============================================================================
/*!
//  \file
//  \brief All types of the Singleton module in \elastica
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

  //****************************************************************************
  // Forward declarations
  //****************************************************************************
  template <typename /* T */>
  class Dependency;

  template <class /* T */, class... /* Dependencies */>
  class Singleton;

}  // namespace elastica