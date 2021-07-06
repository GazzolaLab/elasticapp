//==============================================================================
/*!
//  \file
//  \brief Default domain configuration
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

#include <type_traits>

///
#include "Simulator/Configuration/Defaults/Types.hpp"
///
#include "Simulator/Configuration/ModuleConfiguration/Domain.hpp"
#include "Simulator/Domain/BoundaryTypes.hpp"

//==============================================================================
//
//  DOMAIN CONFIGURATION
//
//==============================================================================
#include "Configuration/Domain.hpp"

namespace elastica {

  namespace configuration {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Default configuration of domain in the simulation
     * \ingroup default_config
     *
     * \details
     * DefaultDomainConfiguration is the default compile-time customization
     * of @ref domain in \elastica.
     * It is default-constructible and the default settings can be changed using
     * ELASTICA_DEFAULT_BOUNDARY_TYPE_ALONG_X(),
     * ELASTICA_DEFAULT_BOUNDARY_TYPE_ALONG_Y(),
     * ELASTICA_DEFAULT_BOUNDARY_TYPE_ALONG_Z().
     */
    struct DefaultDomainConfiguration
        : public DomainConfiguration<ELASTICA_DEFAULT_BOUNDARY_TYPE_ALONG_X,
                                     ELASTICA_DEFAULT_BOUNDARY_TYPE_ALONG_Y,
                                     ELASTICA_DEFAULT_BOUNDARY_TYPE_ALONG_Z> {
     private:
      //**Type definitions******************************************************
      //! Parent type
      using Parent =
          DomainConfiguration<ELASTICA_DEFAULT_BOUNDARY_TYPE_ALONG_X,
                              ELASTICA_DEFAULT_BOUNDARY_TYPE_ALONG_Y,
                              ELASTICA_DEFAULT_BOUNDARY_TYPE_ALONG_Z>;

     public:
      //! Inherit constructors
      DefaultDomainConfiguration() noexcept : Parent(UnboundedDomain{}) {}
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace configuration

}  // namespace elastica
