#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include "Simulator/Environments/Primitives/Types.hpp"
#include "Simulator/Environments/Tags.hpp"
#include "Simulator/Interface/Protocols/Environments.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/TMPL.hpp"

//==============================================================================
//
//  ENVIRONMENT CONFIGURATION
//
//==============================================================================
#include "Configuration/Environments.hpp"

namespace elastica {

  namespace defaults {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    /*!\brief Default configuration of environments in the simulation
    // \ingroup config
    //
    // The EnvironmentsConfiguration struct is the customization point of domain
    // related properties in a simulation involving the Elastica++ library. It
    // offers the ability to customize the simulation with different
    // environments. It defines the nested types \a AdmissibleEnvironmentTags,
    // which indicates the environments for the simulation and
    // \a AdmissibleImposedPrimitives which represents the (non-integrated)
    // primitives in the simulation.
    */
    struct EnvironmentsConfiguration
        : public ::tt::ConformsTo<protocols::EnvironmentsConfiguration> {
     public:
      //**Type definitions******************************************************
      // TODO : Ensure ordering with collision last
      //! Tags of admissible environment primitives in the default simulation
      using AdmissibleEnvironmentTags =
          ELASTICA_DEFAULT_ADMISSIBLE_ENVIRONMENT_TAGS;

      //! Type of admissible imposed primitives
      using AdmissibleImposedPrimitives =
          ELASTICA_DEFAULT_ADMISSIBLE_IMPOSED_PRIMITIVES;
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace defaults

}  // namespace elastica
