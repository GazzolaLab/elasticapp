//==============================================================================
/*!
//  \file
//  \brief Types belonging to the Forcing interface
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

#include <cstdint>

namespace elastica {

  namespace interface {

    namespace forcing {

      //////////////////////////////////////////////////////////////////////////
      //
      // Forward declarations of environment interface types
      //
      //////////////////////////////////////////////////////////////////////////

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      // Unique ID type
      struct ForcingUUID;

      // Abstract Base Type
      template <typename /*ForcingRegistrars*/>
      class ForcingBase;

      // Base Type
      template <typename /*System*/, typename /*ForcingRegistrars*/>
      class Forcing;

      class ForcerBase;

      template <typename /*System*/, typename /*ConcreteBoundaryCondition*/,
                typename /*IndexSet*/>
      class Forcer;

      template <class /*ForcingTraits*/, class /*ForcingModule*/>
      class ExecutionPolicy;
      /*! \endcond */
      //************************************************************************

    }  // namespace forcing

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
    class Forcing;
    /*! \endcond */
    //**************************************************************************
  }  // namespace modules

}  // namespace elastica
