//
// Created by Tejaswin Parthasarathy on 4/20/21.
//

#pragma once

#include "Plugin.hpp"
#include "Systems/Block.hpp"
#include "Systems/Block/Block/AsVariables.hpp"
#include "Systems/Block/Block/TypeTraits.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace blocks {

  template <template <typename> class T>
  class Block<mock::SingleComponentPlugin<Block, T>>
      : public BlockFacade<mock::SingleComponentPlugin<Block, T>> {
    using Parent = BlockFacade<mock::SingleComponentPlugin<Block, T>>;
    using Plugin = typename Parent::PluginType;

    static_assert(::tt::assert_conforms_to<Plugin, blocks::protocols::Plugin>,
                  "Conform to plugin!");

    //! This type
    using This = Block<Plugin>;
    //! This slice type
    using ThisSlice = BlockSlice<Plugin>;
    // friendships
    friend Parent;

   public:
    using Parent::initialize;
  };

  template <template <typename> class ... T>
  class Block<mock::MultiComponentPlugin<Block, T...>>
      : public BlockFacade<mock::MultiComponentPlugin<Block, T...>> {
    using Parent = BlockFacade<mock::MultiComponentPlugin<Block, T...>>;
    using Plugin = typename Parent::PluginType;

    static_assert(::tt::assert_conforms_to<Plugin, blocks::protocols::Plugin>,
                  "Conform to plugin!");

    //! This type
    using This = Block<Plugin>;
    //! This slice type
    using ThisSlice = BlockSlice<Plugin>;
    // friendships
    friend Parent;

   public:
    using Parent::initialize;

    // we need a ghosts function here :(
    template <typename Tag>
    void fill_ghosts_for() const noexcept{};
  };

}  // namespace blocks