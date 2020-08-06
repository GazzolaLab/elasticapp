#pragma once

#include "elastica/Utilities/TMPL.hpp"
#include "elastica/Simulator/Environments/Primitives/Types.hpp"
#include "elastica/Simulator/Environments/Tags.hpp"

namespace elastica {

  namespace interface {

    struct EnvironmentConfiguration {
      // TODO : Ensure ordering with collision last
      using AdmissibleEnvironmentTags =
      tmpl::list<collision::tags::CollisionDetectionTag>;

      //      using AdmissibleImposedPrimitives =
      //      tmpl::list<::elastica::rigidbody::Sphere>;
      //      using AdmissibleEnvironmentTags = tmpl::list<>;
      using AdmissibleImposedPrimitives = tmpl::list<collision::PlanePrimitive>;
    };
    // DONE Type trait for valid Environment Configuration

  }
}  // namespace elastica