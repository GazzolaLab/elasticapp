#pragma once
#include "elastica/Utilities/TMPL.hpp"
// TODO : Types in systems, don't include the entire header
#include "elastica/Systems/RigidBodyBase.hpp"

namespace elastica {

  namespace interface {

    struct SystemsConfiguration {
      using AdmissibleSystemCollections =
      tmpl::list<::elastica::rigidbody::Sphere>;
      //      using AdmissibleSystemCollectionTags =
      //          tmpl::list<::elastica::systems::SphereTag>;
    };
    // DONE Type trait/protocol for valid Systems Configuration
  }
}  // namespace elastica
