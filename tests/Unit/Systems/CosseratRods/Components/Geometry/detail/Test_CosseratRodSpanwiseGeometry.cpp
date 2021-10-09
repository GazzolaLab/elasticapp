//
// Created by Tejaswin Parthasarathy on 4/26/21.
//
#include "Framework/TestingFramework.hpp"
#include "Systems/CosseratRods/Components/Geometry/detail/CosseratRodSpanwiseGeometry.hpp"
#include "Systems/CosseratRods/Components/Geometry/detail/RodSpanwiseGeometry.hpp"
#include "Systems/CosseratRods/CosseratRodTraits.hpp"
///
#include "Systems/CosseratRods/Mocks.hpp"
///
#include "Systems/Block.hpp"
//
#include "Utilities/MakeArray.hpp"
#include "Utilities/TMPL.hpp"

namespace {

  using namespace elastica::cosserat_rod::component::detail;
  using namespace elastica;

  using Traits = typename elastica::cosserat_rod::DefaultCosseratRodTraits;

  // conform to protocol and assert
  template <typename B>
  struct TestCosseratRodSpanwiseGeometry
      : public CosseratRodSpanwiseGeometry<Traits, B>,
        public ::tt::ConformsTo<blocks::protocols::Plugin> {};

}  // namespace

namespace {

  template <typename B>
  struct TestGeometry : public CosseratRodSpanwiseGeometry<Traits, B>{
    using Traits = Traits;
  };

  using Plugin = mock::MockPlugin<TestGeometry>;
//  using Model = blocks::Block<Plugin>;
  using Model = typename Plugin::Model;

  static constexpr bool _result = CheckEbo<sizeof(Model), sizeof(double)*tmpl::size<Plugin::Variables>::value>::_result;

  template <typename T>
  constexpr bool const_check = std::is_const<std::remove_reference_t<T>>::value;

  ELASTICA_TEST_CASE(
      "Unit.Systems.CosseratRods.CosseratRodSpanwiseGeometry.getters",
      "[Systems][Unit]") {
    Model model{};
    auto& const_model = static_cast<const Model&>(model);
    static_assert(
        const_check<decltype(const_model.get_reference_element_length())>,
        "Oops");
    static_assert(const_check<decltype(const_model.get_element_length())>,
                  "Oops");
    static_assert(const_check<decltype(const_model.get_reference_curvature())>,
                  "Oops");
  }

  ELASTICA_TEST_CASE(
      "Unit.Systems.CosseratRods.CosseratRodSpanwiseGeometry.data",
      "[Systems][Unit]") {
    Model model{};

    CHECK(model.get_reference_element_length() == 0.0);
    CHECK(model.get_element_length() == 0.0);
    CHECK(model.get_reference_curvature() == 0.0);

    constexpr auto n_initializers = 2UL;
    auto n_times_called = make_array<n_initializers>(0U);
    auto initializer = mock::initialize_block<Plugin>(
        // from before
        blocks::initialize<tags::Position>([&i = n_times_called[0]](...) {
          ++i;
          return 2.0;
        }),
        blocks::initialize<tags::Director>([&i = n_times_called[1]](...) {
          ++i;
          return 3.0;
        }),
        // from before
        blocks::initialize<tags::ReferenceElementLength>([](...) { return 2.0; }),
        blocks::initialize<tags::ReferenceCurvature>([](...) { return 2.0; }),
        blocks::initialize<tags::ReferenceVoronoiLength>([](...) { return 1.2; }));

    Model::initialize(model, std::move(initializer));

    // check if initialization is done as expected
    CHECK_ITERABLE_APPROX(n_times_called, make_array<2>(1U));

    CHECK(model.get_element_length() == 1.0);
    CHECK(model.get_reference_element_length() == 1.0);
    CHECK(model.get_reference_curvature() == 2.0);
  }

}  // namespace
