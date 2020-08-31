#pragma once

namespace elastica {

  namespace detail_cosserat_rod {

    template <typename CRT,                 // Cosserat Rod Traits
              typename ComputationalBlock,  // Block
              template <typename, typename> class ElasticityModel>
    class ExplicitDampingAdapter
        : public ElasticityModel<CRT, ComputationalBlock> {

    };

  }  // namespace detail_cosserat_rod

}  // namespace elastica
