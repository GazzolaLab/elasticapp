//==============================================================================
/*!
//  \file
//  \brief User configuration for the Collision system of an \elastica Simulator
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
#include <utility>

#include "ModuleSettings/DEMSolver.hpp"
#include "Simulator/Environments/CollisionSystem/ModuleConfiguration/Types.hpp"

namespace elastica {

  namespace configuration {

    template <typename ForEachExecutionPolicy_,
              typename OverPairsExecutionPolicy_, typename ForceModel_>
    class CollisionSystemUserConfiguration {
     public:
      //**Type definitions******************************************************
      //! Policy for each
      using ForEachExecutionPolicy = ForEachExecutionPolicy_;
      //! Policy over pairs
      using OverPairsExecutionPolicy = OverPairsExecutionPolicy_;
      //! Force model
      using ForceModel = ForceModel_;
      //************************************************************************

      //**Constructor***********************************************************
      /*!\name Constructor*/
      //@{

     public:
      //************************************************************************
      /*!\brief The default constructor.
       */
      constexpr CollisionSystemUserConfiguration() = delete;
      //************************************************************************

      //************************************************************************
      /*!\brief The sole constructor.
       */
      CollisionSystemUserConfiguration(ForceModel f)
          : force_model_(std::move(f)){};
      //************************************************************************

      //@}
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Obtains the force model
       */
      inline auto force_model() const& noexcept -> ForceModel_ const& {
        return force_model_;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Obtains the force model
       */
      inline auto force_model() && noexcept -> ForceModel&& {
        return static_cast<ForceModel&&>(force_model_);
      }
      //************************************************************************

     private:
      //**Member variables******************************************************
      //! The force model
      ForceModel force_model_;
      //************************************************************************
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Idiomatically makes a collision configuration
     * \ingroup config
     */
    template <typename ForEachExecutionPolicy_,
              typename OverPairsExecutionPolicy_,
              typename ForceModel_ = ELASTICA_DEFAULT_DEM_RESPONSE_MODEL>
    inline constexpr decltype(auto) make_collision_configuration(
        ForEachExecutionPolicy_ const&, OverPairsExecutionPolicy_ const&,
        ForceModel_ forceModel = {}) noexcept {
      return CollisionSystemUserConfiguration<
          ForEachExecutionPolicy_, OverPairsExecutionPolicy_, ForceModel_>(
          std::move(forceModel));
    }
    //**************************************************************************

  }  // namespace configuration

}  // namespace elastica