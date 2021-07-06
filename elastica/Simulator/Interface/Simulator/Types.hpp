//==============================================================================
/*!
//  \file
//  \brief Types belonging to the Simulator module
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

    ////////////////////////////////////////////////////////////////////////////
    //
    // Forward declarations of interface types
    //
    ////////////////////////////////////////////////////////////////////////////

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <typename SimTraits,  // Configuration parameter
              template <typename /*Simulator*/, typename /*SimTraits*/>
              class... Modules>  // Additional modules
    class Simulator;
    /*! \endcond */
    //**************************************************************************

  }  // namespace interface

}  // namespace elastica