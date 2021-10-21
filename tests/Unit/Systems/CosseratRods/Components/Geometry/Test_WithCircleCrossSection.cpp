//
// Created by Tejaswin Parthasarathy on 4/28/21.
//
#include <cmath>

#include "Framework/TestingFramework.hpp"
// #include "Systems/CosseratRods/Components/Geometry/Protocols.hpp"
#include "Systems/CosseratRods/Components/Geometry/WithCircularCosseratRod.hpp"
#include "Systems/CosseratRods/Components/Geometry/detail/CircleCrossSectionOperations.hpp"
#include "Systems/CosseratRods/Tags.hpp"
#include "Systems/CosseratRods/CosseratRodTraits.hpp"
///
#include "Systems/CosseratRods/Mocks.hpp"
///
#include "Utilities/MakeArray.hpp"
#include "Utilities/Math/Sqrt.hpp"

namespace {

  //  Test static operations first

  using Traits = typename elastica::cosserat_rod::DefaultCosseratRodTraits;
  using Operations = elastica::cosserat_rod::component::detail::CircleCrossSectionOperations<Traits>;

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
      : public WithCircularCosseratRod<Traits, B> {
    // The point of this class is to bring
    //    Frame;
    //    DirectionType;
    // into public scope for testing
   public:
    using Traits = Traits;
    using Parent = WithCircularCosseratRod<Traits, B>;
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
  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.WithCircularCosseratRod: EBO",
                     "[Systems][Unit]") {
    CHECK_EBO(Model);
  }

//  static_assert(
//      tt::assert_conforms_to<Plugin,
//      ::elastica::cosserat_rod::protocols::Shape>, "failure!");
  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.WithCircularCosseratRod.data",
                     "[Systems][Unit]") {

    Model model{};

    // same as cross_section_interface
    CHECK(model.get_element_dimension() == 0.0);
    CHECK(model.get_element_volume() == 0.0);

    constexpr auto n_initializers = 3UL;
    auto n_times_called = make_array<n_initializers>(0U);
    auto initializer = mock::initialize_block<Plugin>(
        // from before
        blocks::initialize<elastica::tags::Position>([&i = n_times_called[0]](std::size_t index) {
          ++i;
          return index * 1.0;
          return 2.0;
        }),
        blocks::initialize<elastica::tags::Director>([&i = n_times_called[1]](std::size_t index) {
          ++i;
          return 0.5;
        }),
        // from before
        blocks::initialize<elastica::tags::ElementDimension>( [&i = n_times_called[2]](...) {
          ++i;
          return 4.0;
        })
    );

    Model::initialize(model, std::move(initializer));

    // check if initialization is done as expected
    auto expected_call_count = make_array(11U, 10U, 10U);
    CHECK_ITERABLE_APPROX(n_times_called, expected_call_count);

    // only these are checked now
    CHECK(model.get_element_dimension() == 4.0);
    // blocks::get<tags::ElementLength>(model) = 2.0;
    // volume = dimension * length, only calculated somewhere inside
    CHECK(model.get_element_volume() == approx(M_PI*4.0*4.0*blaze::sqrt(3)));

    CHECK(model.get_area() == M_PI * 4.0 * 4.0);
    CHECK(model.shape_factor() == 4.0 / 3.0);

    using LocalFrame = typename Model::Frame;
    const double expected_second_moment = M_PI * 4. * 4. * 4.;
    CHECK(model.get_second_moment_of_area<LocalFrame::D1>() ==
          expected_second_moment);
    CHECK(model.get_second_moment_of_area<LocalFrame::D2>() ==
          expected_second_moment);
    // symmetric
    CHECK(model.get_second_moment_of_area<LocalFrame::D3>() ==
          2.0 * expected_second_moment);

    model.compute_shear_strains();
  }

}  // namespace