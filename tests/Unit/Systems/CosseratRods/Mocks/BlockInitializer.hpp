//
// Created by Tejaswin Parthasarathy on 4/22/21.
//

#pragma once

#include "Plugin.hpp"
#include "Systems/Block.hpp"
#include "Systems/Block/BlockVariables/BlockInitializer.hpp"
#include "Systems/common/Tags.hpp"

namespace mock {

  template <class AnyMockPlugin, typename... Initializers>
  class BlockInitializer
      : public ::blocks::BlockInitializer<AnyMockPlugin, Initializers...> {
    using Plugin = AnyMockPlugin;
    using Parent = ::blocks::BlockInitializer<Plugin, Initializers...>;
    using Parent::Parent;

   public:
    static constexpr std::size_t n_leafs = sizeof...(Initializers);
    inline decltype(auto) n_elems() const noexcept {
      using NElementVariable =
          typename Parent::VariableMap::template variable_from<
              ::elastica::tags::NElement>;
      if constexpr (std::is_same_v<NElementVariable, brigand::no_such_type_>) {
        return 10UL;
      } else {
        return blocks::get<::elastica::tags::NElement>(
            static_cast<Parent const&>(*this))();
      }
    }
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
