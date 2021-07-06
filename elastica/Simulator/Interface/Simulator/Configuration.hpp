//==============================================================================
/*!
//  \file
//  \brief Checks if domain configuration is provided
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

#include "Simulator/Configuration/TypeTraits/Domain.hpp"

namespace elastica {

  namespace interface {

    namespace detail {

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <typename DomainConfiguration>
      struct HasDomainConfiguration {
        // assert conforms to
        static_assert(::elastica::tt::IsValidDomainConfiguration<
                          DomainConfiguration>::value,
                      R"error(
A conforming Domain configuration has not been found in the list of
configurations passed in to make_simulator().
Aborting compilation... Please check if you have provided a Domain
configuration using ::elastica::configuration::make_domain_configuration(),
as an argument to make_simulator_config(). If you are using a custom
DomainConfiguration please ensure it derives publicly from
::elastica::configuration::protocols::DomainConfiguration
)error");
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace detail

  }  // namespace modules

}  // namespace elastica
