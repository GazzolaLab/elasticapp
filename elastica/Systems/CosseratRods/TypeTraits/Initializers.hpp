//==============================================================================
/*!
//  \file
//  \brief Initializer type traits for Cosserat rod types
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

#include "Utilities/TypeTraits.hpp"
#include "Utilities/TypeTraits/IsDetected.hpp"

// Forward declarations
#include "Systems/CosseratRods/BlockInitializer.hpp"
#include "Systems/CosseratRods/TypeTraits/Aliases.hpp"

namespace elastica {

  namespace cosserat_rod {

    namespace tt {

      //      template <typename Trait, typename T>
      //      struct IsBlockInitializer
      //          : ::tt::is_a_t<CosseratBlockInitializer<Trait>::template
      //          Initializer,
      //                         T> {};
      //
      //      template <typename T>
      //      struct CheckForInitializerTagType
      //          : ::tt::is_detected<initializer_tag_t, T> {};

    }  // namespace tt

  }  // namespace cosserat_rod

}  // namespace elastica