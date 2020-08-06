#pragma once

#include <utility>

namespace elastica {

  namespace collision {

    namespace tags {
      // Tags for transfer
      struct from_t {};
      struct to_t {};
    }  // namespace tags

    // Transfer Variables for
    constexpr tags::from_t from{};
    constexpr tags::to_t to{};

    // Type traits for
    namespace tt {

      template <typename FromOrToTag>
      struct SwitchTag {
        static constexpr tags::from_t switch_tag(tags::to_t); // NO LINT
        static constexpr tags::to_t switch_tag(tags::from_t);
        // All other cases should return a compiler error
        static constexpr void switch_tag(...);
        using type = decltype(switch_tag(std::declval<FromOrToTag>()));
      };

    }  // namespace tt

    template <typename FromOrToTag>
    using switch_tag_t = typename tt::SwitchTag<FromOrToTag>::type;

  }  // namespace collision

}  // namespace elastica
