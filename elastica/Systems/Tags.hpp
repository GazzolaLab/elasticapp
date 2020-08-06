#pragma once

#include "elastica/Systems/RigidBodyBase.hpp"
#include "elastica/Simulator/Environments/Primitives/Types.hpp"

namespace elastica{

  namespace systems{

    struct SphereTag{
      using SystemCollectionType = ::elastica::rigidbody::Sphere;
      using PrimitiveType = ::elastica::collision::SpherePrimitive;
    };

  }

}