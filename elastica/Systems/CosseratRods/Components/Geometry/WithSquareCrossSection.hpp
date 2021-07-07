//==============================================================================
/*!
//  \file
//  \brief Geometry component implementing a Square cross section Cosserat rod
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
#include "Utilities/Math/Infinity.hpp"
#include "Utilities/ProtocolHelpers.hpp"
// Implementation details
#include "Utilities/Math/Sqrt.hpp"
#include "Utilities/Math/Square.hpp"

namespace elastica {
  //  public RodKinematics<RodTraits>
  // Sparse_strategy
  // Dense strategy
  namespace cosserat_rod {

    namespace component {

      namespace detail {

        //======================================================================
        //
        //  CLASS DEFINITION
        //
        //======================================================================

        //**********************************************************************
        /*!\brief Operations corresponding to a square cross section
         * \ingroup cosserat_rod_component
         *
         * \details
         * CircleCrossSectionOperations contains functions for characterizing a
         * square cross section. Its intended use is as the third template
         * parameter in CosseratRodCrossSectionInterface, which is then
         * eventually used within the Blocks framework for a Cosserat rod
         * data-structure.
         *
         * \tparam CRT A valid Cosserat Rod Traits class
         *
         * \see CosseratRodCrossSectionInterface, WithSquareCrossSection
         */
        template <typename CRT>
        class SquareCrossSectionOperations {
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
           * \param incircle_radius incircle radius of the square
           */
          template <typename InCircleRadius>
          static inline decltype(auto) compute_cross_section_area(
              InCircleRadius const& incircle_radius) noexcept {
            return static_cast<real_type>(4.0) * sq(incircle_radius);
          }
          //********************************************************************

          //********************************************************************
          /*!\brief computes second moment of area
           *
           * \param incircle_radius incircle radius of the square
           */
          template <typename InCircleRadius>
          static inline decltype(auto) compute_second_moment_of_area(
              InCircleRadius const& incircle_radius) noexcept {
            constexpr auto coefficient = static_cast<real_type>(1.0 / 12.0);
            return coefficient *
                   sq(compute_cross_section_area(incircle_radius));
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
            return static_cast<real_type>(0.5) * sqrt(volume / length);
          }
          //********************************************************************

          //********************************************************************
          /*!\brief Returns the (real) shape factor for a shape, derived from
           * theory of elasticity
           */
          static inline constexpr auto shape_factor() noexcept -> real_type {
            // TODO
            return -156.32;
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
       * square cross section
       * \ingroup cosserat_rod_component
       *
       * \details
       * WithSquareCrossSection implements the final Geometry component use
       * within Cosserat rods implemented in the Blocks framework. It denotes a
       * Cosserat rod data-structure with a square cross section in the
       * lateral direction and spanning a single spatial dimension along the
       * transverse dimension.
       *
       * \usage
       * Since WithSquareCrossSection is a valid and complete Geometry component
       * adhering to protocols::Geometry1D, one can use it to declare a
       * CosseratRodPlugin within the @ref blocks framework
       * \code
       * // pre-declare RodTraits, Blocks
       * using CircularCosseratRod = CosseratRodPlugin<RodTraits, Block,
       * components::WithSquareCrossSection>
       * \endcode
       *
       * \tparam CRT A valid Cosserat Rod Traits class
       * \tparam ComputationalBlock The final block which is derived from the
       * current component
       *
       * \see CosseratRodPlugin
       */
      template <typename CRT, typename ComputationalBlock>
      class WithSquareCrossSection
          : public detail::CosseratRodCrossSectionInterface<
                CRT, ComputationalBlock,
                detail::SquareCrossSectionOperations<CRT>>,
            public ::tt::ConformsTo<protocols::Geometry1D>,
            public Component<WithSquareCrossSection<CRT, ComputationalBlock>> {
       private:
        //**Type definitions****************************************************
        //! Traits type
        using Traits = CRT;
        //! Cross section type
        using CrossSection = detail::SquareCrossSectionOperations<Traits>;
        //! This type
        using This = WithSquareCrossSection<Traits, ComputationalBlock>;
        //! Parent type
        using Parent =
            detail::CosseratRodCrossSectionInterface<Traits, ComputationalBlock,
                                                     CrossSection>;
        //**********************************************************************

        //! sanity check cross section classes
        static_assert(
            cpp17::is_same_v<typename Parent::CrossSection, CrossSection>,
            "Oops!");

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
