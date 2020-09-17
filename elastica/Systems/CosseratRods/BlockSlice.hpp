#pragma once

#include <tuple>

/// Forward declarations
#include "Systems/CosseratRods/Types.hpp"
///

#include "DataStructures/TaggedTuple.hpp"
#include "Systems/Block/BlockSlice.hpp"
#include "Systems/CosseratRods/CosseratRodPlugin.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  template <typename CRT,                  // Cosserat Rod Traits
            template <typename, typename>  // Skills
            class... Layers>
  class BlockSlice<CosseratRodPlugin<CRT, BlockSlice, Layers...>>
      : public CosseratRodPlugin<CRT, BlockSlice, Layers...>,
        private NonCopyable {
   protected:
    using Parent = CosseratRodPlugin<CRT, BlockSlice, Layers...>;
    friend Parent;
    using This = BlockSlice<Parent>;

#define REPEAT_2(M, N) M(N) M(N + 1)
#define REPEAT_4(M, N) REPEAT_2(M, N) REPEAT_2(M, N + 2)
#define REPEAT_8(M, N) REPEAT_4(M, N) REPEAT_4(M, N + 4)
#define FRIEND(N)                                                       \
  friend std::tuple_element_t<std::min(static_cast<std::size_t>(N + 1), \
                                       sizeof...(Layers)),              \
                              std::tuple<void, Layers<CRT, This>...>>;

    // Friend also all the layers called in parents
    REPEAT_8(FRIEND, 0)

#undef FRIEND
#undef REPEAT_8
#undef REPEAT_4
#undef REPEAT_2
    using typename Parent::SliceTags;
    template <typename T>
    using GetSliceType = typename Parent::template GetSliceType<T>;
    using Slices = tmpl::as_tuple<SliceTags>;

    explicit BlockSlice(Slices && slices) : rod_property_slices_(slices){};
    ~BlockSlice() = default;

    inline constexpr auto data() noexcept -> Slices& {
      return rod_property_slices_;
    };

    inline constexpr auto data() const noexcept
        -> Slices const& {
      return rod_property_slices_;
    };

    template <typename PropertyTag>
    inline constexpr decltype(auto) get() noexcept{
     return std::get<GetSliceType<PropertyTag>>(data());
    }

    template <typename PropertyTag>
    inline constexpr decltype(auto) get() const noexcept{
      return std::get<GetSliceType<PropertyTag>>(data());
    }

   private:
    //**Member variables********************************************************
    /*!\name Member variables */
    //@{
    //!< All slices of CosseratRod
    Slices rod_property_slices_;
    //@}
    //**************************************************************************
  };

}  // namespace elastica