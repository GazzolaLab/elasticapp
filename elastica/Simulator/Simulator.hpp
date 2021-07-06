//==============================================================================
/*!
//  \file
//  \brief Inclusion of all simulator modules of \elastica
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

#include <utility>

#include "Simulator/Configuration.hpp"
//
#include "Simulator/Interface/Types.hpp"
//
#include "Simulator/Frames.hpp"
#include "Simulator/Indexers.hpp"
#include "Simulator/Interface/Simulator.hpp"

namespace elastica {

  //////////////////////////////////////////////////////////////////////////////
  //
  // Namespace promotion of configuration types
  //
  //////////////////////////////////////////////////////////////////////////////

  //============================================================================
  //
  //  DOXYGEN DOCUMENTATION
  //
  //============================================================================

  //****************************************************************************
  /*!\defgroup simulator Simulator
   *
   * The Simulator contains the main entry points of an \elastica simulation. It
   * contains the different top level modules used to assemble a simulation.
   */
  //****************************************************************************

  //============================================================================
  //
  //  GLOBAL FUNCTIONS
  //
  //============================================================================

  //****************************************************************************
  /*!\brief Configure and create an \elastica simulator
   * \ingroup simulator
   *
   * \param config The configuration of the desired simulation
   * \return Handle (smart pointer) to a customized \elastica simulator
   *
   * This function returns a wrapped \elastica Simulator. It may throw an
   * exception if memory for the simulator object cannot be reserved.
   *
   * The following example demonstrates the creation of an \elastica Simulator
   * using a default configuration (DefaultSimulatorConfiguration):
   *
   * \example
   * \code
   *   using namespace elastica;
   *
   *   // the environments add-on in the template list creates a simulator with
   *   // environment capabilities
   *   auto simulator = make_simulator<Environments>();
   *
   *   // use the simulator with basic simulator and environment capabilities
   *   simulator->add_environment();
   * \endcode
   * The following example demonstrates the creation of an elastica simulator
   * using a customized configuration:
   *
   * \example
   * \code
   *   using namespace elastica;
   *
   *   // Custom configuration conforming to configuration protocols
   *   struct MyCustomConfiguration{
   *     //...details...//
   *   };
   *
   *   // the environments add-on in the template list creates a simulator with
   *   // environment capabilities, but now with custom configuration
   *   auto simulator = make_simulator<Environments>(CustomConfiguration{});
   *
   *   // use the simulator with basic simulator and environment capabilities
   *   // but with your own custom configuration!
   *   simulator->add_environment();
   * \endcode
   *
   * \n \section make_simulator_details Details
   *
   * The Simulator takes the \c SimulationConfig_ also as a template argument
   * but the interface looks unnatural if its explicitly passed, hence this
   * function is a convenient wrapper that uses the \c config parameter to
   * deduce the type of \c SimulationConfig_ and passing on to the simulator.
   * The modules are directly passed to the Simulator without any modification.
   * - If no configuration arguments are provided to elastica::make_simulator
   *   function, the default one will be chosen.
   *
   * \see SimulatorConfiguration , DefaultSimulatorConfiguration , Simulator
   */
  template <template <typename /*Simulator*/,  // base simulator
                      typename /*SimTraits*/>  // simulator traits
            class... AddOns,                   // all capabilities
            typename SimulationConfig_ =       // configuration
            configuration::DefaultSimulatorConfiguration>
  auto make_simulator(SimulationConfig_ config = {}) {
    return interface::Simulator<SimulationConfig_, AddOns...>::initialize(
        std::move(config));
  }
  //****************************************************************************

}  // namespace elastica