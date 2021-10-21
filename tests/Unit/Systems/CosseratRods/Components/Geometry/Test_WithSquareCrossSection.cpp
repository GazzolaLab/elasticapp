//
// Created by Tejaswin Parthasarathy on 4/29/21.
//
#include <cmath>

#include "Framework/TestingFramework.hpp"
// #include "Systems/CosseratRods/Components/Geometry/Protocols.hpp"
#include "Systems/CosseratRods/CosseratRodTraits.hpp"
#include "Systems/CosseratRods/Components/Geometry/WithSquareCosseratRod.hpp"
///
#include "Systems/CosseratRods/Mocks.hpp"
///
#include "Utilities/MakeArray.hpp"
#include "Utilities/Math/Sqrt.hpp"

namespace {

  //  Test static operations first
  using namespace elastica::cosserat_rod::component::detail;
  using namespace elastica;

  using Traits = typename elastica::cosserat_rod::DefaultCosseratRodTraits;

  using Operations = SquareCrossSectionOperations<Traits>;

  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.SquareCrossSectionOperations",
                     "[Systems][Unit]") {
    CHECK(Operations::compute_cross_section_area(1.0) == approx(4.0));
    // A^2 / 12.0
    CHECK(Operations::compute_second_moment_of_area(1.0) ==
          approx(4.0 * 4.0 / 12.0));
    CHECK(Operations::update_dimension(4.0, 1.0) == approx(1.0));
    // this is a failure test, make it pass later on
    CHECK(Operations::shape_factor() == approx(0.0));
  }

}  // namespace

namespace {
  using namespace elastica::cosserat_rod::component;
  using Traits = typename elastica::cosserat_rod::DefaultCosseratRodTraits;

  template <typename B>
  class TestGeometry
      : public WithSquareCosseratRod<Traits, B> {
    // The point of this class is to bring
    //    Frame;
    //    DirectionType;
    // into public scope for testing
   public:
    using Parent = WithSquareCosseratRod<Traits, B>;
    using Parent::Parent;
    using Traits = Traits;
    using typename Parent::ComputedVariables;
    using typename Parent::InitializedVariables;
    using typename Parent::Variables;

    using Parent::initialize;
    using typename Parent::DirectionType;
    using typename Parent::Frame;
  };

  using Plugin = mock::MockPlugin<TestGeometry>;
  // using Model = blocks::Block<Plugin>;
  using Model = typename Plugin::Model;
  // TypeDisplayer<typename Model::MatrixVariables> td{};

  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.WithSquareCosseratRod: EBO",
                     "[Systems][Unit]") {
    // Assert Equal EBO test
    CHECK_EBO(Model);
  }

  //  static_assert(
  //      tt::assert_conforms_to<Plugin,
  //      ::elastica::cosserat_rod::protocols::Shape>, "failure!");

  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.WithSquareCosseratRod.data",
                     "[Systems][Unit]") {
    Model model{};

    // same as cross_section_interface
    CHECK(model.get_element_dimension() == 0.0);
    CHECK(model.get_element_volume() == 0.0);

    constexpr auto n_initializers = 3UL;
    auto n_times_called = make_array<n_initializers>(0U);
    auto initializer = mock::initialize_block<Plugin>(
        blocks::initialize<tags::Position>([&i = n_times_called[0]](std::size_t index) { ++i; return index * 1.0; }),
        blocks::initialize<tags::Director>([&i = n_times_called[1]](...) { ++i; return 0.5; }),
        blocks::initialize<tags::ElementDimension>( [&i = n_times_called[2]](...) { ++i; return 4.0; })
      );

    Model::initialize(model, std::move(initializer));

    // check if initialization is done as expected
    auto expected_call_count = make_array(11U, 10U, 10U);
    CHECK_ITERABLE_APPROX(n_times_called, expected_call_count);

    // only these are checked now
    CHECK(model.get_element_dimension() == 4.0);
    // blocks::get<tags::ElementLength>(model) = 2.0;
    // volume = dimension * length, only calculated somewhere inside
    CHECK(model.get_element_volume() == approx(4.0*4.0*4.0*blaze::sqrt(3)));

    CHECK(model.get_area() == 4.0 * 4.0 * 4.0);
    // this is a failure test, make it pass later on
    CHECK(model.shape_factor() == 0.0);

    using LocalFrame = typename Model::Frame;
    const double expected_second_moment = (64. * 64.) / 12.;
    CHECK(model.get_second_moment_of_area<LocalFrame::D1>() ==
          expected_second_moment);
    CHECK(model.get_second_moment_of_area<LocalFrame::D2>() ==
          expected_second_moment);
    // symmetric
    CHECK(model.get_second_moment_of_area<LocalFrame::D3>() ==
          2.0 * expected_second_moment);
  }

}  // namespace
