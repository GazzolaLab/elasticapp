//=================================================================================================
/*!
 *  \file elastica/Utilities/Byte.hpp
 *  \brief Header file for the byte type
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
#include <type_traits>

namespace elastica {

  //=================================================================================================
  //
  //  TYPE DEFINITION
  //
  //=================================================================================================

  //*************************************************************************************************
  /*!\brief Byte data type of the physics engine.
   * \ingroup util
   *
   * The \a byte data type is guaranteed to be an integral data type of size 1.
   */
  using byte = unsigned char;
  //*************************************************************************************************

  //=================================================================================================
  //
  //  COMPILE TIME CONSTRAINT
  //
  //=================================================================================================

  //*************************************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  namespace check {

    // Constraint on the byte data type
    static_assert(::std::is_integral<byte>::value, "Byte not integral!");
    static_assert(sizeof(byte) == 1, "Byte has non-unit size!");

  }  // namespace check
  /*! \endcond */
  //*************************************************************************************************

}  // namespace elastica
