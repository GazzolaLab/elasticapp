#pragma once

#include <type_traits>

#include "Utilities/TypeTraits.hpp"

// Forward declarations
#include "Systems/CosseratRods/Types.hpp"

namespace elastica{

  namespace tt{

    template <typename T>
    struct IsBlockInitializer : ::tt::is_a_t<CosseratBlockInitializer, T>{
    };

  }

}