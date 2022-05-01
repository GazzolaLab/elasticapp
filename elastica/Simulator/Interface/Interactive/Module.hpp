//==============================================================================
/*!
//  \file
//  \brief Implementation of module for interactive visualization
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
/// Serialization support
#include "Simulator/IO/Serialization/Types.hpp"
///
#include "ErrorHandling/NestedExceptions.hpp"
#include "Utilities/CRTP.hpp"
#include "Utilities/Logging.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace elastica {

  namespace modules {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief The interaction module
     * \ingroup modules
     *
     * The Interactions module is an enhancement to the \elastica Simulator
     * and is responsible for visualizing the systems in 3D (using a Graphical
     * display) at every step, for illustrative purposes.
     *
     * \tparam SimTraits Options for configuration of the Simulator
     * \tparam Sim Simulator
     *
     * \usage
     * To use Interactive, pass it as template argument to
     * ::elastica::make_simulator() as shown below. This need NOT be accompanied
     * by any configuration parameter.
     *
     * \code
     * auto sim =
     * ::elastica::make_simulator<::elastica::modules::Interactive>(
     * ::elastica::configuration::DefaultSystemsConfiguration{},
     * ::elastica::configuration::DefaultDomainConfiguration{},
     *  forcing_configuration
     * );
     * \endcode
     * And the simulator `sim` now has capabilities of interaction. The
     * entry points of controlling this interaction is detailed in TODO
     *
     * \see ::elastica::make_simulator()
     */
    template <class SimTraits,  // Simulator options
              class Sim>        // Simulator class
    class Interactive : public CRTPHelper<Sim, Interactive>,
                        public NonCopyable {
      //**Type definitions******************************************************
      //! This type
      using This = Interactive<SimTraits, Sim>;
      //! CRTP Type
      using CRTP = CRTPHelper<Sim, Interactive>;
      //! CRTP methods
      using CRTP::self;
      //! Internal size type
      using SizeType = std::size_t;
      //! Internal type of index
      using IndexType = std::size_t;
      //! Type of simulation time
      using TimeType = typename SimTraits::TimeType;
      //************************************************************************

      //**Friendships***********************************************************
      //! Friendship with Serialize
      friend struct ::elastica::io::Serialize<This>;
      //************************************************************************

     protected:
      //**Constructors**********************************************************
      /*!\name Constructors */
      //@{
      //************************************************************************
      /*!\brief Constructor of the Interactive class.
       */
      // TODO : mark protected
      Interactive() : n_total_forcing(0UL), forcing_(), forcers_() {
        using namespace ::elastica::logging;
        log<info>([](auto const& log) {
          log << "Successfully initialized the Interactive module";
        });
      };
      //************************************************************************

      //************************************************************************
      /*!\brief Move constructor of the Interactive class.
       *
       * \details This is not needed strictly (as it is ensured to be a
       * singleton class by the Simulator). However, while testing it is useful.
       */
      Interactive(Interactive&& other) noexcept
          : n_total_forcing(other.n_total_forcing),
            forcing_(std::move(other.forcing_)),
            forcers_(std::move(other.forcers_)){};
      //************************************************************************
      //@}
      //************************************************************************

     public:
      //**Destructor************************************************************
      /*!\name Destructor */
      //@{
      ~Interactive() {
        using namespace ::elastica::logging;
        log<info>([](auto const& log) {
          log << "Successfully cleaned up the Interactive module";
        });
      };
      //@}
      //************************************************************************

      //************************************************************************
      /*!\brief Render the simulator to the GUI
       *
       * \param time_v The current time of simulation
       * \param step The current time-step of simulation
       */
      inline auto render(TimeType time_v, TimeStepId step) const noexcept
          -> void {
        // this is internal so a member function suffices.
        auto const& algorithm =
            self().template get_algorithm<typename Algorithm::ForceSystems>();
        algorithm(time_v);
      };
      //************************************************************************

      //************************************************************************
      /*!\brief Render the simulator to the GUI
       *
       * \param time_v The current time of simulation
       */
      inline auto render(TimeType time_v) const noexcept -> void {
        // this is internal so a member function suffices.
        auto const& algorithm =
            self().template get_algorithm<typename Algorithm::ForceSystems>();
        algorithm(time_v);
      };
      //************************************************************************

     protected:
      //************************************************************************
      /*!\brief The finalize method.
       *
       * \details
       * The finalize method is the interface to finalize the forcing module
       * of elastica before launching into temporal integration. It checks for
       * any missing information, then assembles all forcing information
       * received thus far to apply forcing performantly
       *
       * \param time_v The time to finalize, usually 0.0
       * \exception std::runtime_error If any forcing information is
       * missing/faulty
       */
      void finalize(TimeType time_v) {
        // to be safe, force initially
        render(time_v);
      }
      //************************************************************************
    };
    //************************************************************************

  }  // namespace modules

}  // namespace elastica