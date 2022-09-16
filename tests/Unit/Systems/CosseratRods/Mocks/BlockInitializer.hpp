//
// Created by Tejaswin Parthasarathy on 4/22/21.
//

#pragma once

#include "Plugin.hpp"
#include "Systems/Block.hpp"
#include "Systems/Block/BlockVariables/BlockInitializer.hpp"

namespace mock {

  template <class AnyMockPlugin, typename... Initializers>
  class BlockInitializer
      : public ::blocks::BlockInitializer<AnyMockPlugin, Initializers...> {
    using Plugin = AnyMockPlugin;
    using Parent = ::blocks::BlockInitializer<Plugin, Initializers...>;
    using Parent::Parent;

   public:
    static constexpr std::size_t n_leafs = sizeof...(Initializers);
    inline std::size_t n_elems() noexcept { return 10UL; }
    // inline std::size_t n_elems() const&& noexcept { return 10UL; }
  };

  namespace detail {

    template <typename>
    struct Rebind;

    template <typename Plugin,
              typename... InitializerVariables>  // Cosserat rod traits
    struct Rebind<blocks::BlockInitializer<Plugin, InitializerVariables...>> {
      using type = ::mock::BlockInitializer<Plugin, InitializerVariables...>;
    };
  }  // namespace detail

  template <typename AnyMockPlugin, typename... NamedInitializers>
  constexpr inline decltype(auto) initialize_block(
      NamedInitializers&&... initializers) noexcept {
    using ReturnType = typename detail::Rebind<
        decltype(blocks::initialize_block<AnyMockPlugin>(
            std::declval<NamedInitializers>()...))>::type;
    return ReturnType{std::forward<NamedInitializers>(initializers).get()...};
    //    return ReturnType{blocks::initialize_block<AnyMockPlugin>(
    //        std::forward<NamedInitializers>(initializers)...)};
  }

}  // namespace mock