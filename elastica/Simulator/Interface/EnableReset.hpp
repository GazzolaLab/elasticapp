//==============================================================================
/*!
//  \file
//  \brief \elastica Resettable module
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

///
#include "Utilities/CRTP.hpp"
#include "Utilities/Logging.hpp"
#include "Utilities/Logging/Logger.hpp"
#include "Utilities/NonCopyable.hpp"

//******************************************************************************
// Includes
//******************************************************************************

namespace elastica {

  namespace modules {

    ////////////////////////////////////////////////////////////////////////////
    //
    // Forward declarations of the module
    //
    ////////////////////////////////////////////////////////////////////////////

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <class /*SimTraits*/, class /*Simulator*/>
    class EnableReset;
    /*! \endcond */
    //**************************************************************************
  }  // namespace modules

}  // namespace elastica

namespace elastica {

  namespace modules {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief The reset module
     * \ingroup modules
     *
     * The EnableReset module is an enhancement to the \elastica Simulator
     * and is responsible for enabling reset of the simulator, useful especially
     * in RL environments
     *
     * \tparam SimTraits Options for configuration of the Simulator
     * \tparam Sim Simulator
     *
     * \usage
     * To use EnableReset, pass it as template argument to
     * ::elastica::make_simulator() as shown below.
     *
     * \code
     * auto sim =
     * ::elastica::make_simulator<::elastica::modules::EnableReset>(
     * ::elastica::configuration::DefaultSystemsConfiguration{},
     * ::elastica::configuration::DefaultDomainConfiguration{}
     * );
     * \endcode
     * And the simulator `sim` now has capabilities of being reset. The
     * entry point of resetting is the #reset and #clear methods, see their
     * documentation for more details
     *
     * \see ::elastica::make_simulator()
     */
    template <class SimTraits, class Sim>
    class EnableReset : public CRTPHelper<Sim, EnableReset>,
                        public NonCopyable {
     private:
      //**Compile-time checks***************************************************
      //
      //************************************************************************

      //**Type definitions******************************************************
      //! This type
      using This = EnableReset<SimTraits, Sim>;
      //! CRTP Type
      using CRTP = CRTPHelper<Sim, EnableReset>;
      //! CRTP methods
      using CRTP::self;
      //! Internal size type
      using SizeType = typename SimTraits::SizeType;
      //************************************************************************

     public:
      //**Type definitions******************************************************
      //! Type of simulation time
      using TimeType = typename SimTraits::TimeType;
      //************************************************************************

     protected:
      //**Constructors**********************************************************
      /*!\name Constructors */
      //@{

      //************************************************************************
      /*!\brief Constructor of the Reset module
       */
      EnableReset() : n_times_reset_(0UL) {
        using namespace ::elastica::logging;
        log<info>([](auto& log) {
          log << "Successfully initialized the EnableReset module";
        });
      };
      //************************************************************************

      //************************************************************************
      /*!\brief Move constructor of the Reset module.
       *
       * \details This is not needed strictly (as it is ensured to be a
       * singleton class by the Simulator). However, while testing it is useful.
       */
      EnableReset(EnableReset&& other) noexcept
          : n_times_reset_(other.n_times_reset_) {}
      //************************************************************************

      //@}
      //************************************************************************

     public:
      //**Destructor************************************************************
      /*!\name Destructor */
      //@{

      //************************************************************************
      /*!\brief Destructor
       */
      ~EnableReset() {
        using namespace ::elastica::logging;
        log<info>([](auto const& log) {
          log << "Successfully cleaned up the EnableReset module";
        });
      };
      //************************************************************************

      //@}
      //************************************************************************

     public:
      //**Interface methods*****************************************************
      /*!\name Interface methods*/
      //@{

      //************************************************************************
      /*!\brief Resets the simulator
       *
       * \details
       * reset() is an interface for resetting an \elastica simulation. It
       * clears all registered systems, and clears other module's information
       * as well
       *
       * \param time_v The time of reset, will be propagated to the modules
       * \exception std::runtime_error If the reset of any module throws an
       * error, it is propagated here, rather than a hard exit.
       *
       * \example
       * Assuming the simulator has a vector `v` as its member, the following
       * code shows how you use reset() to reset the simulator
       * \snippet Test_EnableReset.cpp reset_eg
       */
      void reset(TimeType time_v = TimeType{0.0}) {
        self().reset_modules(time_v);
        ++n_times_reset_;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Clears the simulator
       *
       * \details
       * clear() is the other interface to reset the simulator, it does not take
       * any parameters and reflects std::vector's clear() method. This is
       * provided for symmetry with the simulator's emplace() method.
       *
       * \example
       * Assuming the simulator has a vector `v` as its member, the following
       * code shows how you use clear() to remove the added information
       * \snippet Test_EnableReset.cpp clear_eg
       */
      void clear() { reset(); }
      //************************************************************************

      //************************************************************************
      /*!\brief Gets the number of times the simulation has been reset
       */
      inline auto n_times_reset() const noexcept -> SizeType {
        return n_times_reset_;
      }
      //************************************************************************

      //@}
      //************************************************************************

     private:
      //**Member variables******************************************************
      //! The number of times the simulator has been reset
      SizeType n_times_reset_;
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace modules

}  // namespace elastica