#pragma once

#include <cmath>
#include <limits>
#include <type_traits>

/// Types first
#include "Simulator/Domain/Types.hpp"
#include "Simulator/Environments/CollisionSystem/Detection/Coarse/HashGridPolicies/DomainPolicies/Types.hpp"
#include "Utilities/DefineTypes.h"
///
#include "Simulator/Domain/Domain.hpp"
#include "Simulator/Environments/CollisionSystem/Detection/Coarse/HashGridPolicies/DomainPolicies/DomainPolicyImplBase.hpp"
#include "Utilities/Requires.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/ValueTraits/IsPowerOf.hpp"

namespace elastica {

  namespace collision {

    namespace detection {

      namespace coarse {

        namespace detail {

          // Specialization for periodic domain
          template <typename SimulatorConfig>
          class DomainPolicyImpl<SimulatorConfig, ::elastica::PeriodicDomain>
              : private DomainPolicyImplBase {
           public:
            //!< Type of the simulation domain
            using DomainType = PeriodicDomain;

            using DomainPolicyImplBase::DirectionType;

            using GhostPrimitives = typename SimulatorConfig::PrimitiveStorage;

            static_assert(
                std::is_same<typename SimulatorConfig::DomainType,
                             DomainType>::value,
                "This should not be reached, please contact the developers!");

            //!< Metafunction that returns type of the simulation boundaries
            template <DirectionType N>
            using BoundaryTypeAlong =
                typename SimulatorConfig::template BoundaryTypeAlong<N>;

           private:
            static constexpr auto X = ::elastica::detail::Directions::X;
            static constexpr auto Y = ::elastica::detail::Directions::Y;
            static constexpr auto Z = ::elastica::detail::Directions::Z;

            using AllBoundaries = tmpl::integral_list<DirectionType, X, Y, Z>;
            // Wrapping BoundaryType along so that brigand doesn't complain
            template <typename I>
            struct IsPeriodicBoundary
                : std::is_same<BoundaryTypeAlong<I::value>, PeriodicBoundary> {
            };

            using PeriodicBoundaries =
                tmpl::filter<AllBoundaries, IsPeriodicBoundary<tmpl::_1>>;

            // Get minimal span of all periodic directinos
            real_t get_min_span_along_periodic_directions() {
              real_t min_span(std::numeric_limits<real_t>::max());
              tmpl::for_each<PeriodicBoundaries>([this, &min_span](auto Dir) {
                using Direction = tmpl::type_from<decltype(Dir)>;
                // implicit conversion to constexpr
                min_span =
                    std::min(domain_->template get_span<Direction>(), min_span);
              });
              return min_span;
            }

           protected:
            explicit DomainPolicyImpl(DomainType const*& periodic_domain,
                                      GhostPrimitives& ghosts)
                : domain_(periodic_domain), ghost_primitives_(ghosts) {}

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

           private:
            template <DirectionType Dir,
                      Requires<std::is_same<BoundaryTypeAlong<Dir>,
                                            PeriodicBoundary>::value> = nullptr>
            inline auto initial_cell_count(
                real_t requested_cell_span) const noexcept -> std::size_t {
              // here we check that the req. cell. span is a power of 2
              // because logically, only an adjusted cell span is input
              auto n_cells = static_cast<std::size_t>(
                  get_min_span_along_periodic_directions() /
                  requested_cell_span);
              ELASTICA_ASSERT(is_power_of_2(n_cells), "Not a power of 2!");
              return n_cells;
            };

            // Does not need require, but i need symmetry
            template <
                DirectionType Dir,
                Requires<std::is_same<BoundaryTypeAlong<Dir>,
                                      UnboundedBoundary>::value> = nullptr>
            inline auto initial_cell_count(
                real_t /*requested_cell_span*/) const noexcept -> std::size_t {
              return DomainPolicyImplBase::initial_cell_count<Dir>();
            }

           protected:
            inline auto initial_cell_counts(
                real_t requested_cell_span) const noexcept {
              return std::make_tuple(
                  initial_cell_count<X>(requested_cell_span),
                  initial_cell_count<Y>(requested_cell_span),
                  initial_cell_count<Z>(requested_cell_span));
            }

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
            // three directions, their lengths must be equal (square domain)

            // If thats the case, then the cell span is min(requested_cell_span,
            // l_x / 2**k) with L / 2**k < requested_cell_span < L / 2**(k-1)
            // ie. k = ceil(log2(L / requested_cell_span))
            // return L / 2**k
            // This is because every object is assigned to one single grid in
            // the hierarchy only, which happens to be the first grid with a
            // cell size larger than the size of the object.
            inline real_t adjust_cell_span(
                real_t requested_cell_span) const noexcept {
              // get n such that
              // 2**(-(n+1)) < cell_span / domain_span  <= 2**(-n)
              // => -(n+1) < log2(cell_span / domain_span) <= -n (monotonic)
              // => n <=  -log2(cell_span / domain_span) < (n + 1)
              // => n = floor(-log2(cell_span / domain_span))
              // or n = static_cast<std::size_t>(-log2(cell_span / domain_span))
              // we then return 2**(-n) * domain_span
              //
              // aka. We do domain_span / cell_span and round down to closest
              // power of 2 for a floating point number and return domain_span /
              // ans

              // TODO : Can this be cached instead of calculating everytime?
              // Get minimal span of all the periodic directions
              real_t min_domain_span(get_min_span_along_periodic_directions());

              // TODO There is definitely a faster alternative, but all I can
              //  see are bit hacks for integer : couldn't find a "safe"
              //  alternative
              //  1.
              //  https://github.com/romeric/fastapprox/tree/master/fastapprox/src
              //  One can use frexp too, but its not clear what the cost is
              //  Based on micro benchmarks, all are almost same in release mode
              return std::exp2(-std::floor(
                         std::log2(min_domain_span / requested_cell_span))) *
                     min_domain_span;
            }

           private:
            struct MixedPeriodicUnboundedTag {};

            inline bool do_should_enlarge(std::size_t total_primitive_count,
                                          std::size_t enlargement_threshold,
                                          MixedPeriodicUnboundedTag /*meta*/) {
              // can be enlarged but only in the unbounded direction
              // so pass the usual check here
              return total_primitive_count == enlargement_threshold;
            }

            struct AllPeriodicTag {};

            inline bool do_should_enlarge(std::size_t /*total_primitive_count*/,
                                          std::size_t /*enlargement_threshold*/,
                                          AllPeriodicTag /* meta */) {
              // Do not enlarge if its all periodic, always suffer
              return false;
            }

           protected:
            inline bool should_enlarge(std::size_t total_primitive_count,
                                       std::size_t enlargement_threshold) {
              return do_should_enlarge(
                  total_primitive_count, enlargement_threshold,
                  std::conditional<tmpl::size<PeriodicBoundaries>::value == 3UL,
                                   AllPeriodicTag,
                                   MixedPeriodicUnboundedTag>::type);
            }

           private:
            // TODO logic for enlargement of hashgrids
            // Do not enlarge in periodic directions
            template <typename Dir,
                      Requires<tmpl::list_contains_v<PeriodicBoundaries, Dir>> =
                          nullptr>
            inline void enlarge(std::size_t& /* cell_count */) {}

            // Only enlarge in non-periodic directions
            template <typename Dir, Requires<not tmpl::list_contains_v<
                                        PeriodicBoundaries, Dir>> = nullptr>
            inline void enlarge(std::size_t& cell_count) {
              cell_count *= 2;
            }

           protected:
            inline void enlarge(std::size_t& x_cell_count,
                                std::size_t& y_cell_count,
                                std::size_t& z_cell_count) {
              enlarge<X>(x_cell_count);
              enlarge<Y>(y_cell_count);
              enlarge<Z>(z_cell_count);
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
