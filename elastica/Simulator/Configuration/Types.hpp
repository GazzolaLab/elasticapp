//==============================================================================
/*!
//  \file
//  \brief All configuration types
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include "Simulator/Configuration/ModuleConfiguration/Types.hpp"
//
#include "Simulator/Configuration/Defaults/Types.hpp"

namespace elastica {

  namespace configuration {

    ////////////////////////////////////////////////////////////////////////////
    //
    // Forward declarations of configuration types
    //
    ////////////////////////////////////////////////////////////////////////////

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <class... ModuleConfigs>
    class SimulatorConfiguration;

    class DefaultSimulatorConfiguration;
    /*! \endcond */
    //**************************************************************************

  }  // namespace configuration

}  // namespace elastica