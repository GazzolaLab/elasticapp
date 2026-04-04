//==============================================================================
/*!
//  \file
//  \brief Type marking multiplication operations between a state and time
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include "Systems/States/Expressions/Expr/StateExpr.hpp"

namespace elastica {

  namespace states {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Base class for all state--time multiplication expression
     * templates.
     * \ingroup states
     *
     * StateTimeMultExpr marks all expression templates that implement
     * state-time multiplications. All classes, that represent a state-time
     * multiplication  and that are used within the expression template
     * environment of \elastica have to derive publicly from this
     * class in order to qualify as multiplication expression template.
     *
     * \tparam MultExpr type of the multiplication expression
     */
    template <typename MultExpr>  // Base type of the expression
    struct StateTimeMultExpr : public StateExpr<MultExpr> {};
    //**************************************************************************

  }  // namespace states

}  // namespace elastica
