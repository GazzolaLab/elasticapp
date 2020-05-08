//=================================================================================================
/*!
 *  \file pe/util/SystemClock.h
 *  \brief Header file for the SystemClock class
 *
 *  Copyright (C) 2009 Klaus Iglberger
 *
 *  This file is part of pe.
 *
 *  pe is free software: you can redistribute it and/or modify it under the terms of the GNU
 *  General Public License as published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  pe is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 *  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with pe. If not,
 *  see <http://www.gnu.org/licenses/>.
 */
//=================================================================================================

#pragma once

#include <ctime>
#include <memory>

#include "Utilities/Singleton/Singleton.hpp"

namespace elastica {

  class SystemClock : private Singleton<SystemClock> {
   private:
    explicit SystemClock();

   public:
    ~SystemClock();
    inline time_t start() const;
    inline time_t now() const;
    inline time_t elapsed() const;

   private:
    static time_t start_;  //!< Timestamp for the start of the process.
    friend std::shared_ptr<SystemClock> theSystemClock();
    BEFRIEND_SINGLETON
  };

  inline std::shared_ptr<SystemClock> theSystemClock() {
    return SystemClock::instance();
  }

  inline time_t SystemClock::start() const { return start_; }

  inline time_t SystemClock::now() const { return time(nullptr); }

  inline time_t SystemClock::elapsed() const {
    return std::time(nullptr) - start_;
  }

}  // namespace elastica
