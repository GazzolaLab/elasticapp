//
// Created by Seung Hyun Kim on 9/16/21.
//

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
         * SquareCrossSectionOperations contains functions for characterizing a
         * square cross section. Its intended use is as the third template
         * parameter in CosseratRodCrossSectionInterface, which is then
         * eventually used within the Blocks framework for a Cosserat rod
         * data-structure.
         *
         * \tparam CRT A valid Cosserat Rod Traits class
         *
         * \see CosseratRodCrossSectionInterface, WithSquareCosseratRod
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
          //**Geometry1D
          // methods************************************************************
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
            // Timoshenko and Gere Mechanics of Materials book
            // (2nd edition page 143)
            // This value is pretty close to more recent published values,
            // hence not touching it.
            return 9.0 / 10.0;
          }
          //********************************************************************

          //@}
          //********************************************************************
        };
        //**********************************************************************

      }  // namespace detail

    }  // namespace component

  }  // namespace cosserat_rod

}  // namespace elastica
