#pragma once

#include "elastica/Utilities/TMPL.hpp"

// DONE : Types in systems, don't include the entire header
////////// Forward declarations come first
#include "Systems/RigidBody/Types.hpp"
//////////
#include "elastica/Systems/Tags.hpp"

// The configuration
#include "elastica/Configuration/Systems.hpp"

namespace elastica {

  namespace defaults {

    struct SystemsConfiguration {
      using AdmissibleSystemCollectionTags =
          ELASTICA_DEFAULT_ADMISSIBLE_SYSTEM_TAGS;
      // tmpl::list<::elastica::rigidbody::Sphere>;
      //      using AdmissibleSystemCollectionTags =
      //          tmpl::list<::elastica::systems::SphereTag>;
    };
    // DONE Type trait/protocol for valid Systems Configuration

  }  // namespace defaults

}  // namespace elastica
