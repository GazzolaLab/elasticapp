//==============================================================================
/*!
//  \file
//  \brief Checks if environments configuration is provided
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

#include "Simulator/Configuration/TypeTraits/Environments.hpp"

namespace elastica {

  namespace modules {

    namespace detail {

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <typename EnvironmentsConfiguration>
      struct HasEnvironmentsConfiguration {
        // assert conforms to
        static_assert(::elastica::tt::IsValidEnvironmentsConfiguration<
                          EnvironmentsConfiguration>::value,
                      R"error(
A conforming Environments configuration has not been found in the list of
configurations passed in to make_simulator(), even though the Environments module
 has been explicitly requested!
Aborting compilation... Please check if you have provided a Environments
configuration using ::elastica::configuration::make_environments_configuration(),
as an argument to make_simulator_config(). If you are using a custom
EnvironmentsConfiguration please ensure it derives publicly from
::elastica::configuration::protocols::EnvironmentsConfiguration
)error");
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace detail

  }  // namespace modules

}  // namespace elastica
