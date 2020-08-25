#pragma once

#include <type_traits>

#include "Systems/CosseratRods/Math/Tags.hpp"
#include "Utilities/TypeTraits.hpp"

namespace elastica {

  namespace detail_cosserat_rod {

    // Detail types

    namespace tt {

      template <typename T>
      struct GetSliceType {
        using type = typename T::SliceType;
      };

      template <typename T, class = cpp17::void_t<>>
      struct CheckForInitializerTagType : std::false_type {};

      template <class T>
      struct CheckForInitializerTagType<
          T, cpp17::void_t<typename T::InitializerTag>> : std::true_type {};

      template <typename T>
      struct GetInitializerTagType {
        using type = typename T::InitializerTag;
      };

      // TODO : shouldn' be here, should be part of strong types
      template <typename T>
      struct GetParameterType {
        using type = typename T::ParameterType;
      };

      template <typename T>
      struct IsNodeStaggered : std::is_same<typename T::TagType, NodeTagImpl> {
      };

      template <typename T>
      struct IsElementStaggered
          : std::is_same<typename T::TagType, ElementTagImpl> {};

      template <typename T>
      struct IsVoronoiStaggered
          : std::is_same<typename T::TagType, VoronoiTagImpl> {};

    }  // namespace tt

  }  // namespace detail_cosserat_rod

}  // namespace elastica
