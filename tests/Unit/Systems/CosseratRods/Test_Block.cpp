//
// Created by Tejaswin Parthasarathy on 5/4/21.
//
#include "Framework/TestingFramework.hpp"
#include "Systems/CosseratRods/Block.hpp"
#include "Systems/CosseratRods/BlockInitializer.hpp"
#include "Systems/CosseratRods/BlockSlice.hpp"
#include "Systems/CosseratRods/Components.hpp"
#include "Systems/CosseratRods/CosseratRodPlugin.hpp"
#include "Systems/CosseratRods/CosseratRodTraits.hpp"
///
#include <algorithm>
#include <stdexcept>

#include "Systems/CosseratRods/Mocks.hpp"
#include "Utilities/End.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"

namespace {
  using namespace elastica;
  using namespace elastica::cosserat_rod;
  using namespace elastica::cosserat_rod::detail;
}  // namespace

namespace {

  template <typename Plugin>
  class TestBlock : public blocks::Block<Plugin> {
    //  Bring things into public scope
   public:
    using Parent = blocks::Block<Plugin>;

    // all types
    using typename Parent::OnElement;
    using typename Parent::OnNode;
    using typename Parent::OnVoronoi;
    using typename Parent::Variables;
    using typename Parent::VariablesOnElement;
    using typename Parent::VariablesOnNode;
    using typename Parent::VariablesOnRod;
    using typename Parent::VariablesOnVoronoi;

    //
    template <typename... Initializers>
    using CosseratInitializer =
        typename Parent::template CosseratInitializer<Initializers...>;
    template <typename OnStaggerType>
    using get_variables_placed =
        typename Parent::template get_variables_placed<OnStaggerType>;
    using typename Parent::GhostNodeIndices;

    // functions
    using Parent::check;
    using Parent::get_size_of_slice;
    using Parent::get_start_of_slice;
    using Parent::initialize_elements_buffer;
    using Parent::n_elements;  // also brings in the non-const version here
    using Parent::n_ghosts;
    using Parent::n_units;
    using Parent::prepare_for_resize;
    using Parent::resize_internal;
    using Parent::resize_members_not_on_rod;
    using Parent::resize_members_on_rod;
    using Parent::resize_members_placed;
    using Parent::total_n_elements;
    using Parent::total_n_nodes;
    using Parent::total_n_voronois;

    using Parent::fill_ghosts;
    using Parent::fill_ghosts_placed;

    // variable function
    using Parent::get_ghost_node_buffer;
  };

}  // namespace

namespace {

  template <template <typename, typename> class... T>
  using GenerateTestPlugin =
      CosseratRodPlugin<mock::MinimalCosseratRodTraits, blocks::Block, T...>;

  template <template <typename, typename> class... T>
  using GenerateTestSlicePlugin =
      CosseratRodPlugin<mock::MinimalCosseratRodTraits, blocks::BlockSlice,
                        T...>;

  template <typename Plugin>
  using GenerateBlock = TestBlock<Plugin>;

  template <typename Traits>
  struct VariableCache {
    template <template <typename> class VariableDefinition, class Dummy = void>
    struct V;

#define SPECIALIZE_START(VARS)                  \
  template <class Dummy>                        \
  struct V<VARS, Dummy> : public VARS<Traits> { \
   public:                                      \
    using Parent = VARS<Traits>;

#define SPECIALIZE_END(VARS) \
  }                          \
  ;

    SPECIALIZE_START(CosseratRodPluginVariables)
    using typename Parent::NElement;
    SPECIALIZE_END(CosseratRodPluginVariables)

#undef SPECIALIZE_END
#undef SPECIALIZE_START
  };  // namespace

  template <template <typename> class VariableDefinition>
  using VariablesIn =
      VariableCache<mock::MinimalCosseratRodTraits>::V<VariableDefinition>;

  template <typename ModelVariables, typename ExpectedVariables>
  struct ReportVariableDifference : std::true_type {
    using Difference = tmpl::list_difference<ModelVariables, ExpectedVariables>;
    static_assert(not tmpl::size<Difference>::value,
                  "Mismatch in expected and reported variables");
  };

  template <typename ModelVariables, typename ExpectedVariables>
  static constexpr bool report_variable_difference =
      ReportVariableDifference<ModelVariables, ExpectedVariables>::value;

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

// before doing anything lets test if the test plugin works well.

namespace testing {
  using TestPlugin = GenerateTestPlugin<EmptyComponent>;
  using Model = GenerateBlock<TestPlugin>;

  // before running other tests see if there is no problem in type definitions
  // etc.
  using ModelAlias = decltype(Model{});
}  // namespace testing

namespace {

  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.Block.plugin_members",
                     "[Systems][Unit]") {
    using TestPlugin = GenerateTestPlugin<EmptyComponent>;
    using Model = GenerateBlock<TestPlugin>;
    Model model{};
    using CRPV = VariablesIn<CosseratRodPluginVariables>;

    static_assert(
        report_variable_difference<typename Model::Variables,
                                   tmpl::list<typename CRPV::NElement>>,
        "Block metaprogramming failure!");
    static_assert(report_variable_difference<typename Model::VariablesOnNode,
                                             tmpl::empty_sequence>,
                  "Block metaprogramming failure!");
    static_assert(report_variable_difference<typename Model::VariablesOnElement,
                                             tmpl::empty_sequence>,
                  "Block metaprogramming failure!");
    static_assert(report_variable_difference<typename Model::VariablesOnVoronoi,
                                             tmpl::empty_sequence>,
                  "Block metaprogramming failure!");
    static_assert(
        report_variable_difference<typename Model::VariablesOnRod,
                                   tmpl::list<typename CRPV::NElement>>,
        "Block metaprogramming failure!");

    // check if members work for an empty block
    CHECK(model.empty());
    // cannot use total_n_elements() because there is an assert inside it
    // CHECK(model.total_n_elements() == 0UL);

    // block check
    CHECK_THROWS_AS(model.check(0UL), std::out_of_range);
    CHECK_THROWS_AS(model.check(end - 1UL), std::out_of_range);

    CHECK_THROWS_WITH(model.check(0UL), Catch::Contains("Index to be "));
    CHECK_THROWS_WITH(model.check(end - 1UL), Catch::Contains("Index to be "));
  }

  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.Block.empty_initialization",
                     "[Systems][Unit]") {
    using TestPlugin = GenerateTestPlugin<EmptyComponent>;
    using Model = GenerateBlock<TestPlugin>;
    using CRPV = VariablesIn<CosseratRodPluginVariables>;

    using ExpectedSlicePlugin = GenerateTestSlicePlugin<EmptyComponent>;
    using ExpectedSlice = blocks::BlockSlice<ExpectedSlicePlugin>;

    using ElementsVector =
        typename mock::MinimalCosseratRodTraits::DataType::Index::type;
    using ElementsVectorSlice =
        typename mock::MinimalCosseratRodTraits::DataType::Index::slice_type;
    using ElementsVectorConstSlice = typename mock::MinimalCosseratRodTraits::
        DataType::Index::const_slice_type;
    using GhostNodeIndices = typename Model::GhostNodeIndices;

    auto test_empty = []() {
      Model model{};
      // check if members work for an empty block
      CHECK(model.n_units() == 0UL);
      CHECK(model.empty());

      // block check
      CHECK_THROWS_AS(model.check(0UL), std::out_of_range);
      CHECK_THROWS_AS(model.check(end - 1UL), std::out_of_range);

      CHECK_THROWS_WITH(model.check(0UL), Catch::Contains("Index to be "));
      CHECK_THROWS_WITH(model.check(end - 1UL),
                        Catch::Contains("Index to be "));

      return model;
    };

    // no state
    {
      auto model(test_empty());

      static_assert(std::is_same<decltype(model.get_ghost_node_buffer()),
                                 GhostNodeIndices&>::value,
                    "Meta failure!");
      static_assert(
          std::is_same<decltype(model.n_elements()), ElementsVector&>::value,
          "Meta failure!");

      const std::size_t new_n_elements = 20UL;

      // 1. check resizing logic in block
      auto total_n_elements_after_resizing =
          model.prepare_for_resize(new_n_elements);
      CHECK(total_n_elements_after_resizing == 20UL);

      CHECK(model.n_elements().empty());
      model.resize_members_on_rod(1UL);
      CHECK(model.n_units() == 1UL);

      // 2. check start and size before checking slices
      CHECK(model.get_start_of_slice<typename CRPV::NElement::Stagger>(0UL) ==
            0UL);
      CHECK(model.get_size_of_slice<typename CRPV::NElement::Stagger>(0UL) ==
            1UL);

      // 3.1 Then check individual variables slicing here
      auto first_element = model.n_elements(0UL);
      CHECK(model.slice<typename CRPV::NElement>(0UL) ==
            ElementsVectorSlice{first_element});

      // 3.2 const slice
      ElementsVectorConstSlice const_expected{first_element};
      CHECK(static_cast<decltype(model) const&>(model)
                .slice<typename CRPV::NElement>(0UL) == const_expected);
    }

    auto test_one_initializer = [test_empty](std::size_t new_n_elements) {
      auto model = test_empty();

      auto initializer(cosserat_rod::initialize_cosserat_rod<TestPlugin>(
          blocks::initialize<tags::NElement>(new_n_elements)));

      auto sliced_model = model.emplace(std::move(initializer));
      static_assert(cpp17::is_same_v<decltype(sliced_model), ExpectedSlice>,
                    "Block slice failure with Cosserat Rods!");

      // check the base model
      CHECK(model.total_n_elements() == new_n_elements);
      CHECK(model.total_n_nodes() == new_n_elements + 1UL);
      CHECK(model.total_n_voronois() == new_n_elements - 1UL);
      CHECK(model.n_units() == 1UL);

      CHECK(model.n_elements() == ElementsVector{new_n_elements});
      CHECK(model.size() == new_n_elements);
      CHECK(not model.empty());

      // block check for first emplacement
      // cannot test it earlier as size_of_slice calls to assert
      CHECK(model.get_start_of_slice<typename CRPV::NElement::Stagger>(0UL) ==
            0UL);
      CHECK(model.get_size_of_slice<typename CRPV::NElement::Stagger>(0UL) ==
            1UL);

      CHECK(model.check(0UL) == 0UL);
      CHECK(model.check(end - 1UL) == 0UL);

      CHECK_THROWS_AS(model.check(1UL), std::out_of_range);
      CHECK_THROWS_AS(model.check(end - 2UL), std::out_of_range);

      CHECK_THROWS_WITH(model.check(1UL), Catch::Contains("Index to be "));
      CHECK_THROWS_WITH(model.check(end - 2UL),
                        Catch::Contains("Index to be "));

      // check the sliced model

      // this weird staic cast is because the non-const version of
      // 'n_elements' is a protected member of
      // 'elastica::cosserat_rod::CosseratRodPlugin
      CHECK(static_cast<decltype(sliced_model) const&>(sliced_model)
                .n_elements(0UL) == new_n_elements);
      CHECK(sliced_model.total_n_elements() == new_n_elements);
      CHECK(sliced_model.total_n_nodes() == new_n_elements + 1UL);
      CHECK(sliced_model.total_n_voronois() == new_n_elements - 1UL);
      CHECK(sliced_model.size() == new_n_elements);
      CHECK(not sliced_model.empty());
      CHECK(sliced_model.check(0UL) == 0UL);
      CHECK(sliced_model.check(end - 1UL) == new_n_elements - 1UL);

      CHECK_THROWS_AS(sliced_model.check(new_n_elements), std::out_of_range);
      CHECK_THROWS_AS(sliced_model.check(end - new_n_elements - 1UL),
                      std::out_of_range);

      CHECK_THROWS_WITH(sliced_model.check(new_n_elements),
                        Catch::Contains("Index to be "));
      CHECK_THROWS_WITH(sliced_model.check(end - new_n_elements - 1UL),
                        Catch::Contains("Index to be "));

      // CHECK(sliced_model.size() == new_n_elements);
      // CHECK(not sliced_model.empty());

      // this is always 1 bruv and cannot be checked because n_units() is
      // protected
      // CHECK(sliced_model.n_units() == 1UL);
      //      CHECK_ITERABLE_APPROX(sliced_model.n_elements(),
      //                            ElementsVectorSlice{std::ref(new_n_elements)});

      // check if the individual slices work as expected
      ElementsVectorSlice expected{new_n_elements};
      CHECK(model.slice<typename CRPV::NElement>(0UL) == expected);

      // const slice
      ElementsVectorConstSlice const_expected{new_n_elements};
      CHECK(static_cast<decltype(model) const&>(model)
                .slice<typename CRPV::NElement>(0UL) == const_expected);

      // noe check if a new slice matches the old slice
      CHECK(blocks::slice(model, 0UL) == sliced_model);

      // check for a new slice of type const
      const auto const_sliced_model(
          blocks::slice(static_cast<decltype(model) const&>(model), 0UL));
      CHECK((const_sliced_model).n_elements(0UL) == new_n_elements);
      CHECK(const_sliced_model.total_n_elements() == new_n_elements);
      CHECK(const_sliced_model.total_n_nodes() == new_n_elements + 1UL);
      CHECK(const_sliced_model.total_n_voronois() == new_n_elements - 1UL);
      CHECK(const_sliced_model.size() == new_n_elements);
      CHECK(not const_sliced_model.empty());
      CHECK(const_sliced_model.check(0UL) == 0UL);
      CHECK(const_sliced_model.check(end - 1UL) == new_n_elements - 1UL);

      CHECK_THROWS_AS(const_sliced_model.check(new_n_elements),
                      std::out_of_range);
      CHECK_THROWS_AS(const_sliced_model.check(end - new_n_elements - 1UL),
                      std::out_of_range);

      CHECK_THROWS_WITH(const_sliced_model.check(new_n_elements),
                        Catch::Contains("Index to be "));
      CHECK_THROWS_WITH(const_sliced_model.check(end - new_n_elements - 1UL),
                        Catch::Contains("Index to be "));

      return model;
    };

    // with state
    {
      const std::size_t first_n_elements(20UL);

      auto model(test_one_initializer(first_n_elements));

      // now that the model has some state, we can check the logic of other
      // members that depend on this state again.

      // 1. check resizing logic in block
      const std::size_t second_n_elements(50UL);
      auto total_n_elements_after_resizing =
          model.prepare_for_resize(second_n_elements);
      // +2 to include element ghosts
      CHECK(total_n_elements_after_resizing ==
            first_n_elements + second_n_elements + 2UL);
      // 1 extra because it is a NODE
      CHECK(model.get_ghost_node_buffer() == GhostNodeIndices{21UL});

      CHECK(not model.n_elements().empty());
      CHECK(model.n_units() == 1UL);
      model.resize_members_on_rod(2UL);
      CHECK(model.n_units() == 2UL);

      // 2. check start and size before checking slices
      // its okay to call it here before the second slice as we explciitly
      // resize members on rod, although it will have garbage data (0)
      CHECK(model.get_size_of_slice<typename CRPV::NElement::Stagger>(1UL) ==
            1UL);
      CHECK(model.get_size_of_slice<typename CRPV::NElement::Stagger>(1UL) ==
            1UL);

      // 3.1 Then check individual variables slicing here
      auto second_element = model.n_elements(1UL);
      CHECK(model.slice<typename CRPV::NElement>(1UL) ==
            ElementsVectorSlice{second_element});

      // 3.2 const slice
      ElementsVectorConstSlice const_expected{second_element};
      CHECK(static_cast<decltype(model) const&>(model)
                .slice<typename CRPV::NElement>(1UL) == const_expected);
    }

    auto test_two_initializer = [test_one_initializer](
                                    std::size_t first_n_elements,
                                    std::size_t second_n_elements) {
      auto model(test_one_initializer(first_n_elements));

      auto second_initializer(cosserat_rod::initialize_cosserat_rod<TestPlugin>(
          blocks::initialize<tags::NElement>(second_n_elements)));

      auto sliced_model = model.emplace(std::move(second_initializer));
      static_assert(cpp17::is_same_v<decltype(sliced_model), ExpectedSlice>,
                    "Block slice failure with Cosserat Rods!");

      // check the base model
      CHECK(model.total_n_elements() ==
            first_n_elements + second_n_elements + 2UL);
      CHECK(model.total_n_nodes() ==
            first_n_elements + second_n_elements + 3UL);
      CHECK(model.total_n_voronois() ==
            first_n_elements + second_n_elements + 1UL);
      CHECK(model.n_units() == 2UL);

      CHECK(model.n_elements() ==
            (ElementsVector{first_n_elements, second_n_elements}));
      CHECK(model.size() == first_n_elements + second_n_elements + 2UL);
      CHECK(not model.empty());

      // block check for first emplacement
      // cannot test it earlier as size_of_slice calls to assert
      CHECK(model.get_start_of_slice<typename CRPV::NElement::Stagger>(0UL) ==
            0UL);
      CHECK(model.get_size_of_slice<typename CRPV::NElement::Stagger>(0UL) ==
            1UL);
      CHECK(model.get_size_of_slice<typename CRPV::NElement::Stagger>(1UL) ==
            1UL);
      CHECK(model.get_size_of_slice<typename CRPV::NElement::Stagger>(1UL) ==
            1UL);

      CHECK(model.check(0UL) == 0UL);
      CHECK(model.check(1UL) == 1UL);
      CHECK(model.check(end - 1UL) == 1UL);
      CHECK(model.check(end - 2UL) == 0UL);

      CHECK_THROWS_AS(model.check(2UL), std::out_of_range);
      CHECK_THROWS_AS(model.check(end - 3UL), std::out_of_range);

      CHECK_THROWS_WITH(model.check(2UL), Catch::Contains("Index to be "));
      CHECK_THROWS_WITH(model.check(end - 3UL),
                        Catch::Contains("Index to be "));

      // 1 extra because it is a NODE
      CHECK(model.get_ghost_node_buffer() ==
            GhostNodeIndices{first_n_elements + 1UL});

      // check if the individual slices work as expected
      ElementsVectorSlice expected{
          typename ElementsVectorSlice::value_type{second_n_elements}};
      CHECK(model.slice<typename CRPV::NElement>(1UL) == expected);

      auto const const_second_n_elments =
          static_cast<std::size_t>(second_n_elements);
      ElementsVectorConstSlice const_expected{const_second_n_elments};
      CHECK(static_cast<decltype(model) const&>(model)
                .slice<typename CRPV::NElement>(1UL) == const_expected);

      // noe check if a new slice matches the old slice
      CHECK(blocks::slice(model, 1UL) == sliced_model);
      // same thing really
      CHECK(model[1UL] == sliced_model);

      // check the sliced model
      CHECK(sliced_model.total_n_elements() == second_n_elements);
      CHECK(sliced_model.total_n_nodes() == second_n_elements + 1UL);
      CHECK(sliced_model.total_n_voronois() == second_n_elements - 1UL);
      CHECK(sliced_model.size() == second_n_elements);
      CHECK(not sliced_model.empty());
      CHECK(sliced_model.check(0UL) == 0UL);
      CHECK(sliced_model.check(end - 1UL) == second_n_elements - 1UL);

      CHECK_THROWS_AS(sliced_model.check(second_n_elements), std::out_of_range);
      CHECK_THROWS_AS(sliced_model.check(end - second_n_elements - 1UL),
                      std::out_of_range);

      CHECK_THROWS_WITH(sliced_model.check(second_n_elements),
                        Catch::Contains("Index to be "));
      CHECK_THROWS_WITH(sliced_model.check(end - second_n_elements - 1UL),
                        Catch::Contains("Index to be "));

      // check for a new slice of type const
      const auto const_sliced_model(
          blocks::slice(static_cast<decltype(model) const&>(model), 1UL));
      CHECK(static_cast<decltype(const_sliced_model) const&>(const_sliced_model)
                .n_elements(0UL) == second_n_elements);
      CHECK(const_sliced_model.total_n_elements() == second_n_elements);
      CHECK(const_sliced_model.total_n_nodes() == second_n_elements + 1UL);
      CHECK(const_sliced_model.total_n_voronois() == second_n_elements - 1UL);
      CHECK(const_sliced_model.size() == second_n_elements);
      CHECK(not const_sliced_model.empty());
      CHECK(const_sliced_model.check(0UL) == 0UL);
      CHECK(const_sliced_model.check(end - 1UL) == second_n_elements - 1UL);

      CHECK_THROWS_AS(const_sliced_model.check(second_n_elements),
                      std::out_of_range);
      CHECK_THROWS_AS(const_sliced_model.check(end - second_n_elements - 1UL),
                      std::out_of_range);

      CHECK_THROWS_WITH(const_sliced_model.check(second_n_elements),
                        Catch::Contains("Index to be "));
      CHECK_THROWS_WITH(const_sliced_model.check(end - second_n_elements - 1UL),
                        Catch::Contains("Index to be "));

      return model;
    };

    // two initializations
    {
      const std::size_t first_n_elements(5UL);
      // dont put const for element slice comparison
      const std::size_t second_n_elements(10UL);
      auto model(test_two_initializer(first_n_elements, second_n_elements));

      // now that the model has some state, we can check the logic of other
      // members that depend on this state again.
      const std::size_t third_n_elements(20UL);
      auto total_n_elements_after_resizing =
          model.prepare_for_resize(third_n_elements);
      // +2 * 2 to include two element ghosts
      CHECK(total_n_elements_after_resizing ==
            first_n_elements + second_n_elements + third_n_elements + 2UL +
                2UL);
      // 1 extra because it is a NODE
      CHECK(model.get_ghost_node_buffer() ==
            GhostNodeIndices{6UL, 6UL + second_n_elements + 2UL});

      CHECK(not model.n_elements().empty());
      CHECK(model.n_units() == 2UL);
      model.resize_members_on_rod(3UL);
      CHECK(model.n_units() == 3UL);

      // no asserts for the index passed in because check should automatically
      // take care of it
      CHECK(model.get_start_of_slice<typename CRPV::NElement::Stagger>(0UL) ==
            0UL);
      CHECK(model.get_start_of_slice<typename CRPV::NElement::Stagger>(1UL) ==
            1UL);
      CHECK(model.get_start_of_slice<typename CRPV::NElement::Stagger>(2UL) ==
            2UL);
      // its okay to call it here before the second slice as we explciitly
      // resize members on rod, although it will have garbage data (0)
      CHECK(model.get_size_of_slice<typename CRPV::NElement::Stagger>(0UL) ==
            1UL);
      CHECK(model.get_size_of_slice<typename CRPV::NElement::Stagger>(1UL) ==
            1UL);
      CHECK(model.get_size_of_slice<typename CRPV::NElement::Stagger>(2UL) ==
            1UL);
    }

    // three initializations
    {
      const std::size_t first_n_elements(5UL);
      // dont put const for element slice comparison
      const std::size_t second_n_elements(10UL);
      auto model(test_two_initializer(first_n_elements, second_n_elements));

      const std::size_t third_n_elements(20UL);
      auto third_initializer(cosserat_rod::initialize_cosserat_rod<TestPlugin>(
          blocks::initialize<tags::NElement>(third_n_elements)));

      auto sliced_model = model.emplace(std::move(third_initializer));
      static_assert(cpp17::is_same_v<decltype(sliced_model), ExpectedSlice>,
                    "Block slice failure with Cosserat Rods!");

      // check the base model
      CHECK(model.total_n_elements() == first_n_elements + 2UL +
                                            second_n_elements + 2UL +
                                            third_n_elements);
      CHECK(model.total_n_nodes() == (first_n_elements + 1UL) + 1UL +
                                         (second_n_elements + 1UL) + 1UL +
                                         (third_n_elements + 1UL));
      CHECK(model.total_n_voronois() == (first_n_elements - 1UL) + 3UL +
                                            (second_n_elements - 1UL) + 3UL +
                                            (third_n_elements - 1UL));
      CHECK(model.n_units() == 3UL);

      CHECK(model.n_elements() ==
            (ElementsVector{first_n_elements, second_n_elements,
                            third_n_elements}));
      CHECK(model.size() == first_n_elements + 2UL + second_n_elements + 2UL +
                                third_n_elements);
      CHECK(not model.empty());

      // block check for first emplacement
      // cannot test it earlier as size_of_slice calls to assert
      CHECK(model.get_start_of_slice<typename CRPV::NElement::Stagger>(0UL) ==
            0UL);
      CHECK(model.get_start_of_slice<typename CRPV::NElement::Stagger>(1UL) ==
            1UL);
      CHECK(model.get_start_of_slice<typename CRPV::NElement::Stagger>(2UL) ==
            2UL);
      CHECK(model.get_size_of_slice<typename CRPV::NElement::Stagger>(0UL) ==
            1UL);
      CHECK(model.get_size_of_slice<typename CRPV::NElement::Stagger>(1UL) ==
            1UL);
      CHECK(model.get_size_of_slice<typename CRPV::NElement::Stagger>(2UL) ==
            1UL);

      CHECK(model.check(0UL) == 0UL);
      CHECK(model.check(1UL) == 1UL);
      CHECK(model.check(2UL) == 2UL);
      CHECK(model.check(end - 1UL) == 2UL);
      CHECK(model.check(end - 2UL) == 1UL);
      CHECK(model.check(end - 3UL) == 0UL);

      CHECK_THROWS_AS(model.check(3UL), std::out_of_range);
      CHECK_THROWS_AS(model.check(end - 4UL), std::out_of_range);

      CHECK_THROWS_WITH(model.check(3UL), Catch::Contains("Index to be "));
      CHECK_THROWS_WITH(model.check(end - 4UL),
                        Catch::Contains("Index to be "));

      // 1 extra because it is a NODE
      CHECK(model.get_ghost_node_buffer() ==
            GhostNodeIndices{
                first_n_elements + 1UL,          // n_nodes in first
                (first_n_elements + 1UL)         // n_nodes in first
                    + (second_n_elements + 1UL)  // n_nodes in second
                    + 1UL  // offset acquired by having a previous unit
            });
      // i > 0
      // get_ghost_node_buffer[i] = get_ghost_node_buffer[i - 1] +
      // (n_elements[i] + 1) i = 0 get_ghost_node_buffer[0UL] = n_elements[0UL]
      // + 1 get_ghost_node_buffer[i] = (n_elements[i] + 1) + (i - 1) *

      // check if the individual slices work as expected
      auto non_const_third_n_elements =
          static_cast<std::size_t>(third_n_elements);
      ElementsVectorSlice expected{
          typename ElementsVectorSlice::value_type{non_const_third_n_elements}};
      CHECK(model.slice<typename CRPV::NElement>(2UL) == expected);

      const auto const_third_n_elments =
          static_cast<std::size_t>(third_n_elements);
      ElementsVectorConstSlice const_expected{const_third_n_elments};
      CHECK(static_cast<decltype(model) const&>(model)
                .slice<typename CRPV::NElement>(2UL) == const_expected);

      // noe check if a new slice matches the old slice
      CHECK(blocks::slice(model, 2UL) == sliced_model);
      // same thing really
      CHECK(model[2UL] == sliced_model);

      // check the sliced model
      CHECK(sliced_model.total_n_elements() == third_n_elements);
      CHECK(sliced_model.total_n_nodes() == third_n_elements + 1UL);
      CHECK(sliced_model.total_n_voronois() == third_n_elements - 1UL);
      CHECK(sliced_model.size() == third_n_elements);
      CHECK(not sliced_model.empty());

      CHECK(sliced_model.check(0UL) == 0UL);
      CHECK(sliced_model.check(end - 1UL) == third_n_elements - 1UL);

      CHECK_THROWS_AS(sliced_model.check(third_n_elements), std::out_of_range);
      CHECK_THROWS_AS(sliced_model.check(end - third_n_elements - 1UL),
                      std::out_of_range);

      CHECK_THROWS_WITH(sliced_model.check(third_n_elements),
                        Catch::Contains("Index to be "));
      CHECK_THROWS_WITH(sliced_model.check(end - third_n_elements - 1UL),
                        Catch::Contains("Index to be "));

      // check for a new slice of type const
      const auto const_sliced_model(
          blocks::slice(static_cast<decltype(model) const&>(model), 2UL));
      CHECK((const_sliced_model).n_elements(0UL) == third_n_elements);
      CHECK(const_sliced_model.total_n_elements() == third_n_elements);
      CHECK(const_sliced_model.total_n_nodes() == third_n_elements + 1UL);
      CHECK(const_sliced_model.total_n_voronois() == third_n_elements - 1UL);
      CHECK(const_sliced_model.size() == third_n_elements);
      CHECK(not const_sliced_model.empty());
      CHECK(const_sliced_model.check(0UL) == 0UL);
      CHECK(const_sliced_model.check(end - 1UL) == third_n_elements - 1UL);

      CHECK_THROWS_AS(const_sliced_model.check(third_n_elements),
                      std::out_of_range);
      CHECK_THROWS_AS(const_sliced_model.check(end - third_n_elements - 1UL),
                      std::out_of_range);

      CHECK_THROWS_WITH(const_sliced_model.check(third_n_elements),
                        Catch::Contains("Index to be "));
      CHECK_THROWS_WITH(const_sliced_model.check(end - third_n_elements - 1UL),
                        Catch::Contains("Index to be "));
    }
  }

}  // namespace

namespace test_tags {
  struct VarOnNode;
  struct VarOnElement;
  struct VarOnVoronoi;
}  // namespace test_tags

namespace {

  template <typename CRT>
  struct VectorComponentVariables {
   private:
    //**Type definitions****************************************************
    //! Traits type
    using Traits = CRT;

   public:
    //! Number of elements within each unit
    struct VarOnNode : public Traits::template CosseratRodInitializedVariable<
                           test_tags::VarOnNode,               //
                           typename Traits::DataType::Vector,  //
                           typename Traits::Place::OnNode> {};
    struct VarOnElement
        : public Traits::template CosseratRodInitializedVariable<
              test_tags::VarOnElement,            //
              typename Traits::DataType::Vector,  //
              typename Traits::Place::OnElement> {};
    struct VarOnVoronoi
        : public Traits::template CosseratRodInitializedVariable<
              test_tags::VarOnVoronoi,            //
              typename Traits::DataType::Vector,  //
              typename Traits::Place::OnVoronoi> {};

    using InitializedVariables =
        tmpl::list<VarOnNode, VarOnElement, VarOnVoronoi>;
    using ComputedVariables = tmpl::list<>;
    using Variables = tmpl::append<InitializedVariables, ComputedVariables>;
  };

  template <typename CRT, typename Block>
  struct VectorComponent : public VectorComponentVariables<CRT>,
                           elastica::CRTPHelper<Block, VectorComponent> {
   protected:
    using Parent = VectorComponentVariables<CRT>;
    using CRTP = elastica::CRTPHelper<Block, VectorComponent>;
    using CRTP::self;

    template <class DownstreamBlock, class CosseratInitializer>
    static void initialize(DownstreamBlock& downstream_block,
                           CosseratInitializer&& initializers) {
      // copy pasted code
      struct {
        CosseratInitializer value;
      } cap{std::forward<CosseratInitializer>(initializers)};
      tmpl::for_each<typename Parent::InitializedVariables>(
          [&downstream_block, &cap
           // cap = std::move(cap)
      ](auto v) mutable -> void {
            using Variable = tmpl::type_from<decltype(v)>;
            using Tag = blocks::initializer_t<Variable>;

            auto& variable(blocks::get<Tag>(downstream_block));
            auto&& initializer(blocks::get<Tag>(std::move(cap.value)));

            auto dofs = Variable::get_dofs(cap.value.n_elems());
            for (std::size_t i = 0; i < dofs; ++i) {
              // TODO : This doesn't work on slices, consider having the
              // slice member in the initializer itself
              Variable::slice(variable, i) = initializer(i);
            }
          });
    }

   public:
    // some stupid function for seeing if it gets inherited
    int tell_me_number() const noexcept { return 42; }

    inline constexpr decltype(auto) get_node_variable() noexcept {
      return blocks::get<test_tags::VarOnNode>(self());
    }
    inline constexpr decltype(auto) get_element_variable() noexcept {
      return blocks::get<test_tags::VarOnElement>(self());
    }
    inline constexpr decltype(auto) get_voronoi_variable() noexcept {
      return blocks::get<test_tags::VarOnVoronoi>(self());
    }
  };
}  // namespace

namespace testing {
  using TestVectorPlugin = GenerateTestPlugin<VectorComponent>;
  using VectorModel = GenerateBlock<TestVectorPlugin>;

  // before running other tests see if there is no problem in type definitions
  // etc.
  using VectorModelAlias = decltype(VectorModel{});
}  // namespace testing

namespace {

  ELASTICA_TEST_CASE(
      "Unit.Systems.CosseratRods.Block.all_placement_members.functions",
      "[Systems][Unit]") {
    using TestPlugin = GenerateTestPlugin<VectorComponent>;
    using Model = GenerateBlock<TestPlugin>;
    Model model{};
    using CRPV = VariablesIn<CosseratRodPluginVariables>;
    using VCPV = VectorComponentVariables<mock::MinimalCosseratRodTraits>;

    static_assert(
        report_variable_difference<
            typename Model::Variables,
            tmpl::list<typename CRPV::NElement, typename VCPV::VarOnNode,
                       typename VCPV::VarOnElement,
                       typename VCPV::VarOnVoronoi>>,
        "Block metaprogramming failure!");
    static_assert(
        report_variable_difference<typename Model::VariablesOnNode,
                                   tmpl::list<typename VCPV::VarOnNode>>,
        "Block metaprogramming failure!");
    static_assert(
        report_variable_difference<typename Model::VariablesOnElement,
                                   tmpl::list<typename VCPV::VarOnElement>>,
        "Block metaprogramming failure!");
    static_assert(
        report_variable_difference<typename Model::VariablesOnVoronoi,
                                   tmpl::list<typename VCPV::VarOnVoronoi>>,
        "Block metaprogramming failure!");
    static_assert(
        report_variable_difference<typename Model::VariablesOnRod,
                                   tmpl::list<typename CRPV::NElement>>,
        "Block metaprogramming failure!");

    // check if members work for an empty block
    CHECK(model.empty());
    CHECK(model.tell_me_number() == 42);
    // cannot use total_n_elements() because there is an assert inside it
    // CHECK(model.total_n_elements() == 0UL);

    // block check
    CHECK_THROWS_AS(model.check(0UL), std::out_of_range);
    CHECK_THROWS_AS(model.check(end - 1UL), std::out_of_range);

    CHECK_THROWS_WITH(model.check(0UL), Catch::Contains("Index to be "));
    CHECK_THROWS_WITH(model.check(end - 1UL), Catch::Contains("Index to be "));
  }

  ELASTICA_TEST_CASE(
      "Unit.Systems.CosseratRods.Block.all_placement_members.initialization",
      "[Systems][Unit]") {
    using TestPlugin = GenerateTestPlugin<VectorComponent>;
    using Model = GenerateBlock<TestPlugin>;
    using CRPV = VariablesIn<CosseratRodPluginVariables>;
    using VCPV = VectorComponentVariables<mock::MinimalCosseratRodTraits>;

    using ExpectedSlicePlugin = GenerateTestSlicePlugin<VectorComponent>;
    using ExpectedSlice = blocks::BlockSlice<ExpectedSlicePlugin>;

    using ElementsVector =
        typename mock::MinimalCosseratRodTraits::DataType::Index::type;
    using DataVector =
        typename mock::MinimalCosseratRodTraits::DataType::Vector::type;
    using ElementsVectorSlice =
        typename mock::MinimalCosseratRodTraits::DataType::Index::slice_type;
    using ElementsVectorConstSlice = typename mock::MinimalCosseratRodTraits::
        DataType::Index::const_slice_type;
    using DataVectorSlice =
        typename mock::MinimalCosseratRodTraits::DataType::Vector::slice_type;
    using DataVectorConstSlice = typename mock::MinimalCosseratRodTraits::
        DataType::Vector::const_slice_type;
    using GhostNodeIndices = typename Model::GhostNodeIndices;

    auto test_empty = []() {
      Model model{};
      // check if members work for an empty block
      CHECK(model.n_units() == 0UL);
      CHECK(model.empty());

      // block check
      CHECK_THROWS_AS(model.check(0UL), std::out_of_range);
      CHECK_THROWS_AS(model.check(end - 1UL), std::out_of_range);

      CHECK_THROWS_WITH(model.check(0UL), Catch::Contains("Index to be "));
      CHECK_THROWS_WITH(model.check(end - 1UL),
                        Catch::Contains("Index to be "));

      return model;
    };

    // no state
    {
      auto model(test_empty());
      const std::size_t new_n_elements = 20UL;

      // 1. check resizing logic in block
      auto total_n_elements_after_resizing =
          model.prepare_for_resize(new_n_elements);
      CHECK(total_n_elements_after_resizing == 20UL);

      CHECK(model.n_elements().empty());
      model.resize_members_on_rod(1UL);
      CHECK(model.n_units() == 1UL);

      // 2. check start and size of slices

      // 2.1 First check elements
      CHECK(model.get_start_of_slice<typename CRPV::NElement::Stagger>(0UL) ==
            0UL);
      CHECK(model.get_size_of_slice<typename CRPV::NElement::Stagger>(0UL) ==
            1UL);
      // this is to make "slice" work, see below
      model.initialize_elements_buffer(new_n_elements, 0UL);

      auto first_element = model.n_elements(0UL);
      CHECK(first_element == new_n_elements);

      // 3.1 Check slices
      CHECK(model.slice<typename CRPV::NElement>(0UL) ==
            ElementsVectorSlice{first_element});
      CHECK(static_cast<decltype(model) const&>(model)
                .slice<typename CRPV::NElement>(0UL) ==
            ElementsVectorConstSlice{first_element});

      // 1.2 now we can check logic for resizing members on different parts
      CHECK(model.get_node_variable().empty());
      CHECK(model.get_element_variable().empty());
      CHECK(model.get_voronoi_variable().empty());

      model.resize_members_not_on_rod(new_n_elements);

      CHECK(model.get_node_variable().size() == (new_n_elements + 1UL));
      CHECK(model.get_element_variable().size() == (new_n_elements));
      CHECK(model.get_voronoi_variable().size() == (new_n_elements - 1UL));

      // 2.2
      // doesn't work off the shelf because .slice calls the get_size_of_slice
      // function which needs the degrees of freedom, and hence doesnt work if
      // n_elements = 0. So we need to populate it with n_elements first
      CHECK(model.get_start_of_slice<typename VCPV::VarOnNode::Stagger>(0UL) ==
            0UL);
      CHECK(model.get_size_of_slice<typename VCPV::VarOnNode::Stagger>(0UL) ==
            (new_n_elements + 1UL));

      CHECK(model.get_start_of_slice<typename VCPV::VarOnElement::Stagger>(
                0UL) == 0UL);
      CHECK(model.get_size_of_slice<typename VCPV::VarOnElement::Stagger>(
                0UL) == (new_n_elements));

      CHECK(model.get_start_of_slice<typename VCPV::VarOnVoronoi::Stagger>(
                0UL) == 0UL);
      CHECK(model.get_size_of_slice<typename VCPV::VarOnVoronoi::Stagger>(
                0UL) == (new_n_elements - 1UL));

      // 3.2
      CHECK(model.tell_me_number() == 42);

      auto& node_variable = model.get_node_variable();
      CHECK_ITERABLE_APPROX(
          model.slice<typename VCPV::VarOnNode>(0UL),
          DataVectorSlice(node_variable.begin(), node_variable.end()));
      CHECK_ITERABLE_APPROX(
          static_cast<decltype(model) const&>(model)
              .slice<typename VCPV::VarOnNode>(0UL),
          DataVectorConstSlice(node_variable.begin(), node_variable.end()));

      auto& element_variable = model.get_element_variable();
      CHECK_ITERABLE_APPROX(
          model.slice<typename VCPV::VarOnElement>(0UL),
          DataVectorSlice(element_variable.begin(), element_variable.end()));
      CHECK_ITERABLE_APPROX(static_cast<decltype(model) const&>(model)
                                .slice<typename VCPV::VarOnElement>(0UL),
                            DataVectorConstSlice(element_variable.begin(),
                                                 element_variable.end()));

      auto& voronoi_variable = model.get_voronoi_variable();
      CHECK_ITERABLE_APPROX(
          model.slice<typename VCPV::VarOnVoronoi>(0UL),
          DataVectorSlice(voronoi_variable.begin(), voronoi_variable.end()));
      CHECK_ITERABLE_APPROX(static_cast<decltype(model) const&>(model)
                                .slice<typename VCPV::VarOnVoronoi>(0UL),
                            DataVectorConstSlice(voronoi_variable.begin(),
                                                 voronoi_variable.end()));
    }

    auto test_one_initializer = [test_empty](std::size_t new_n_elements) {
      auto model = test_empty();

      auto initializer(cosserat_rod::initialize_cosserat_rod<TestPlugin>(
          blocks::initialize<tags::NElement>(new_n_elements),
          blocks::initialize<test_tags::VarOnNode>(
              [](std::size_t i) { return 3.0 + static_cast<double>(i); }),
          blocks::initialize<test_tags::VarOnElement>(
              [](std::size_t i) { return 2.0 + static_cast<double>(i); }),
          blocks::initialize<test_tags::VarOnVoronoi>(
              [](std::size_t i) { return 1.0 + static_cast<double>(i); })));

      auto sliced_model = model.emplace(std::move(initializer));
      static_assert(cpp17::is_same_v<decltype(sliced_model), ExpectedSlice>,
                    "Block slice failure with Cosserat Rods!");

      // A. Base model
      // 1. check the sizes
      CHECK(model.total_n_elements() == new_n_elements);
      CHECK(model.total_n_nodes() == new_n_elements + 1UL);
      CHECK(model.total_n_voronois() == new_n_elements - 1UL);
      CHECK(model.n_units() == 1UL);

      CHECK(model.n_elements() == ElementsVector{new_n_elements});
      CHECK(model.size() == new_n_elements);
      CHECK(not model.empty());

      // 2. check the members in the base model
      CHECK(model.tell_me_number() == 42);

      DataVector expected_node_variable(model.total_n_nodes());
      std::generate(expected_node_variable.begin(),
                    expected_node_variable.end(), [i = -1]() mutable {
                      ++i;
                      return 3.0 + static_cast<double>(i);
                    });
      CHECK_ITERABLE_APPROX(model.get_node_variable(), expected_node_variable);

      DataVector expected_element_variable(model.total_n_elements());
      std::generate(expected_element_variable.begin(),
                    expected_element_variable.end(), [i = -1]() mutable {
                      ++i;
                      return 2.0 + static_cast<double>(i);
                    });
      CHECK_ITERABLE_APPROX(model.get_element_variable(),
                            expected_element_variable);

      DataVector expected_voronoi_variable(model.total_n_voronois());
      std::generate(expected_voronoi_variable.begin(),
                    expected_voronoi_variable.end(), [i = -1]() mutable {
                      ++i;
                      return 1.0 + static_cast<double>(i);
                    });
      CHECK_ITERABLE_APPROX(model.get_voronoi_variable(),
                            expected_voronoi_variable);

      // 3.
      // block check for first emplacement
      // cannot test it earlier as size_of_slice calls to assert
      CHECK(model.get_start_of_slice<typename CRPV::NElement::Stagger>(0UL) ==
            0UL);
      CHECK(model.get_size_of_slice<typename CRPV::NElement::Stagger>(0UL) ==
            1UL);
      CHECK(model.get_start_of_slice<typename VCPV::VarOnNode::Stagger>(0UL) ==
            0UL);
      CHECK(model.get_size_of_slice<typename VCPV::VarOnNode::Stagger>(0UL) ==
            (new_n_elements + 1UL));
      CHECK(model.get_start_of_slice<typename VCPV::VarOnElement::Stagger>(
                0UL) == 0UL);
      CHECK(model.get_size_of_slice<typename VCPV::VarOnElement::Stagger>(
                0UL) == (new_n_elements));
      CHECK(model.get_start_of_slice<typename VCPV::VarOnVoronoi::Stagger>(
                0UL) == 0UL);
      CHECK(model.get_size_of_slice<typename VCPV::VarOnVoronoi::Stagger>(
                0UL) == (new_n_elements - 1UL));

      // 4. check for slices (should be fine since its initialized, but check
      // anyways)
      CHECK(model.slice<typename CRPV::NElement>(0UL) ==
            ElementsVectorSlice{new_n_elements});
      CHECK(static_cast<decltype(model) const&>(model)
                .slice<typename CRPV::NElement>(0UL) ==
            ElementsVectorConstSlice{new_n_elements});

      auto& node_variable = model.get_node_variable();
      CHECK_ITERABLE_APPROX(
          model.slice<typename VCPV::VarOnNode>(0UL),
          DataVectorSlice(node_variable.begin(), node_variable.end()));
      CHECK_ITERABLE_APPROX(
          static_cast<decltype(model) const&>(model)
              .slice<typename VCPV::VarOnNode>(0UL),
          DataVectorConstSlice(node_variable.begin(), node_variable.end()));

      auto& element_variable = model.get_element_variable();
      CHECK_ITERABLE_APPROX(
          model.slice<typename VCPV::VarOnElement>(0UL),
          DataVectorSlice(element_variable.begin(), element_variable.end()));
      CHECK_ITERABLE_APPROX(static_cast<decltype(model) const&>(model)
                                .slice<typename VCPV::VarOnElement>(0UL),
                            DataVectorConstSlice(element_variable.begin(),
                                                 element_variable.end()));

      auto& voronoi_variable = model.get_voronoi_variable();
      CHECK_ITERABLE_APPROX(
          model.slice<typename VCPV::VarOnVoronoi>(0UL),
          DataVectorSlice(voronoi_variable.begin(), voronoi_variable.end()));
      CHECK_ITERABLE_APPROX(static_cast<decltype(model) const&>(model)
                                .slice<typename VCPV::VarOnVoronoi>(0UL),
                            DataVectorConstSlice(voronoi_variable.begin(),
                                                 voronoi_variable.end()));

      // 5. check for out of range throws
      CHECK(model.check(0UL) == 0UL);
      CHECK(model.check(end - 1UL) == 0UL);

      CHECK_THROWS_AS(model.check(1UL), std::out_of_range);
      CHECK_THROWS_AS(model.check(end - 2UL), std::out_of_range);

      CHECK_THROWS_WITH(model.check(1UL), Catch::Contains("Index to be "));
      CHECK_THROWS_WITH(model.check(end - 2UL),
                        Catch::Contains("Index to be "));

      // B. check the sliced model
      // 1.check the element sizes
      CHECK(sliced_model.total_n_elements() == new_n_elements);
      CHECK(sliced_model.total_n_nodes() == new_n_elements + 1UL);
      CHECK(sliced_model.total_n_voronois() == new_n_elements - 1UL);
      CHECK(sliced_model.size() == new_n_elements);
      CHECK(not sliced_model.empty());

      // 2. check dofs
      CHECK(sliced_model.check(0UL) == 0UL);
      CHECK(sliced_model.check(end - 1UL) == new_n_elements - 1UL);

      CHECK_THROWS_AS(sliced_model.check(new_n_elements), std::out_of_range);
      CHECK_THROWS_AS(sliced_model.check(end - new_n_elements - 1UL),
                      std::out_of_range);

      CHECK_THROWS_WITH(sliced_model.check(new_n_elements),
                        Catch::Contains("Index to be "));
      CHECK_THROWS_WITH(sliced_model.check(end - new_n_elements - 1UL),
                        Catch::Contains("Index to be "));

      // 3. check the new members
      CHECK(sliced_model.tell_me_number() == 42);
      CHECK_ITERABLE_APPROX(sliced_model.get_node_variable(),
                            DataVectorSlice(expected_node_variable.begin(),
                                            expected_node_variable.end()));
      CHECK_ITERABLE_APPROX(sliced_model.get_element_variable(),
                            DataVectorSlice(expected_element_variable.begin(),
                                            expected_element_variable.end()));
      CHECK_ITERABLE_APPROX(sliced_model.get_voronoi_variable(),
                            DataVectorSlice(expected_voronoi_variable.begin(),
                                            expected_voronoi_variable.end()));

      // C. check the const sliced model
      auto const_sliced_model =
          blocks::slice(static_cast<decltype(model) const&>(model), 0UL);
      // 1.check the element sizes
      CHECK(const_sliced_model.total_n_elements() == new_n_elements);
      CHECK(const_sliced_model.total_n_nodes() == new_n_elements + 1UL);
      CHECK(const_sliced_model.total_n_voronois() == new_n_elements - 1UL);
      CHECK(const_sliced_model.size() == new_n_elements);
      CHECK(not const_sliced_model.empty());

      // 2. check dofs
      CHECK(const_sliced_model.check(0UL) == 0UL);
      CHECK(const_sliced_model.check(end - 1UL) == new_n_elements - 1UL);

      CHECK_THROWS_AS(const_sliced_model.check(new_n_elements),
                      std::out_of_range);
      CHECK_THROWS_AS(const_sliced_model.check(end - new_n_elements - 1UL),
                      std::out_of_range);

      CHECK_THROWS_WITH(const_sliced_model.check(new_n_elements),
                        Catch::Contains("Index to be "));
      CHECK_THROWS_WITH(const_sliced_model.check(end - new_n_elements - 1UL),
                        Catch::Contains("Index to be "));

      // 3. check the new members
      CHECK(const_sliced_model.tell_me_number() == 42);
      CHECK_ITERABLE_APPROX(const_sliced_model.get_node_variable(),
                            DataVectorConstSlice(expected_node_variable.begin(),
                                                 expected_node_variable.end()));
      CHECK_ITERABLE_APPROX(
          const_sliced_model.get_element_variable(),
          DataVectorConstSlice(expected_element_variable.begin(),
                               expected_element_variable.end()));
      CHECK_ITERABLE_APPROX(
          const_sliced_model.get_voronoi_variable(),
          DataVectorConstSlice(expected_voronoi_variable.begin(),
                               expected_voronoi_variable.end()));

      return model;
    };

    // after one initializer
    {
      std::size_t first_n_elements(5UL);

      auto model(test_one_initializer(first_n_elements));

      const std::size_t new_n_elements = 10UL;

      // 1. check resizing logic in block
      auto total_n_elements_after_resizing =
          model.prepare_for_resize(new_n_elements);
      CHECK(total_n_elements_after_resizing == (5UL + 2UL + 10UL));
      CHECK(model.get_ghost_node_buffer() == GhostNodeIndices{5UL + 1UL});

      CHECK(model.n_units() == 1UL);
      model.resize_members_on_rod(2UL);
      CHECK(model.n_units() == 2UL);

      // 2. check start and size of slices

      // 2.1 First check elements
      CHECK(model.get_start_of_slice<typename CRPV::NElement::Stagger>(0UL) ==
            0UL);
      CHECK(model.get_size_of_slice<typename CRPV::NElement::Stagger>(0UL) ==
            1UL);
      CHECK(model.get_start_of_slice<typename CRPV::NElement::Stagger>(1UL) ==
            1UL);
      CHECK(model.get_size_of_slice<typename CRPV::NElement::Stagger>(1UL) ==
            1UL);

      // this is to make "slice" work, see below
      model.initialize_elements_buffer(new_n_elements, 1UL);

      auto second_n_elements = model.n_elements(1UL);
      CHECK(second_n_elements == new_n_elements);

      // 3.1 Check slices
      CHECK(model.slice<typename CRPV::NElement>(0UL) ==
            ElementsVectorSlice{first_n_elements});
      CHECK(static_cast<decltype(model) const&>(model)
                .slice<typename CRPV::NElement>(0UL) ==
            ElementsVectorConstSlice{first_n_elements});
      CHECK(model.slice<typename CRPV::NElement>(1UL) ==
            ElementsVectorSlice{second_n_elements});
      CHECK(static_cast<decltype(model) const&>(model)
                .slice<typename CRPV::NElement>(1UL) ==
            ElementsVectorConstSlice{second_n_elements});

      // 1.2 now we can check logic for resizing members on different parts
      {
        CHECK(model.get_node_variable().size() == (first_n_elements + 1UL));
        CHECK(model.get_element_variable().size() == (first_n_elements));
        CHECK(model.get_voronoi_variable().size() == (first_n_elements - 1UL));

        model.resize_members_not_on_rod(total_n_elements_after_resizing);

        CHECK(model.get_node_variable().size() ==
              ((first_n_elements + 1UL) + 1UL + (second_n_elements + 1UL)));
        CHECK(model.get_element_variable().size() ==
              (first_n_elements + 2UL + second_n_elements));
        CHECK(model.get_voronoi_variable().size() ==
              ((first_n_elements - 1UL) + 3UL + (second_n_elements - 1UL)));
      }

      // 2.2
      // doesn't work off the shelf because .slice calls the get_size_of_slice
      // function which needs the degrees of freedom, and hence doesnt work if
      // n_elements = 0. So we need to populate it with n_elements first
      {
        {
          CHECK(model.get_start_of_slice<typename VCPV::VarOnNode::Stagger>(
                    0UL) == 0UL);
          CHECK(model.get_size_of_slice<typename VCPV::VarOnNode::Stagger>(
                    0UL) == (first_n_elements + 1UL));
          CHECK(model.get_start_of_slice<typename VCPV::VarOnNode::Stagger>(
                    1UL) == (first_n_elements + 2UL));
          CHECK(model.get_size_of_slice<typename VCPV::VarOnNode::Stagger>(
                    1UL) == (new_n_elements + 1UL));
        }
        {
          CHECK(model.get_start_of_slice<typename VCPV::VarOnElement::Stagger>(
                    0UL) == 0UL);
          CHECK(model.get_size_of_slice<typename VCPV::VarOnElement::Stagger>(
                    0UL) == (first_n_elements));
          CHECK(model.get_start_of_slice<typename VCPV::VarOnElement::Stagger>(
                    1UL) == (first_n_elements + 2UL));
          CHECK(model.get_size_of_slice<typename VCPV::VarOnElement::Stagger>(
                    1UL) == (new_n_elements));
        }
        {
          CHECK(model.get_start_of_slice<typename VCPV::VarOnVoronoi::Stagger>(
                    0UL) == 0UL);
          CHECK(model.get_size_of_slice<typename VCPV::VarOnVoronoi::Stagger>(
                    0UL) == (first_n_elements - 1UL));
          CHECK(model.get_start_of_slice<typename VCPV::VarOnVoronoi::Stagger>(
                    1UL) == (first_n_elements + 2UL));
          CHECK(model.get_size_of_slice<typename VCPV::VarOnVoronoi::Stagger>(
                    1UL) == (new_n_elements - 1UL));
        }
      }
      // 3.2
      CHECK(model.tell_me_number() == 42);

      // check individual members along with ghosting
      {
        auto& node_variable = model.get_node_variable();
        const std::size_t end_idx = first_n_elements + 1UL;
        CHECK_ITERABLE_APPROX(
            model.slice<typename VCPV::VarOnNode>(0UL),
            DataVectorSlice(&node_variable[0UL], &node_variable[end_idx]));
        // offset of 1 here to account fo the ghost
        CHECK_ITERABLE_APPROX(
            model.slice<typename VCPV::VarOnNode>(1UL),
            DataVectorSlice(&node_variable[end_idx + 1UL],
                            &node_variable[node_variable.size()]));
        CHECK_ITERABLE_APPROX(
            static_cast<decltype(model) const&>(model)
                .slice<typename VCPV::VarOnNode>(0UL),
            DataVectorConstSlice(&node_variable[0UL], &node_variable[end_idx]));
        CHECK_ITERABLE_APPROX(
            static_cast<decltype(model) const&>(model)
                .slice<typename VCPV::VarOnNode>(1UL),
            DataVectorConstSlice(&node_variable[end_idx + 1UL],
                                 &node_variable[node_variable.size()]));

        CHECK(node_variable[end_idx] != approx(3.14159));
        model.fill_ghosts_placed<typename Model::OnNode>();
        // the part between rod 1 and 2 should be zerod
        // in this case we explicitly request the node to be non-default, say
        //
        CHECK(node_variable[end_idx] == approx(3.14159));
      }

      {
        auto& element_variable = model.get_element_variable();
        const std::size_t end_idx = first_n_elements;
        CHECK_ITERABLE_APPROX(model.slice<typename VCPV::VarOnElement>(0UL),
                              DataVectorSlice(&element_variable[0UL],
                                              &element_variable[end_idx]));
        CHECK_ITERABLE_APPROX(
            model.slice<typename VCPV::VarOnElement>(1UL),
            DataVectorSlice(&element_variable[end_idx + 2UL],
                            &element_variable[element_variable.size()]));
        CHECK_ITERABLE_APPROX(static_cast<decltype(model) const&>(model)
                                  .slice<typename VCPV::VarOnElement>(0UL),
                              DataVectorConstSlice(&element_variable[0UL],
                                                   &element_variable[end_idx]));
        CHECK_ITERABLE_APPROX(
            static_cast<decltype(model) const&>(model)
                .slice<typename VCPV::VarOnElement>(1UL),
            DataVectorConstSlice(&element_variable[end_idx + 2UL],
                                 &element_variable[element_variable.size()]));

        CHECK(element_variable[end_idx] != approx(3.14159));
        CHECK(element_variable[end_idx + 1UL] != approx(3.14159));
        model.fill_ghosts_placed<typename Model::OnElement>();
        CHECK(element_variable[end_idx] == approx(3.14159));
        CHECK(element_variable[end_idx + 1UL] == approx(3.14159));
      }

      {
        auto& voronoi_variable = model.get_voronoi_variable();
        const std::size_t end_idx = first_n_elements - 1UL;
        CHECK_ITERABLE_APPROX(model.slice<typename VCPV::VarOnVoronoi>(0UL),
                              DataVectorSlice(&voronoi_variable[0UL],
                                              &voronoi_variable[end_idx]));
        CHECK_ITERABLE_APPROX(
            model.slice<typename VCPV::VarOnVoronoi>(1UL),
            DataVectorSlice(&voronoi_variable[end_idx + 3UL],
                            &voronoi_variable[voronoi_variable.size()]));

        CHECK_ITERABLE_APPROX(static_cast<decltype(model) const&>(model)
                                  .slice<typename VCPV::VarOnVoronoi>(0UL),
                              DataVectorConstSlice(&voronoi_variable[0UL],
                                                   &voronoi_variable[end_idx]));
        CHECK_ITERABLE_APPROX(
            static_cast<decltype(model) const&>(model)
                .slice<typename VCPV::VarOnVoronoi>(1UL),
            DataVectorConstSlice(&voronoi_variable[end_idx + 3UL],
                                 &voronoi_variable[voronoi_variable.size()]));

        CHECK(voronoi_variable[end_idx] != approx(3.14159));
        CHECK(voronoi_variable[end_idx + 1UL] != approx(3.14159));
        CHECK(voronoi_variable[end_idx + 2UL] != approx(3.14159));
        model.fill_ghosts_placed<typename Model::OnVoronoi>();
        CHECK(voronoi_variable[end_idx] == approx(3.14159));
        CHECK(voronoi_variable[end_idx + 1UL] == approx(3.14159));
        CHECK(voronoi_variable[end_idx + 2UL] == approx(3.14159));
      }

    }  // end after one initializer

    auto test_two_initializer = [test_one_initializer](
                                    std::size_t first_n_elements,
                                    std::size_t new_n_elements) {
      auto model = test_one_initializer(first_n_elements);

      auto initializer(cosserat_rod::initialize_cosserat_rod<TestPlugin>(
          blocks::initialize<tags::NElement>(new_n_elements),
          blocks::initialize<test_tags::VarOnNode>(
              [](std::size_t) { return 1.0; }),
          blocks::initialize<test_tags::VarOnElement>(
              [](std::size_t) { return 2.0; }),
          blocks::initialize<test_tags::VarOnVoronoi>(
              [](std::size_t) { return 3.0; })));

      auto sliced_model = model.emplace(std::move(initializer));
      static_assert(cpp17::is_same_v<decltype(sliced_model), ExpectedSlice>,
                    "Block slice failure with Cosserat Rods!");

      // A. Base model
      // 1. check the sizes
      CHECK(model.total_n_elements() ==
            first_n_elements + 2UL + new_n_elements);
      CHECK(model.total_n_nodes() ==
            ((first_n_elements + 1UL) + 1UL + (new_n_elements + 1UL)));
      CHECK(model.total_n_voronois() ==
            ((first_n_elements - 1UL) + 3UL + (new_n_elements - 1UL)));
      CHECK(model.n_units() == 2UL);

      CHECK(model.n_elements() ==
            ElementsVector{first_n_elements, new_n_elements});
      CHECK(model.size() == (first_n_elements + 2UL + new_n_elements));
      CHECK(model.get_ghost_node_buffer() ==
            GhostNodeIndices{first_n_elements + 1UL});
      CHECK(not model.empty());

      // 2. check the members in the base model
      CHECK(model.tell_me_number() == 42);

      // TODO : need to check members in the base model

      // 3.
      // block check for first emplacement
      // cannot test it earlier as size_of_slice calls to assert
      CHECK(model.get_start_of_slice<typename CRPV::NElement::Stagger>(0UL) ==
            0UL);
      CHECK(model.get_size_of_slice<typename CRPV::NElement::Stagger>(0UL) ==
            1UL);
      CHECK(model.get_start_of_slice<typename CRPV::NElement::Stagger>(1UL) ==
            1UL);
      CHECK(model.get_size_of_slice<typename CRPV::NElement::Stagger>(1UL) ==
            1UL);
      CHECK(model.get_start_of_slice<typename VCPV::VarOnNode::Stagger>(0UL) ==
            0UL);
      CHECK(model.get_size_of_slice<typename VCPV::VarOnNode::Stagger>(0UL) ==
            (first_n_elements + 1UL));
      CHECK(model.get_start_of_slice<typename VCPV::VarOnNode::Stagger>(1UL) ==
            (first_n_elements + 2UL));
      CHECK(model.get_size_of_slice<typename VCPV::VarOnNode::Stagger>(1UL) ==
            (new_n_elements + 1UL));
      CHECK(model.get_start_of_slice<typename VCPV::VarOnElement::Stagger>(
                0UL) == 0UL);
      CHECK(model.get_size_of_slice<typename VCPV::VarOnElement::Stagger>(
                0UL) == (first_n_elements));
      CHECK(model.get_start_of_slice<typename VCPV::VarOnElement::Stagger>(
                1UL) == (first_n_elements + 2UL));
      CHECK(model.get_size_of_slice<typename VCPV::VarOnElement::Stagger>(
                1UL) == (new_n_elements));
      CHECK(model.get_start_of_slice<typename VCPV::VarOnVoronoi::Stagger>(
                0UL) == 0UL);
      CHECK(model.get_size_of_slice<typename VCPV::VarOnVoronoi::Stagger>(
                0UL) == (first_n_elements - 1UL));
      CHECK(model.get_start_of_slice<typename VCPV::VarOnVoronoi::Stagger>(
                1UL) == (first_n_elements + 2UL));
      CHECK(model.get_size_of_slice<typename VCPV::VarOnVoronoi::Stagger>(
                1UL) == (new_n_elements - 1UL));

      // 4. check for slices (should be fine since its initialized, but check
      // anyways)
      CHECK(model.slice<typename CRPV::NElement>(0UL) ==
            ElementsVectorSlice{first_n_elements});
      CHECK(static_cast<decltype(model) const&>(model)
                .slice<typename CRPV::NElement>(0UL) ==
            ElementsVectorConstSlice{first_n_elements});
      CHECK(model.slice<typename CRPV::NElement>(1UL) ==
            ElementsVectorSlice{new_n_elements});
      CHECK(static_cast<decltype(model) const&>(model)
                .slice<typename CRPV::NElement>(1UL) ==
            ElementsVectorConstSlice{new_n_elements});

      // check individual members along with ghosting
      {
        auto& node_variable = model.get_node_variable();
        const std::size_t end_idx = first_n_elements + 1UL;
        CHECK_ITERABLE_APPROX(
            model.slice<typename VCPV::VarOnNode>(0UL),
            DataVectorSlice(&node_variable[0UL], &node_variable[end_idx]));
        // offset of 1 here to account fo the ghost
        CHECK_ITERABLE_APPROX(
            model.slice<typename VCPV::VarOnNode>(1UL),
            DataVectorSlice(&node_variable[end_idx + 1UL],
                            &node_variable[node_variable.size()]));
        CHECK_ITERABLE_APPROX(
            static_cast<decltype(model) const&>(model)
                .slice<typename VCPV::VarOnNode>(0UL),
            DataVectorConstSlice(&node_variable[0UL], &node_variable[end_idx]));
        CHECK_ITERABLE_APPROX(
            static_cast<decltype(model) const&>(model)
                .slice<typename VCPV::VarOnNode>(1UL),
            DataVectorConstSlice(&node_variable[end_idx + 1UL],
                                 &node_variable[node_variable.size()]));

        // the part between rod 1 and 2 should be zerod
        // in this case we explicitly request the node to be non-default, say
        //
        CHECK(node_variable[end_idx] == approx(3.14159));
      }

      {
        auto& element_variable = model.get_element_variable();
        const std::size_t end_idx = first_n_elements;
        CHECK_ITERABLE_APPROX(model.slice<typename VCPV::VarOnElement>(0UL),
                              DataVectorSlice(&element_variable[0UL],
                                              &element_variable[end_idx]));
        CHECK_ITERABLE_APPROX(
            model.slice<typename VCPV::VarOnElement>(1UL),
            DataVectorSlice(&element_variable[end_idx + 2UL],
                            &element_variable[element_variable.size()]));
        CHECK_ITERABLE_APPROX(static_cast<decltype(model) const&>(model)
                                  .slice<typename VCPV::VarOnElement>(0UL),
                              DataVectorConstSlice(&element_variable[0UL],
                                                   &element_variable[end_idx]));
        CHECK_ITERABLE_APPROX(
            static_cast<decltype(model) const&>(model)
                .slice<typename VCPV::VarOnElement>(1UL),
            DataVectorConstSlice(&element_variable[end_idx + 2UL],
                                 &element_variable[element_variable.size()]));

        CHECK(element_variable[end_idx] == approx(3.14159));
        CHECK(element_variable[end_idx + 1UL] == approx(3.14159));
      }

      {
        auto& voronoi_variable = model.get_voronoi_variable();
        const std::size_t end_idx = first_n_elements - 1UL;
        CHECK_ITERABLE_APPROX(model.slice<typename VCPV::VarOnVoronoi>(0UL),
                              DataVectorSlice(&voronoi_variable[0UL],
                                              &voronoi_variable[end_idx]));
        CHECK_ITERABLE_APPROX(
            model.slice<typename VCPV::VarOnVoronoi>(1UL),
            DataVectorSlice(&voronoi_variable[end_idx + 3UL],
                            &voronoi_variable[voronoi_variable.size()]));

        CHECK_ITERABLE_APPROX(static_cast<decltype(model) const&>(model)
                                  .slice<typename VCPV::VarOnVoronoi>(0UL),
                              DataVectorConstSlice(&voronoi_variable[0UL],
                                                   &voronoi_variable[end_idx]));
        CHECK_ITERABLE_APPROX(
            static_cast<decltype(model) const&>(model)
                .slice<typename VCPV::VarOnVoronoi>(1UL),
            DataVectorConstSlice(&voronoi_variable[end_idx + 3UL],
                                 &voronoi_variable[voronoi_variable.size()]));

        CHECK(voronoi_variable[end_idx] == approx(3.14159));
        CHECK(voronoi_variable[end_idx + 1UL] == approx(3.14159));
        CHECK(voronoi_variable[end_idx + 2UL] == approx(3.14159));
      }

      // 5. check for out of range throws
      CHECK(model.check(0UL) == 0UL);
      CHECK(model.check(1UL) == 1UL);
      CHECK(model.check(end - 1UL) == 1UL);
      CHECK(model.check(end - 2UL) == 0UL);

      CHECK_THROWS_AS(model.check(2UL), std::out_of_range);
      CHECK_THROWS_AS(model.check(end - 3UL), std::out_of_range);

      CHECK_THROWS_WITH(model.check(2UL), Catch::Contains("Index to be "));
      CHECK_THROWS_WITH(model.check(end - 3UL),
                        Catch::Contains("Index to be "));

      // B. check the sliced model
      {
        // 1.check the element sizes
        CHECK(sliced_model.total_n_elements() == new_n_elements);
        CHECK(sliced_model.total_n_nodes() == new_n_elements + 1UL);
        CHECK(sliced_model.total_n_voronois() == new_n_elements - 1UL);
        CHECK(sliced_model.size() == new_n_elements);
        CHECK(not sliced_model.empty());

        // 2. check dofs for out of range
        CHECK(sliced_model.check(0UL) == 0UL);
        CHECK(sliced_model.check(end - 1UL) == new_n_elements - 1UL);

        CHECK_THROWS_AS(sliced_model.check(new_n_elements), std::out_of_range);
        CHECK_THROWS_AS(sliced_model.check(end - new_n_elements - 1UL),
                        std::out_of_range);

        CHECK_THROWS_WITH(sliced_model.check(new_n_elements),
                          Catch::Contains("Index to be "));
        CHECK_THROWS_WITH(sliced_model.check(end - new_n_elements - 1UL),
                          Catch::Contains("Index to be "));

        // 3. check the new members
        DataVector expected_node_variable(sliced_model.total_n_nodes());
        std::fill(expected_node_variable.begin(), expected_node_variable.end(),
                  1.0);
        DataVector expected_element_variable(sliced_model.total_n_elements());
        std::fill(expected_element_variable.begin(),
                  expected_element_variable.end(), 2.0);
        DataVector expected_voronoi_variable(sliced_model.total_n_voronois());
        std::fill(expected_voronoi_variable.begin(),
                  expected_voronoi_variable.end(), 3.0);

        CHECK(sliced_model.tell_me_number() == 42);
        CHECK_ITERABLE_APPROX(sliced_model.get_node_variable(),
                              DataVectorSlice(expected_node_variable.begin(),
                                              expected_node_variable.end()));
        CHECK_ITERABLE_APPROX(sliced_model.get_element_variable(),
                              DataVectorSlice(expected_element_variable.begin(),
                                              expected_element_variable.end()));
        CHECK_ITERABLE_APPROX(sliced_model.get_voronoi_variable(),
                              DataVectorSlice(expected_voronoi_variable.begin(),
                                              expected_voronoi_variable.end()));

        // C. check the const sliced model
        auto const_sliced_model = blocks::slice(
            static_cast<decltype(model) const&>(model), end - 1UL);
        // 1.check the element sizes
        CHECK(const_sliced_model.total_n_elements() == new_n_elements);
        CHECK(const_sliced_model.total_n_nodes() == new_n_elements + 1UL);
        CHECK(const_sliced_model.total_n_voronois() == new_n_elements - 1UL);
        CHECK(const_sliced_model.size() == new_n_elements);
        CHECK(not const_sliced_model.empty());

        // 2. check dofs for out of range
        CHECK(const_sliced_model.check(0UL) == 0UL);
        CHECK(const_sliced_model.check(end - 1UL) == new_n_elements - 1UL);

        CHECK_THROWS_AS(const_sliced_model.check(new_n_elements),
                        std::out_of_range);
        CHECK_THROWS_AS(const_sliced_model.check(end - new_n_elements - 1UL),
                        std::out_of_range);

        CHECK_THROWS_WITH(const_sliced_model.check(new_n_elements),
                          Catch::Contains("Index to be "));
        CHECK_THROWS_WITH(const_sliced_model.check(end - new_n_elements - 1UL),
                          Catch::Contains("Index to be "));

        // 3. check the new members
        CHECK(const_sliced_model.tell_me_number() == 42);
        CHECK_ITERABLE_APPROX(
            const_sliced_model.get_node_variable(),
            DataVectorConstSlice(expected_node_variable.begin(),
                                 expected_node_variable.end()));
        CHECK_ITERABLE_APPROX(
            const_sliced_model.get_element_variable(),
            DataVectorConstSlice(expected_element_variable.begin(),
                                 expected_element_variable.end()));
        CHECK_ITERABLE_APPROX(
            const_sliced_model.get_voronoi_variable(),
            DataVectorConstSlice(expected_voronoi_variable.begin(),
                                 expected_voronoi_variable.end()));
      }

      return model;
    };

    // after two initializers
    {
      std::size_t first_n_elements(10UL);
      std::size_t second_n_elements(20UL);
      auto model(test_two_initializer(first_n_elements, second_n_elements));

      const std::size_t new_n_elements = 30UL;

      // 1. check resizing logic in block
      auto total_n_elements_after_resizing =
          model.prepare_for_resize(new_n_elements);
      CHECK(total_n_elements_after_resizing ==
            (10UL + 2UL + 20UL + 2UL + 30UL));
      CHECK(model.get_ghost_node_buffer() ==
            GhostNodeIndices{
                10UL + 1UL, (10UL + 1UL)        // nodes in first
                                + (20UL + 1UL)  // noeds is second
                                + 1UL           // offset because of previous
            });

      CHECK(model.n_units() == 2UL);
      model.resize_members_on_rod(3UL);
      CHECK(model.n_units() == 3UL);

      // 2. check start and size of slices

      // 2.1 First check elements
      CHECK(model.get_start_of_slice<typename CRPV::NElement::Stagger>(0UL) ==
            0UL);
      CHECK(model.get_size_of_slice<typename CRPV::NElement::Stagger>(0UL) ==
            1UL);
      CHECK(model.get_start_of_slice<typename CRPV::NElement::Stagger>(1UL) ==
            1UL);
      CHECK(model.get_size_of_slice<typename CRPV::NElement::Stagger>(1UL) ==
            1UL);
      CHECK(model.get_start_of_slice<typename CRPV::NElement::Stagger>(2UL) ==
            2UL);
      CHECK(model.get_size_of_slice<typename CRPV::NElement::Stagger>(2UL) ==
            1UL);

      // this is to make "slice" work, see below
      model.initialize_elements_buffer(new_n_elements, 2UL);

      auto third_n_elements = model.n_elements(2UL);
      CHECK(third_n_elements == new_n_elements);

      // 3.1 Check slices
      CHECK(model.slice<typename CRPV::NElement>(0UL) ==
            ElementsVectorSlice{first_n_elements});
      CHECK(static_cast<decltype(model) const&>(model)
                .slice<typename CRPV::NElement>(0UL) ==
            ElementsVectorConstSlice{first_n_elements});
      CHECK(model.slice<typename CRPV::NElement>(1UL) ==
            ElementsVectorSlice{second_n_elements});
      CHECK(static_cast<decltype(model) const&>(model)
                .slice<typename CRPV::NElement>(1UL) ==
            ElementsVectorConstSlice{second_n_elements});
      CHECK(model.slice<typename CRPV::NElement>(2UL) ==
            ElementsVectorSlice{third_n_elements});
      CHECK(static_cast<decltype(model) const&>(model)
                .slice<typename CRPV::NElement>(2UL) ==
            ElementsVectorConstSlice{third_n_elements});

      // 1.2 now we can check logic for resizing members on different parts
      {
        CHECK(model.get_node_variable().size() ==
              ((first_n_elements + 1UL) + 1UL + (second_n_elements + 1UL)));
        CHECK(model.get_element_variable().size() ==
              ((first_n_elements) + 2UL + (second_n_elements)));
        CHECK(model.get_voronoi_variable().size() ==
              ((first_n_elements - 1UL) + 3UL + (second_n_elements - 1UL)));

        model.resize_members_not_on_rod(total_n_elements_after_resizing);

        CHECK(model.get_node_variable().size() ==
              ((first_n_elements + 1UL) + 1UL + (second_n_elements + 1UL) +
               1UL + (third_n_elements + 1UL)));
        CHECK(model.get_element_variable().size() ==
              (first_n_elements + 2UL + second_n_elements + 2UL +
               third_n_elements));
        CHECK(model.get_voronoi_variable().size() ==
              ((first_n_elements - 1UL) + 3UL + (second_n_elements - 1UL) +
               3UL + (third_n_elements - 1UL)));
      }

      // 2.2
      // doesn't work off the shelf because .slice calls the get_size_of_slice
      // function which needs the degrees of freedom, and hence doesnt work if
      // n_elements = 0. So we need to populate it with n_elements first
      {
        {
          CHECK(model.get_start_of_slice<typename VCPV::VarOnNode::Stagger>(
                    0UL) == 0UL);
          CHECK(model.get_size_of_slice<typename VCPV::VarOnNode::Stagger>(
                    0UL) == (first_n_elements + 1UL));
          CHECK(model.get_start_of_slice<typename VCPV::VarOnNode::Stagger>(
                    1UL) == (first_n_elements + 2UL));
          CHECK(model.get_size_of_slice<typename VCPV::VarOnNode::Stagger>(
                    1UL) == (second_n_elements + 1UL));
          CHECK(model.get_start_of_slice<typename VCPV::VarOnNode::Stagger>(
                    2UL) == (first_n_elements + 1UL + 1UL) +
                                (second_n_elements + 1UL + 1UL));
          CHECK(model.get_size_of_slice<typename VCPV::VarOnNode::Stagger>(
                    2UL) == (new_n_elements + 1UL));
        }
        {
          CHECK(model.get_start_of_slice<typename VCPV::VarOnElement::Stagger>(
                    0UL) == 0UL);
          CHECK(model.get_size_of_slice<typename VCPV::VarOnElement::Stagger>(
                    0UL) == (first_n_elements));
          CHECK(model.get_start_of_slice<typename VCPV::VarOnElement::Stagger>(
                    1UL) == (first_n_elements + 2UL));
          CHECK(model.get_size_of_slice<typename VCPV::VarOnElement::Stagger>(
                    1UL) == (second_n_elements));
          CHECK(model.get_start_of_slice<typename VCPV::VarOnElement::Stagger>(
                    2UL) ==
                (first_n_elements + 2UL) + (second_n_elements + 2UL));
          CHECK(model.get_size_of_slice<typename VCPV::VarOnElement::Stagger>(
                    2UL) == (new_n_elements));
        }
        {
          CHECK(model.get_start_of_slice<typename VCPV::VarOnVoronoi::Stagger>(
                    0UL) == 0UL);
          CHECK(model.get_size_of_slice<typename VCPV::VarOnVoronoi::Stagger>(
                    0UL) == (first_n_elements - 1UL));
          CHECK(model.get_start_of_slice<typename VCPV::VarOnVoronoi::Stagger>(
                    1UL) == (first_n_elements - 1UL + 3UL));
          CHECK(model.get_size_of_slice<typename VCPV::VarOnVoronoi::Stagger>(
                    1UL) == (second_n_elements - 1UL));
          CHECK(model.get_start_of_slice<typename VCPV::VarOnVoronoi::Stagger>(
                    2UL) == (first_n_elements - 1UL + 3UL) +
                                (second_n_elements - 1UL + 3UL));
          CHECK(model.get_size_of_slice<typename VCPV::VarOnVoronoi::Stagger>(
                    2UL) == (new_n_elements - 1UL));
        }
      }
      // 3.2
      CHECK(model.tell_me_number() == 42);

      // check individual members along with ghosting
      {
        auto& node_variable = model.get_node_variable();
        const std::size_t first_end_idx = first_n_elements + 1UL;
        const std::size_t second_end_idx =
            (first_n_elements + 1UL) + 1UL + (second_n_elements + 1UL);
        CHECK_ITERABLE_APPROX(model.slice<typename VCPV::VarOnNode>(0UL),
                              DataVectorSlice(&node_variable[0UL],
                                              &node_variable[first_end_idx]));
        CHECK_ITERABLE_APPROX(
            static_cast<decltype(model) const&>(model)
                .slice<typename VCPV::VarOnNode>(0UL),
            DataVectorConstSlice(&node_variable[0UL],
                                 &node_variable[first_end_idx]));
        // offset of 1 here to account fo the ghost
        CHECK_ITERABLE_APPROX(
            model.slice<typename VCPV::VarOnNode>(1UL),
            DataVectorSlice(&node_variable[first_end_idx + 1UL],
                            &node_variable[second_end_idx]));
        CHECK_ITERABLE_APPROX(
            static_cast<decltype(model) const&>(model)
                .slice<typename VCPV::VarOnNode>(1UL),
            DataVectorConstSlice(&node_variable[first_end_idx + 1UL],
                                 &node_variable[second_end_idx]));
        CHECK_ITERABLE_APPROX(
            model.slice<typename VCPV::VarOnNode>(2UL),
            DataVectorSlice(&node_variable[second_end_idx + 1UL],
                            &node_variable[node_variable.size()]));
        CHECK_ITERABLE_APPROX(
            static_cast<decltype(model) const&>(model)
                .slice<typename VCPV::VarOnNode>(2UL),
            DataVectorConstSlice(&node_variable[second_end_idx + 1UL],
                                 &node_variable[node_variable.size()]));

        CHECK(node_variable[first_end_idx] == approx(3.14159));
        CHECK(node_variable[second_end_idx] != approx(3.14159));

        model.fill_ghosts_placed<typename Model::OnNode>();
        // the part between rod 1 and 2 should be zerod
        // in this case we explicitly request the node to be non-default, say
        //
        CHECK(node_variable[first_end_idx] == approx(3.14159));
        CHECK(node_variable[second_end_idx] == approx(3.14159));
      }

      {
        auto& element_variable = model.get_element_variable();
        const std::size_t first_end_idx = first_n_elements;
        const std::size_t second_end_idx =
            first_n_elements + 2UL + second_n_elements;
        CHECK_ITERABLE_APPROX(
            model.slice<typename VCPV::VarOnElement>(0UL),
            DataVectorSlice(&element_variable[0UL],
                            &element_variable[first_end_idx]));
        CHECK_ITERABLE_APPROX(
            static_cast<decltype(model) const&>(model)
                .slice<typename VCPV::VarOnElement>(0UL),
            DataVectorConstSlice(&element_variable[0UL],
                                 &element_variable[first_end_idx]));
        CHECK_ITERABLE_APPROX(
            model.slice<typename VCPV::VarOnElement>(1UL),
            DataVectorSlice(&element_variable[first_end_idx + 2UL],
                            &element_variable[second_end_idx]));
        CHECK_ITERABLE_APPROX(
            static_cast<decltype(model) const&>(model)
                .slice<typename VCPV::VarOnElement>(1UL),
            DataVectorConstSlice(&element_variable[first_end_idx + 2UL],
                                 &element_variable[second_end_idx]));
        CHECK_ITERABLE_APPROX(
            model.slice<typename VCPV::VarOnElement>(2UL),
            DataVectorSlice(&element_variable[second_end_idx + 2UL],
                            &element_variable[element_variable.size()]));
        CHECK_ITERABLE_APPROX(
            static_cast<decltype(model) const&>(model)
                .slice<typename VCPV::VarOnElement>(2UL),
            DataVectorConstSlice(&element_variable[second_end_idx + 2UL],
                                 &element_variable[element_variable.size()]));

        CHECK(element_variable[first_end_idx] == approx(3.14159));
        CHECK(element_variable[first_end_idx + 1UL] == approx(3.14159));
        CHECK(element_variable[second_end_idx] != approx(3.14159));
        CHECK(element_variable[second_end_idx + 1UL] != approx(3.14159));
        model.fill_ghosts_placed<typename Model::OnElement>();
        CHECK(element_variable[first_end_idx] == approx(3.14159));
        CHECK(element_variable[first_end_idx + 1UL] == approx(3.14159));
        CHECK(element_variable[second_end_idx] == approx(3.14159));
        CHECK(element_variable[second_end_idx + 1UL] == approx(3.14159));
      }

      {
        auto& voronoi_variable = model.get_voronoi_variable();
        const std::size_t first_end_idx = first_n_elements - 1UL;
        const std::size_t second_end_idx =
            (first_n_elements - 1UL + 3UL) + second_n_elements - 1UL;
        CHECK_ITERABLE_APPROX(
            model.slice<typename VCPV::VarOnVoronoi>(0UL),
            DataVectorSlice(&voronoi_variable[0UL],
                            &voronoi_variable[first_end_idx]));
        CHECK_ITERABLE_APPROX(
            static_cast<decltype(model) const&>(model)
                .slice<typename VCPV::VarOnVoronoi>(0UL),
            DataVectorConstSlice(&voronoi_variable[0UL],
                                 &voronoi_variable[first_end_idx]));
        CHECK_ITERABLE_APPROX(
            model.slice<typename VCPV::VarOnVoronoi>(1UL),
            DataVectorSlice(&voronoi_variable[first_end_idx + 3UL],
                            &voronoi_variable[second_end_idx]));
        CHECK_ITERABLE_APPROX(
            static_cast<decltype(model) const&>(model)
                .slice<typename VCPV::VarOnVoronoi>(1UL),
            DataVectorConstSlice(&voronoi_variable[first_end_idx + 3UL],
                                 &voronoi_variable[second_end_idx]));
        CHECK_ITERABLE_APPROX(
            model.slice<typename VCPV::VarOnVoronoi>(2UL),
            DataVectorSlice(&voronoi_variable[second_end_idx + 3UL],
                            &voronoi_variable[voronoi_variable.size()]));
        CHECK_ITERABLE_APPROX(
            static_cast<decltype(model) const&>(model)
                .slice<typename VCPV::VarOnVoronoi>(2UL),
            DataVectorConstSlice(&voronoi_variable[second_end_idx + 3UL],
                                 &voronoi_variable[voronoi_variable.size()]));

        CHECK(voronoi_variable[first_end_idx] == approx(3.14159));
        CHECK(voronoi_variable[first_end_idx + 1UL] == approx(3.14159));
        CHECK(voronoi_variable[first_end_idx + 2UL] == approx(3.14159));
        CHECK(voronoi_variable[second_end_idx] != approx(3.14159));
        CHECK(voronoi_variable[second_end_idx + 1UL] != approx(3.14159));
        CHECK(voronoi_variable[second_end_idx + 2UL] != approx(3.14159));
        model.fill_ghosts_placed<typename Model::OnVoronoi>();
        CHECK(voronoi_variable[first_end_idx] == approx(3.14159));
        CHECK(voronoi_variable[first_end_idx + 1UL] == approx(3.14159));
        CHECK(voronoi_variable[first_end_idx + 2UL] == approx(3.14159));
        CHECK(voronoi_variable[second_end_idx] == approx(3.14159));
        CHECK(voronoi_variable[second_end_idx + 1UL] == approx(3.14159));
        CHECK(voronoi_variable[second_end_idx + 2UL] == approx(3.14159));
      }
    }
  }

}  // namespace

// begin testing CosseratRodPlugin with actual Traits of blaze

namespace {
  using ::elastica::cosserat_rod::DefaultCosseratRodTraits;

  template <template <typename, typename> class... T>
  using GenerateTestPluginDefaultTraits =
      CosseratRodPlugin<DefaultCosseratRodTraits, blocks::Block, T...>;

  template <template <typename, typename> class... T>
  using GenerateTestSlicePluginDefaultTraits =
      CosseratRodPlugin<DefaultCosseratRodTraits, blocks::BlockSlice, T...>;

  template <template <typename> class VariableDefinition>
  using DefaultVariablesIn =
      VariableCache<DefaultCosseratRodTraits>::V<VariableDefinition>;

}  // namespace

namespace testing {
  using TestVectorPluginDefaultTraits =
      GenerateTestPluginDefaultTraits<VectorComponent>;
  using VectorModelDefaultTraits = GenerateBlock<TestVectorPluginDefaultTraits>;

  // before running other tests see if there is no problem in type definitions
  // etc.
  using VectorModelDefaultTraitsAlias = decltype(VectorModelDefaultTraits{});
}  // namespace testing

namespace {

  ELASTICA_TEST_CASE(
      "Unit.Systems.CosseratRods.Block.with_default_traits.functions",
      "[Systems][Unit]") {
    using TestPlugin = GenerateTestPluginDefaultTraits<VectorComponent>;
    using Model = GenerateBlock<TestPlugin>;
    Model model{};
    using CRPV = DefaultVariablesIn<CosseratRodPluginVariables>;
    using VCPV = VectorComponentVariables<DefaultCosseratRodTraits>;

    static_assert(
        report_variable_difference<
            typename Model::Variables,
            tmpl::list<typename CRPV::NElement, typename VCPV::VarOnNode,
                       typename VCPV::VarOnElement,
                       typename VCPV::VarOnVoronoi>>,
        "Block metaprogramming failure!");
    static_assert(
        report_variable_difference<typename Model::VariablesOnNode,
                                   tmpl::list<typename VCPV::VarOnNode>>,
        "Block metaprogramming failure!");
    static_assert(
        report_variable_difference<typename Model::VariablesOnElement,
                                   tmpl::list<typename VCPV::VarOnElement>>,
        "Block metaprogramming failure!");
    static_assert(
        report_variable_difference<typename Model::VariablesOnVoronoi,
                                   tmpl::list<typename VCPV::VarOnVoronoi>>,
        "Block metaprogramming failure!");
    static_assert(
        report_variable_difference<typename Model::VariablesOnRod,
                                   tmpl::list<typename CRPV::NElement>>,
        "Block metaprogramming failure!");

    // check if members work for an empty block
    CHECK(model.empty());
    CHECK(model.tell_me_number() == 42);
    // cannot use total_n_elements() because there is an assert inside it
    // CHECK(model.total_n_elements() == 0UL);

    // block check
    CHECK_THROWS_AS(model.check(0UL), std::out_of_range);
    CHECK_THROWS_AS(model.check(end - 1UL), std::out_of_range);

    CHECK_THROWS_WITH(model.check(0UL), Catch::Contains("Index to be "));
    CHECK_THROWS_WITH(model.check(end - 1UL), Catch::Contains("Index to be "));
  }

  ELASTICA_TEST_CASE(
      "Unit.Systems.CosseratRods.Block.with_default_traits.initialization",
      "[Systems][Unit]") {
    using TestPlugin = GenerateTestPluginDefaultTraits<VectorComponent>;
    using Model = GenerateBlock<TestPlugin>;
    using CRPV = DefaultVariablesIn<CosseratRodPluginVariables>;
    using VCPV = VectorComponentVariables<DefaultCosseratRodTraits>;

    using ExpectedSlicePlugin =
        GenerateTestSlicePluginDefaultTraits<VectorComponent>;
    using ExpectedSlice = blocks::BlockSlice<ExpectedSlicePlugin>;

    using ElementsVector =
        typename DefaultCosseratRodTraits::DataType::Index::type;
    using DataVector =
        typename DefaultCosseratRodTraits::DataType::Vector::type;
    using ElementsVectorSlice =
        typename DefaultCosseratRodTraits::DataType::Index::slice_type;
    using ElementsVectorConstSlice =
        typename DefaultCosseratRodTraits::DataType::Index::const_slice_type;
    using DataVectorSlice =
        typename DefaultCosseratRodTraits::DataType::Vector::slice_type;
    using DataVectorConstSlice =
        typename DefaultCosseratRodTraits::DataType::Vector::const_slice_type;
    using GhostNodeIndices = typename Model::GhostNodeIndices;

    auto to_vec = [](auto&& blaze_struct) -> std::vector<double> {
      return std::vector<double>(
          blaze_struct.data(), blaze_struct.data() + blaze::size(blaze_struct));
    };

    auto test_empty = []() {
      Model model{};
      // check if members work for an empty block
      CHECK(model.n_units() == 0UL);
      CHECK(model.empty());

      // block check
      CHECK_THROWS_AS(model.check(0UL), std::out_of_range);
      CHECK_THROWS_AS(model.check(end - 1UL), std::out_of_range);

      CHECK_THROWS_WITH(model.check(0UL), Catch::Contains("Index to be "));
      CHECK_THROWS_WITH(model.check(end - 1UL),
                        Catch::Contains("Index to be "));

      return model;
    };

    // no state
    {
      auto model(test_empty());
      const std::size_t new_n_elements = 20UL;

      // 1. check resizing logic in block
      auto total_n_elements_after_resizing =
          model.prepare_for_resize(new_n_elements);
      CHECK(total_n_elements_after_resizing == 20UL);

      CHECK(model.empty());
      model.resize_members_on_rod(1UL);
      CHECK(model.n_units() == 1UL);

      // 2. check start and size of slices

      // 2.1 First check elements
      CHECK(model.get_start_of_slice<typename CRPV::NElement::Stagger>(0UL) ==
            0UL);
      CHECK(model.get_size_of_slice<typename CRPV::NElement::Stagger>(0UL) ==
            1UL);
      // this is to make "slice" work, see below
      model.initialize_elements_buffer(new_n_elements, 0UL);

      auto first_element = model.n_elements(0UL);
      CHECK(first_element == new_n_elements);

      // 3.1 Check slices
      CHECK(model.slice<typename CRPV::NElement>(0UL) ==
            ElementsVectorSlice{model.n_elements(), 0UL, 1UL});
      CHECK(static_cast<decltype(model) const&>(model)
                .slice<typename CRPV::NElement>(0UL) ==
            ElementsVectorConstSlice{model.n_elements(), 0UL, 1UL});

      // 1.2 now we can check logic for resizing members on different parts
      CHECK(blaze::isEmpty(model.get_node_variable()));
      CHECK(blaze::isEmpty(model.get_element_variable()));
      CHECK(blaze::isEmpty(model.get_voronoi_variable()));

      model.resize_members_not_on_rod(new_n_elements);

      CHECK(blaze::columns(model.get_node_variable()) ==
            (new_n_elements + 1UL));
      CHECK(blaze::columns(model.get_element_variable()) == (new_n_elements));
      CHECK(blaze::columns(model.get_voronoi_variable()) ==
            (new_n_elements - 1UL));

      // 2.2
      // doesn't work off the shelf because .slice calls the get_size_of_slice
      // function which needs the degrees of freedom, and hence doesnt work if
      // n_elements = 0. So we need to populate it with n_elements first
      CHECK(model.get_start_of_slice<typename VCPV::VarOnNode::Stagger>(0UL) ==
            0UL);
      CHECK(model.get_size_of_slice<typename VCPV::VarOnNode::Stagger>(0UL) ==
            (new_n_elements + 1UL));

      CHECK(model.get_start_of_slice<typename VCPV::VarOnElement::Stagger>(
                0UL) == 0UL);
      CHECK(model.get_size_of_slice<typename VCPV::VarOnElement::Stagger>(
                0UL) == (new_n_elements));

      CHECK(model.get_start_of_slice<typename VCPV::VarOnVoronoi::Stagger>(
                0UL) == 0UL);
      CHECK(model.get_size_of_slice<typename VCPV::VarOnVoronoi::Stagger>(
                0UL) == (new_n_elements - 1UL));

      // 3.2
      CHECK(model.tell_me_number() == 42);

      auto& node_variable = model.get_node_variable();
      const auto node_size = blaze::size(node_variable);
      ignore_unused(node_size);
      CHECK_ITERABLE_APPROX(
          to_vec(model.slice<typename VCPV::VarOnNode>(0UL)),
          to_vec(DataVectorSlice(node_variable, 0UL, 0UL, node_variable.rows(),
                                 node_variable.columns())));
      CHECK_ITERABLE_APPROX(to_vec(static_cast<decltype(model) const&>(model)
                                       .slice<typename VCPV::VarOnNode>(0UL)),
                            to_vec(DataVectorConstSlice(
                                node_variable, 0UL, 0UL, node_variable.rows(),
                                node_variable.columns())));

      auto& element_variable = model.get_element_variable();
      CHECK_ITERABLE_APPROX(
          to_vec(model.slice<typename VCPV::VarOnElement>(0UL)),
          to_vec(DataVectorSlice(element_variable, 0UL, 0UL,
                                 element_variable.rows(),
                                 element_variable.columns())));
      CHECK_ITERABLE_APPROX(
          to_vec(static_cast<decltype(model) const&>(model)
                     .slice<typename VCPV::VarOnElement>(0UL)),
          to_vec(DataVectorConstSlice(element_variable, 0UL, 0UL,
                                      element_variable.rows(),
                                      element_variable.columns())));

      auto& voronoi_variable = model.get_voronoi_variable();
      CHECK_ITERABLE_APPROX(
          to_vec(model.slice<typename VCPV::VarOnVoronoi>(0UL)),
          to_vec(DataVectorSlice(voronoi_variable, 0UL, 0UL,
                                 voronoi_variable.rows(),
                                 voronoi_variable.columns())));
      CHECK_ITERABLE_APPROX(
          to_vec(static_cast<decltype(model) const&>(model)
                     .slice<typename VCPV::VarOnVoronoi>(0UL)),
          to_vec(DataVectorConstSlice(voronoi_variable, 0UL, 0UL,
                                      voronoi_variable.rows(),
                                      voronoi_variable.columns())));
    }

    auto test_one_initializer = [test_empty,
                                 to_vec](std::size_t new_n_elements) {
      auto model = test_empty();

      auto initializer(cosserat_rod::initialize_cosserat_rod<TestPlugin>(
          blocks::initialize<tags::NElement>(new_n_elements),
          blocks::initialize<test_tags::VarOnNode>(
              [](std::size_t i) { return 3.0 + static_cast<double>(i); }),
          blocks::initialize<test_tags::VarOnElement>(
              [](std::size_t i) { return 2.0 + static_cast<double>(i); }),
          blocks::initialize<test_tags::VarOnVoronoi>(
              [](std::size_t i) { return 1.0 + static_cast<double>(i); })));

      auto sliced_model = model.emplace(std::move(initializer));
      static_assert(cpp17::is_same_v<decltype(sliced_model), ExpectedSlice>,
                    "Block slice failure with Cosserat Rods!");

      // A. Base model
      // 1. check the sizes
      CHECK(model.total_n_elements() == new_n_elements);
      CHECK(model.total_n_nodes() == new_n_elements + 1UL);
      CHECK(model.total_n_voronois() == new_n_elements - 1UL);
      CHECK(model.n_units() == 1UL);

      CHECK(model.n_elements() == ElementsVector{new_n_elements});
      CHECK(model.size() == new_n_elements);
      CHECK(not model.empty());

      // 2. check the members in the base model
      CHECK(model.tell_me_number() == 42);

      DataVector expected_node_variable(3UL, model.total_n_nodes());
      for (auto idx = 0UL; idx < 3UL; ++idx) {
        std::generate(expected_node_variable.begin(idx),
                      expected_node_variable.end(idx), [i = -1]() mutable {
                        ++i;
                        return 3.0 + static_cast<double>(i);
                      });
      }
      CHECK_ITERABLE_APPROX(to_vec(model.get_node_variable()),
                            to_vec(expected_node_variable));

      DataVector expected_element_variable(3UL, model.total_n_elements());
      for (auto idx = 0UL; idx < 3UL; ++idx) {
        std::generate(expected_element_variable.begin(idx),
                      expected_element_variable.end(idx), [i = -1]() mutable {
                        ++i;
                        return 2.0 + static_cast<double>(i);
                      });
      }
      CHECK_ITERABLE_APPROX(to_vec(model.get_element_variable()),
                            to_vec(expected_element_variable));

      DataVector expected_voronoi_variable(3UL, model.total_n_voronois());
      for (auto idx = 0UL; idx < 3UL; ++idx) {
        std::generate(expected_voronoi_variable.begin(idx),
                      expected_voronoi_variable.end(idx), [i = -1]() mutable {
                        ++i;
                        return 1.0 + static_cast<double>(i);
                      });
      }
      CHECK_ITERABLE_APPROX(to_vec(model.get_voronoi_variable()),
                            to_vec(expected_voronoi_variable));

      // 3.
      // block check for first emplacement
      // cannot test it earlier as size_of_slice calls to assert
      CHECK(model.get_start_of_slice<typename CRPV::NElement::Stagger>(0UL) ==
            0UL);
      CHECK(model.get_size_of_slice<typename CRPV::NElement::Stagger>(0UL) ==
            1UL);
      CHECK(model.get_start_of_slice<typename VCPV::VarOnNode::Stagger>(0UL) ==
            0UL);
      CHECK(model.get_size_of_slice<typename VCPV::VarOnNode::Stagger>(0UL) ==
            (new_n_elements + 1UL));
      CHECK(model.get_start_of_slice<typename VCPV::VarOnElement::Stagger>(
                0UL) == 0UL);
      CHECK(model.get_size_of_slice<typename VCPV::VarOnElement::Stagger>(
                0UL) == (new_n_elements));
      CHECK(model.get_start_of_slice<typename VCPV::VarOnVoronoi::Stagger>(
                0UL) == 0UL);
      CHECK(model.get_size_of_slice<typename VCPV::VarOnVoronoi::Stagger>(
                0UL) == (new_n_elements - 1UL));

      // 4. check for slices (should be fine since its initialized, but check
      // anyways)
      CHECK(model.slice<typename CRPV::NElement>(0UL) ==
            ElementsVectorSlice{model.n_elements(), 0UL, 1UL});
      CHECK(static_cast<decltype(model) const&>(model)
                .slice<typename CRPV::NElement>(0UL) ==
            ElementsVectorConstSlice{model.n_elements(), 0UL, 1UL});

      auto& node_variable = model.get_node_variable();
      const auto node_size = blaze::size(node_variable);
      ignore_unused(node_size);
      CHECK_ITERABLE_APPROX(
          to_vec(model.slice<typename VCPV::VarOnNode>(0UL)),
          to_vec(DataVectorSlice(node_variable, 0UL, 0UL, node_variable.rows(),
                                 node_variable.columns())));
      CHECK_ITERABLE_APPROX(to_vec(static_cast<decltype(model) const&>(model)
                                       .slice<typename VCPV::VarOnNode>(0UL)),
                            to_vec(DataVectorConstSlice(
                                node_variable, 0UL, 0UL, node_variable.rows(),
                                node_variable.columns())));

      auto& element_variable = model.get_element_variable();
      CHECK_ITERABLE_APPROX(
          to_vec(model.slice<typename VCPV::VarOnElement>(0UL)),
          to_vec(DataVectorSlice(element_variable, 0UL, 0UL,
                                 element_variable.rows(),
                                 element_variable.columns())));
      CHECK_ITERABLE_APPROX(
          to_vec(static_cast<decltype(model) const&>(model)
                     .slice<typename VCPV::VarOnElement>(0UL)),
          to_vec(DataVectorConstSlice(element_variable, 0UL, 0UL,
                                      element_variable.rows(),
                                      element_variable.columns())));

      auto& voronoi_variable = model.get_voronoi_variable();
      CHECK_ITERABLE_APPROX(
          to_vec(model.slice<typename VCPV::VarOnVoronoi>(0UL)),
          to_vec(DataVectorSlice(voronoi_variable, 0UL, 0UL,
                                 voronoi_variable.rows(),
                                 voronoi_variable.columns())));
      CHECK_ITERABLE_APPROX(
          to_vec(static_cast<decltype(model) const&>(model)
                     .slice<typename VCPV::VarOnVoronoi>(0UL)),
          to_vec(DataVectorConstSlice(voronoi_variable, 0UL, 0UL,
                                      voronoi_variable.rows(),
                                      voronoi_variable.columns())));

      // 5. check for out of range throws
      CHECK(model.check(0UL) == 0UL);
      CHECK(model.check(end - 1UL) == 0UL);

      CHECK_THROWS_AS(model.check(1UL), std::out_of_range);
      CHECK_THROWS_AS(model.check(end - 2UL), std::out_of_range);

      CHECK_THROWS_WITH(model.check(1UL), Catch::Contains("Index to be "));
      CHECK_THROWS_WITH(model.check(end - 2UL),
                        Catch::Contains("Index to be "));

      // B. check the sliced model
      // 1.check the element sizes
      CHECK(sliced_model.total_n_elements() == new_n_elements);
      CHECK(sliced_model.total_n_nodes() == new_n_elements + 1UL);
      CHECK(sliced_model.total_n_voronois() == new_n_elements - 1UL);
      CHECK(sliced_model.size() == new_n_elements);
      CHECK(not sliced_model.empty());

      // 2. check dofs
      CHECK(sliced_model.check(0UL) == 0UL);
      CHECK(sliced_model.check(end - 1UL) == new_n_elements - 1UL);

      CHECK_THROWS_AS(sliced_model.check(new_n_elements), std::out_of_range);
      CHECK_THROWS_AS(sliced_model.check(end - new_n_elements - 1UL),
                      std::out_of_range);

      CHECK_THROWS_WITH(sliced_model.check(new_n_elements),
                        Catch::Contains("Index to be "));
      CHECK_THROWS_WITH(sliced_model.check(end - new_n_elements - 1UL),
                        Catch::Contains("Index to be "));

      // 3. check the new members
      CHECK(sliced_model.tell_me_number() == 42);
      CHECK_ITERABLE_APPROX(
          to_vec(sliced_model.get_node_variable()),
          to_vec(DataVectorSlice(expected_node_variable, 0UL, 0UL,
                                 expected_node_variable.rows(),
                                 expected_node_variable.columns())));
      CHECK_ITERABLE_APPROX(
          to_vec(sliced_model.get_element_variable()),
          to_vec(DataVectorSlice(expected_element_variable, 0UL, 0UL,
                                 expected_element_variable.rows(),
                                 expected_element_variable.columns())));
      CHECK_ITERABLE_APPROX(
          to_vec(sliced_model.get_voronoi_variable()),
          to_vec(DataVectorSlice(expected_voronoi_variable, 0UL, 0UL,
                                 expected_voronoi_variable.rows(),
                                 expected_voronoi_variable.columns())));

      // C. check the const sliced model
      auto const_sliced_model =
          blocks::slice(static_cast<decltype(model) const&>(model), 0UL);
      // 1.check the element sizes
      CHECK(const_sliced_model.total_n_elements() == new_n_elements);
      CHECK(const_sliced_model.total_n_nodes() == new_n_elements + 1UL);
      CHECK(const_sliced_model.total_n_voronois() == new_n_elements - 1UL);
      CHECK(const_sliced_model.size() == new_n_elements);
      CHECK(not const_sliced_model.empty());

      // 2. check dofs
      CHECK(const_sliced_model.check(0UL) == 0UL);
      CHECK(const_sliced_model.check(end - 1UL) == new_n_elements - 1UL);

      CHECK_THROWS_AS(const_sliced_model.check(new_n_elements),
                      std::out_of_range);
      CHECK_THROWS_AS(const_sliced_model.check(end - new_n_elements - 1UL),
                      std::out_of_range);

      CHECK_THROWS_WITH(const_sliced_model.check(new_n_elements),
                        Catch::Contains("Index to be "));
      CHECK_THROWS_WITH(const_sliced_model.check(end - new_n_elements - 1UL),
                        Catch::Contains("Index to be "));

      // 3. check the new members
      CHECK(const_sliced_model.tell_me_number() == 42);
      //      CHECK_ITERABLE_APPROX(const_sliced_model.get_node_variable(),
      //                            DataVectorConstSlice(expected_node_variable.begin(),
      //                                                 expected_node_variable.end()));
      //      CHECK_ITERABLE_APPROX(
      //          const_sliced_model.get_element_variable(),
      //          DataVectorConstSlice(expected_element_variable.begin(),
      //                               expected_element_variable.end()));
      //      CHECK_ITERABLE_APPROX(
      //          const_sliced_model.get_voronoi_variable(),
      //          DataVectorConstSlice(expected_voronoi_variable.begin(),
      //                               expected_voronoi_variable.end()));

      CHECK_ITERABLE_APPROX(
          to_vec(const_sliced_model.get_node_variable()),
          to_vec(DataVectorConstSlice(expected_node_variable, 0UL, 0UL,
                                      expected_node_variable.rows(),
                                      expected_node_variable.columns())));
      CHECK_ITERABLE_APPROX(
          to_vec(const_sliced_model.get_element_variable()),
          to_vec(DataVectorConstSlice(expected_element_variable, 0UL, 0UL,
                                      expected_element_variable.rows(),
                                      expected_element_variable.columns())));
      CHECK_ITERABLE_APPROX(
          to_vec(const_sliced_model.get_voronoi_variable()),
          to_vec(DataVectorConstSlice(expected_voronoi_variable, 0UL, 0UL,
                                      expected_voronoi_variable.rows(),
                                      expected_voronoi_variable.columns())));

      return model;
    };

    // after one initializer
    {
      std::size_t first_n_elements(5UL);

      auto model(test_one_initializer(first_n_elements));

      const std::size_t new_n_elements = 10UL;

      // 1. check resizing logic in block
      auto total_n_elements_after_resizing =
          model.prepare_for_resize(new_n_elements);
      CHECK(total_n_elements_after_resizing == (5UL + 2UL + 10UL));
      CHECK(model.get_ghost_node_buffer() == GhostNodeIndices{5UL + 1UL});

      CHECK(model.n_units() == 1UL);
      model.resize_members_on_rod(2UL);
      CHECK(model.n_units() == 2UL);

      // 2. check start and size of slices

      // 2.1 First check elements
      CHECK(model.get_start_of_slice<typename CRPV::NElement::Stagger>(0UL) ==
            0UL);
      CHECK(model.get_size_of_slice<typename CRPV::NElement::Stagger>(0UL) ==
            1UL);
      CHECK(model.get_start_of_slice<typename CRPV::NElement::Stagger>(1UL) ==
            1UL);
      CHECK(model.get_size_of_slice<typename CRPV::NElement::Stagger>(1UL) ==
            1UL);

      // this is to make "slice" work, see below
      model.initialize_elements_buffer(new_n_elements, 1UL);

      auto second_n_elements = model.n_elements(1UL);
      CHECK(second_n_elements == new_n_elements);

      // 3.1 Check slices
      CHECK(model.slice<typename CRPV::NElement>(0UL) ==
            ElementsVectorSlice{model.n_elements(), 0UL, 1UL});
      CHECK(static_cast<decltype(model) const&>(model)
                .slice<typename CRPV::NElement>(0UL) ==
            ElementsVectorConstSlice{model.n_elements(), 0UL, 1UL});
      CHECK(model.slice<typename CRPV::NElement>(1UL) ==
            ElementsVectorSlice{model.n_elements(), 1UL, 1UL});
      CHECK(static_cast<decltype(model) const&>(model)
                .slice<typename CRPV::NElement>(1UL) ==
            ElementsVectorConstSlice{model.n_elements(), 1UL, 1UL});

      // 1.2 now we can check logic for resizing members on different parts
      {
        CHECK(model.get_node_variable().columns() == (first_n_elements + 1UL));
        CHECK(model.get_element_variable().columns() == (first_n_elements));
        CHECK(model.get_voronoi_variable().columns() ==
              (first_n_elements - 1UL));

        model.resize_members_not_on_rod(total_n_elements_after_resizing);

        CHECK(model.get_node_variable().columns() ==
              ((first_n_elements + 1UL) + 1UL + (second_n_elements + 1UL)));
        CHECK(model.get_element_variable().columns() ==
              (first_n_elements + 2UL + second_n_elements));
        CHECK(model.get_voronoi_variable().columns() ==
              ((first_n_elements - 1UL) + 3UL + (second_n_elements - 1UL)));
      }

      // 2.2
      // doesn't work off the shelf because .slice calls the get_size_of_slice
      // function which needs the degrees of freedom, and hence doesnt work if
      // n_elements = 0. So we need to populate it with n_elements first
      {
        {
          CHECK(model.get_start_of_slice<typename VCPV::VarOnNode::Stagger>(
                    0UL) == 0UL);
          CHECK(model.get_size_of_slice<typename VCPV::VarOnNode::Stagger>(
                    0UL) == (first_n_elements + 1UL));
          CHECK(model.get_start_of_slice<typename VCPV::VarOnNode::Stagger>(
                    1UL) == (first_n_elements + 2UL));
          CHECK(model.get_size_of_slice<typename VCPV::VarOnNode::Stagger>(
                    1UL) == (new_n_elements + 1UL));
        }
        {
          CHECK(model.get_start_of_slice<typename VCPV::VarOnElement::Stagger>(
                    0UL) == 0UL);
          CHECK(model.get_size_of_slice<typename VCPV::VarOnElement::Stagger>(
                    0UL) == (first_n_elements));
          CHECK(model.get_start_of_slice<typename VCPV::VarOnElement::Stagger>(
                    1UL) == (first_n_elements + 2UL));
          CHECK(model.get_size_of_slice<typename VCPV::VarOnElement::Stagger>(
                    1UL) == (new_n_elements));
        }
        {
          CHECK(model.get_start_of_slice<typename VCPV::VarOnVoronoi::Stagger>(
                    0UL) == 0UL);
          CHECK(model.get_size_of_slice<typename VCPV::VarOnVoronoi::Stagger>(
                    0UL) == (first_n_elements - 1UL));
          CHECK(model.get_start_of_slice<typename VCPV::VarOnVoronoi::Stagger>(
                    1UL) == (first_n_elements + 2UL));
          CHECK(model.get_size_of_slice<typename VCPV::VarOnVoronoi::Stagger>(
                    1UL) == (new_n_elements - 1UL));
        }
      }
      // 3.2
      CHECK(model.tell_me_number() == 42);

      // check individual members along with ghosting
      {
        auto& node_variable = model.get_node_variable();
        const std::size_t end_idx = first_n_elements + 1UL;
        CHECK_ITERABLE_APPROX(
            to_vec(model.slice<typename VCPV::VarOnNode>(0UL)),
            to_vec(DataVectorSlice(node_variable, 0UL, 0UL, 3UL, end_idx)));
        // offset of 1 here to account fo the ghost
        // TODO potential bug in testing : we only resize but never fill
        // elements with valid doubles. So sometimes we get nans instead of
        // zeros which cause errors with Catch's check() statements
        CHECK_ITERABLE_APPROX(
            to_vec(model.slice<typename VCPV::VarOnNode>(1UL)),
            to_vec(DataVectorSlice(node_variable, 0UL, end_idx + 1UL, 3UL,
                                   second_n_elements + 1UL)));
        CHECK_ITERABLE_APPROX(to_vec(static_cast<decltype(model) const&>(model)
                                         .slice<typename VCPV::VarOnNode>(0UL)),
                              to_vec(DataVectorConstSlice(node_variable, 0UL,
                                                          0UL, 3UL, end_idx)));
        CHECK_ITERABLE_APPROX(
            to_vec(static_cast<decltype(model) const&>(model)
                       .slice<typename VCPV::VarOnNode>(1UL)),
            to_vec(DataVectorConstSlice(node_variable, 0UL, end_idx + 1UL, 3UL,
                                        second_n_elements + 1UL)));

        CHECK(node_variable(0UL, end_idx) != approx(3.14159));
        CHECK(node_variable(1UL, end_idx) != approx(3.14159));
        CHECK(node_variable(2UL, end_idx) != approx(3.14159));
        model.fill_ghosts_placed<typename Model::OnNode>();
        // the part between rod 1 and 2 should be zerod
        // in this case we explicitly request the node to be non-default, say
        //
        CHECK(node_variable(0UL, end_idx) == approx(0.0));
        CHECK(node_variable(1UL, end_idx) == approx(0.0));
        CHECK(node_variable(2UL, end_idx) == approx(0.0));
      }

      {
        auto& element_variable = model.get_element_variable();
        const std::size_t end_idx = first_n_elements;
        CHECK_ITERABLE_APPROX(
            to_vec(model.slice<typename VCPV::VarOnElement>(0UL)),
            to_vec(DataVectorSlice(element_variable, 0UL, 0UL, 3UL, end_idx)));
        // offset of 1 here to account fo the ghost
        CHECK_ITERABLE_APPROX(
            to_vec(model.slice<typename VCPV::VarOnElement>(1UL)),
            to_vec(DataVectorSlice(element_variable, 0UL, end_idx + 2UL, 3UL,
                                   second_n_elements)));
        CHECK_ITERABLE_APPROX(
            to_vec(static_cast<decltype(model) const&>(model)
                       .slice<typename VCPV::VarOnElement>(0UL)),
            to_vec(DataVectorConstSlice(element_variable, 0UL, 0UL, 3UL,
                                        end_idx)));
        CHECK_ITERABLE_APPROX(
            to_vec(static_cast<decltype(model) const&>(model)
                       .slice<typename VCPV::VarOnElement>(1UL)),
            to_vec(DataVectorConstSlice(element_variable, 0UL, end_idx + 2UL,
                                        3UL, second_n_elements)));

        for (auto row_idx : {0UL, 1UL, 2UL}) {
          CHECK(element_variable(row_idx, end_idx) != approx(3.14159));
          CHECK(element_variable(row_idx, end_idx + 1UL) != approx(3.14159));
        }
        model.fill_ghosts_placed<typename Model::OnElement>();
        for (auto row_idx : {0UL, 1UL, 2UL}) {
          CHECK(element_variable(row_idx, end_idx) == approx(0.0));
          CHECK(element_variable(row_idx, end_idx + 1UL) == approx(0.0));
        }
      }

      {
        auto& voronoi_variable = model.get_voronoi_variable();
        const std::size_t end_idx = first_n_elements - 1UL;
        CHECK_ITERABLE_APPROX(
            to_vec(model.slice<typename VCPV::VarOnVoronoi>(0UL)),
            to_vec(DataVectorSlice(voronoi_variable, 0UL, 0UL, 3UL, end_idx)));
        // offset of 1 here to account fo the ghost
        CHECK_ITERABLE_APPROX(
            to_vec(model.slice<typename VCPV::VarOnVoronoi>(1UL)),
            to_vec(DataVectorSlice(voronoi_variable, 0UL, end_idx + 3UL, 3UL,
                                   second_n_elements - 1UL)));
        CHECK_ITERABLE_APPROX(
            to_vec(static_cast<decltype(model) const&>(model)
                       .slice<typename VCPV::VarOnVoronoi>(0UL)),
            to_vec(DataVectorConstSlice(voronoi_variable, 0UL, 0UL, 3UL,
                                        end_idx)));
        CHECK_ITERABLE_APPROX(
            to_vec(static_cast<decltype(model) const&>(model)
                       .slice<typename VCPV::VarOnVoronoi>(1UL)),
            to_vec(DataVectorConstSlice(voronoi_variable, 0UL, end_idx + 3UL,
                                        3UL, second_n_elements - 1UL)));

        for (auto row_idx : {0UL, 1UL, 2UL}) {
          CHECK(voronoi_variable(row_idx, end_idx) != approx(3.14159));
          CHECK(voronoi_variable(row_idx, end_idx + 1UL) != approx(3.14159));
          CHECK(voronoi_variable(row_idx, end_idx + 2UL) != approx(3.14159));
        }
        model.fill_ghosts_placed<typename Model::OnVoronoi>();
        for (auto row_idx : {0UL, 1UL, 2UL}) {
          CHECK(voronoi_variable(row_idx, end_idx) == approx(0.0));
          CHECK(voronoi_variable(row_idx, end_idx + 1UL) == approx(0.0));
          CHECK(voronoi_variable(row_idx, end_idx + 2UL) == approx(0.0));
        }
      }

    }  // end after one initializer
  }

}  // namespace