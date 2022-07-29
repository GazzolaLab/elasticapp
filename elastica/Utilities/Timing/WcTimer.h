//==============================================================================
/*!
//  \file
//  \brief Definition of WCTimer
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include "Utilities/Timing/Timer.h"
#include "Utilities/Timing/WcPolicy.h"

namespace elastica {

  namespace timing {

    //==========================================================================
    //
    //  TYPE DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Progress timer for wall clock time measurements.
     * \ingroup timing
     *
     * The WcTimer combines the Timer class template with the WcPolicy timing
     * policy. It measures the amount of "wall clock" time elapsing for the
     * processing of a program or code fragment. In contrast to the measurement
     * of CPU time, the wall clock time also contains waiting times such as
     * input/output operations.
     */
    using WcTimer = Timer<WcPolicy>;
    //**************************************************************************

  }  // namespace timing

}  // namespace elastica
