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

namespace elastica{

namespace cosserat_rod {

  namespace component {

    namespace detail {

      // TODO : Static test : assure that the size is 0 or 1
      /* DONE : Make shape protocol with
       *  1. get_dimension
       *  2. get_area
       *  3. get_volume
       *  4. template function get_second_moment_of_inertia
       *  5. shape_factor
       */
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
       * \see CosseratRodCrossSectionInterface, WithCircularCosseratRod
       */
      //**********************************************************************
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
        static inline decltype(auto) update_dimension( /* TODO rename: compute_dimension_conserve_volume */
            Volume const& volume, Length const& length) noexcept {
          constexpr auto inv_pi = static_cast<real_type>(M_1_PI);
          // radius[k] = np.sqrt(volume[k] / lengths[k] / np.pi)
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

    }  // namespace detail

  }  // namespace component

  }  // namespace cosserat_rod

}  // namespace elastica
