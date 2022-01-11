//==============================================================================
/*!
//  \file
//  \brief A CosseratRod without damping in \elastica
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
// common
#include "Systems/common/Types.hpp"
// blocks
#include "Systems/Block/Types.hpp"
// cosserat rod components
#include "Systems/CosseratRods/Components/Types.hpp"

namespace elastica {

  namespace cosserat_rod {

    namespace detail {

      template <typename T, typename B>
      using CosseratRodWithoutDampingComponents = ::elastica::SymplecticPolicy<
          T, B, ::elastica::cosserat_rod::component::WithCircularCosseratRod,
          ::elastica::cosserat_rod::component::
              WithDiagonalLinearHyperElasticModel,
          ::elastica::cosserat_rod::component::WithRodKinematics>;

    }  // namespace detail

    using CosseratRodWithoutDamping =
        ::elastica::cosserat_rod::CosseratRodPlugin<
            ::elastica::cosserat_rod::DefaultCosseratRodTraits, ::blocks::Block,
            detail::CosseratRodWithoutDampingComponents>;

  }  // namespace cosserat_rod

}  // namespace elastica
