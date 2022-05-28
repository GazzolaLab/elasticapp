//
// Created by Tejaswin Parthasarathy on 5/3/21.
//
#include "Framework/TestingFramework.hpp"
#include "Systems/CosseratRods/Components.hpp"
#include "Systems/CosseratRods/CosseratRodPlugin.hpp"
///
///
#include "Systems/CosseratRods/Mocks.hpp"
#include "Utilities/MakeArray.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/TMPL.hpp"

namespace {
  using namespace elastica;
  using namespace elastica::cosserat_rod;
}  // namespace

namespace {

  template <template <typename>
            class ComputationalBlock,  // Block with implementation
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>  // Skills
  class TestCosseratRodPlugin
      : public CosseratRodPlugin<mock::BasicCosseratRodTraits,
                                 ComputationalBlock, Components...> {};

}  // namespace

namespace {

  template <typename D>
  struct DummyBlock : public D {};

  using PluginsToBeTested = tmpl::list<
      TestCosseratRodPlugin<DummyBlock, component::WithCircularCosseratRod>,
      TestCosseratRodPlugin<DummyBlock, component::WithSquareCosseratRod>,
      TestCosseratRodPlugin<DummyBlock, component::WithCircularCosseratRod,
                            component::WithDiagonalLinearHyperElasticModel>,
      TestCosseratRodPlugin<DummyBlock, component::WithSquareCosseratRod,
                            component::WithDiagonalLinearHyperElasticModel>,
      TestCosseratRodPlugin<
          DummyBlock, component::WithCircularCosseratRod,
          component::WithExplicitlyDampedDiagonalLinearHyperElasticModel>,
      TestCosseratRodPlugin<
          DummyBlock, component::WithSquareCosseratRod,
          component::WithExplicitlyDampedDiagonalLinearHyperElasticModel>>;

  template <typename Plugin>
  struct ReportPluginProtocolConformation : std::true_type {
    static_assert(::tt::assert_conforms_to<Plugin, ::blocks::protocols::Plugin>,
                  "Plugin above does not conform to the protocols "
                  "expected by a blocks Plugin!");
  };

  using plugin_checks IGNORE_UNUSED =
      decltype(tmpl::transform<PluginsToBeTested,
                               ReportPluginProtocolConformation<tmpl::_1>>{});

}  // namespace

namespace {

  template <typename, typename>
  struct EmptyComponent {
    using InitializedVariables = tmpl::list<>;
    using ComputedVariables = tmpl::list<>;
    using Variables = tmpl::list<>;

    template <class DownstreamBlock, class CosseratInitializer>
    static void initialize(DownstreamBlock&, CosseratInitializer&&) {}
  };

}  // namespace

// namespace blocks {
//
//   template <template <typename, typename> class... T>
//   class Block<TestCosseratRodPlugin<Block, T...>>
//       : public BlockFacade<TestCosseratRodPlugin<Block, T...>> {
//     using Parent = BlockFacade<TestCosseratRodPlugin<Block, T...>>;
//     using Plugin = typename Parent::PluginType;
//
//     static_assert(::tt::assert_conforms_to<Plugin,
//     blocks::protocols::Plugin>,
//                   "Conform to plugin!");
//   };
//
// }  // namespace blocks