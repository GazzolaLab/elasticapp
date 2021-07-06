//==============================================================================
/*!
//  \file
//  \brief Types belonging to the Constraints module
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

  namespace constraints {

    ////////////////////////////////////////////////////////////////////////////
    //
    // Forward declarations of constraint (boundary condition) types
    //
    ////////////////////////////////////////////////////////////////////////////

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    // Abstract Base Type
    class AbstractBoundaryConditionBase;

    // Base Type
    template <typename BC,  // The implemented type of the boundary
              typename S>   // The system
    class BoundaryConditionBase;

    // All other types
    class FreeBoundaryCondition;
    class FixedBoundaryCondition;
    class HingedBoundaryCondition;

    //==========================================================================
    //
    //  ALIAS DECLARATIONS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Alias declaration for a BoundaryConditionBase
    // \ingroup constraints
    */
    //    using ConstraintBase = BoundaryConditionBase;
    //**************************************************************************

    /*! \endcond */
    //**************************************************************************

  }  // namespace constraints

}  // namespace elastica
