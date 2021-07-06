//==============================================================================
/*!
//  \file
//  \brief Checks if connections configuration is provided
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

#include "Simulator/Configuration/TypeTraits/Connections.hpp"

namespace elastica {

  namespace modules {

    namespace detail {

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <typename ConnectionsConfiguration>
      struct HasConnectionsConfiguration {
        // assert conforms to
        static_assert(::elastica::tt::IsValidConnectionsConfiguration<
                          ConnectionsConfiguration>::value,
                      R"error(
A conforming Connections configuration has not been found in the list of
configurations passed in to make_simulator(), even though the Connections module
 has been explicitly requested!
Aborting compilation... Please check if you have provided a Connections
configuration using ::elastica::configuration::make_connections_configuration(),
as an argument to make_simulator_config(). If you are using a custom
ConnectionsConfiguration please ensure it derives publicly from
::elastica::configuration::protocols::ConnectionsConfiguration
)error");
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace detail

  }  // namespace modules

}  // namespace elastica
