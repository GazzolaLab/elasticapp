//==============================================================================
/*!
//  \file
//  \brief Directions implementation
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

#include <cstddef>

#include "Utilities/NonCreatable.hpp"

namespace elastica {

  namespace detail {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Trait definiing unique IDs for different directions
    // \ingroup domain
    //
    // Directions is a Trait defining the type and number of directions in 3D.
    // It serves as a strong type for Directions within the Elastica++ library.
    */
    struct Directions : public NonCreatable {
     public:
      //**Type definitions******************************************************
      //! The type of direction ID
      enum class DirectionType : std::uint8_t { x, y, z };
      //************************************************************************

      //**Static members********************************************************
      //! unique ID for X direction
      static constexpr DirectionType X = DirectionType::x;

      //! unique ID for Y direction
      static constexpr DirectionType Y = DirectionType::y;

      //! unique ID for Z direction
      static constexpr DirectionType Z = DirectionType::z;

      //! Dimension of simulation
      static constexpr std::uint8_t Dimension = 3;
      //************************************************************************
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace detail

}  // namespace elastica
