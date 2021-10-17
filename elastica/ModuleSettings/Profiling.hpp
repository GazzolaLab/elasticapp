//==============================================================================
/*!
//  \file
//  \brief ModuleSettings for profiling
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

namespace elastica {

  //============================================================================
  //
  //  PROFILING CONFIGURATION
  //
  //============================================================================

#include "Configuration/Profiling.hpp"

//******************************************************************************
/*!\brief Profiling section for time measurements and code analysis
 * \ingroup config
 *
 * This macro enables profiling if its switched on (i.e., elastica::profiling is
 * set to \a true). The following code demonstrates the use of this macro for
 * the time measurement of a certain code section:
 *
 * \code
 * elastica::timing::WcTimer timer;

 * // Starting the timer
 * ELASTICA_PROFILING_SECTION {
 *    timer.start();
 * }

 * // Code section to be analysed
 * ...

 * // Ending the timer
 * ELASTICA_PROFILING_SECTION {
 *    timer.end();
 * }
 * \endcode
 */
#define ELASTICA_PROFILING_SECTION if (elastica_profiling_mode())

}  // namespace elastica