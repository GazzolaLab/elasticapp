//==============================================================================
/*!
//  \file
//  \brief Definition of CPUTimer
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

#include "Utilities/Timing/CpuPolicy.h"
#include "Utilities/Timing/Timer.h"

namespace elastica {

  namespace timing {

    //==========================================================================
    //
    //  TYPE DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Progress timer for CPU time measurements.
     * \ingroup timing
     *
     * The CpuTimer combines the Timer class template with the CpuPolicy timing
     * policy. It measures the amount of time the measured program or code
     * fragment uses in processing central processing unit (CPU) instructions.
     */
    using CpuTimer = Timer<CpuPolicy>;
    //**************************************************************************

  }  // namespace timing

}  // namespace elastica
