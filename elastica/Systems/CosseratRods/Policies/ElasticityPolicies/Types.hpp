#pragma once

namespace elastica {

  namespace detail_cosserat_rod {

    // Only the final "interface" types are listed here

    // Forward declare all Layers and policies
    template <typename /* Cosserat Rod Traits */, typename /* Block */>
    class LinearHyperelasticModel;

    //    template <typename /* Cosserat Rod Traits */, typename /* Block */>
    //    class LinearViscoHyperelasticModel;

    template <
        typename /* Cosserat Rod Traits */, typename /* Block */,
        template <typename /* Cosserat Rod Traits */, typename /* Block */>
        class ElasticityModel>
    class ExplicitDampingAdapter;

    template <typename CRT, typename ComputationalBlock>
    using DampedLinearHyperelasticModel =
        ExplicitDampingAdapter<CRT, ComputationalBlock,
                               LinearHyperelasticModel>;

  }  // namespace detail_cosserat_rod

}  // namespace elastica
