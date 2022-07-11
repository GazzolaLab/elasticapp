#pragma once

/// Types first
#include "DomainPolicy.hpp"
#include "Simulator/Domain/Types.hpp"
#include "Utilities/DefineTypes.h"
///

namespace elastica {

  namespace collision {

    namespace detail {

      // Specialization for unbounded domain
      template <typename SimulatorConfig>
      class DomainPolicyImpl<SimulatorConfig, ::elastica::UnboundedDomain> {
       public:
        //!< Type of the simulation domain
        using DomainType = UnboundedDomain;
        // Here we don't need GhostPrimitives, but keep it around?
        //            using GhostPrimitives = unsigned char;
        static_assert(
            std::is_same<typename SimulatorConfig::DomainType,
                         DomainType>::value,
            "This should not be reached, please contact the developers!");

       protected:
        explicit DomainPolicyImpl(const DomainType* /*unbounded_domain*/)
        /*: domain_(periodic_domain) , ghost_primitives_(false)*/
        {}

        inline ~DomainPolicyImpl() = default;
        DomainPolicyImpl(DomainPolicyImpl const&) = default;
        DomainPolicyImpl(DomainPolicyImpl&&) noexcept = default;
        DomainPolicyImpl& operator=(DomainPolicyImpl const&) = default;
        DomainPolicyImpl& operator=(DomainPolicyImpl&&) noexcept = default;

        /* These need not be specified, the using class gives the scope
        template <typename SC, typename DT>
        DomainPolicyImpl(const DomainPolicyImpl<SC, DT>&) = delete;
        */

        inline real_t adjust_span(real_t requested_cell_span) const noexcept {
          return requested_cell_span;
        }

        DomainType const* domain_;  //!< Pointer to the simulation domain
        /*GhostPrimitives ghost_primitives_;*/
      };

    }  // namespace detail

  }  // namespace collision

}  // namespace elastica
