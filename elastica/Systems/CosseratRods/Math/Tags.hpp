#pragma once

// Different tags for the Cosserat rod data-structures

#include "ErrorHandling/Assert.hpp"

namespace elastica {

  namespace detail_cosserat_rod {

    struct NodeTag {
      static inline auto get_dofs(std::size_t n_elems) noexcept -> std::size_t {
        ELASTICA_ASSERT(n_elems > 0UL,
                        "presribed number of elements incorrect");
        return (n_elems + 1UL);
      }
    };

    struct ElementTag {
      inline static auto get_dofs(std::size_t n_elems) noexcept -> std::size_t {
        ELASTICA_ASSERT(n_elems > 0UL,
                        "presribed number of elements incorrect");
        return n_elems;
      }
    };

    struct VoronoiTag {
      inline static auto get_dofs(std::size_t n_elems) noexcept -> std::size_t {
        ELASTICA_ASSERT(n_elems > 0UL,
                        "presribed number of elements incorrect");
        return (n_elems - 1UL);
      }
    };

    // Declare all operations allowed for these tags

  }  // namespace detail_cosserat_rod

}  // namespace elastica