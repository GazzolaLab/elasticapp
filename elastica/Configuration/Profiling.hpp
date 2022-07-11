//==============================================================================
/*!
//  \file
//  \brief Configuration of profiling
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
/*!\brief Profiling mode switch.
 * \ingroup config
 *
 * This value toggles the profiling mode of \elastica. If set to \a
 * true, profiling data will be gathered and written to the log file(s).
 * In case the profiling mode is activated and the current logging level is set
 * too low, a compile time error is created. Also note that collecting profiling
 * data might induce a noticeable runtime overhead. Therefore the profiling mode
 * is switched off per default, which completely removes all runtime overhead.
 *
 * Possible settings for the profiling mode:
 *  - Deactivated: \a false (default)
 *  - Activated  : \a true
 */
constexpr bool elastica_profiling_mode() {
#if defined(ELASTICA_PROFILING_MODE)
  return true;
#else
  return false;
#endif
}
//******************************************************************************
