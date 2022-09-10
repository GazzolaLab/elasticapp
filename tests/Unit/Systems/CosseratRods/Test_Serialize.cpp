//
// Created by Tejaswin Parthasarathy on 5/20/21.
//

#include <sstream>

#include "Framework/TestingFramework.hpp"
#include "Systems/CosseratRods/Block.hpp"
#include "Systems/CosseratRods/Components.hpp"
#include "Systems/CosseratRods/CosseratRodTraits.hpp"
#include "Systems/CosseratRods/Serialize.hpp"

// copy pasted from Test_Block.cpp, can be refactored
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
      CosseratRodPlugin<DefaultCosseratRodTraits, blocks::Block, T...>;

  template <typename Plugin>
  using GenerateActualBlock = blocks::Block<Plugin>;

  template <typename Plugin>
  using GenerateBlock = TestBlock<Plugin>;

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
  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.serialize.empty_component",
                     "[Systems][Unit]") {
    using TestPlugin = GenerateTestPlugin<EmptyComponent>;
    using Model = GenerateBlock<TestPlugin>;

    using namespace elastica::io;

    auto as_block = [](Model& m) -> decltype(auto) {
      return static_cast<GenerateActualBlock<TestPlugin>&>(m);
    };
    auto as_const_block = [](Model const& m) -> decltype(auto) {
      return static_cast<GenerateActualBlock<TestPlugin> const&>(m);
    };

    std::stringstream ss;  // any stream can be used
    Model model{};
    Model loaded_model{};

    {
      using GhostNodeIndices = typename Model::GhostNodeIndices;
      model.get_ghost_node_buffer() = GhostNodeIndices{1UL, 55UL, 200UL};

      BinaryOutputArchive oarchive(ss);  //  Create an output archive
      oarchive(as_const_block(model));   // Write the data to the archive
    }

    {
      BinaryInputArchive iarchive(ss);  //  Create an input archive

      iarchive(as_block(loaded_model));  // Read the data from the archive
    }

    CHECK(model.data() == loaded_model.data());
    CHECK(model.get_ghost_node_buffer() ==
          loaded_model.get_ghost_node_buffer());
  }

}  // namespace

namespace {

  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.serialize.non_empty_component",
                     "[Systems][Unit]") {
    // two vectors and two tensors, all randomized here
    using TestPlugin =
        GenerateTestPlugin<component::WithDiagonalLinearHyperElasticModel>;
    using Model = GenerateBlock<TestPlugin>;

    using namespace elastica::io;

    auto as_block = [](Model& m) -> decltype(auto) {
      return static_cast<GenerateActualBlock<TestPlugin>&>(m);
    };
    auto as_const_block = [](Model const& m) -> decltype(auto) {
      return static_cast<GenerateActualBlock<TestPlugin> const&>(m);
    };

    std::stringstream ss;  // any stream can be used
    Model model{};
    Model loaded_model{};

    {
      using GhostNodeIndices = typename Model::GhostNodeIndices;
      model.get_ghost_node_buffer() = GhostNodeIndices{1UL, 55UL, 200UL};
      blaze::randomize(blocks::get<tags::InternalLoads>(model));
      blaze::randomize(blocks::get<tags::InternalTorques>(model));
      blaze::randomize(blocks::get<tags::BendingTwistRigidityMatrix>(model));
      blaze::randomize(blocks::get<tags::ShearStretchRigidityMatrix>(model));

      BinaryOutputArchive oarchive(ss);  //  Create an output archive
      oarchive(as_const_block(model));   // Write the data to the archive
    }

    {
      BinaryInputArchive iarchive(ss);  //  Create an input archive

      iarchive(as_block(loaded_model));  // Read the data from the archive
    }

    // do we need to check approx values here
    CHECK(model.data() == loaded_model.data());
    CHECK(model.get_ghost_node_buffer() ==
          loaded_model.get_ghost_node_buffer());
  }
}  // namespace
