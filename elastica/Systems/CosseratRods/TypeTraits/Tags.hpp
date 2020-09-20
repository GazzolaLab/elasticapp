//==============================================================================
/*!
//  \file
//  \brief Type traits for Cosserat rod tag types
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

#include <type_traits>

#include "Systems/CosseratRods/Math/Tags.hpp"
#include "Systems/CosseratRods/TypeTraits/Aliases.hpp"

namespace elastica {

  namespace detail_cosserat_rod {

    namespace tt {

      template <typename T>
      struct IsNodeStaggered : std::is_same<tag_type_t<T>, NodeTagImpl> {};

      template <typename T>
      struct IsElementStaggered : std::is_same<tag_type_t<T>, ElementTagImpl> {
      };

      template <typename T>
      struct IsVoronoiStaggered : std::is_same<tag_type_t<T>, VoronoiTagImpl> {
      };

    }  // namespace tt

  }  // namespace detail_cosserat_rod

}  // namespace elastica
