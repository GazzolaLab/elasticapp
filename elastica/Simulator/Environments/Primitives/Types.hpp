#pragma once

//#include "elastica/Simulator/Environments/Primitives/Protocols.hpp"
//#include "elastica/Simulator/Environments/Primitives/TypeTraits.hpp"
//#include "elastica/Utilities/ProtocolHelpers.hpp"
//#include "elastica/Utilities/Requires.hpp"
//#include "elastica/Utilities/TypeTraits.hpp"

namespace elastica {

  namespace collision {

    // Forward declarations of primitive types
    class RigidBodyPrimitive;
    class GeomPrimitive;
    class SpherePrimitive;
    class PlanePrimitive;
    class ApproximateCylinderPrimitive;
    class CylinderPrimitive;
    class CapsulePrimitive;

    // Skip forward declaration of templated
    // class for better
    /*
    template <typename PT,
              Requires<cpp17::conjunction_v<
                  tt::is_rigid_primitive_v<PT>,
                  ::tt::assert_conforms_to<PT, protocols::ConvexPrimitive>>> =
                  nullptr>
    class ContiguousPrimitive;
    */

  }  // namespace collision

}  // namespace elastica