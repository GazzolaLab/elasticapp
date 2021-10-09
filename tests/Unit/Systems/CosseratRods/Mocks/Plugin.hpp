//
// Created by Tejaswin Parthasarathy on 4/22/21.
//

#pragma once

#include "Systems/Block.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TMPLDebugging.hpp"

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

    using Traits = tmpl::front<tmpl::list<typename T<Model>::Traits...>>;

    //    template <typename V>
    //    struct A {
    //        using type = typename V::type;
    //    };
    using IndexType = typename Traits::DataType::Index;
    using ScalarType = typename Traits::DataType::Scalar;
    using VectorType = typename Traits::DataType::Vector;
    using MatrixType = typename Traits::DataType::Matrix;
    //    using ScalarVariables = tmpl::filter<Variables,
    //    std::is_same<ScalarType, tmpl::bind<tmpl::type_from, tmpl::_1>>>;
    //    using VectorVariables = tmpl::filter<Variables,
    //    std::is_same<VectorType, tmpl::bind<tmpl::type_from, tmpl::_1>>>;
    //    using MatrixVariables = tmpl::filter<Variables,
    //    std::is_same<MatrixType, tmpl::bind<tmpl::type_from, tmpl::_1>>>;

    template <typename DataType>
    static constexpr auto n_variables_of() noexcept -> std::size_t {
      using VariablesOfDataType = tmpl::filter<
          Variables,
          std::is_same<DataType, tmpl::bind<tmpl::type_from, tmpl::_1>>>;
      return tmpl::size<VariablesOfDataType>::value;
    }

    template <typename DataType>
    static constexpr auto size_of() noexcept -> std::size_t {
      return sizeof(typename DataType::type) *
             This::template n_variables_of<DataType>();
    }

    static constexpr auto size_of_block() noexcept -> std::size_t {
      return This::size_of<IndexType>() + This::size_of<ScalarType>() +
             This::size_of<VectorType>() + This::size_of<MatrixType>();
    }

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
