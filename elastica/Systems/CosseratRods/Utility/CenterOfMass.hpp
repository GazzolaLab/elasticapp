//
// Created by Seung Hyun Kim on 10/9/21.
//

#pragma once

#include "Systems/Block.hpp"
#include "Systems/CosseratRods/Components/helpers.hpp"
#include "Systems/CosseratRods/CosseratRodTraits.hpp"
#include "Systems/CosseratRods/Tags.hpp"
#include "Utilities/CRTP.hpp"
#include "Utilities/TMPL.hpp"
#include "blaze/Blaze.h"
#include "blaze_tensor/Blaze.h"

namespace elastica {

  namespace cosserat_rod {

    template <typename CRT>
    class CenterOfMassComputation {
     private:
      using Traits = CRT;
      using real_type = typename Traits::real_type;

     public:
      //************************************************************************
      /*!\brief Computes center of mass position of the body.
      //
      // \details
      // Computes center of mass position of the body given nodal masses and
      // positions. \n
      // COM_position = sum(body.mass_batch{i} * body.position_batch{i})
      // / sum(body.mass_batch{i})
      //
      // \example
      // The following shows a typical use of the
      // compute_position_center_of_mass() function with the expected
      // (correct) result also shown.
      // \snippet test_gov_eqns.cpp compute_position_center_of_mass_example
      //
      // \param[in] body object whose center of mass position is to be computed
      //
      // \return COM_position expression type
      //
      // \see fill later?
      */
      template <typename MassType, typename DimensionType,
                typename PositionType>
      static inline decltype(auto) compute_position_center_of_mass(
          const MassType& mass_batch, const DimensionType& dimension_batch,
          const PositionType& position_batch) {
        // TODO: Check math, and move blaze operation into trait
        auto mass_times_position_batch =
            Traits::Operations::expand_for_broadcast(mass_batch) %
            position_batch;
        return Traits::Operations::transpose(
            blaze::sum<blaze::rowwise>(mass_times_position_batch) /
            blaze::sum(mass_batch));
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Computes center of mass velocity of the body.
      //
      // \details
      // Computes center of mass velocity of the body given nodal masses and
      // velocities. \n
      // COM_velocity = sum(body.mass_batch{i} * body.velocity_batch{i})
      // / sum(body.mass_batch{i})
      //
      // \example
      // The following shows a typical use of the
      // compute_velocity_center_of_mass() function with the expected (correct)
      // result also shown.
      // \snippet test_gov_eqns.cpp compute_velocity_center_of_mass_example
      //
      // \param[in] body object whose center of mass velocity is to be computed
      //
      // \return COM_velocity expression type
      //
      // \see fill later?
      */
      template <typename MassType, typename DimensionType,
                typename VelocityType>
      auto compute_velocity_center_of_mass(const MassType& mass_batch,
                                           const DimensionType& dimension_batch,
                                           const VelocityType& velocity_batch) {
        // TODO: Check math, and move blaze operation into trait
        auto mass_times_velocity_batch =
            Traits::Operations::expand_for_broadcast(mass_batch,
                                                     dimension_batch) %
            velocity_batch;
        return blaze::trans(
            blaze::sum<blaze::rowwise>(mass_times_velocity_batch) /
            blaze::sum(mass_batch));
      }
      //************************************************************************
    };

  }  // namespace cosserat_rod

}  // namespace elastica
