//==============================================================================
/*!
//  \file
//  \brief Block type aliases
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

namespace blocks {

  //============================================================================
  //
  //  ALIAS DECLARATIONS
  //
  //============================================================================

  //****************************************************************************
  /*!\brief Gets the nested `InitializedVariables` from `T`
  // \ingroup blocks
  //
  // The initialized_variables_t alias declaration provides a convenient
  // shortcut to access the nested `InitializedVariables` type definition of
  // the given type \a T. The following code example shows both ways to access
  // the nested type definition:
  //
  // \example
     \code
       using Type1 = typename T::InitializedVariables;
       using Type2 = initialized_variables_t<T>;
     \endcode
  //
  // \see Block, BlockSlice
  */
  /// [initialized_variables_t]
  template <typename T>
  using initialized_variables_t = typename T::InitializedVariables;
  /// [initialized_variables_t]
  //****************************************************************************

}  // namespace blocks
