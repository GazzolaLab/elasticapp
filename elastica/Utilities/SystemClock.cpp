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

//******************************************************************************
// Includes
//******************************************************************************

#include "SystemClock.hpp"

namespace elastica {

  //============================================================================
  //
  //  DEFINITION AND INITIALIZATION OF THE STATIC MEMBER VARIABLES
  //
  //============================================================================

  SystemClock::TimePoint SystemClock::start_(UnderlyingSystemClock::now());

  //============================================================================
  //
  //  CONSTRUCTOR
  //
  //============================================================================

  //****************************************************************************
  /*!\brief Constructor for the SystemClock class.
   */
  SystemClock::SystemClock()
      : SingletonType()  // Initialization of the Singleton base object
  {}
  //****************************************************************************

}  // namespace elastica
