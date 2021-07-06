//==============================================================================
/*!
//  \file
//  \brief Checks if forcing configuration is provided
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

#include "Simulator/Configuration/TypeTraits/Forcing.hpp"

namespace elastica {

  namespace modules {

    namespace detail {

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <typename ForcingConfiguration>
      struct HasForcingConfiguration {
        // assert conforms to
        static_assert(::elastica::tt::IsValidForcingConfiguration<
                          ForcingConfiguration>::value,
                      R"error(
A conforming Forcing configuration has not been found in the list of
configurations passed in to make_simulator(), even though the Forcing module
 has been explicitly requested!
Aborting compilation... Please check if you have provided a Forcing
configuration using ::elastica::configuration::make_forcing_configuration(),
as an argument to make_simulator_config(). If you are using a custom
ForcingConfiguration please ensure it derives publicly from
::elastica::configuration::protocols::ForcingConfiguration
)error");
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace detail

  }  // namespace modules

}  // namespace elastica
