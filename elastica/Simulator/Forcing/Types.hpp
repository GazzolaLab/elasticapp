//==============================================================================
/*!
//  \file
//  \brief Types belonging to the Forcing module
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//#include "Simulator/Forcing/Registration.hpp"

namespace elastica {

  namespace forcing {

    ////////////////////////////////////////////////////////////////////////////
    //
    // Forward declarations of forcing (external forces) types
    //
    ////////////////////////////////////////////////////////////////////////////

    namespace detail {
      //**************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      // Base Type
      template <typename /*Registrars*/>
      class ForcingBase;
      /*! \endcond */
      //**************************************************************************
    }  // namespace detail

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <typename /*Registrars*/, typename /*Concrete*/>
    class Forcing;

    // All other types

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
