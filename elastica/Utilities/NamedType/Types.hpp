//==============================================================================
/*!
//  \file
//  \brief Types belonging to the Named Type module
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

namespace named_type {

  //////////////////////////////////////////////////////////////////////////////
  //
  // Forward declarations of named types
  //
  //////////////////////////////////////////////////////////////////////////////
  struct _DefaultNamedTypeParameterTag_;

  template <typename T, typename Tag, template <typename> class... Affordances>
  class NamedType;

}  // namespace named_type