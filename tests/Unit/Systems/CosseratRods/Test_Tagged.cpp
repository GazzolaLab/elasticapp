//
// Created by Tejaswin Parthasarathy on 5/20/21.
//

#include <vector>

#include "Framework/TestingFramework.hpp"
#include "Systems/CosseratRods/Block.hpp"
#include "Systems/CosseratRods/BlockInitializer.hpp"
#include "Systems/CosseratRods/BlockSlice.hpp"
#include "Systems/CosseratRods/CosseratRodPlugin.hpp"
#include "Systems/CosseratRods/CosseratRodTraits.hpp"
#include "Utilities/AsConst.hpp"

// copy pasted from Test_Block.cpp, can be refactored
namespace {
  using namespace elastica;
  using namespace elastica::cosserat_rod;
  using namespace elastica::cosserat_rod::detail;
}  // namespace

namespace {

  struct TestTag;

  template <template <typename, typename> class... T>
  using GenerateTestPlugin =
      TaggedCosseratRodPlugin<DefaultCosseratRodTraits, blocks::Block, TestTag,
                              T...>;
  template <template <typename, typename> class... T>
  using GenerateTestPluginSlice =
      TaggedCosseratRodPlugin<DefaultCosseratRodTraits, blocks::BlockSlice,
                              TestTag, T...>;
  template <template <typename, typename> class... T>
  using GenerateTestPluginConstSlice =
      TaggedCosseratRodPlugin<DefaultCosseratRodTraits, blocks::ConstBlockSlice,
                              TestTag, T...>;

  template <typename Plugin>
  using GenerateBlock = blocks::Block<Plugin>;

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

namespace {

  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.tagged.empty_component",
                     "[Systems][Unit]") {
    // two vectors and two tensors, all randomized here
    using TestPlugin = GenerateTestPlugin<EmptyComponent>;
    using Model = GenerateBlock<TestPlugin>;

    Model model{};

    std::vector<std::size_t> n_elements{10UL, 20UL, 30UL};

    for (auto element : n_elements) {
      model.emplace(cosserat_rod::initialize_cosserat_rod<TestPlugin>(
          blocks::initialize<tags::NElement>(element)));
    }

    // 1. check the sizes
    CHECK(model.total_n_elements() == 10UL + 2UL + 20UL + 2UL + 30UL);
    // CHECK(model.n_units() == 3UL);
    auto to_vec = [](auto&& blaze_struct) -> std::vector<std::size_t> {
      return std::vector<std::size_t>(
          blaze_struct.data(), blaze_struct.data() + blaze::size(blaze_struct));
    };

    CHECK(to_vec(blocks::get<tags::NElement>(model)) == n_elements);

    {
      // using the [] syntax does not lead to the correct slice type (it returns
      // slice of the parent instead).
      auto first_slice = blocks::slice(model, 0UL);
      static_assert(
          cpp17::is_same_v<
              decltype(first_slice),
              blocks::BlockSlice<GenerateTestPluginSlice<EmptyComponent>>>,
          "Tagged plugin failure!");
      CHECK(first_slice.total_n_elements() == 10UL);
      CHECK(to_vec(blocks::get<tags::NElement>(first_slice)) ==
            std::vector<std::size_t>{10UL});
    }

    {
      auto&& const_model = cpp17::as_const(model);
      auto second_slice = blocks::slice(const_model, 1UL);
      static_assert(
          cpp17::is_same_v<decltype(second_slice),
                           blocks::ConstBlockSlice<
                               GenerateTestPluginConstSlice<EmptyComponent>>>,
          "Tagged plugin failure!");

      CHECK(second_slice.total_n_elements() == 20UL);
      CHECK(to_vec(blocks::get<tags::NElement>(second_slice)) ==
            std::vector<std::size_t>{20UL});
    }

    {
      auto third_slice = model[2UL];
      CHECK(third_slice.total_n_elements() == 30UL);
      CHECK(to_vec(blocks::get<tags::NElement>(third_slice)) ==
            std::vector<std::size_t>{30UL});
    }
  }

  // todo : check for non empty components, borrowed from Test_Components.hpp

}  // namespace