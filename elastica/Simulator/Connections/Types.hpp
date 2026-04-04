//==============================================================================
/*!
//  \file
//  \brief Types belonging to the Connections module
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//#include "Simulator/Connections/Registration.hpp"

namespace elastica {

  namespace connections {

    ////////////////////////////////////////////////////////////////////////////
    //
    // Forward declarations of connection types
    //
    ////////////////////////////////////////////////////////////////////////////

    namespace detail {
      //**************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      // Base Type
      template <typename /*Registrars*/>
      class ConnectionBase;
      /*! \endcond */
      //**************************************************************************
    }  // namespace detail

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <typename /*Registrars*/, typename /*Concrete*/>
    class Connection;
    /*! \endcond */
    //**************************************************************************

    //**All other types*********************************************************
    /*! \cond ELASTICA_INTERNAL */
    // TODO
    /*! \endcond */
    //**************************************************************************

  }  // namespace connections

}  // namespace elastica
