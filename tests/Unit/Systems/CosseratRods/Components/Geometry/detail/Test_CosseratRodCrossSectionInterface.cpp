//
// Created by Tejaswin Parthasarathy on 4/28/21.
//

#include "Framework/TestingFramework.hpp"
#include "Systems/CosseratRods/Components/Geometry/detail/CosseratRodCrossSectionInterface.hpp"
#include "Systems/CosseratRods/CosseratRodTraits.hpp"
///
#include "Systems/CosseratRods/Mocks.hpp"
///
#include "Utilities/MakeArray.hpp"
#include "Utilities/Math/Sqrt.hpp"

namespace {

  using namespace elastica::cosserat_rod::component::detail;
  using namespace elastica;

  class MockCrossSectionOperations {
   public:
    template <typename Radius>
    static inline decltype(auto) compute_cross_section_area(
        Radius const& r) noexcept {
      return r;
    }
    template <typename Radius>
    static inline decltype(auto) compute_second_moment_of_area(
        Radius const& r) noexcept {
      return r * r;
    }

    template <typename Volume, typename Length>
    static inline decltype(auto) update_dimension(Volume const&,
                                                  Length const&) noexcept {
      return 50.0;
    }

    // is division constexpr?
    static inline constexpr auto shape_factor() noexcept -> double {
      return 8.0 / 5.0;
    }
  };

}  // namespace

namespace {
  using Traits = typename elastica::cosserat_rod::DefaultCosseratRodTraits;

  template <typename B>
  class TestGeometry
      : public CosseratRodCrossSectionInterface<Traits, B,
                                                MockCrossSectionOperations> {
    // The point of this class is to bring
    //    Frame;
    //    DirectionType;
    // into public scope for testing
   public:
    using Parent =
        CosseratRodCrossSectionInterface<Traits, B, MockCrossSectionOperations>;
    using Parent::Parent;
    using typename Parent::ComputedVariables;
    using typename Parent::ElementDimension;
    using typename Parent::ElementVolume;
    using typename Parent::InitializedVariables;
    using typename Parent::Variables;

    using typename Parent::DirectionType;
    using typename Parent::Frame;

    template <typename CosseratInitializer>
    static void initialize(
        TestGeometry& this_component,
        CosseratInitializer&& initializer) {
      auto&& stable_initializer = initializer;

      auto&& dimensions =
          blocks::get<ElementDimension>(this_component.self());
      ElementDimension::resize(dimensions,
                               ElementDimension::get_dofs(
                                   stable_initializer.n_elems()));

      auto&& volumes = blocks::get<ElementVolume>(this_component.self());
      ElementVolume::resize(volumes,
                            ElementVolume::get_dofs(
                                stable_initializer.n_elems()));

      Parent::initialize(this_component, stable_initializer);
    }
  };

  using Plugin = mock::MockPlugin<Traits, TestGeometry>;
  //  using Model = blocks::Block<Plugin>;
  using Model = typename Plugin::Model;
  static_assert(
      std::is_same_v<
          typename Model::VariableMap::template variable_from<
              tags::ElementDimension>,
          typename TestGeometry<Model>::ElementDimension>,
      "Cross-section model VariableMap is missing ElementDimension");
  static_assert(
      std::is_same_v<
          typename Model::VariableMap::template variable_from<
              tags::ElementVolume>,
          typename TestGeometry<Model>::ElementVolume>,
      "Cross-section model VariableMap is missing ElementVolume");

  ELASTICA_TEST_CASE(
      "Unit.Systems.CosseratRods.CosseratRodCrossSectionInterface.EBO",
      "[Systems][Unit]") {
    EMPTY_TEST();

    CHECK_EBO(Model);
  }

  template <typename T>
  constexpr bool const_check = std::is_const<std::remove_reference_t<T>>::value;

  ELASTICA_TEST_CASE(
      "Unit.Systems.CosseratRods.CosseratRodCrossSectionInterface.getters",
      "[Systems][Unit]") {
    EMPTY_TEST();

    Model model{};
    auto& const_model = static_cast<const Model&>(model);
    static_assert(const_check<decltype(const_model.get_element_dimension())>,
                  "Oops");
    static_assert(const_check<decltype(const_model.get_element_volume())>,
                  "Oops");
  }

  ELASTICA_TEST_CASE(
      "Unit.Systems.CosseratRods.CosseratRodCrossSectionInterface.data",
      "[Systems][Unit]") {
    constexpr auto n_initializers = 6UL;
    auto n_times_called = make_array<n_initializers>(0U);
    auto initializer = mock::initialize_block<Plugin>(
        // FIXME : Remove 10UL taken from mock::initialize_block
        blocks::initialize<elastica::tags::NElement>([]() { return 10UL; }),
        // from before
        blocks::initialize<tags::Position>(
            [&i = n_times_called[0]](std::size_t index) {
              ++i;
              return index * 1.0;
            }),
        blocks::initialize<tags::Director>([&i = n_times_called[1]](...) {
          ++i;
          return 0.5;
        }),
        // from before
        blocks::initialize<tags::ElementDimension>(
            [&i = n_times_called[2]](...) {
              ++i;
              return 4.0;
            }),
        blocks::initialize<tags::ReferenceCurvature>(
            [&i = n_times_called[3]](...) {
              ++i;
              return 1.0;
            }),
        blocks::initialize<tags::ReferenceShearStretchStrain>(
            [&i = n_times_called[4]](...) {
              ++i;
              return 1.0;
            }),
        blocks::initialize<tags::ElementDilatation>(
            [&i = n_times_called[5]](...) {
              ++i;
              return 2.0;
            })
        // blocks::initialize<tags::MassSecondMomentOfInertia>([&i =
        // n_times_called[5]](...) { ++i; return 1.0; })
    );

    Model model(std::move(initializer));

    // After compute geometry, dimension should be updated to 50.0
    // get_second_moment_of_area return r*r (MockCrossSectionOperations)
    // get_area is set to return r (MockCrossSectionOperations)
    // volume = area * (initial_length / dilatation)
    constexpr elastica::real_t r(50.0);
    constexpr elastica::real_t area(r);
    constexpr elastica::real_t rr(r * r);
    constexpr elastica::real_t sf(1.6);
    const elastica::real_t volume(4.0 * sqrt(3) / 2.0);
    CHECK(model.get_element_dimension() == approx(r));
    CHECK(model.get_element_volume() == approx(volume));

    // check if initialization is done as expected
    auto expected_call_count = make_array(11U, 10U, 10U, 9U, 10U, 10U);
    CHECK_ITERABLE_APPROX(n_times_called, expected_call_count);

    CHECK(model.get_area() == approx(area));
    CHECK(model.shape_factor() == approx(sf));

    //    using LocalFrame = typename TestGeometry<Model>::Frame;
    using LocalFrame = typename Model::Frame;
    CHECK(model.get_second_moment_of_area<LocalFrame::D1>(0UL) == approx(rr));
    CHECK(model.get_second_moment_of_area<LocalFrame::D2>(0UL) == approx(rr));
    // CustomDouble has trouble with Catch for unknown reasons
    CHECK(model.get_second_moment_of_area<LocalFrame::D1>(0UL) == approx(rr));
    CHECK(model.get_second_moment_of_area<LocalFrame::D2>(0UL) == approx(rr));
    CHECK(model.get_second_moment_of_area<LocalFrame::D1>(5UL) == approx(rr));
    CHECK(model.get_second_moment_of_area<LocalFrame::D2>(5UL) == approx(rr));
    // symmetric
    CHECK(model.get_second_moment_of_area<LocalFrame::D3>(0UL) ==
          approx(2 * rr));
    CHECK(model.get_second_moment_of_area<LocalFrame::D3>(0UL) ==
          approx(2 * rr));
    CHECK(model.get_second_moment_of_area<LocalFrame::D3>(5UL) ==
          approx(2 * rr));

    // This is not a good test case to have, or even a good API. So we ignore
    // this for now.
    const auto smoa = model.get_second_moment_of_area(0UL);
    CHECK(smoa[0] == approx(rr));
    CHECK(smoa[1] == approx(rr));
    CHECK(smoa[2] == approx(2 * rr));
  }

}  // namespace
