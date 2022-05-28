//==============================================================================
/*!
//  \file
//  \brief The default "CosseratRod" in \elastica
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
#include "Systems/CosseratRods/_Types.hpp"

namespace elastica {

  namespace cosserat_rod {

    namespace detail {

      template <typename T, typename B>
      using CosseratRodComponents = ::elastica::SymplecticPolicy<
          T, B, ::elastica::cosserat_rod::component::WithCircularCosseratRod,
          ::elastica::cosserat_rod::component::
              WithExplicitlyDampedDiagonalLinearHyperElasticModel,
          ::elastica::cosserat_rod::component::WithRodKinematics>;

      using CosseratRod = ::elastica::cosserat_rod::CosseratRodPlugin<
          ::elastica::cosserat_rod::DefaultCosseratRodTraits, ::blocks::Block,
          detail::CosseratRodComponents>;
      using CosseratRodSlice = ::elastica::cosserat_rod::CosseratRodPlugin<
          ::elastica::cosserat_rod::DefaultCosseratRodTraits,
          ::blocks::BlockSlice, detail::CosseratRodComponents>;
      using CosseratRodConstSlice = ::elastica::cosserat_rod::CosseratRodPlugin<
          ::elastica::cosserat_rod::DefaultCosseratRodTraits,
          ::blocks::ConstBlockSlice, detail::CosseratRodComponents>;

      using CosseratRodBlock = ::blocks::Block<CosseratRod>;
      using CosseratRodBlockSlice = ::blocks::BlockSlice<CosseratRodSlice>;
      using CosseratRodBlockConstSlice =
          ::blocks::ConstBlockSlice<CosseratRodConstSlice>;

    }  // namespace detail

    using detail::CosseratRod;

  }  // namespace cosserat_rod

}  // namespace elastica
