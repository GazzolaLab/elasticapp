//==============================================================================
/*!
//  \file
//  \brief Types belonging to the interface indexing module
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

  namespace interface {

    namespace detail {

      //////////////////////////////////////////////////////////////////////////
      //
      // Forward declarations of indexing routines
      //
      //////////////////////////////////////////////////////////////////////////

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <std::uint8_t Nints, std::uint8_t Nends>
      struct At_t;

      template <typename StartType, typename StopType>
      struct Range;

      /*! \endcond */
      //************************************************************************

    }  // namespace detail

  }  // namespace interface

}  // namespace elastica
