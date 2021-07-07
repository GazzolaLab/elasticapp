//==============================================================================
/*!
//  \file
//  \brief Block Variable types
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

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */

  //////////////////////////////////////////////////////////////////////////////
  //
  // Forward declarations of block variable types
  //
  //////////////////////////////////////////////////////////////////////////////

  template <typename Variable>
  struct VariableSlice;

  template <typename Variable>
  struct ConstVariableSlice;

  template <typename ParameterTag, typename RankTag, typename... Tags>
  struct Variable;

  template <typename ParameterTag, typename RankTag, typename... Tags>
  struct InitializedVariable;

  template <typename VariableOrDerivedVariable>
  struct same_as;

  //////////////////////////////////////////////////////////////////////////////
  //
  // Forward declarations of Initializer
  //
  //////////////////////////////////////////////////////////////////////////////
  namespace detail{
    template <typename ParameterTag, typename NamedFunction>
    struct BlockInitializerVariable;
  }

  template <typename Plugin, typename ...InitializerVariables>
  struct BlockInitializer;
  /*! \endcond */
  //****************************************************************************

}  // namespace blocks
