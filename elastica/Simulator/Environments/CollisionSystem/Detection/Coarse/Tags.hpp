#pragma once

#include <utility>

namespace elastica {

  namespace collision {

    namespace detection {

      namespace coarse {

        // Do not expose outside
        namespace tags {

          struct SamePrimitivesTag : std::true_type {};
          struct DifferentPrimitivesTag : std::false_type {};

        }  // namespace tags

      }  // namespace coarse

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica
