//==============================================================================
/*!
//  \file
//  \brief A uniform gravity field over all systems
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <blaze/math/Vector.h>

#include "Simulator/Environments/Common.hpp"
//
#include "Simulator/Environments/GravityField/Tags.hpp"
#include "Simulator/Environments/GravityField/Types.hpp"
//
#include "Simulator/Environments/GravityField/ModuleConfiguration/ModuleConfiguration.hpp"
//
#include "Systems/Tags.hpp"
//
#include "Time/Types.hpp"
//
#include "Utilities/Get.hpp"
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/Singleton.hpp"

namespace elastica {

  namespace environments {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Applies a uniform gravity field over all systems
     * \ingroup environments
     */
    template <typename Traits>
    class GravityField final : public elastica::Singleton<GravityField<Traits>>,
                               public ::tt::ConformsTo<protocols::Environment> {
     private:
      //**Friendships***********************************************************
      //! Friendship with Singleton
      BEFRIEND_SINGLETON;
      //************************************************************************

      //**Constructor***********************************************************
      /*!\name Constructor */
      //@{
      //************************************************************************
      /*!\brief Constructor of the Gravity field class.
       */
      GravityField() : gravity_acc_{0.0, 0.0, 0.0} {};
      //************************************************************************

      //@}
      //************************************************************************

      //**Type definitions******************************************************
      //! This type
      using This = GravityField<Traits>;
      //! Configuration type
      using ParameterConfiguration =
          configuration::GravityFieldUserConfiguration;
      //************************************************************************

     public:
      //**Type definitions******************************************************
      //! Type of acceleration
      using GravityAcceleration = ::elastica::Vec3;
      //! Type of time
      using TimeType = typename Traits::TimeType;
      //************************************************************************

      //**Destructor************************************************************
      /*!\name Destructor */
      //@{

      //************************************************************************
      /*!\brief Destructor of the GravityField class.
       */
      ~GravityField() = default;
      //************************************************************************

      //@}
      //************************************************************************

      //**Get functions*********************************************************
      /*!\name Get functions */
      //@{

      //************************************************************************
      /*!\name Gravity functions
       * \brief Gets the gravity
       */
      //@{
      auto gravity() & noexcept -> GravityAcceleration& { return gravity_acc_; }
      auto gravity() const& noexcept -> GravityAcceleration const& {
        return gravity_acc_;
      }
      //@}
      //************************************************************************

      //**Friendships***********************************************************
      template <typename OtherTraits, typename OtherConfiguration>
      friend void initialize_parameters(GravityField<OtherTraits>&,
                                        OtherConfiguration const&) noexcept;
      template <typename OtherTraits>
      friend void reset(GravityField<OtherTraits>&,
                        typename GravityField<OtherTraits>::TimeType) noexcept;
      //************************************************************************

      //************************************************************************
      /*!\brief Initializes the gravity field environment from the main
       * simulator
       *
       * \param simulator Current simulation
       *
       * \note
       * `noexcept` not guaranteed since coarse detector has internal state
       */
      template <typename Simulator>
      void initialize(Simulator const&) const noexcept {}
      //************************************************************************

      //************************************************************************
      /*!\brief Resolves gravity for the current simulation world
       *
       * \details
       * Serves as the main entry points to resolving the effect of Gravity
       * environment within \elastica.
       *
       * \param simulator Current simulation.
       * \param time Current simulation time.
       * \return void
       */
      template <typename Simulator>
      void resolve(Simulator& simulator, TimeType) {
        // This kernel gets profiled as part of environments, so use the untimed
        // version of block
        simulator.for_each_block_untimed([this](auto& system) {
          using LoadTag = ::elastica::tags::ExternalLoads;
          using MassTag = ::elastica::tags::Mass;
          ::elastica::get<LoadTag>(system) =
              blaze::outer(gravity_acc_, ::elastica::get<MassTag>(system));
        });
      }
      //************************************************************************

     private:
      //**Member variables******************************************************
      /*!\name Member variables */
      //@{
      //! Acceleration due to gravity.
      GravityAcceleration gravity_acc_;
      //@}
      //************************************************************************
    };
    //**************************************************************************

    //==========================================================================
    //
    //  FREE FUNCTIONS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Resets a gravity field
     * \ingroup environments
     *
     * \param field Gravity field to be reset
     * \param time_v Time value
     */
    template <typename Traits>
    void reset(GravityField<Traits>& field,
               typename GravityField<Traits>::TimeType /* meta */) noexcept {
      field.gravity() =
          typename GravityField<Traits>::GravityAcceleration{0., 0, 0.};
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Initializes parameters of a gravity field from its configuration
     * \ingroup environments
     *
     * \param field Gravity field to be initialized
     * \param config Configuration of a gravity field
     */
    template <typename Traits, typename Configuration>
    void initialize_parameters(
        GravityField<Traits>& field,
        /*configuration::GravityFieldUserConfiguration*/ Configuration const&
            config) noexcept {
      field.gravity() = config.gravity();
    }
    //**************************************************************************

  }  // namespace environments

}  // namespace elastica
