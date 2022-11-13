//==============================================================================
/*!
//  \file
//  \brief All component types within @ref magnetized_cosserat_rod_component
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
/// Tags first
#include <elastica/Systems/CosseratRods/Components/Types.hpp>
///
#include "Extension/Systems/CosseratRods/Components/Tags.hpp"
#include "Extension/Systems/CosseratRods/Components/helpers/Types.hpp"
#include "Extension/Systems/CosseratRods/Components/Names/Types.hpp"

namespace elastica {

  namespace cosserat_rod {

    namespace component {

      //**************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <typename /* Cosserat Rod Traits */, typename /* Block */>
      class WithMagnetization;

      /*! \endcond */
      //**************************************************************************

    }  // namespace component

  }  // namespace cosserat_rod

}  // namespace elastica
