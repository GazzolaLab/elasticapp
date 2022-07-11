#pragma once

#include <type_traits>

/// Types first
#include "DomainPolicy.hpp"
#include "Simulator/Domain/Types.hpp"
#include "Utilities/DefineTypes.h"
///

namespace elastica {

  namespace collision {

    namespace detail {

      // Specialization for periodic domain
      template <typename SimulatorConfig>
      class DomainPolicyImpl<SimulatorConfig, ::elastica::PeriodicDomain> {
       public:
        //!< Type of the simulation domain
        using DomainType = PeriodicDomain;

        using GhostPrimitives = typename SimulatorConfig::PrimitiveStorage;

        static_assert(
            std::is_same<typename SimulatorConfig::DomainType,
                         DomainType>::value,
            "This should not be reached, please contact the developers!");

       protected:
        explicit DomainPolicyImpl(DomainType const* periodic_domain)
            : domain_(periodic_domain), ghost_primitives_() {}

        inline ~DomainPolicyImpl() = default;
        /*
        DomainPolicyImpl(DomainPolicyImpl const&) = default;
        DomainPolicyImpl(DomainPolicyImpl&&) noexcept = default;
        DomainPolicyImpl& operator=(DomainPolicyImpl const&) = default;
        DomainPolicyImpl& operator=(DomainPolicyImpl&&) noexcept = default;
        */

        /* These need not be specified, the using class gives the scope
        template <typename SC, typename DT>
        DomainPolicyImpl(const DomainPolicyImpl<SC, DT>&) = delete;
        */

        // NOTE : Adjusting cell spans
        // The hash calculation, for an infinite grid, always puts
        // the object in a domain of [0, cell_count * cell_size]
        // where cell_size is determined from the inserted object
        // Hence its periodic with L = cell_count * cell_size

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
        inline real_t adjust_span(real_t requested_cell_span) const noexcept {
          // TODO Whats the logic here? (its easy to do 2**cell_span =
          // domain-size)
          return requested_cell_span;
        }

        // TODO logic for enlargement of hashgrids

        DomainType const* domain_;  //!< Pointer to the simulation domain
        GhostPrimitives ghost_primitives_;
      };

    }  // namespace detail

  }  // namespace collision

}  // namespace elastica
