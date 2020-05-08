//=================================================================================================
/*!
 *  \file pe/util/Time.h
 *  \brief Header file for time functions
 *
 *  Copyright (C) 2009 Klaus Iglberger
 *
 *  This file is part of pe.
 *
 *  pe is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 *  pe is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with
 * pe. If not, see <http://www.gnu.org/licenses/>.
 */
//=================================================================================================

#pragma once

//*************************************************************************************************
// Includes
//*************************************************************************************************

#if defined(_MSC_VER)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <sys/timeb.h>
#include <time.h>
#include <windows.h>
#include <winsock.h>
#else
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#endif
#include <ctime>
#include <string>

namespace elastica {

  //=================================================================================================
  //
  //  TIME FUNCTIONS
  //
  //=================================================================================================

  //*************************************************************************************************
  /*!\name Time functions */
  //@{
  inline std::string get_date();
  inline std::string get_time();
  inline double get_wc_time();
  inline double get_cpu_time();
  //@}
  //*************************************************************************************************

  //*************************************************************************************************
  /*!\brief Creating a formated date string in the form YYYY-MM-DD
   * \ingroup util
   *
   * \return Formated date string
   */
  inline std::string get_date() {
    // Things have not improved much in C++14 for this function
    // Hence using C utilities
    std::time_t t;
    std::tm* local_time;
    char c[50];

    std::time(&t);
    local_time = std::localtime(&t);
    std::strftime(c, 50, "%Y-%m-%d", local_time);

    return std::string(c);
  }
  //*************************************************************************************************

  //*************************************************************************************************
  /*!\brief Creating a formated time and date string
   * \ingroup util
   *
   * \return Formated time and date string in the format WEEKDAY DAY.MONTH YEAR,
   * HOUR:MINUTES
   */
  inline std::string get_time() {
    // Things have not improved much in C++14 for this function
    // Hence using C utilities
    std::time_t t;
    std::tm* localTime;
    char c[50];

    std::time(&t);
    localTime = std::localtime(&t);
    std::strftime(c, 50, "%A, %d.%B %Y, %H:%M", localTime);

    return std::string(c);
  }
  //*************************************************************************************************

  //*************************************************************************************************
  /*!\brief Returns the current wall clock time in seconds.
   * \ingroup util
   *
   * \return The current wall clock time in seconds.
   */
  inline double get_wc_time() {
    // TODO : Modernize using <chrono>
#ifdef WIN32
    struct _timeb timeptr;
    _ftime(&timeptr);
    return (static_cast<double>(timeptr.time) +
            static_cast<double>(timeptr.millitm) / 1E3);
#else
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return (static_cast<double>(tp.tv_sec) +
            static_cast<double>(tp.tv_usec) / 1E6);
#endif
  }
  //*************************************************************************************************

  //*************************************************************************************************
  /*!\brief Returns the current CPU time in seconds.
   * \ingroup util
   *
   * \return The current CPU time in seconds.
   */
  inline double get_cpu_time() {
    // TODO : Modernize using <chrono>
#ifdef WIN32
    FILETIME CreateTime, ExitTime, KernelTime, UserTime;
    SYSTEMTIME SysTime;

    if (GetProcessTimes(GetCurrentProcess(), &CreateTime, &ExitTime,
                        &KernelTime, &UserTime) != TRUE) {
      return 0.0;
    } else {
      FileTimeToSystemTime(&UserTime, &SysTime);
      return (static_cast<double>(SysTime.wSecond) +
              static_cast<double>(SysTime.wMilliseconds) / 1E3);
    }
#else
    struct rusage ruse;
    getrusage(RUSAGE_SELF, &ruse);
    return (static_cast<double>(ruse.ru_utime.tv_sec) +
            static_cast<double>(ruse.ru_utime.tv_usec) / 1E6);
#endif
  }
  //*************************************************************************************************

}  // namespace elastica
