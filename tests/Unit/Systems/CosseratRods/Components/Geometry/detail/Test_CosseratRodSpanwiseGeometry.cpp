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
#include "Utilities/Math/Sqrt.hpp"
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
  struct TestGeometry : public CosseratRodSpanwiseGeometry<Traits, B> {};

  using Plugin = mock::MockPlugin<Traits, TestGeometry>;
  //  using Model = blocks::Block<Plugin>;
  using Model = typename Plugin::Model;

  ELASTICA_TEST_CASE(
      "Unit.Systems.CosseratRods.CosseratRodSpanwiseGeometry.EBO",
      "[Systems][Unit]") {
    EMPTY_TEST();

    CHECK_EBO(Model);
  }

  template <typename T>
  constexpr bool const_check = std::is_const<std::remove_reference_t<T>>::value;

  ELASTICA_TEST_CASE(
      "Unit.Systems.CosseratRods.CosseratRodSpanwiseGeometry.getters",
      "[Systems][Unit]") {
    EMPTY_TEST();

    Model model{};
    auto& const_model = static_cast<const Model&>(model);
    static_assert(
        const_check<decltype(const_model.get_reference_element_length())>,
        "Oops");
    static_assert(const_check<decltype(const_model.get_element_length())>,
                  "Oops");
    static_assert(const_check<decltype(const_model.get_reference_curvature())>,
                  "Oops");
    static_assert(
        const_check<decltype(const_model.get_reference_shear_stretch_strain())>,
        "Oops");
  }

  ELASTICA_TEST_CASE(
      "Unit.Systems.CosseratRods.CosseratRodSpanwiseGeometry.data",
      "[Systems][Unit]") {
    Model empty_model{};

    CHECK(empty_model.get_reference_element_length() == 0.0);
    CHECK(empty_model.get_element_length() == 0.0);
    CHECK(empty_model.get_reference_curvature() == 0.0);
    CHECK(empty_model.get_reference_shear_stretch_strain() == 0.0);

    constexpr auto n_initializers = 5UL;
    auto n_times_called = make_array<n_initializers>(0U);
    auto initializer = mock::initialize_block<Plugin>(
        // FIXME : Remove 10UL taken from mock::initialize_block
        blocks::initialize<elastica::tags::NElement>([]() { return 10UL; }),
        blocks::initialize<tags::Position>(
            [&i = n_times_called[0]](std::size_t index) {
              ++i;
              return index * 1.0;
            }),
        blocks::initialize<tags::Director>([&i = n_times_called[1]](...) {
          ++i;
          return 0.5;
        }),
        blocks::initialize<tags::ReferenceCurvature>(
            [&i = n_times_called[2]](...) {
              ++i;
              return 1.0;
            }),
        blocks::initialize<tags::ReferenceShearStretchStrain>(
            [&i = n_times_called[3]](...) {
              ++i;
              return 1.0;
            }),
        blocks::initialize<tags::ElementDilatation>(
            [&i = n_times_called[4]](...) {
              ++i;
              return 1.0;
            }));

    Model model(std::move(initializer));

    // check if initialization is done as expected
    auto expected_call_count = make_array(11U, 10U, 9U, 10U, 10U);
    CHECK_ITERABLE_APPROX(n_times_called, expected_call_count);

    CHECK(model.get_element_length() == sqrt(3.0));
    CHECK(model.get_reference_element_length() == sqrt(3.0));
    CHECK(model.get_reference_curvature() == 1.0);
    CHECK(model.get_reference_shear_stretch_strain() == 1.0);
  }

}  // namespace
