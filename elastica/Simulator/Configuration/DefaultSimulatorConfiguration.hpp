//==============================================================================
/*!
//  \file
//  \brief Default Simulator Configuration of \elastica
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

// The base simulator configuration
#include "Simulator/Configuration/SimulatorConfiguration.hpp"

// The default module configurations
#include "Simulator/Configuration/Defaults.hpp"

namespace elastica {

  namespace configuration {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Default configuration of all simulations in \elastica
     * \ingroup config
     *
     * The DefaultSimulatorConfiguration class is the customized entry point of
     * compile-time setup of a default \elastica simulation. It configures the
     * SimulatorConfiguration with default values as obtained from @ref config
     * module. The following code example shows how to configure different
     * default simulations.
     *
     * \example
     * \code
     *  // A simulation configuration with bare minimum requirements
     *  using BareMinimumDefaultSimulatorConfiguration =
     *     SimulatorConfiguration<DefaultDomainConfiguration,
     *                            DefaultSystemsConfiguration,
     *                            DefaultParallelConfiguration>;
     *
     *  // A simulation configuration with more modules
     *  using SimulatorConfigurationWithEnvironments =
     *     SimulatorConfiguration<DefaultDomainConfiguration,
     *                            DefaultSystemsConfiguration,
     *                            DefaultParallelConfiguration,
     *                            DefaultEnvironmentsConfiguration>;
     * \endcode
     * \see SimulatorConfiguration, make_simulator()
     */
    class DefaultSimulatorConfiguration
        : public SimulatorConfiguration<
              DefaultConnectionsConfiguration, DefaultConstraintsConfiguration,
              DefaultDomainConfiguration, DefaultEnvironmentsConfiguration,
              DefaultForcingConfiguration, DefaultParallelConfiguration,
              DefaultSystemsConfiguration> {
      // all should be default constructible
     private:
      //**Type definitions******************************************************
      using Parent = SimulatorConfiguration<
          DefaultConnectionsConfiguration, DefaultConstraintsConfiguration,
          DefaultDomainConfiguration, DefaultEnvironmentsConfiguration,
          DefaultForcingConfiguration, DefaultParallelConfiguration,
          DefaultSystemsConfiguration>;
      //************************************************************************

     public:
      //**Constructors**********************************************************
      /*!\name Constructors */
      //@{
      DefaultSimulatorConfiguration() noexcept
          : Parent(
                DefaultConnectionsConfiguration{},
                DefaultConstraintsConfiguration{}, DefaultDomainConfiguration{},
                DefaultEnvironmentsConfiguration{},
                DefaultForcingConfiguration{}, DefaultParallelConfiguration{},
                DefaultSystemsConfiguration{}){};
      //@}
      //************************************************************************

      //**Destructor************************************************************
      /*!\name Destructor*/
      //@{
      ~DefaultSimulatorConfiguration() = default;
      //@}
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace configuration

}  // namespace elastica