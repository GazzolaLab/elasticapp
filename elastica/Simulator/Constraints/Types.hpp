//==============================================================================
/*!
//  \file
//  \brief Types belonging to the Constraints module
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//#include "Simulator/Constraints/Registration.hpp"

namespace elastica {

  namespace constraints {

    ////////////////////////////////////////////////////////////////////////////
    //
    // Forward declarations of constraint (boundary condition) types
    //
    ////////////////////////////////////////////////////////////////////////////

    namespace detail {
      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      // Base Type
      template <typename /*Registrars*/>
      class ConstraintBase;
      /*! \endcond */
      //************************************************************************
    }  // namespace detail

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <typename /*Registrars*/, typename /*Concrete*/>
    class Constraint;
    /*! \endcond */
    //**************************************************************************

    //**All other types*********************************************************
    /*! \cond ELASTICA_INTERNAL */
    class FreeConstraint;
    class FixedConstraint;
    class HingedConstraint;
    /*! \endcond */
    //**************************************************************************

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


  }  // namespace constraints

}  // namespace elastica
