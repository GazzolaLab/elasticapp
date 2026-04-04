//==============================================================================
/*!
//  \file
//  \brief A system clock wrapping around std::chrono
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
//
// The file was modified from the pe library, with the following LICENSE.
*/
//==============================================================================

//==============================================================================
/*!
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
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <chrono>
#include <memory>

#include "Utilities/Singleton/Singleton.hpp"

namespace elastica {

  //============================================================================
  //
  //  CLASS DEFINITION
  //
  //============================================================================

  //****************************************************************************
  /*!\brief System clock of \elastica
   * \ingroup utils
   *
   * \details
   * SystemClock represents the system clock of \elastica. The system clock is
   * the central timing functionality that can be used to query for the start
   * time of the process, the current timestamp and the elapsed time since the
   * start of the process. It is a convenient wrapper around
   * std::chrono::system_clock for formatting.
   *
   * \usage
   * The following example demonstrates how the single system clock instance can
   * be acquired
   * \snippet Test_SystemClock.cpp system_clock_creation
   * and the following code shows typical use
   * \snippet Test_SystemClock.cpp system_clock_usage
   *
   * SystemClock is mostly intended for use internally, within the logging
   * libraries of \elastica.
   *
   */
  class SystemClock : private Singleton<SystemClock> {
   public:
    //**Type definitions********************************************************
    //! Type of the underlying clock
    using UnderlyingSystemClock = std::chrono::system_clock;
    //! Type of a time point
    using TimePoint = typename UnderlyingSystemClock::time_point;
    //! Type of singleton
    using Singleton<SystemClock>::SingletonType;
    //**************************************************************************

   protected:
    //**Constructors************************************************************
    /*!\name Constructors */
    //@{
    explicit SystemClock();
    //@}
    //**************************************************************************

    //**Friendships*************************************************************
    //! Friends for the singleton pattern
    BEFRIEND_SINGLETON;
    //! Friends for the instantiation of the clock
    friend std::shared_ptr<SystemClock> the_system_clock();
    //**************************************************************************

   public:
    //**************************************************************************
    /*!\brief Destructor
     */
    ~SystemClock() = default;
    //**************************************************************************

    //**Utility functions*******************************************************
    /*!\name Utility functions */
    //@{

    //**************************************************************************
    /*!\brief Returns the timestamp for the start of the process.
     *
     * \return Timestamp for the start of the process.
     */
    static inline std::time_t start() {
      return UnderlyingSystemClock::to_time_t(start_);
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Returns the current timestamp.
     *
     * \return The current timestamp.
     */
    static inline std::time_t now() {
      return UnderlyingSystemClock::to_time_t(UnderlyingSystemClock::now());
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Returns the elapsed time since the start of the process
     *
     * \return Elapsed time since the start of the process
     */
    static inline std::time_t elapsed() {
      return UnderlyingSystemClock::to_time_t(
          TimePoint(UnderlyingSystemClock::now() - start_));
    }
    //**************************************************************************

    //@}
    //**************************************************************************

   private:
    //**Member variables********************************************************
    static TimePoint start_;  //!< Timestamp for the start of the process.
    //**************************************************************************
  };
  //****************************************************************************

  //****************************************************************************
  /*!\brief Returns a handle to the system clock.
   * \ingroup utils
   *
   * \return Handle to the active system clock.
   */
  inline std::shared_ptr<SystemClock> the_system_clock() {
    return SystemClock::instance();
  }
  //****************************************************************************

}  // namespace elastica
