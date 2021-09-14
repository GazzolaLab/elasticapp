//
// Created by Tejaswin Parthasarathy on 5/3/21.
//
#include "Framework/TestingFramework.hpp"
#include "Systems/CosseratRods/Components/Elasticity/WithExplicitlyDampedDiagonalLinearHyperElasticModel.hpp"
#include "Systems/CosseratRods/Components/Geometry.hpp"
///
///
#include "Systems/CosseratRods/Mocks.hpp"
#include "Utilities/MakeArray.hpp"

namespace {
  using namespace elastica;
  using namespace elastica::cosserat_rod;

  template <typename B>
  using TestGeometry =
      component::WithCircularCosseratRod<mock::BasicCosseratRodTraits, B>;

  template <typename B>
  using TestElasticity =
      component::WithExplicitlyDampedDiagonalLinearHyperElasticModel<
          mock::BasicCosseratRodTraits, B>;

  using Plugin = mock::MockPlugin<TestGeometry, TestElasticity>;
  using Model = typename Plugin::Model;

  //  static_assert(
  //      tt::assert_conforms_to<Plugin,
  //      ::elastica::cosserat_rod::protocols::Shpae>, "failure!");

  //  static_assert(
  //      tt::assert_conforms_to<Plugin,
  //      ::elastica::cosserat_rod::protocols::Elastic1D>, "failure!");

}  // namespace

namespace {

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.WithExplicitlyDampedDiagonalLinearHyperElasticModel.data",
                     "[Systems][Unit]") {
    // clang-format on
    Model model{};

    static_assert(
        sizeof(Model) == sizeof(double) * tmpl::size<Plugin::Variables>::value,
        "EBO failure!");

    constexpr auto n_initializers = 9UL;
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
            [&i = n_times_called[2]](...) {
              ++i;
              return 2.0;
            }),
        blocks::initialize<tags::ReferenceCurvature>(
            [&i = n_times_called[3]](...) {
              ++i;
              return 2.0;
            }),
        blocks::initialize<tags::ElementDimension>(
            [&i = n_times_called[4]](...) {
              ++i;
              return 4.0;
            }),
        blocks::initialize<tags::ShearStretchRigidityMatrix>(
            [&i = n_times_called[5]](...) {
              ++i;
              return 4.0;
            }),
        blocks::initialize<tags::BendingTwistRigidityMatrix>(
            [&i = n_times_called[6]](...) {
              ++i;
              return 4.0;
            }),
        // from before
        blocks::initialize<tags::ForceDamping>([&i = n_times_called[7]](...) {
          ++i;
          return 4.0;
        }),
        blocks::initialize<tags::TorqueDamping>([&i = n_times_called[8]](...) {
          ++i;
          return 4.0;
        }));

    Model::initialize(model, std::move(initializer));

    // check if initialization is done as expected
    CHECK_ITERABLE_APPROX(n_times_called, make_array<n_initializers>(1U));

    // [0] element : stiff  * I_0
    const double area = M_PI * 4.0 * 4.0;
    const double I_0 = area * area * 0.25 * M_1_PI;
    CHECK(double(blocks::get<tags::BendingTwistRigidityMatrix>(model)) ==
          approx(4.0 * I_0));
    CHECK(double(blocks::get<tags::ShearStretchRigidityMatrix>(model)) ==
          approx(4.0 * area * 4.0 / 3.0));

    // 1 + whatever
    CHECK(approx(model.compute_internal_forces()) == 2.00);
    CHECK(approx(model.compute_internal_torques()) == 3.0);
  }

}  // namespace
