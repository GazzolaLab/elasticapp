//
// Created by Tejaswin Parthasarathy on 4/22/21.
//

#pragma once

#include "Systems/Block.hpp"
#include "Systems/common/Cardinality.hpp"
#include "Systems/common/Initialization.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TMPLDebugging.hpp"

namespace mock {

  // B can be block or block slice
  template <template <typename> class B, template <typename> class T>
  struct SingleComponentPlugin
      : public T<B<SingleComponentPlugin<B, T>>>,
        public ::elastica::HasMultipleCardinality<SingleComponentPlugin<B, T>>,
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
  template <typename Tr, template <typename> class B,
            template <typename> class... T>
  struct MultiComponentPlugin
      : public T<B<MultiComponentPlugin<Tr, B, T...>>>...,
        public ::elastica::HasMultipleCardinality<
            MultiComponentPlugin<Tr, B, T...>>,
        public ::tt::ConformsTo<blocks::protocols::Plugin> {
    using This = MultiComponentPlugin<Tr, B, T...>;
    using Model = B<This>;
    using Variables = tmpl::append<typename T<Model>::Variables...>;
    using InitializedVariables =
        tmpl::append<typename T<Model>::InitializedVariables...>;
    using ComputedVariables =
        tmpl::append<typename T<Model>::ComputedVariables...>;

    // using Traits = tmpl::front<tmpl::list<typename T<Model>::Traits...>>;
    using Traits = Tr;

    using IndexType = typename Traits::DataType::Index;
    using ScalarType = typename Traits::DataType::Scalar;
    using VectorType = typename Traits::DataType::Vector;
    using MatrixType = typename Traits::DataType::Matrix;

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

    // needed for the way new force damping is done
    auto size() const noexcept -> std::size_t {
      return 10UL;  // This is from initialize_block's logic
    }

    //**************************************************************************
    /*!\brief Resizes variables placed `OnStaggerType`
     *
     * \param total_elements The total elements across rods
     *
     * \tparam OnStaggerType The staggering/placement type
     */
    void resize_members(std::size_t const total_elements) {
      // new size does not depend on the variable, so we can factor it outside
      // the lambda
      tmpl::for_each<Variables>([this, total_elements](auto v) {
        using Variable = tmpl::type_from<decltype(v)>;
        auto& var = blocks::get<Variable>(static_cast<Model&>(*this));
        const std::size_t new_size = Variable::get_dofs(total_elements);
        Variable::resize(var, new_size);
      });
    }
    //**************************************************************************

    template <class DownstreamBlock, class CosseratInitializer>
    static void initialize(DownstreamBlock& downstream_block,
                           CosseratInitializer&& initializers) {
      downstream_block.resize_members(initializers.n_elems());
      EXPAND_PACK_LEFT_TO_RIGHT(T<Model>::initialize(
          downstream_block, std::forward<CosseratInitializer>(initializers)));
    }
  };

  //  template <template <typename> class T>
  //  using MockPlugin = SingleComponentPlugin<blocks::Block, T>;

  template <typename Tr, template <typename> class... T>
  using MockPlugin = MultiComponentPlugin<Tr, blocks::Block, T...>;

}  // namespace mock

namespace elastica {

  template <typename Variables, typename Tr, template <typename> class... T,
            typename Initializer>
  void initialize_component(::blocks::BlockSlice<mock::MultiComponentPlugin<
                                Tr, ::blocks::BlockSlice, T...>>& block_like,
                            Initializer initializer) {
    detail::initialize_component<Variables>(block_like, std::move(initializer));
  }

  template <typename Variables, typename Tr, template <typename> class... T,
            typename Initializer>
  void initialize_component(
      ::blocks::Block<mock::MultiComponentPlugin<Tr, ::blocks::Block, T...>>&
          block_like,
      Initializer initializer) {
    detail::initialize_component<Variables>(block_like, std::move(initializer));
  }

}  // namespace elastica