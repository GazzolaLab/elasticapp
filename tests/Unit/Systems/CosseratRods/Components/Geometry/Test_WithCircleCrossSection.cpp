//
// Created by Tejaswin Parthasarathy on 4/28/21.
//
#include <cmath>

#include "Framework/TestingFramework.hpp"
// #include "Systems/CosseratRods/Components/Geometry/Protocols.hpp"
#include "Systems/CosseratRods/Components/Geometry/WithCircularCosseratRod.hpp"
///
#include "Systems/CosseratRods/Mocks.hpp"
///
#include "Utilities/MakeArray.hpp"

namespace {

  //  Test static operations first
  using namespace elastica::cosserat_rod::component::detail;
  using namespace elastica;

  using Operations = CircleCrossSectionOperations<mock::BasicCosseratRodTraits>;

  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.CircleCrossSectionOperations",
                     "[Systems][Unit]") {
    CHECK(Operations::compute_cross_section_area(1.0) == approx(M_PI));
    // A^2 / 4 / pi
    CHECK(Operations::compute_second_moment_of_area(1.0) ==
          approx(0.25 * M_PI));
    CHECK(Operations::update_dimension(M_PI, 1.0) == approx(1.0));
    CHECK(Operations::shape_factor() == approx(4.0 / 3.0));
  }

}  // namespace

namespace {
  using namespace elastica::cosserat_rod::component;

  template <typename B>
  class TestGeometry
      : public WithCircularCosseratRod<mock::BasicCosseratRodTraits, B> {
    // The point of this class is to bring
    //    Frame;
    //    DirectionType;
    // into public scope for testing
   public:
    using Parent = WithCircularCosseratRod<mock::BasicCosseratRodTraits, B>;
    using Parent::Parent;
    using typename Parent::ComputedVariables;
    using typename Parent::InitializedVariables;
    using typename Parent::Variables;

    using Parent::initialize;
    using typename Parent::DirectionType;
    using typename Parent::Frame;
  };

  using Plugin = mock::MockPlugin<TestGeometry>;
  using Model = typename Plugin::Model;

  static_assert(sizeof(Model) ==
                    sizeof(double) * tmpl::size<Plugin::Variables>::value,
                "EBO failure!");

  //  static_assert(
  //      tt::assert_conforms_to<Plugin,
  //      ::elastica::cosserat_rod::protocols::Shape>, "failure!");
  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.WithCircularCosseratRod.data",
                     "[Systems][Unit]") {
    Model model{};

    // same as cross_section_interface
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

    // only these are checked now
    CHECK(approx(model.get_element_dimension()) == 4.0);
    // blocks::get<tags::ElementLength>(model) = 2.0;
    // volume = dimension * length, only calculated somewhere inside
    CHECK(approx(model.get_element_volume()) == 4.0);

    CHECK(approx(model.get_area()) == M_PI * 4.0 * 4.0);
    CHECK(approx(model.shape_factor()) == 4.0 / 3.0);

    using LocalFrame = typename Model::Frame;
    const double expected_second_moment = M_PI * 4. * 4. * 4.;
    CHECK(approx(model.get_second_moment_of_area<LocalFrame::D1>()) ==
          expected_second_moment);
    CHECK(approx(model.get_second_moment_of_area<LocalFrame::D2>()) ==
          expected_second_moment);
    // symmetric
    CHECK(approx(model.get_second_moment_of_area<LocalFrame::D3>()) ==
          2.0 * expected_second_moment);
  }

}  // namespace