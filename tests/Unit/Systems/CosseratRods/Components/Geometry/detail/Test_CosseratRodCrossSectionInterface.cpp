//
// Created by Tejaswin Parthasarathy on 4/28/21.
//

#include "Framework/TestingFramework.hpp"
#include "Systems/CosseratRods/Components/Geometry/detail/CosseratRodCrossSectionInterface.hpp"
///
#include "Systems/CosseratRods/Mocks.hpp"
///
#include "Utilities/MakeArray.hpp"

namespace {

  using namespace elastica::cosserat_rod::component::detail;
  using namespace elastica;

  class MockCrossSectionOperations {
   public:
    template <typename Radius>
    static inline decltype(auto) compute_cross_section_area(
        Radius const&) noexcept {
      return 4.0;
    }
    template <typename Radius>
    static inline decltype(auto) compute_second_moment_of_area(
        Radius const&) noexcept {
      return 16.0;
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

  //  template <typename B>
  //  using TestGeometry =
  //      CosseratRodCrossSectionInterface<mock::BasicCosseratRodTraits, B,
  //                                       MockCrossSectionOperations>;
  template <typename B>
  class TestGeometry
      : public CosseratRodCrossSectionInterface<mock::BasicCosseratRodTraits, B,
                                                MockCrossSectionOperations> {
    // The point of this class is to bring
    //    Frame;
    //    DirectionType;
    // into public scope for testing
   public:
    using Parent =
        CosseratRodCrossSectionInterface<mock::BasicCosseratRodTraits, B,
                                         MockCrossSectionOperations>;
    using Parent::Parent;
    using typename Parent::ComputedVariables;
    using typename Parent::InitializedVariables;
    using typename Parent::Variables;

    using Parent::initialize;
    using typename Parent::DirectionType;
    using typename Parent::Frame;
  };

  using Plugin = mock::MockPlugin<TestGeometry>;
  //  using Model = blocks::Block<Plugin>;
  using Model = typename Plugin::Model;

  static_assert(sizeof(Model) ==
                    sizeof(double) * tmpl::size<Plugin::Variables>::value,
                "EBO failure!");
  template <typename T>
  constexpr bool const_check = std::is_const<std::remove_reference_t<T>>::value;

  ELASTICA_TEST_CASE(
      "Unit.Systems.CosseratRods.CosseratRodCrossSectionInterface.getters",
      "[Systems][Unit]") {
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
    Model model{};

    CHECK(approx(model.get_element_dimension()) == 0.0);
    CHECK(approx(model.get_element_volume()) == 0.0);

    constexpr auto n_initializers = 5UL;
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
        blocks::initialize<tags::ReferenceElementLength>(
            [&i = n_times_called[3]](...) {
              ++i;
              return 2.0;
            }),
        blocks::initialize<tags::ReferenceCurvature>(
            [&i = n_times_called[4]](...) {
              ++i;
              return 2.0;
            }),
        // from before
        blocks::initialize<tags::ElementDimension>(
            [&i = n_times_called[2]](...) {
              ++i;
              return 4.0;
            }));

    Model::initialize(model, std::move(initializer));

    // check if initialization is done as expected
    CHECK_ITERABLE_APPROX(n_times_called, make_array<n_initializers>(1U));

    CHECK(approx(model.get_element_dimension()) == 4.0);
    // volume = dimension * length
    CHECK(approx(model.get_element_volume()) == 4.0);

    CHECK(approx(model.get_area()) == 4.0);
    CHECK(approx(model.shape_factor()) == 1.6);

    //    using LocalFrame = typename TestGeometry<Model>::Frame;
    using LocalFrame = typename Model::Frame;
    CHECK(approx(model.get_second_moment_of_area<LocalFrame::D1>()) == 16.0);
    CHECK(approx(model.get_second_moment_of_area<LocalFrame::D2>()) == 16.0);
    // CustomDouble has trouble with Catch for unknown reasons
    CHECK(double(model.get_second_moment_of_area<LocalFrame::D1>(0UL)) ==
          approx(16.0));
    CHECK(double(model.get_second_moment_of_area<LocalFrame::D2>(0UL)) ==
          approx(16.0));
    CHECK(double(model.get_second_moment_of_area<LocalFrame::D1>(5UL)) ==
          approx(16.0));
    CHECK(double(model.get_second_moment_of_area<LocalFrame::D2>(5UL)) ==
          approx(16.0));
    // symmetric
    CHECK(double(model.get_second_moment_of_area<LocalFrame::D3>()) ==
          approx(32.0));
    CHECK(double(model.get_second_moment_of_area<LocalFrame::D3>(0UL)) ==
          approx(32.0));
    CHECK(double(model.get_second_moment_of_area<LocalFrame::D3>(5UL)) ==
          approx(32.0));

    // This is not a good test case to have, or even a good API. So we ignore
    // this for now.
    const auto smoa = model.get_second_moment_of_area(0UL);
    CHECK(smoa[0] == approx(16.0));
    CHECK(smoa[1] == approx(16.0));
    CHECK(smoa[2] == approx(32.0));
  }

}  // namespace