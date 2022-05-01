//==============================================================================
/*!
//  \file
//  \brief Programmatic implementation of the rotation convention in \elastica
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

namespace elastica {

  //**************************************************************************
  /*!\brief The rotation convention followed by \elastica
   * \ingroup utils
   *
   * \details
   * TODO
   */
  struct RotationConvention {
    using type = std::uint8_t;
    //! unique ID for the normal direction
    static constexpr type Normal = 0U;
    //! unique ID for the binormal direction
    static constexpr type Binormal = 1U;
    //! unique ID for the tangent direction
    static constexpr type Tangent = 2U;
  };
  //****************************************************************************

}  // namespace elastica