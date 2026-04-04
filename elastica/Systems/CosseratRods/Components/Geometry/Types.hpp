//==============================================================================
/*!
//  \file
//  \brief Types belonging to the Geometry component of
//  @ref cosserat_rod_component
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include "Systems/CosseratRods/Components/Geometry/Tags.hpp"
#include "Systems/CosseratRods/Components/Geometry/detail/Types.hpp"

namespace elastica {

  namespace cosserat_rod {

    namespace component {

      //////////////////////////////////////////////////////////////////////////
      //
      // Forward declarations of geometry types in the interface
      //
      //////////////////////////////////////////////////////////////////////////

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <typename /* Cosserat Rod Traits */, typename /* Block */>
      class WithCircularCosseratRod;

      template <typename /* Cosserat Rod Traits */, typename /* Block */>
      class WithSquareCosseratRod;
      /*! \endcond */
      //************************************************************************

    }  // namespace component

  }  // namespace cosserat_rod

}  // namespace elastica
