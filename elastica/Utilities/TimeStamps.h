//==============================================================================
/*!
//  \file
//  \brief Time stamp utilities
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

#include <chrono>
#include <ctime>
#include <string>

namespace elastica {

  //============================================================================
  //
  //  TIME FUNCTIONS
  //
  //============================================================================

  //****************************************************************************
  /*!\name Time functions */
  //@{
  inline std::string get_date();
  inline std::string get_time();
  inline std::chrono::high_resolution_clock::time_point get_wc_time();
  inline std::clock_t get_cpu_time();
  //@}
  //****************************************************************************

  //****************************************************************************
  /*!\brief Creating a formatted date string in the form YYYY-MM-DD
   * \ingroup utils
   *
   * \return formatted date string
   */
  inline std::string get_date() {
    // Things have not improved much in C++14 for this function
    // Hence using C utilities
    std::time_t t;
    std::tm* local_time;

    constexpr std::size_t buffer_size(50UL);
    char c[buffer_size];

    std::time(&t);
    local_time = std::localtime(&t);
    std::strftime(c, buffer_size, "%Y-%m-%d", local_time);

    return {c};
  }
  //****************************************************************************

  //****************************************************************************
  /*!\brief Creating a formatted time and date string
   * \ingroup utils
   *
   * \return formatted time and date string in the format WEEKDAY DAY.MONTH
   * YEAR, HOUR:MINUTES
   */
  inline std::string get_time() {
    // Things have not improved much in C++14 for this function
    // Hence using C utilities
    std::time_t t;
    std::tm* localTime;

    constexpr std::size_t buffer_size(50UL);
    char c[buffer_size];

    std::time(&t);
    localTime = std::localtime(&t);
    std::strftime(c, buffer_size, "%A, %d.%B %Y, %H:%M", localTime);

    return {c};
  }
  //****************************************************************************

  //****************************************************************************
  /*!\brief Returns the current wall clock time in seconds.
   * \ingroup utils
   *
   * \return The current wall clock time in seconds.
   */
  inline std::chrono::high_resolution_clock::time_point get_wc_time() {
    // TODO : Modernize using <chrono>
    return std::chrono::high_resolution_clock::now();
  }
  //****************************************************************************

  //****************************************************************************
  /*!\brief Returns the current CPU time in seconds.
   * \ingroup util
   *
   * \return The current CPU time in seconds.
   */
  inline std::clock_t get_cpu_time() { return std::clock(); }
  //****************************************************************************

}  // namespace elastica
