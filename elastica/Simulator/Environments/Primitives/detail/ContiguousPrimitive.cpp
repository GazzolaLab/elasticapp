#include "ContiguousPrimitive.hpp"

#include "elastica/Simulator/Environments/Primitives/CapsulePrimitive.hpp"

namespace elastica{

  namespace collision{

    namespace detail{

      // Explicitly instantiate only the classes we want to be defined.
      template class ContiguousPrimitive<CapsulePrimitive>;
    }

  }

}
