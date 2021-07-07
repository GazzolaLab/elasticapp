//==============================================================================
/*!
//  \file
//  \brief Types belonging to State(collection)s
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
    template <typename /*DerivedState*/>  // Type of the state
    class StatesBase;

    template <typename /*FirstState*/, typename /*SecondState*/>
    class StatesStatesAddExpr;

    template <typename /*FirstState*/, typename /*TimeDelta*/>
    class StatesTimeDeltaMultExpr;

    template <typename...>
    class States;
    /*! \endcond */
    //**************************************************************************

  }  // namespace states

}  // namespace elastica
