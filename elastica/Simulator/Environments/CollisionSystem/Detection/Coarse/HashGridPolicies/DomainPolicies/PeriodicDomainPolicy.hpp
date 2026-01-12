//==============================================================================
/*!
//  \file
//  \brief Periodic domain customization for HashGrid implementation
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include <cmath>
#include <limits>
#include <type_traits>

/// Types first
#include "Simulator/Domain/Types.hpp"
#include "Simulator/Environments/CollisionSystem/Detection/Coarse/HashGridPolicies/DomainPolicies/Types.hpp"
//
#include "Simulator/Domain/Domain.hpp"
#include "Simulator/Environments/CollisionSystem/Detection/Coarse/HashGridPolicies/DomainPolicies/DomainPolicyBase.hpp"
//
#include "Utilities/DefineTypes.h"
#include "Utilities/Requires.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/ValueTraits/IsPowerOf.hpp"

namespace elastica {

  namespace collision {

    namespace detection {

      namespace coarse {

        namespace detail {

          //********************************************************************
          /*! \cond ELASTICA_INTERNAL */
          /*!\brief Customization of the HashGrid for periodic domains
           * \ingroup coarse_collision_detection
           *
           * \tparam SimulatorConfig The configuration of collision detection
           */
          template <typename SimulatorConfig>
          class DomainPolicyImpl<SimulatorConfig, ::elastica::PeriodicDomain>
              : private DomainPolicyBase {
           public:
            //**Type definitions************************************************
            //! Type of the simulation domain
            using DomainType = UnboundedDomain;
            //! Type of the direction
            using DomainPolicyBase::DirectionType;
            //!< Metafunction that returns type of the simulation boundaries
            template <DirectionType N>
            using BoundaryTypeAlong =
                typename SimulatorConfig::template BoundaryTypeAlong<N>;
            //******************************************************************

            //**Compile time checks*********************************************
            /*! \cond ELASTICA_INTERNAL */
            static_assert(
                std::is_same_v<typename SimulatorConfig::DomainType,
                                 DomainType>,
                "This should not be reached, please contact the developers!");
            /*! \endcond */
            //******************************************************************

           private:
            //**Static members**************************************************
            static constexpr auto X = DomainType::X;
            static constexpr auto Y = DomainType::Y;
            static constexpr auto Z = DomainType::Z;
            //******************************************************************

            //**Type definitions************************************************
            //! Typelist of all boundaries
            using AllBoundaries = tmpl::integral_list<DirectionType, X, Y, Z>;
            //! Metafunction to check if the direction is periodic
            template <DirectionType Dir>
            static constexpr auto is_periodic() noexcept
                -> std::is_same<BoundaryTypeAlong<Dir>, IsPeriodic>;
            //! Metafunction to check if the direction is unbounded
            template <DirectionType Dir>
            static constexpr auto is_unbounded() noexcept
                -> std::is_same<BoundaryTypeAlong<Dir>, IsUnbounded>;

            // Wrapper for brigand integral list types so that brigand's
            // integral list doesn't complain
            template <typename IntegralConstant>
            struct IsPeriodicWrapper
                : decltype(is_periodic<IntegralConstant::value>()) {};
            //! Typelist of periodic boundaries
            using PeriodicBoundaries =
                tmpl::filter<AllBoundaries, IsPeriodicWrapper<tmpl::_1>>;
            //******************************************************************

            //******************************************************************
            /*!\brief Get minimal span of domain across all periodic directions
             */
            real_t get_min_span_along_periodic_directions() const noexcept {
              real_t min_span(std::numeric_limits<real_t>::max());
              tmpl::for_each<PeriodicBoundaries>([this, &min_span](auto Dir) {
                using Direction = tmpl::type_from<decltype(Dir)>;
                // implicit conversion to constexpr
                min_span =
                    std::min(domain_->template get_span<Direction>(), min_span);
              });
              return min_span;
            }
            //******************************************************************

           protected:
            //**Constructor*****************************************************
            /*!\name Constructor */
            //@{
            explicit DomainPolicyImpl(DomainType const* periodic_domain)
                : domain_(periodic_domain) {}
            //******************************************************************

            //**Destructor******************************************************
            /*!\name Destructor */
            //@{
            inline ~DomainPolicyImpl() = default;
            //@}
            //******************************************************************
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
            //******************************************************************
            /*!\brief Gets the initial cell count in the DirectionType `Dir`
             * of the hashgrid.
             *
             * \details
             * This is a specialization for periodic directions
             *
             * \tparam Dir Direction type Dir
             */
            template <DirectionType Dir, Requires<is_periodic<Dir>()> = nullptr>
            inline auto initial_cell_count(
                real_t requested_cell_span) const noexcept -> std::size_t {
              // here we check that the req. cell. span is a power of 2
              // because logically, only an adjusted cell span is input
              auto n_cells = static_cast<std::size_t>(
                  get_min_span_along_periodic_directions() /
                  requested_cell_span);
              ELASTICA_ASSERT(is_power_of_2(n_cells), "Not a power of 2!");
              return n_cells;
            }
            //******************************************************************

            //******************************************************************
            /*!\brief Gets the initial cell count in the DirectionType `Dir`
             * of the hashgrid.
             *
             * \details
             * This is a specialization for unbounded directions
             *
             * \tparam Dir Direction type Dir
             */
            template <DirectionType Dir,
                      Requires<is_unbounded<Dir>()> = nullptr>
            inline auto initial_cell_count(
                real_t /*requested_cell_span*/) const noexcept -> std::size_t {
              return DomainPolicyBase::initial_cell_count<Dir>();
            }
            //******************************************************************

           protected:
            //******************************************************************
            /*!\brief Gets the initial cell counts in all the Directions
             * of the hashgrid.
             */
            inline auto initial_cell_counts(
                real_t requested_cell_span) const noexcept {
              return std::make_tuple(
                  initial_cell_count<X>(requested_cell_span),
                  initial_cell_count<Y>(requested_cell_span),
                  initial_cell_count<Z>(requested_cell_span));
            }
            //******************************************************************

            //******************************************************************
            /*!\brief Adjusts the cell-span if necessary
             *
             * \note
             * The hash calculation, for an infinite grid, always puts
             * the object in a domain of [0, cell_count * cell_size]
             * where cell_size is determined from the inserted object
             * Hence its periodic with L = cell_count * cell_size

             * However, for a periodic domain, one can enforce strict
             * requirements for the cell spans in the x, y or z directions
             * (requiring that the grid fits snugly within the domain)

             * If x, y and z have different dimensions, then in an ideal
             * world we will have three different cell_spans which we use
             * in the hash calculation. So the hashed grids are not cubes
             * but cuboids. We skip this anisotropy for now, but rather
             * enforce that if someone gives a periodic domain in two or
             * three directions, their lengths must be equal (square domain)

             * If thats the case, then the cell span is min(requested_cell_span,
             * l_x / 2**k) with L / 2**k < requested_cell_span < L / 2**(k-1)
             * ie. k = ceil(log2(L / requested_cell_span))
             * return L / 2**k
             * This is because every object is assigned to one single grid in
             * the hierarchy only, which happens to be the first grid with a
             * cell size larger than the size of the object.
             */
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
            //******************************************************************

           private:
            //******************************************************************
            struct MixedPeriodicUnboundedTag {};
            struct AllPeriodicTag {};
            //******************************************************************

            //******************************************************************
            /*!\brief Are all the directions periodic?
             */
            static inline constexpr bool is_all_periodic() noexcept {
              return tmpl::size<PeriodicBoundaries>::value ==
                     EulerianFrame::Dimension;
            }
            //******************************************************************

            //******************************************************************
            /*!\brief Specialization of should enlarge for directions being
             * partly mixed and partly unbounded
             */
            static inline bool do_should_enlarge(
                std::size_t total_primitive_count,
                std::size_t enlargement_threshold,
                MixedPeriodicUnboundedTag /*meta*/) noexcept {
              // can be enlarged but only in the unbounded direction
              // so pass the usual check here
              return total_primitive_count == enlargement_threshold;
            }
            //******************************************************************

            //******************************************************************
            /*!\brief Specialization of should enlarge for all directions being
             * periodic
             */
            static constexpr inline bool do_should_enlarge(
                std::size_t /*total_primitive_count*/,
                std::size_t /*enlargement_threshold*/,
                AllPeriodicTag /* meta */) noexcept {
              // Do not enlarge if its all periodic, always suffer
              return false;
            }
            //******************************************************************

           protected:
            //******************************************************************
            /*!\brief Enlargement criterion for the hash-grid
             */
            static constexpr inline bool should_enlarge(
                std::size_t total_primitive_count,
                std::size_t enlargement_threshold) noexcept {
              return do_should_enlarge(
                  total_primitive_count, enlargement_threshold,
                  std::conditional_t<is_all_periodic(), AllPeriodicTag,
                                     MixedPeriodicUnboundedTag>{});
            }
            //******************************************************************

           private:
            //******************************************************************
            /*!\brief Enlarge cell count in a particular direction
             *
             * \details
             * Specialization for a periodic direction
             */
            template <DirectionType Dir, Requires<is_periodic<>()> = nullptr>
            static inline constexpr void enlarge(
                std::size_t& /* cell_count */) noexcept {
              // ?? TODO logic for enlargement of hashgrids
              // Do not enlarge in periodic directions
            }
            //******************************************************************

            //******************************************************************
            /*!\brief Enlarge cell count in a particular direction
             *
             * \details
             * Specialization for an unboudned direction
             */
            template <DirectionType Dir,
                      Requires<is_unbounded<Dir>()> = nullptr>
            static inline constexpr void enlarge(
                std::size_t& cell_count) noexcept {
              cell_count *= 2;
            }
            //******************************************************************

           protected:
            //******************************************************************
            /*!\brief Enlarge the cell counts for the hash-grid across all the
             * dimensions
             */
            inline void enlarge(std::size_t& x_cell_count,
                                std::size_t& y_cell_count,
                                std::size_t& z_cell_count) {
              enlarge<X>(x_cell_count);
              enlarge<Y>(y_cell_count);
              enlarge<Z>(z_cell_count);
            }
            //******************************************************************

           public:
            //**Get functions***************************************************
            /*!\name Get functions */
            //@{
            DomainType const*& domain() & noexcept { return domain_; }
            DomainType const* const& domain() const& noexcept {
              return domain_;
            }
            //@}
            //******************************************************************

           protected:
            //**Member variables************************************************
            /*!\name Member variables */
            //@{
            //! Pointer to the simulation domain
            DomainType const* domain_;
            //@}
            //******************************************************************
          };
          /*! \endcond */
          //********************************************************************

        }  // namespace detail

      }  // namespace coarse

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica
