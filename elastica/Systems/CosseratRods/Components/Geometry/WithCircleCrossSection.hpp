//==============================================================================
/*!
//  \file
//  \brief Geometry component implementing a Circular cross section Cosserat rod
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
#include <utility>

#include "Systems/CosseratRods/Components/Geometry/Protocols.hpp"
#include "Systems/CosseratRods/Components/Geometry/Types.hpp"
///
#include "Systems/CosseratRods/Components/Geometry/detail/CosseratRodCrossSectionInterface.hpp"
#include "Systems/CosseratRods/Components/helpers.hpp"
#include "Utilities/ProtocolHelpers.hpp"
// Implementation details
#include "Utilities/Math/Sqrt.hpp"
#include "Utilities/Math/Square.hpp"

namespace elastica {

  namespace cosserat_rod {

    namespace component {

      // TODO : Static test : assure that the size is 0 or 1
      /* DONE : Make shape protocol with
       *  1. get_dimension
       *  2. get_area
       *  3. get_volume
       *  4. template function get_second_moment_of_inertia
       *  5. shape_factor
       */
      namespace detail {

        //======================================================================
        //
        //  CLASS DEFINITION
        //
        //======================================================================

        //**********************************************************************
        /*!\brief Operations corresponding to a circular cross section
         * \ingroup cosserat_rod_component
         *
         * \details
         * CircleCrossSectionOperations contains functions for characterizing a
         * circular cross section. Its intended use is as the third template
         * parameter in CosseratRodCrossSectionInterface, which is then
         * eventually used within the Blocks framework for a Cosserat rod
         * data-structure.
         *
         * \tparam CRT A valid Cosserat Rod Traits class
         *
         * \see CosseratRodCrossSectionInterface, WithCircleCrossSection
         */
        template <typename CRT>
        class CircleCrossSectionOperations {
         private:
          //**Type definitions**************************************************
          //! Traits type
          using Traits = CRT;
          //! Real number type
          using real_type = typename Traits::real_type;
          //********************************************************************

         public:
          //**Geometry1D methods*****************************************************
          /*!\name Geometry1D methods*/
          //@{

          //********************************************************************
          /*!\brief computes cross section area
           *
           * \param radius radius of the circle
           */
          template <typename Radius>
          static inline decltype(auto) compute_cross_section_area(
              Radius const& radius) noexcept {
            return static_cast<real_type>(M_PI) * sq(radius);
          }
          //********************************************************************

          //********************************************************************
          /*!\brief computes second moment of area
           *
           * \param radius radius of the circle
           */
          template <typename Radius>
          static inline decltype(auto) compute_second_moment_of_area(
              Radius const& radius) noexcept {
            return static_cast<real_type>(0.25 * M_1_PI) *
                   sq(compute_cross_section_area(radius));
          }
          //********************************************************************

          //********************************************************************
          /*!\brief Updates dimension (aka radius) for given volume and length
           *
           * \param volume Volume of the rod
           * \param length Length of the rod
           */
          template <typename Volume, typename Length>
          static inline decltype(auto) update_dimension(
              Volume const& volume, Length const& length) noexcept {
            constexpr auto inv_pi = static_cast<real_type>(M_1_PI);
            return sqrt(inv_pi * volume / length);
          }
          //********************************************************************

          //********************************************************************
          /*!\brief Returns the (real) shape factor for a shape, derived from
           * theory of elasticity
           */
          static inline constexpr auto shape_factor() noexcept -> real_type {
            return static_cast<real_type>(4.) / static_cast<real_type>(3.);
          }
          //********************************************************************

          //@}
          //********************************************************************
        };
        //**********************************************************************

      }  // namespace detail

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Geometry component corresponding to a spanwise rod with a
       * circular cross section
       * \ingroup cosserat_rod_component
       *
       * \details
       * WithCircleCrossSection implements the final Geometry component use
       * within Cosserat rods implemented in the Blocks framework. It denotes a
       * Cosserat rod data-structure with a circular cross section in the
       * lateral direction and spanning a single spatial dimension along the
       * transverse dimension.
       *
       * \usage
       * Since WithCircleCrossSection is a valid and complete Geometry component
       * adhering to protocols::Geometry1D, one can use it to declare a
       * CosseratRodPlugin within the @ref blocks framework
       * \code
       * // pre-declare RodTraits, Blocks
       * using CircularCosseratRod = CosseratRodPlugin<RodTraits, Block,
       * components::WithCircleCrossSection>
       * \endcode
       *
       * \tparam CRT A valid Cosserat Rod Traits class
       * \tparam ComputationalBlock The final block which is derived from the
       * current component
       *
       * \see CosseratRodPlugin
       */
      template <typename CRT, typename ComputationalBlock>
      class WithCircleCrossSection
          : public detail::CosseratRodCrossSectionInterface<
                CRT, ComputationalBlock,
                detail::CircleCrossSectionOperations<CRT>>,
            public ::tt::ConformsTo<protocols::Geometry1D>,
            public Component<WithCircleCrossSection<CRT, ComputationalBlock>> {
       private:
        //**Type definitions****************************************************
        //! Traits type
        using Traits = CRT;
        //! Cross section type
        using CrossSection = detail::CircleCrossSectionOperations<Traits>;
        //! This type
        using This = WithCircleCrossSection<Traits, ComputationalBlock>;
        //! Parent type
        using Parent =
            detail::CosseratRodCrossSectionInterface<Traits, ComputationalBlock,
                                                     CrossSection>;
        //**********************************************************************

        //! sanity check cross section classes
        static_assert(
            cpp17::is_same_v<typename Parent::CrossSection, CrossSection>,
            "Cross section assertion failure, contact the developers!");

       protected:
        //**Parent methods and aliases******************************************
        //! Initialize method inherited from parent class
        using Parent::initialize;
        //! List of computed variables
        using typename Parent::ComputedVariables;
        //! List of initialized variables
        using typename Parent::InitializedVariables;
        //! List of all variables
        using typename Parent::Variables;
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace component

  }  // namespace cosserat_rod

}  // namespace elastica

// public RodKinematics<RodTraits>
// Sparse_strategy
// Dense strategy
