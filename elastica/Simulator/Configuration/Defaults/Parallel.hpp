//==============================================================================
/*!
//  \file
//  \brief Default parallel configuration
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
///
#include "Simulator/Configuration/Defaults/Types.hpp"
///
#include "Simulator/Configuration/ModuleConfiguration/Parallel.hpp"

//==============================================================================
//
//  CONFIGURATION
//
//==============================================================================
#include "Configuration/Parallel.hpp"

namespace elastica {

  namespace configuration {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Default configuration of parallelism (threads) in the simulation
     * \ingroup default_config
     *
     * \details
     * DefaultParallelismConfiguration is the default constructible
     * compile-time customization of parallelism in \elastica
     */
    class DefaultParallelConfiguration : public ParallelConfiguration {
     private:
      //**Type definitions******************************************************
      //! Parent type
     public:
      DefaultParallelConfiguration() noexcept
          : ParallelConfiguration(1, elastica_threads_hint()) {}
    };
    //**************************************************************************

  }  // namespace configuration

}  // namespace elastica
