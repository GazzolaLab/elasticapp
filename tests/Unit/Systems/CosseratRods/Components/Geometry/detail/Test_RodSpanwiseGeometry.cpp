//
// Created by Tejaswin Parthasarathy on 4/22/21.
//
#include "Framework/TestingFramework.hpp"
#include "Systems/CosseratRods/Components/Geometry/detail/RodSpanwiseGeometry.hpp"
#include "Systems/CosseratRods/CosseratRodTraits.hpp"
///
#include "Systems/CosseratRods/Mocks.hpp"
///
#include "Systems/Block.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/TMPL.hpp"

namespace {

  using namespace elastica::cosserat_rod::component::detail;
  using namespace elastica;

  using Traits = typename elastica::cosserat_rod::DefaultCosseratRodTraits;

  // conform to protocol and assert
  template <typename B>
  struct TestRodSpanwiseGeometry
      : public RodSpanwiseGeometry<Traits, B> {
    using Traits = Traits;
  };

}  // namespace

namespace {

  template <typename B>
  using TestGeometry = TestRodSpanwiseGeometry<B>;

  using Plugin = mock::MockPlugin<TestGeometry>;
  // using Model = blocks::Block<Plugin>;
  using Model = typename Plugin::Model;

  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.RodSpanwiseGeometry:EBO",
                     "[Systems][Unit]") {
    // Assert Equal EBO test
    CHECK_EBO(Model);
  }

  template <typename T>
  constexpr bool const_check = std::is_const<std::remove_reference_t<T>>::value;
  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.RodSpanwiseGeometry.getters",
                     "[Systems][Unit]") {
    Model model{};
    auto& const_model = static_cast<const Model&>(model);
    static_assert(const_check<decltype(const_model.get_position())>, "Oops");
    static_assert(const_check<decltype(const_model.get_director())>, "Oops");
  }

  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.RodSpanwiseGeometry.data",
                     "[Systems][Unit]") {
    Model model{};

    CHECK(model.get_position() == 0.0);
    CHECK(model.get_director() == 0.0);

    Model::initialize(
      model,
      mock::initialize_block<Plugin>(
        blocks::initialize<tags::Position>([](...) { return 2.0; }),
        blocks::initialize<tags::Director>([](...) { return 3.0; })
      )
    );

    CHECK(model.get_position() == 2.0);
    CHECK(model.get_director() == 3.0);
  }

}  // namespace
