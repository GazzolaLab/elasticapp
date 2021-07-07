//==============================================================================
/*!
//  \file
//  \brief Types belonging to SO3 group
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

  namespace states {

    ////////////////////////////////////////////////////////////////////////////
    //
    // Forward declarations of state types
    //
    ////////////////////////////////////////////////////////////////////////////

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <typename ST>  // Type of the state
    class SO3Base;

    template <typename Type,      // storage type
              typename OrderTag>  // order of derivative
    class SO3;

    template <typename ST1,  // Type of the left-hand side state
              typename ST2>  // Type of the right-hand side state
    class SO3RotRotAddExpr;

    template <typename ST1,  // Type of the left-hand side state
              typename ST2>  // Type of the right-hand side state
    class SO3SO3AddExpr;

    template <typename ST1,  // Type of the left-hand side state
              typename TDT>  // Type of the right-hand time-delta value
    class SO3TimeDeltaMultExpr;
    /*! \endcond */
    //**************************************************************************

  }  // namespace states

}  // namespace elastica