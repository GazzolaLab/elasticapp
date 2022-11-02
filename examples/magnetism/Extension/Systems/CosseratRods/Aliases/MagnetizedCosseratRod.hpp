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
#include <elastica/Systems/Block/Types.hpp>
#include <elastica/Systems/CosseratRods/Types.hpp>
#include <elastica/Systems/common/Types.hpp>

#include "Extension/Systems/CosseratRods/Types.hpp"

namespace elastica {

  namespace cosserat_rod {

    namespace detail {

      template <typename T, typename B>
      using MagnetizedCosseratRodComponents = ::elastica::SymplecticPolicy<
          T, B, ::elastica::cosserat_rod::component::WithCircularCosseratRod,
          ::elastica::cosserat_rod::component::
              WithExplicitlyDampedDiagonalLinearHyperElasticModel,
          ::elastica::cosserat_rod::component::WithRodKinematics,
          ::elastica::cosserat_rod::component::WithMagnetization>;

      using MagnetizedCosseratRod = ::elastica::cosserat_rod::CosseratRodPlugin<
          ::elastica::cosserat_rod::DefaultCosseratRodTraits, ::blocks::Block,
          detail::MagnetizedCosseratRodComponents>;
      using MagnetizedCosseratRodSlice =
          ::elastica::cosserat_rod::CosseratRodPlugin<
              ::elastica::cosserat_rod::DefaultCosseratRodTraits,
              ::blocks::BlockSlice, detail::MagnetizedCosseratRodComponents>;
      using MagnetizedCosseratRodConstSlice =
          ::elastica::cosserat_rod::CosseratRodPlugin<
              ::elastica::cosserat_rod::DefaultCosseratRodTraits,
              ::blocks::ConstBlockSlice,
              detail::MagnetizedCosseratRodComponents>;
      using MagnetizedCosseratRodView =
          ::elastica::cosserat_rod::CosseratRodPlugin<
              ::elastica::cosserat_rod::DefaultCosseratRodTraits,
              ::blocks::BlockView, detail::MagnetizedCosseratRodComponents>;
      using MagnetizedCosseratRodConstView =
          ::elastica::cosserat_rod::CosseratRodPlugin<
              ::elastica::cosserat_rod::DefaultCosseratRodTraits,
              ::blocks::ConstBlockView,
              detail::MagnetizedCosseratRodComponents>;

      using MagnetizedCosseratRodBlock = ::blocks::Block<MagnetizedCosseratRod>;
      using MagnetizedCosseratRodBlockSlice =
          ::blocks::BlockSlice<MagnetizedCosseratRodSlice>;
      using MagnetizedCosseratRodBlockConstSlice =
          ::blocks::ConstBlockSlice<MagnetizedCosseratRodConstSlice>;
      using MagnetizedCosseratRodBlockView =
          ::blocks::BlockView<MagnetizedCosseratRodView>;
      using MagnetizedCosseratRodBlockConstView =
          ::blocks::ConstBlockView<MagnetizedCosseratRodConstView>;

    }  // namespace detail

    using detail::MagnetizedCosseratRod;

  }  // namespace cosserat_rod

}  // namespace elastica
