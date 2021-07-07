//==============================================================================
/*!
//  \file
//  \brief Types belonging to the Elasticity component of
//  @ref cosserat_rod_component
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
#include "Systems/CosseratRods/Components/Elasticity/Tags.hpp"
#include "Systems/CosseratRods/Components/Elasticity/detail/Types.hpp"

namespace elastica {

  namespace cosserat_rod {

    namespace component {

      //////////////////////////////////////////////////////////////////////////
      //
      // Forward declarations of elasticity types in the interface
      //
      //////////////////////////////////////////////////////////////////////////

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <typename /* Cosserat Rod Traits */, typename /* Block */>
      class WithDiagonalLinearHyperElasticModel;

      using detail::ExplicitDampingAdapter;

      using detail::ExplicitDampingAdapterPerRod;

      template <typename /* Cosserat Rod Traits */, typename /* Block */>
      class WithExplicitlyDampedDiagonalLinearHyperElasticModel;
      /*! \endcond */
      //************************************************************************

    }  // namespace component

  }  // namespace cosserat_rod

}  // namespace elastica
