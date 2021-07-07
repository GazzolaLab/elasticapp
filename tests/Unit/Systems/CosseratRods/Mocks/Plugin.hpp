//
// Created by Tejaswin Parthasarathy on 4/22/21.
//

#pragma once

#include "Systems/Block.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/TMPL.hpp"

namespace mock {

  // B can be block or block slice
  template <template <typename> class B, template <typename> class T>
  struct SingleComponentPlugin
      : public T<B<SingleComponentPlugin<B, T>>>,
        public ::tt::ConformsTo<blocks::protocols::Plugin> {
    using This = SingleComponentPlugin<B, T>;
    using Model = B<This>;
    using Component = T<Model>;

    // important for BlockFacade to recognize that it need
    using typename Component::ComputedVariables;
    using typename Component::InitializedVariables;
    using typename Component::Variables;
  };

  // B can be block or block slice
  template <template <typename> class B, template <typename> class... T>
  struct MultiComponentPlugin
      : public T<B<MultiComponentPlugin<B, T...>>>...,
        public ::tt::ConformsTo<blocks::protocols::Plugin> {
    using This = MultiComponentPlugin<B, T...>;
    using Model = B<This>;
    using Variables = tmpl::append<typename T<Model>::Variables...>;
    using InitializedVariables =
        tmpl::append<typename T<Model>::InitializedVariables...>;
    using ComputedVariables =
        tmpl::append<typename T<Model>::ComputedVariables...>;

    template <class DownstreamBlock, class CosseratInitializer>
    static void initialize(DownstreamBlock& downstream_block,
                           CosseratInitializer&& initializers) {
      EXPAND_PACK_LEFT_TO_RIGHT(T<Model>::initialize(
          downstream_block, std::forward<CosseratInitializer>(initializers)));
    }
  };

  //  template <template <typename> class T>
  //  using MockPlugin = SingleComponentPlugin<blocks::Block, T>;

  template <template <typename> class... T>
  using MockPlugin = MultiComponentPlugin<blocks::Block, T...>;

}  // namespace mock
