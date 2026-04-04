//==============================================================================
/*!
//  \file
//  \brief Checks if constraints configuration is provided
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

#include "Simulator/Configuration/TypeTraits/Constraints.hpp"

namespace elastica {

  namespace modules {

    namespace detail {

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <typename ConstraintsConfiguration>
      struct HasConstraintsConfiguration {
        // assert conforms to
        static_assert(::elastica::tt::IsValidConstraintsConfiguration<
                          ConstraintsConfiguration>::value,
                      R"error(
A conforming Constraints configuration has not been found in the list of
configurations passed in to make_simulator(), even though the Constraints module
 has been explicitly requested!
Aborting compilation... Please check if you have provided a Constraints
configuration using ::elastica::configuration::make_constraints_configuration(),
as an argument to make_simulator_config(). If you are using a custom
ConstraintsConfiguration please ensure it derives publicly from
::elastica::configuration::protocols::ConstraintsConfiguration
)error");
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace detail

  }  // namespace modules

}  // namespace elastica
