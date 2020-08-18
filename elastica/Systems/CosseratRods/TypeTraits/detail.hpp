#pragma once

#include <type_traits>

#include "Utilities/TypeTraits.hpp"

namespace elastica {

  namespace detail_cosserat_rod {

    // Detail types

    namespace tt {

      template <typename T>
      struct AddSliceType {
        using type = typename T::SliceType;
      };

    }  // namespace tt

  }    // namespace detail_cosserat_rod

}  // namespace elastica
