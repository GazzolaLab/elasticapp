//==============================================================================
/*!
//  \file
//  \brief Types belonging to the Constraints interface
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

  namespace interface {

    namespace constraints {

      //////////////////////////////////////////////////////////////////////////
      //
      // Forward declarations of constraint interface types
      //
      //////////////////////////////////////////////////////////////////////////

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      // Unique ID type
      struct ConstraintUUID;

      // Abstract Base Type
      template <typename /*ConstraintRegistrars*/>
      class ConstraintBase;

      // Base Type
      template <typename /*System*/, typename /*ConstraintRegistrars*/>
      class Constraint;

      class ConstrainerBase;

      template <typename /*System*/, typename /*ConcreteBoundaryCondition*/,
                typename /*IndexSet*/>
      class Constrainer;

      template <class /*ConstraintTraits*/, class /*ConstraintsModule*/>
      class ExecutionPolicy;
      /*! \endcond */
      //************************************************************************

    }  // namespace constraints

  }  // namespace interface

  namespace modules {
    ////////////////////////////////////////////////////////////////////////////
    //
    // Forward declarations of the module
    //
    ////////////////////////////////////////////////////////////////////////////

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <class /*SimTraits*/, class /*Simulator*/>
    class Constraints;
    /*! \endcond */
    //**************************************************************************
  }  // namespace modules

}  // namespace elastica
