#pragma once

//******************************************************************************
// Includes
//******************************************************************************

////////// Forward declarations come first
#include "Systems/RigidBody/Types.hpp"
//////////
#include "Simulator/Interface/Protocols/Systems.hpp"
#include "Systems/Tags.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/TMPL.hpp"

//==============================================================================
//
//  SYSTEM CONFIGURATION
//
//==============================================================================
#include "Configuration/Systems.hpp"

namespace elastica {

  namespace configuration {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    /*!\brief Default configuration of systems in the simulation
    // \ingroup config
    //
    // The SystemsConfiguration struct is the customization point of domain
    // related properties in a simulation involving the Elastica++ library. It
    // offers the ability to customize the simulation with different boundary
    // conditions. It defines the nested types \a AdmissibleSystemTags,
    // which indicates the systems in the simulation
    */
    struct DefaultSystemsConfiguration
        : public ::tt::ConformsTo<protocols::SystemsConfiguration> {
      //**Type definitions******************************************************
      //! Tags of admissible systems in the default simulation
      using AdmissibleSystemTags = ELASTICA_DEFAULT_ADMISSIBLE_SYSTEM_TAGS;
      //************************************************************************
    };
    // DONE Type trait/protocol for valid Systems Configuration

  }  // namespace configuration

}  // namespace elastica
