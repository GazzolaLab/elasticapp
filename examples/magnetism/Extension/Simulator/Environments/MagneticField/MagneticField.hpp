//==============================================================================
/*!
//  \file
//  \brief A spatially uniform magnetic field over all systems
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

#include <elastica/Simulator/Environments/Common.hpp>
//
#include <elastica/Time/Types.hpp>
//
#include <elastica/Utilities/Get.hpp>
#include <elastica/Utilities/Math/Vec3.hpp>
#include <elastica/Utilities/Singleton.hpp>
//
#include "Extension/Simulator/Environments/MagneticField/Tags.hpp"
#include "Extension/Simulator/Environments/MagneticField/Types.hpp"
//
#include "Extension/Simulator/Environments/MagneticField/ModuleConfiguration/ModuleConfiguration.hpp"
#include "Extension/Simulator/Environments/MagneticField/kernel.hpp"
//
#include "Extension/Systems/Tags.hpp"
//
#include "Extension/Systems/CosseratRods/Components/helpers/TypeTraits.hpp"

namespace elastica {

  namespace environments {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Applies a uniform magnetic field over all systems
     * \ingroup environments
     */
    template <typename Traits>
    class MagneticField final
        : public elastica::Singleton<MagneticField<Traits>>,
          public ::tt::ConformsTo<protocols::Environment> {
     public:
      //**Type definitions******************************************************
      //! Type of acceleration
      using MagneticFieldProfile = typename Traits::MagneticFieldProfile;
      //! Type of time
      using TimeType = typename Traits::TimeType;
      //************************************************************************

     private:
      //**Friendships***********************************************************
      //! Friendship with Singleton
      BEFRIEND_SINGLETON;
      //************************************************************************

      //**Constructor***********************************************************
      /*!\name Constructor */
      //@{
      //************************************************************************
      /*!\brief Constructor of the Magnetic field class.
       */
      MagneticField() : magnetic_field_profile_{} {};
      //************************************************************************

      //@}
      //************************************************************************

      //**Type definitions******************************************************
      //! This type
      using This = MagneticField<Traits>;
      //************************************************************************

     public:
      //**Destructor************************************************************
      /*!\name Destructor */
      //@{

      //************************************************************************
      /*!\brief Destructor of the MagneticField class.
       */
      ~MagneticField() = default;
      //************************************************************************

      //@}
      //************************************************************************

      //**Get functions*********************************************************
      /*!\name Get functions */
      //@{

      //************************************************************************
      /*!\name Magnetic functions
       * \brief Gets the profile
       */
      //@{
      auto magnetic_field_profile() & noexcept -> MagneticFieldProfile& {
        return magnetic_field_profile_;
      }
      auto magnetic_field_profile() const& noexcept
          -> MagneticFieldProfile const& {
        return magnetic_field_profile_;
      }
      //@}
      //************************************************************************

      //************************************************************************
      /*!\name Magnetic functions
       * \brief Gets the profile
       */
      //@{
      auto profile() & noexcept -> MagneticFieldProfile& {
        return magnetic_field_profile();
      }
      auto profile() const& noexcept -> MagneticFieldProfile const& {
        return magnetic_field_profile();
      }
      //@}
      //************************************************************************

      //**Friendships***********************************************************
      template <typename OtherTraits, typename OtherConfiguration>
      friend void initialize_parameters(MagneticField<OtherTraits>&,
                                        OtherConfiguration const&) noexcept;
      template <typename OtherTraits>
      friend void reset(MagneticField<OtherTraits>&,
                        typename MagneticField<OtherTraits>::TimeType) noexcept;
      //************************************************************************

      //************************************************************************
      /*!\brief Initializes the magnetic field environment from the main
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

     protected:
      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      struct Op {
        template <typename System>
        void operator()(System& system) const noexcept {
          using ::elastica::cosserat_rod::component::tt::
              MagnetismComponentTrait;
          operator()(system,
                     typename MagnetismComponentTrait<System>::value_type{});
        }

        using magnetic_tag = std::true_type;
        using non_magnetic_tag = std::false_type;

        template <typename System>
        void operator()(System& system, magnetic_tag /*meta*/) const noexcept {
          using TorquesTag = ::elastica::tags::ExternalTorques;
          using MagnetizationTag = ::elastica::tags::Magnetization;
          using DirectorTag = ::elastica::tags::Director;

          // for now it is a scalar implementation, can be SIMDized if needed
          detail::kernel(::elastica::get<TorquesTag>(system),
                         ::elastica::get<DirectorTag>(system),
                         ::elastica::get<MagnetizationTag>(system),
                         magnetic_field_value);
        }

        template <typename System>
        inline void operator()(System& /*meta*/,
                               non_magnetic_tag /*meta*/) const noexcept {}

        // spatially independent for now
        ::elastica::Vec3 magnetic_field_value;
      };
      /*! \endcond */
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Resolves magnetic for the current simulation world
       *
       * \details
       * Serves as the main entry points to resolving the effect of Magnetic
       * environment within \elastica.
       *
       * \param simulator Current simulation.
       * \param time Current simulation time.
       * \return void
       */
      template <typename Simulator>
      void resolve(Simulator& simulator, TimeType time_v) const {
        Op op{magnetic_field_profile()(time_v)};
        simulator.for_each_block(op);
      }
      //************************************************************************

     private:
      //**Member variables******************************************************
      /*!\name Member variables */
      //@{
      //! Acceleration due to magnetic.
      MagneticFieldProfile magnetic_field_profile_;
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
    /*!\brief Resets a magnetic field
     * \ingroup environments
     *
     * \param field Magnetic field to be reset
     * \param time_v Time value
     */
    template <typename Traits>
    void reset(MagneticField<Traits>& /*field*/,
               typename MagneticField<Traits>::TimeType /* meta */) noexcept {}
    //**************************************************************************

    //**************************************************************************
    /*!\brief Initializes parameters of a magnetic field from its configuration
     * \ingroup environments
     *
     * \param field Magnetic field to be initialized
     * \param config Configuration of a magnetic field
     */
    template <typename Traits, typename Configuration>
    void initialize_parameters(
        MagneticField<Traits>& field,
        /*configuration::MagneticFieldUserConfiguration*/ Configuration const&
            config) noexcept {
      field.magnetic_field_profile() = config.magnetic_field_profile();
    }
    //**************************************************************************

  }  // namespace environments

}  // namespace elastica
