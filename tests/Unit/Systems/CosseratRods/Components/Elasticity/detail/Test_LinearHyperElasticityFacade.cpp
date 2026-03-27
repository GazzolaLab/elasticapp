//
// Created by Tejaswin Parthasarathy on 4/20/21.
//

#include "Framework/TestingFramework.hpp"
#include "Systems/CosseratRods/Components/Elasticity/detail/LinearHyperElasticityFacade.hpp"
#include "Systems/CosseratRods/Components/Geometry.hpp"
#include "Systems/CosseratRods/CosseratRodTraits.hpp"
#include "Systems/CosseratRods/Tags.hpp"
///
#include "Systems/CosseratRods/Mocks.hpp"
///
#include "Utilities/CRTP.hpp"
#include "Utilities/MakeArray.hpp"
#include "Utilities/Math/Rot3.hpp"
#include "Utilities/Math/Sqrt.hpp"

namespace {
  //  Test static operations first
  using namespace elastica;
  using namespace elastica::cosserat_rod::component;
  using namespace elastica::cosserat_rod::component::detail;

  using Traits = typename elastica::cosserat_rod::DefaultCosseratRodTraits;

  template <typename B>
  struct TestGeometry : public WithCircularCosseratRod<Traits, B> {};

  template <typename B>
  struct TestElasticity : public LinearHyperElasticityFacade<Traits, B> {};

  using Plugin = mock::MockPlugin<Traits, TestGeometry, TestElasticity>;
  using Model = typename Plugin::Model;

  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.LinearHyperElasticityBase.EBO",
                     "[Systems][Unit]") {
    EMPTY_TEST();

    CHECK_EBO(Model);
  }

  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.LinearHyperElasticityBase.data",
                     "[Systems][Unit]") {
    // same as cross_section_interface
    // CHECK(model.get_element_dimension() == 0.0);
    // CHECK(model.get_element_volume() == 0.0);

    const std::size_t n_initializers = 10UL;
    auto n_times_called = make_array<n_initializers>(0U);
    auto initializer = mock::initialize_block<Plugin>(
        // FIXME : Remove 10UL taken from mock::initialize_block
        blocks::initialize<tags::NElement>([]() { return 10UL; }),
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
        blocks::initialize<tags::ElementDimension>(
            [&i = n_times_called[2]](...) {
              ++i;
              return 4.0;
            }),
        // from before
        blocks::initialize<tags::ShearStretchRigidityMatrix>(
            [&i = n_times_called[3]](...) {
              ++i;
              return i;
            }),
        blocks::initialize<tags::BendingTwistRigidityMatrix>(
            [&i = n_times_called[4]](...) {
              ++i;
              return i;
            }),
        blocks::initialize<tags::ReferenceCurvature>(
            [&i = n_times_called[5]](...) {
              ++i;
              return 1.0;
            }),
        blocks::initialize<tags::ReferenceShearStretchStrain>(
            [&i = n_times_called[6]](...) {
              ++i;
              return 1.0;
            }),
        blocks::initialize<tags::ElementDilatation>(
            [&i = n_times_called[7]](...) {
              ++i;
              return 1.0;
            }));

    Model model(std::move(initializer));

    CHECK(model.get_element_dimension() == 4.0);
    CHECK(model.get_element_volume() == 4.0 * 4.0 * M_PI * blaze::sqrt(3));

    // check if initialization is done as expected
    auto expected_call_count =
        make_array(11U, 10U, 10U, 10U, 10U, 9U, 10U, 10U, 0U, 0U);
    CHECK_ITERABLE_APPROX(n_times_called, expected_call_count);

    // [0] element : stiff  * I_0
    const double area = M_PI * 4.0 * 4.0;
    const double I_0 = area * area * 0.25 * M_1_PI;
    auto alpha = model.shape_factor();
    // Rot3 btrm_slice{{I_0, 0, 0}, {0, I_0, 0}, {0, 0, I_0 * 2}};
    Vec3 btrm_slice{I_0, I_0, I_0 * 2};
    Vec3 ssrm_slice{alpha * area, alpha * area, area};

    auto BTRM(blocks::get<elastica::tags::BendingTwistRigidityMatrix>(model));
    auto SSRM(blocks::get<elastica::tags::ShearStretchRigidityMatrix>(model));
    CHECK(BTRM.columns() == n_initializers - 1UL);
    CHECK(SSRM.columns() == n_initializers);
    using DataType = Traits::DataType::Vector;

    for (size_t i = 0UL; i < n_initializers - 1; ++i) {
      CHECK(DataType::slice(BTRM, i) == btrm_slice * (i + 1.5));
    }

    for (size_t i = 0UL; i < n_initializers; ++i) {
      CHECK(DataType::slice(SSRM, i) == ssrm_slice * (i + 1.0));
    }
  }

}  // namespace
