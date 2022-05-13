//
// Created by Tejaswin Parthasarathy on 11/10/21.
//

#include "Framework/TestingFramework.hpp"
#include "Simulator/Materials.hpp"
#include "Systems/CosseratRods/Initializers/ComposeInitializers.hpp"
#include "Systems/CosseratRods/Initializers/detail/Common.hpp"
#include "Systems/CosseratRods/Initializers/detail/Elasticity.hpp"
#include "Utilities/Math/Rot3.hpp"
#include "Utilities/Math/Zero.hpp"

namespace {
  using namespace elastica::cosserat_rod;
  using namespace elastica::cosserat_rod::detail;
  using ::elastica::Rot3;
  using ::elastica::Vec3;
  using ::elastica::zero3;
}  // namespace

namespace {

  class TestLinearHyperElasticModel
      : public ComposeInitializers<CommonInitializerToAllCosseratRods,
                                   LinearHyperElasticModel> {
    using P = ComposeInitializers<CommonInitializerToAllCosseratRods,
                                  LinearHyperElasticModel>;
    using P::P;

   public:
    using P::get_optional_parameters;
    using P::get_required_parameters;
  };

  void test_linear_hyperelastic_initializer() {
    TestLinearHyperElasticModel initializer{
        TestLinearHyperElasticModel::NElement{20UL},
        TestLinearHyperElasticModel::Material{::elastica::iron}};

    // a tuple of functions depending on index
    auto required_params = initializer.get_required_parameters();
    const std::size_t dummy_index = 0UL;

    constexpr std::size_t nelement_initializer = 0UL;
    constexpr std::size_t material_initializer = 1UL;
    constexpr std::size_t bend_twist_initializer = 2UL;
    constexpr std::size_t shear_stretch_initializer = 3UL;

    CHECK(std::get<nelement_initializer>(required_params).get()(dummy_index) ==
          20UL);
    static_assert(cpp17::is_same_v<decltype(std::get<material_initializer>(
                                                required_params)
                                                .get()(dummy_index)),
                                   ::elastica::MaterialID>,
                  "Failure");
    static_assert(cpp17::is_same_v<decltype(std::get<bend_twist_initializer>(
                                                required_params)
                                                .get()(dummy_index)),
                                   Vec3>,
                  "Failure");
    static_assert(cpp17::is_same_v<decltype(std::get<shear_stretch_initializer>(
                                                required_params)
                                                .get()(dummy_index)),
                                   Vec3>,
                  "Failure");
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Systems.CosseratRods.Initializers.Elasticity.LinearHyperelasticModel",
      "[Systems][Unit]") {
    // clang-format on
    test_linear_hyperelastic_initializer();
  }

}  // namespace

namespace {

  class TestLinearHyperElasticModelWithDamping
      : public ComposeInitializers<CommonInitializerToAllCosseratRods,
                                   LinearHyperElasticModelWithDamping> {
    using P = ComposeInitializers<CommonInitializerToAllCosseratRods,
                                  LinearHyperElasticModelWithDamping>;
    using P::P;

   public:
    using P::get_optional_parameters;
    using P::get_required_parameters;
  };

  void test_linear_hyperelastic_damped_initializer() {
    TestLinearHyperElasticModelWithDamping initializer{
        TestLinearHyperElasticModelWithDamping::NElement{20UL},
        TestLinearHyperElasticModelWithDamping::Material{::elastica::iron},
        TestLinearHyperElasticModelWithDamping::ForceDampingRate{0.2},
        TestLinearHyperElasticModelWithDamping::TorqueDampingRate{0.3},
    };

    // a tuple of functions depending on index
    auto required_params = initializer.get_required_parameters();
    const std::size_t dummy_index = 0UL;

    constexpr std::size_t nelement_initializer = 0UL;
    constexpr std::size_t material_initializer = 1UL;
    constexpr std::size_t bend_twist_initializer = 2UL;
    constexpr std::size_t shear_stretch_initializer = 3UL;
    constexpr std::size_t force_damping_initializer = 4UL;
    constexpr std::size_t torque_damping_initializer = 5UL;

    CHECK(std::get<nelement_initializer>(required_params).get()(dummy_index) ==
          20UL);
    CHECK(std::get<material_initializer>(required_params).get()(dummy_index) ==
          ::elastica::iron);
    static_assert(cpp17::is_same_v<decltype(std::get<bend_twist_initializer>(
                                                required_params)
                                                .get()(dummy_index)),
                                   Vec3>,
                  "Failure");
    static_assert(cpp17::is_same_v<decltype(std::get<shear_stretch_initializer>(
                                                required_params)
                                                .get()(dummy_index)),
                                   Vec3>,
                  "Failure");
    CHECK(std::get<force_damping_initializer>(required_params)
              .get()(dummy_index) == 0.2);
    CHECK(std::get<torque_damping_initializer>(required_params)
              .get()(dummy_index) == 0.3);
  }

  void test_linear_hyperelastic_damped_error() {
    SECTION("Force damping not properly initialized") {
      TestLinearHyperElasticModelWithDamping initializer{
          TestLinearHyperElasticModelWithDamping::NElement{20UL},
          TestLinearHyperElasticModelWithDamping::Material{::elastica::iron},
          TestLinearHyperElasticModelWithDamping::ForceDampingRate{-0.2},
          TestLinearHyperElasticModelWithDamping::TorqueDampingRate{0.3},
      };

      // a tuple of functions depending on index
      CHECK_THROWS(initializer.get_required_parameters());
    }
    SECTION("Torque damping not properly initialized") {
      TestLinearHyperElasticModelWithDamping initializer{
          TestLinearHyperElasticModelWithDamping::NElement{20UL},
          TestLinearHyperElasticModelWithDamping::Material{::elastica::iron},
          TestLinearHyperElasticModelWithDamping::ForceDampingRate{0.2},
          TestLinearHyperElasticModelWithDamping::TorqueDampingRate{-0.3},
      };

      // a tuple of functions depending on index
      CHECK_THROWS(initializer.get_required_parameters());
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Systems.CosseratRods.Initializers.Elasticity.LinearHyperelasticModelWithDamping",
      "[Systems][Unit]") {
    // clang-format on
    SECTION("Correct initialization") {
      test_linear_hyperelastic_damped_initializer();
    }
    SECTION("Error testing") { test_linear_hyperelastic_damped_error(); }
  }

}  // namespace
