//==============================================================================
/*!
//  \file
//  \brief Type marking Lagrangian-frame in \elastica
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

#include <cstdint>

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
    /*!\brief Trait defining unique IDs for different directions
     * \ingroup domain
     *
     * LagrangianFrame is a trait defining the type and number of directions in
     * 3D, in a convected frame. It serves as a strong type for all Lagrangian
     * directions within the \elastica library.
     */
    struct LagrangianFrame : public NonCreatable {
     public:
      //**Type definitions******************************************************
      //! The type of direction ID
      enum class DirectionType : std::uint8_t { d1 = 0, d2, d3, Count };
      //************************************************************************

      //**Static members********************************************************
      //! unique ID for D1 direction
      static constexpr DirectionType D1 = DirectionType::d1;
      //! unique ID for D2 direction
      static constexpr DirectionType D2 = DirectionType::d2;
      //! unique ID for D3 direction
      static constexpr DirectionType D3 = DirectionType::d3;
      //! Dimension of simulation
      static constexpr auto Dimension =
          static_cast<std::uint8_t>(DirectionType::Count);
      //************************************************************************
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*!\brief Casts a Lagrangian frame direction into an index value
     * \ingroup simulator
     *
     * \details
     * Converts a LagrangianFrame::DirectionType into an index value in
     * associative arrays.
     *
     * \example
     * \snippet Test_LagrangianFrame.cpp cast_along_eg
     *
     * \param Dir A Direction to cast as index to
     */
    inline constexpr auto cast_along(LagrangianFrame::DirectionType Dir)
    -> std::size_t {
      return static_cast<std::size_t>(Dir);
    }
    //**************************************************************************

  }  // namespace detail

}  // namespace elastica
