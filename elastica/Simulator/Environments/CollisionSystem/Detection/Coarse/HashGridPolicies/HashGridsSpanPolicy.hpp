#pragma once

#include "elastica/Utilities/DefineTypes.h"

namespace elastica {

  namespace collision {

    namespace detection {

      namespace coarse {

        // TODO
        // The hash calculation, for an infinite grid, always puts
        // the object in a domain of [0, cell_count * cell_size]
        // where cell_size is determined from the inserted object

        // However, for a periodic domain, one can enforce strict
        // requirements for the cell spans in the x, y or z directions
        // (requiring that the grid fits snugly within the domain)

        // If x, y and z have different dimensions, then in an ideal
        // world we will have three different cell_spans which we use
        // in the hash calculation. So the hashed grids are not cubes
        // but cuboids. We skip this anisotropy for now, but rather
        // enforce that if someone gives a periodic domain in two or
        // three directions, their lengths mus be equal (square domain)

        // If thats the case, then the cell span is min(requested_cell_span,
        // l_x / 2**k) with L / 2**k < requested_cell_span < L / 2**(k-1)
        // ie. k = ceil(log2(L / requested_cell_span))
        // return L / 2**k
        // This is because every object is assigned to one single grid in
        // the hierarchy only, which happens to be the first grid with a
        // cell size larger than the size of the object.

        template <typename DomainType>
        struct HashGridsSpanPolicy;

        template <>
        struct HashGridsSpanPolicy<InfiniteDomain> {
          explicit HashGridsSpanPolicy(InfiniteDomain /*meta*/) {}

          inline real_t adjust_span(real_t requested_cell_span) const noexcept {
            return requested_cell_span;
          }
        };

        // Specialization for periodic domain
        template <>
        struct HashGridsSpanPolicy<PeriodicDomain> {
          explicit HashGridsSpanPolicy(PeriodicDomain periodic_domain)
              : domain_(periodic_domain) {
          }

          inline real_t adjust_span(real_t requested_cell_span) const noexcept {
            return requested_cell_span;
          }

          PeriodicDomain domain_;
        };

      }  // namespace coarse

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica
