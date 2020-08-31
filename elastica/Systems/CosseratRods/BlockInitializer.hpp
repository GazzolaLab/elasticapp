#pragma once

#include <cstddef>
#include <tuple>

#include "Systems/CosseratRods/Types.hpp"

namespace elastica{
  // We don't have a piecewise constructor for std::tuple
  // and hence as an alternative, we use this class
  template <typename... Initializers>
  struct CosseratBlockInitializer {
    using InitializerType = std::tuple<Initializers...>;
    std::size_t n_elems;
    InitializerType initializers;
  };

  // Ease in syntax compared to forward_as_tuple?
  template <typename... Initializers>
  constexpr auto make_cosserat_rod(std::size_t n_elems,
                                   Initializers&&... initializers) {
    return CosseratBlockInitializer<Initializers...>{
        n_elems, std::forward_as_tuple(initializers...)};
  }

  /*
template <typename... Initializers>
constexpr auto make_cosserat_rod(std::size_t n_elems,
                                 Initializers... initializers) {
  return BlockInitializer<Initializers...>{
      n_elems, std::forward_as_tuple(initializers...)};
}
   */

}