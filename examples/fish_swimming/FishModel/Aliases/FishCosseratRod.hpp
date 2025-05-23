#pragma once

// common
#include <elastica/Systems/common/Types.hpp>
// blocks
#include <elastica/Systems/Block/Types.hpp>
// cosserat rod components
#include <elastica/Systems/CosseratRods/Components/Names/CosseratRod.hpp>
#include <examples/fish_swimming/FishModel/Components/WithFishGeometry.hpp>

namespace fish {

  namespace details {

    template <typename T, typename B>
    using FishComponents = ::elastica::SymplecticPolicy<
        T, B, ::fish::component::WithFishGeometry,
        ::elastica::cosserat_rod::component::
            WithDiagonalLinearHyperElasticModel,
        ::elastica::cosserat_rod::component::WithRodKinematics,
        ::elastica::cosserat_rod::component::CosseratRodNameAdapted>;

    using Fish = ::elastica::cosserat_rod::CosseratRodPlugin<
        ::elastica::cosserat_rod::DefaultCosseratRodTraits, ::blocks::Block,
        FishComponents>;
    using FishSlice = ::elastica::cosserat_rod::CosseratRodPlugin<
        ::elastica::cosserat_rod::DefaultCosseratRodTraits,
        ::blocks::BlockSlice, FishComponents>;
    using FishConstSlice = ::elastica::cosserat_rod::CosseratRodPlugin<
        ::elastica::cosserat_rod::DefaultCosseratRodTraits,
        ::blocks::ConstBlockSlice, FishComponents>;
    using FishView = ::elastica::cosserat_rod::CosseratRodPlugin<
        ::elastica::cosserat_rod::DefaultCosseratRodTraits, ::blocks::BlockView,
        FishComponents>;
    using FishConstView = ::elastica::cosserat_rod::CosseratRodPlugin<
        ::elastica::cosserat_rod::DefaultCosseratRodTraits,
        ::blocks::ConstBlockView, FishComponents>;

    using FishBlock = ::blocks::Block<Fish>;
    using FishBlockSlice = ::blocks::BlockSlice<FishSlice>;
    using FishBlockConstSlice = ::blocks::ConstBlockSlice<FishConstSlice>;
    using FishBlockView = ::blocks::BlockView<FishView>;
    using FishBlockConstView = ::blocks::ConstBlockView<FishConstView>;

  }  // namespace details

  using Fish = details::Fish;

}  // namespace fish
