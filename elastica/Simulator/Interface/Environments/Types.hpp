//==============================================================================
/*!
//  \file
//  \brief Types belonging to the Environments interface
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

    namespace environments {

      //////////////////////////////////////////////////////////////////////////
      //
      // Forward declarations of environment types
      //
      //////////////////////////////////////////////////////////////////////////

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <class /*EnvironmentsTraits*/, class /* Sim*/>
      class WithImposedPrimitives;

      template <class /*EnvironmentsTraits*/, class /* Sim*/>
      class WithPrimitives;

      template <class /*EnvironmentsTraits*/, class /*EnvironmentsModule*/>
      class WithPrimitivesExecutionPolicy;

      template <class /*EnvironmentsTraits*/, class /*EnvironmentsModule*/>
      class ExecutionPolicy;
      /*! \endcond */
      //************************************************************************

    }  // namespace environments

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
    class Environments;
    /*! \endcond */
    //**************************************************************************
  }  // namespace modules

}  // namespace elastica
