//==============================================================================
/*!
//  \file
//  \brief Types belonging to the Connections interface
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

    namespace connections {

      //////////////////////////////////////////////////////////////////////////
      //
      // Forward declarations of connection interface types
      //
      //////////////////////////////////////////////////////////////////////////

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      // Unique ID type
      struct ConnectionUUID;

      // Abstract Base Type
      template <typename /*ConnectionRegistrars*/>
      class ConnectionBase;

      // Base Type
      template <typename /*FirstSystem*/, typename /* SecondSystem*/,
                typename /*ConnectionRegistrars*/>
      class Connection;

      class ConnecterBase;

      template <typename /*ConnectionPolicy*/, typename /*FirstSystem*/,
                typename /* SecondSystem*/, typename /*ConcreteConnection*/>
      class Connecter;

      template <class /*ConnectionTraits*/, class /*ConnectionsModule*/>
      class ExecutionPolicy;
      /*! \endcond */
      //************************************************************************

    }  // namespace connections

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
    class Connections;
    /*! \endcond */
    //**************************************************************************
  }  // namespace modules

}  // namespace elastica
