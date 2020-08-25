#pragma once

#include "Systems/CosseratRods/Math/Tags.hpp"

///
#include "Utilities/DefineTypes.h"
///

namespace elastica {

  namespace detail_cosserat_rod {

    VoronoiTagImpl operator+(
        VoronoiTagImpl, VoronoiTagImpl);  // Enabling addition between elements

    VoronoiTagImpl operator*(
        VoronoiTagImpl,
        real_t);  // Enabling multiplication between real and element

  }  // namespace detail_cosserat_rod

}  // namespace elastica
