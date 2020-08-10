#pragma once

#include "elastica/Simulator/Environments/Primitives/Types.hpp"
#include "elastica/Simulator/Environments/Tags.hpp"
#include "elastica/Utilities/TMPL.hpp"

// The configuration file
#include "elastica/Configuration/Environments.hpp"

namespace elastica {

  namespace defaults {

    struct EnvironmentConfiguration {
      // TODO : Ensure ordering with collision last
      using AdmissibleEnvironmentTags =
          ELASTICA_DEFAULT_ADMISSIBLE_ENVIRONMENT_TAGS;
      // tmpl::list<collision::tags::CollisionDetectionTag>;

      //      using AdmissibleImposedPrimitives =
      //      tmpl::list<::elastica::rigidbody::Sphere>;
      //      using AdmissibleEnvironmentTags = tmpl::list<>;
      using AdmissibleImposedPrimitives =
          ELASTICA_DEFAULT_ADMISSIBLE_IMPOSED_PRIMITIVES;
      // tmpl::list<collision::PlanePrimitive>;
    };

  }  // namespace defaults

}  // namespace elastica
