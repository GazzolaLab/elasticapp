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

namespace {

  struct SomeVectorVariableTag;
  struct SomeScalarVariableTag;

  template <typename Traits, typename>
  struct NonEmptyComponent {
    struct SomeVectorVariable
        : public Traits::template CosseratRodInitializedVariable<
              SomeVectorVariableTag,              //
              typename Traits::DataType::Vector,  //
              typename Traits::Place::OnNode> {};

    struct SomeScalarVariable
        : public Traits::template CosseratRodInitializedVariable<
              SomeScalarVariableTag,              //
              typename Traits::DataType::Scalar,  //
              typename Traits::Place::OnNode> {};

    using InitializedVariables =
        tmpl::list<SomeVectorVariable, SomeScalarVariable>;
    using ComputedVariables = tmpl::list<>;
    using Variables = tmpl::list<>;

    template <class DownstreamBlock, class CosseratInitializer>
    static void initialize(DownstreamBlock& downstream_block,
                           CosseratInitializer&& initializers) {
      //      {
      //        using Variable = SomeVectorVariable;
      //        using Tag = SomeVectorVariableTag;
      //        auto& variable(blocks::get<Tag>(downstream_block));
      //        auto&& init = blocks::get<Tag>(std::move(initializers));
      //
      //        const auto dofs = Variable::get_dofs(initializers.n_elems());
      //        for (std::size_t idx = 0U; idx < dofs; ++idx)
      //          Variable::slice(variable, idx) = init(idx);
      //      }
      //      {
      //        using Variable = SomeScalarVariable;
      //        using Tag = SomeScalarVariableTag;
      //        auto& variable(blocks::get<Tag>(downstream_block));
      //        auto&& init = blocks::get<Tag>(std::move(initializers));
      //
      //        const auto dofs = Variable::get_dofs(initializers.n_elems());
      //        for (std::size_t idx = 0U; idx < dofs; ++idx)
      //          Variable::slice(variable, idx) = init(idx);
      //      }
    }
  };

}  // namespace

namespace {

  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.Initializer.tuple_initialize",
                     "[Systems][Unit]") {
    using TestPlugin = GenerateTestPlugin<NonEmptyComponent>;

    constexpr auto n_initializers = 3UL;
    auto n_times_called = make_array<n_initializers>(0U);
    auto initializer =
        initialize_cosserat_rod<TestPlugin>(std::forward_as_tuple(
            // from before
            blocks::initialize<tags::NElement>([&i = n_times_called[0]](...) {
              ++i;
              return 20UL;
            }),
            blocks::initialize<SomeVectorVariableTag>(
                [&i = n_times_called[1]](...) {
                  ++i;
                  return 1.0;
                }),
            blocks::initialize<SomeScalarVariableTag>(
                [&i = n_times_called[2]](...) {
                  ++i;
                  return 2.0;
                })));

    // Model::initialize(model, std::move(initializer));

    // check if initialization is done as expected and can be called multiple
    // times even if the initializer is not moved
    CHECK(initializer.n_elems() == 20UL);
    CHECK(n_times_called[0UL] == 1UL);
  }

}  // namespace
