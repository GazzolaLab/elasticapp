//
// Created by Tejaswin Parthasarathy on 5/3/21.
//
#include "Framework/TestingFramework.hpp"
// for defintion of Cosserat rod plugin specialization
#include "Systems/CosseratRods/Block.hpp"
#include "Systems/CosseratRods/BlockInitializer.hpp"
#include "Systems/CosseratRods/Components.hpp"
#include "Systems/CosseratRods/CosseratRodPlugin.hpp"
///
///
#include "Systems/CosseratRods/Mocks.hpp"
#include "Utilities/MakeArray.hpp"
#include "Utilities/TMPL.hpp"

namespace {
  using namespace elastica;
  using namespace elastica::cosserat_rod;
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

  template <template <typename, typename> class... T>
  using GenerateTestPlugin =
      CosseratRodPlugin<mock::MinimalCosseratRodTraits, blocks::Block, T...>;

  template <typename Plugin>
  using GenerateBlock = blocks::Block<Plugin>;

}  // namespace

namespace {

  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.Initializer.basic",
                     "[Systems][Unit]") {
    using TestPlugin = GenerateTestPlugin<EmptyComponent>;

    constexpr auto n_initializers = 1UL;
    auto n_times_called = make_array<n_initializers>(0U);
    auto initializer = initialize_cosserat_rod<TestPlugin>(
        // from before
        blocks::initialize<tags::NElement>([&i = n_times_called[0]](...) {
          ++i;
          return 2UL;
        }));

    // Model::initialize(model, std::move(initializer));

    // check if initialization is done as expected and can be called multiple
    // times even if the initializer is not moved
    CHECK(initializer.n_elems() == 2UL);
    CHECK_ITERABLE_APPROX(n_times_called, make_array<n_initializers>(1U));
    CHECK(initializer.n_elems() == 2UL);
    CHECK_ITERABLE_APPROX(n_times_called, make_array<n_initializers>(2U));

    // initialize directly without a lambda
    auto another_initializer = initialize_cosserat_rod<TestPlugin>(
        // from before
        blocks::initialize<tags::NElement>(20UL));
    CHECK(another_initializer.n_elems() == 20UL);
    CHECK(std::move(another_initializer).n_elems() == 20UL);
  }

}  // namespace