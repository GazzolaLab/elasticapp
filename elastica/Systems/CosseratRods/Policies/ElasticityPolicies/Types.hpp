#pragma once

namespace elastica {

  namespace detail_cosserat_rod {

    // Only the final "interface" types are listed here

    // Forward declare all Layers and policies
    template <typename /* Cosserat Rod Traits */, typename /* Block */>
    class LinearHyperelasticModel;

    //    template <typename /* Cosserat Rod Traits */, typename /* Block */>
    //    class LinearViscoHyperelasticModel;

  }  // namespace detail_cosserat_rod

}  // namespace elastica
