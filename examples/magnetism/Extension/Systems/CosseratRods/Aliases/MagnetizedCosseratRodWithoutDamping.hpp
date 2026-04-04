//==============================================================================
/*!
//  \file
//  \brief The default "CosseratRod" in \elastica
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
// common
#include <elastica/Systems/Block/Types.hpp>
#include <elastica/Systems/CosseratRods/Types.hpp>
#include <elastica/Systems/common/Types.hpp>

#include "Extension/Systems/CosseratRods/Types.hpp"
#include "Extension/Systems/CosseratRods/Components/Names/MagnetizedCosseratRodWithoutDamping.hpp"

namespace elastica {

  namespace cosserat_rod {

    namespace detail {

      template <typename T, typename B>
      using MagnetizedCosseratRodWithoutDampingComponents =
          ::elastica::SymplecticPolicy<
              T, B,
              ::elastica::cosserat_rod::component::WithCircularCosseratRod,
              ::elastica::cosserat_rod::component::
                  WithDiagonalLinearHyperElasticModel,
              ::elastica::cosserat_rod::component::WithRodKinematics,
              ::elastica::cosserat_rod::component::WithMagnetization,
              ::elastica::cosserat_rod::component::
                  MagnetizedCosseratRodWithoutDampingNameAdapted>;

      using MagnetizedCosseratRodWithoutDamping =
          ::elastica::cosserat_rod::CosseratRodPlugin<
              ::elastica::cosserat_rod::DefaultCosseratRodTraits,
              ::blocks::Block,
              detail::MagnetizedCosseratRodWithoutDampingComponents>;
      using MagnetizedCosseratRodWithoutDampingSlice =
          ::elastica::cosserat_rod::CosseratRodPlugin<
              ::elastica::cosserat_rod::DefaultCosseratRodTraits,
              ::blocks::BlockSlice,
              detail::MagnetizedCosseratRodWithoutDampingComponents>;
      using MagnetizedCosseratRodWithoutDampingConstSlice =
          ::elastica::cosserat_rod::CosseratRodPlugin<
              ::elastica::cosserat_rod::DefaultCosseratRodTraits,
              ::blocks::ConstBlockSlice,
              detail::MagnetizedCosseratRodWithoutDampingComponents>;
      using MagnetizedCosseratRodWithoutDampingView =
          ::elastica::cosserat_rod::CosseratRodPlugin<
              ::elastica::cosserat_rod::DefaultCosseratRodTraits,
              ::blocks::BlockView,
              detail::MagnetizedCosseratRodWithoutDampingComponents>;
      using MagnetizedCosseratRodWithoutDampingConstView =
          ::elastica::cosserat_rod::CosseratRodPlugin<
              ::elastica::cosserat_rod::DefaultCosseratRodTraits,
              ::blocks::ConstBlockView,
              detail::MagnetizedCosseratRodWithoutDampingComponents>;

      using MagnetizedCosseratRodWithoutDampingBlock =
          ::blocks::Block<MagnetizedCosseratRodWithoutDamping>;
      using MagnetizedCosseratRodWithoutDampingBlockSlice =
          ::blocks::BlockSlice<MagnetizedCosseratRodWithoutDampingSlice>;
      using MagnetizedCosseratRodWithoutDampingBlockConstSlice =
          ::blocks::ConstBlockSlice<
              MagnetizedCosseratRodWithoutDampingConstSlice>;
      using MagnetizedCosseratRodWithoutDampingBlockView =
          ::blocks::BlockView<MagnetizedCosseratRodWithoutDampingView>;
      using MagnetizedCosseratRodWithoutDampingBlockConstView =
          ::blocks::ConstBlockView<
              MagnetizedCosseratRodWithoutDampingConstView>;

    }  // namespace detail

    using detail::MagnetizedCosseratRodWithoutDamping;

  }  // namespace cosserat_rod

}  // namespace elastica
