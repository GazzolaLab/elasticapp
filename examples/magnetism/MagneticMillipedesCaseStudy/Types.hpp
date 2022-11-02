//
// Created by Tejaswin Parthasarathy on 11/1/22.
//

#pragma once

#include <elastica/Systems/Types.hpp>

// Make rods with different types

namespace elastica {

  namespace cosserat_rod {

    namespace detail {

      struct BackBoneCrissTag {};
      using BackBoneCrissRod =
          ::elastica::cosserat_rod::TaggedCosseratRodPlugin<
              ::elastica::cosserat_rod::DefaultCosseratRodTraits,
              ::blocks::Block, BackBoneCrissTag,
              CosseratRodWithoutDampingComponents>;
      using BackBoneCrissRodSlice =
          ::elastica::cosserat_rod::TaggedCosseratRodPlugin<
              ::elastica::cosserat_rod::DefaultCosseratRodTraits,
              ::blocks::BlockSlice, BackBoneCrissTag,
              CosseratRodWithoutDampingComponents>;
      using BackBoneCrissRodConstSlice =
          ::elastica::cosserat_rod::TaggedCosseratRodPlugin<
              ::elastica::cosserat_rod::DefaultCosseratRodTraits,
              ::blocks::ConstBlockSlice, BackBoneCrissTag,
              CosseratRodWithoutDampingComponents>;
      using BackBoneCrissRodView =
          ::elastica::cosserat_rod::TaggedCosseratRodPlugin<
              ::elastica::cosserat_rod::DefaultCosseratRodTraits,
              ::blocks::BlockView, BackBoneCrissTag,
              CosseratRodWithoutDampingComponents>;
      using BackBoneCrissRodConstView =
          ::elastica::cosserat_rod::TaggedCosseratRodPlugin<
              ::elastica::cosserat_rod::DefaultCosseratRodTraits,
              ::blocks::ConstBlockView, BackBoneCrissTag,
              CosseratRodWithoutDampingComponents>;

      using BackBoneCrissRodBlock = ::blocks::Block<BackBoneCrissRod>;
      using BackBoneCrissRodBlockSlice =
          ::blocks::BlockSlice<BackBoneCrissRodSlice>;
      using BackBoneCrissRodBlockConstSlice =
          ::blocks::ConstBlockSlice<BackBoneCrissRodConstSlice>;
      using BackBoneCrissRodBlockView =
          ::blocks::BlockView<BackBoneCrissRodView>;
      using BackBoneCrissRodBlockConstView =
          ::blocks::ConstBlockView<BackBoneCrissRodConstView>;

      struct BackBoneCrossTag {};
      using BackBoneCrossRod =
          ::elastica::cosserat_rod::TaggedCosseratRodPlugin<
              ::elastica::cosserat_rod::DefaultCosseratRodTraits,
              ::blocks::Block, BackBoneCrossTag,
              CosseratRodWithoutDampingComponents>;
      using BackBoneCrossRodSlice =
          ::elastica::cosserat_rod::TaggedCosseratRodPlugin<
              ::elastica::cosserat_rod::DefaultCosseratRodTraits,
              ::blocks::BlockSlice, BackBoneCrossTag,
              CosseratRodWithoutDampingComponents>;
      using BackBoneCrossRodConstSlice =
          ::elastica::cosserat_rod::TaggedCosseratRodPlugin<
              ::elastica::cosserat_rod::DefaultCosseratRodTraits,
              ::blocks::ConstBlockSlice, BackBoneCrossTag,
              CosseratRodWithoutDampingComponents>;
      using BackBoneCrossRodView =
          ::elastica::cosserat_rod::TaggedCosseratRodPlugin<
              ::elastica::cosserat_rod::DefaultCosseratRodTraits,
              ::blocks::BlockView, BackBoneCrossTag,
              CosseratRodWithoutDampingComponents>;
      using BackBoneCrossRodConstView =
          ::elastica::cosserat_rod::TaggedCosseratRodPlugin<
              ::elastica::cosserat_rod::DefaultCosseratRodTraits,
              ::blocks::ConstBlockView, BackBoneCrossTag,
              CosseratRodWithoutDampingComponents>;

      using BackBoneCrossRodBlock = ::blocks::Block<BackBoneCrossRod>;
      using BackBoneCrossRodBlockSlice =
          ::blocks::BlockSlice<BackBoneCrossRodSlice>;
      using BackBoneCrossRodBlockConstSlice =
          ::blocks::ConstBlockSlice<BackBoneCrossRodConstSlice>;
      using BackBoneCrossRodBlockView =
          ::blocks::BlockView<BackBoneCrossRodView>;
      using BackBoneCrossRodBlockConstView =
          ::blocks::ConstBlockView<BackBoneCrossRodConstView>;

    }  // namespace detail

  }  // namespace cosserat_rod

}  // namespace elastica