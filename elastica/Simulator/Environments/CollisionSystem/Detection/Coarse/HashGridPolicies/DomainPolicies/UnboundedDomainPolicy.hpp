#pragma once

/// Types first
#include "Simulator/Domain/Types.hpp"
#include "Simulator/Environments/CollisionSystem/Detection/Coarse/HashGridPolicies/DomainPolicies/Types.hpp"
#include "elastica/Utilities/DefineTypes.h"
///

#include "Simulator/Environments/CollisionSystem/Detection/Coarse/HashGridPolicies/DomainPolicies/DomainPolicyImplBase.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits.hpp"

namespace elastica {

  namespace collision {

    namespace detection {

      namespace coarse {

        namespace detail {

          // Specialization for unbounded domain
          template <typename SimulatorConfig>
          class DomainPolicyImpl<SimulatorConfig, ::elastica::UnboundedDomain>
              : private DomainPolicyImplBase {
           public:
            //!< Type of the simulation domain
            using DomainType = UnboundedDomain;

            //!< Type of ghost primitives
            using GhostPrimitives = typename SimulatorConfig::PrimitiveStorage;
            // Here we don't need GhostPrimitives, but keep it around?
            //            using GhostPrimitives = unsigned char;

            static_assert(
                std::is_same<typename SimulatorConfig::DomainType,
                             DomainType>::value,
                "This should not be reached, please contact the developers!");

           protected:
            explicit DomainPolicyImpl(const DomainType*& unbounded_domain,
                                      GhostPrimitives& ghosts)
                : domain_(unbounded_domain), ghost_primitives_(ghosts) {}

            inline ~DomainPolicyImpl() = default;
            DomainPolicyImpl(DomainPolicyImpl const&) = default;
            DomainPolicyImpl(DomainPolicyImpl&&) noexcept = default;
            DomainPolicyImpl& operator=(DomainPolicyImpl const&) = default;
            DomainPolicyImpl& operator=(DomainPolicyImpl&&) noexcept = default;

            /* These need not be specified, the using class gives the scope
            template <typename SC, typename DT>
            DomainPolicyImpl(const DomainPolicyImpl<SC, DT>&) = delete;
            */
           private:
            template <DirectionType Dir>
            inline constexpr auto initial_cell_count(
                real_t requested_cell_span) const noexcept -> std::size_t {
              return DomainPolicyImplBase::initial_cell_count<Dir>();
            };

           protected:
            inline auto initial_cell_counts(
                real_t requested_cell_span) const noexcept {
              static constexpr auto X = ::elastica::detail::Directions::X;
              static constexpr auto Y = ::elastica::detail::Directions::Y;
              static constexpr auto Z = ::elastica::detail::Directions::Z;
              return std::make_tuple(
                  initial_cell_count<X>(requested_cell_span),
                  initial_cell_count<Y>(requested_cell_span),
                  initial_cell_count<Z>(requested_cell_span));
            }

            inline auto adjust_cell_span(
                real_t requested_cell_span) const noexcept -> real_t {
              return requested_cell_span;
            }

            inline bool should_enlarge(std::size_t total_primitive_count,
                                       std::size_t enlargement_threshold) {
              return total_primitive_count == enlargement_threshold;
            }

            // Always enlarge in non-periodic settings
            // Overkill dude.
            template <typename... CellCounts,
                      Requires<cpp17::conjunction_v<
                          std::is_same<CellCounts, std::size_t>...> > = nullptr>
            inline void enlarge(CellCounts&... cell_counts) {
              EXPAND_PACK_LEFT_TO_RIGHT(cell_counts *= 2);
            }

            DomainType const*& domain_;  //!< Pointer to the simulation domain
            GhostPrimitives&
                ghost_primitives_;  //!< Pointer to ghost primitives
          };

        }  // namespace detail

      }  // namespace coarse

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica
