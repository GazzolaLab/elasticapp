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
      using MagnetizedCosseratRodWithoutDampingComponents =
          ::elastica::SymplecticPolicy<
              T, B,
              ::elastica::cosserat_rod::component::WithCircularCosseratRod,
              ::elastica::cosserat_rod::component::
                  WithDiagonalLinearHyperElasticModel,
              ::elastica::cosserat_rod::component::WithRodKinematics,
              ::elastica::cosserat_rod::component::WithMagnetization>;

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

      using MagnetizedCosseratRodWithoutDampingBlock =
          ::blocks::Block<MagnetizedCosseratRodWithoutDamping>;
      using MagnetizedCosseratRodWithoutDampingBlockSlice =
          ::blocks::BlockSlice<MagnetizedCosseratRodWithoutDampingSlice>;
      using MagnetizedCosseratRodWithoutDampingBlockConstSlice =
          ::blocks::ConstBlockSlice<
              MagnetizedCosseratRodWithoutDampingConstSlice>;

    }  // namespace detail

    using detail::MagnetizedCosseratRodWithoutDamping;

  }  // namespace cosserat_rod

}  // namespace elastica
