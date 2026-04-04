//==============================================================================
/*!
//  \file
//  \brief Types belonging to the systems module
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

namespace elastica {

  namespace interface {

    namespace systems {

      ////////////////////////////////////////////////////////////////////////////
      //
      // Forward declarations of interface types
      //
      ////////////////////////////////////////////////////////////////////////////

      //**************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <class /*SystemsTraits*/, class /*SystemsModule*/>
      class ExecutionPolicy;
      /*! \endcond */
      //**************************************************************************

    }  // namespace systems

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
    class Systems;
    /*! \endcond */
    //**************************************************************************

  }  // namespace modules

}  // namespace elastica
