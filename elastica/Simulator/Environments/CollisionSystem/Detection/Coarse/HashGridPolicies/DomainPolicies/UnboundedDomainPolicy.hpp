//==============================================================================
/*!
//  \file
//  \brief Unbounded domain customization for HashGrid implementation
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

/// Types first
#include "Simulator/Domain/Types.hpp"
#include "Simulator/Environments/CollisionSystem/Detection/Coarse/HashGridPolicies/DomainPolicies/DomainPolicyBase.hpp"
#include "Simulator/Environments/CollisionSystem/Detection/Coarse/HashGridPolicies/DomainPolicies/Types.hpp"
///
#include "Utilities/DefineTypes.h"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits.hpp"

namespace elastica {

  namespace collision {

    namespace detection {

      namespace coarse {

        namespace detail {

          //********************************************************************
          /*! \cond ELASTICA_INTERNAL */
          /*!\brief Customization of the HashGrid for unbounded domains
           * \ingroup coarse_collision_detection
           *
           * \tparam SimulatorConfig The configuration of collision detection
           */
          template <typename SimulatorConfig>
          class DomainPolicyImpl<SimulatorConfig, ::elastica::UnboundedDomain>
              : private DomainPolicyBase {
           public:
            //**Type definitions************************************************
            //! Type of the simulation domain
            using DomainType = UnboundedDomain;
            //******************************************************************

            //**Compile time checks*********************************************
            /*! \cond ELASTICA_INTERNAL */
            static_assert(
                std::is_same_v<typename SimulatorConfig::DomainType,
                                 DomainType>,
                "This should not be reached, please contact the developers!");
            /*! \endcond */
            //******************************************************************

           protected:
            //**Constructor*****************************************************
            /*!\name Constructor */
            //@{
            explicit DomainPolicyImpl(DomainType const* unbounded_domain)
                : domain_(unbounded_domain) {}
            // DomainPolicyImpl(DomainPolicyImpl const&) = default;
            // DomainPolicyImpl(DomainPolicyImpl&&) noexcept = default;
            // DomainPolicyImpl& operator=(DomainPolicyImpl const&) = default;
            // DomainPolicyImpl& operator=(DomainPolicyImpl&&) noexcept =
            // default;
            //@}
            //******************************************************************

            //**Destructor******************************************************
            /*!\name Destructor */
            //@{
            inline ~DomainPolicyImpl() = default;
            //@}
            //******************************************************************

            /* These need not be specified, the using class gives the scope
            template <typename SC, typename DT>
            DomainPolicyImpl(const DomainPolicyImpl<SC, DT>&) = delete;
            */
           private:
            //**Utility functions***********************************************
            /*!\name Utility functions */
            //@{

            //******************************************************************
            /*!\brief Gets the initial cell count in the DirectionType `Dir`
             * of the hashgrid.
             *
             * \tparam Dir Direction type Dir
             */
            template <DirectionType Dir>
            static inline constexpr auto initial_cell_count(
                real_t /*requested_cell_span*/) noexcept -> std::size_t {
              return DomainPolicyBase::initial_cell_count<Dir>();
            }
            //******************************************************************

           protected:
            //******************************************************************
            /*!\brief Gets the initial cell counts in all the Directions
             * of the hashgrid.
             */
            static inline auto initial_cell_counts(
                real_t requested_cell_span) noexcept {
              return std::make_tuple(
                  initial_cell_count<DomainType::X>(requested_cell_span),
                  initial_cell_count<DomainType::Y>(requested_cell_span),
                  initial_cell_count<DomainType::Z>(requested_cell_span));
            }
            //******************************************************************

            //******************************************************************
            /*!\brief Adjusts the cell-span if necessary
             *
             * \note
             * This function is trivial for Unbounded domains but important for
             * periodic domains.
             */
            static inline auto adjust_cell_span(
                real_t requested_cell_span) noexcept -> real_t {
              return requested_cell_span;
            }
            //******************************************************************

            //******************************************************************
            /*!\brief Enlargement criterion for the hash-grid
             */
            static inline bool should_enlarge(
                std::size_t total_primitive_count,
                std::size_t enlargement_threshold) noexcept {
              return total_primitive_count == enlargement_threshold;
            }
            //******************************************************************

           private:
            //******************************************************************
            /*!\brief Enlarge cell count in a particular direction
             */
            template <DirectionType Dir>
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
            static inline void enlarge(std::size_t& x_cell_count,
                                       std::size_t& y_cell_count,
                                       std::size_t& z_cell_count) {
              enlarge<DomainType::X>(x_cell_count);
              enlarge<DomainType::Y>(y_cell_count);
              enlarge<DomainType::Z>(z_cell_count);
            }
            //******************************************************************

            //@}
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
