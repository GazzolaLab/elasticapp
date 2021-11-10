//
// Created by Tejaswin Parthasarathy on 11/9/21.
//

#include "Framework/TestingFramework.hpp"
#include "Simulator/Materials.hpp"
#include "Systems/CosseratRods/Initializers/ComposeInitializers.hpp"
#include "Systems/CosseratRods/Initializers/detail/Common.hpp"
#include "Systems/CosseratRods/Initializers/detail/Geometry.hpp"
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

  class TestStraightRod
      : public ComposeInitializers<CommonInitializerToAllCosseratRods,
                                   GeometryInitializers::StraightRod> {
    using P =
        ComposeInitializers<CommonInitializerToAllCosseratRods, StraightRod>;
    using P::P;

   public:
    using P::get_optional_parameters;
    using P::get_required_parameters;
  };

  void test_straight_rod() {
    TestStraightRod initializer{TestStraightRod::NElement{20UL},
                                TestStraightRod::Material{::elastica::iron},
                                TestStraightRod::Radius{0.2},
                                TestStraightRod::Length{1.0},
                                TestStraightRod::Origin{Vec3{0.0, 0.0, 0.0}},
                                TestStraightRod::Direction{Vec3{0.0, 0.0, 1.0}},
                                TestStraightRod::Normal{Vec3{0.0, 1.0, 0.0}}};

    // a tuple of functions depending on index
    auto required_params = initializer.get_required_parameters();
    const std::size_t dummy_index = 0UL;

    constexpr std::size_t nelement_initializer = 0UL;
    constexpr std::size_t radius_initializer = 1UL;
    constexpr std::size_t position_initializer = 2UL;
    constexpr std::size_t director_initializer = 3UL;

    CHECK(std::get<nelement_initializer>(required_params).get()(dummy_index) ==
          20UL);
    CHECK(std::get<radius_initializer>(required_params).get()(dummy_index) ==
          approx(0.2));
    CHECK(std::get<position_initializer>(required_params).get()(dummy_index) ==
          Vec3{0.0, 0.0, 0.0});
    // n_elements + 1UL
    CHECK(std::get<position_initializer>(required_params).get()(20UL) ==
          Vec3{0.0, 0.0, 1.0});
    const Rot3 expected_director{
        {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}};
    CHECK(std::get<director_initializer>(required_params).get()(dummy_index) ==
          expected_director);
    CHECK(std::get<director_initializer>(required_params).get()(20UL - 5UL) ==
          expected_director);

    auto defaults = initializer.default_initializers();
    constexpr std::size_t ReferenceCurvatureDefaultParameter = 0UL;
    constexpr std::size_t ReferenceShearStretchStrainDefaultParameter = 1UL;
    constexpr std::size_t DilatationDefaultParameter = 2UL;

    // velocity is nil
    CHECK(std::get<ReferenceCurvatureDefaultParameter>(defaults).get()(
              dummy_index) == zero3());
    CHECK(std::get<ReferenceShearStretchStrainDefaultParameter>(defaults).get()(
              dummy_index) == zero3());
    CHECK(std::get<DilatationDefaultParameter>(defaults).get()(dummy_index) ==
          1.0);

    auto replaced = initializer.get_optional_parameters(
        ::blocks::initialize<::elastica::tags::ReferenceCurvature>([](...) {
          return Vec3{1.0, 1.0, 1.0};
        }));
    CHECK(std::get<ReferenceCurvatureDefaultParameter>(replaced).get()(
              dummy_index) == Vec3{1.0, 1.0, 1.0});
    CHECK(std::get<ReferenceShearStretchStrainDefaultParameter>(replaced).get()(
              dummy_index) == zero3());
    CHECK(std::get<DilatationDefaultParameter>(replaced).get()(dummy_index) ==
          1.0);

    auto another_replaced = initializer.get_optional_parameters(
        ::blocks::initialize<::elastica::tags::ReferenceCurvature>([](...) {
          return Vec3{1.0, 1.0, 1.0};
        }),
        ::blocks::initialize<::elastica::tags::ReferenceShearStretchStrain>(
            [](...) {
              return Vec3{2.0, 1.0, 2.0};
            }));
    CHECK(std::get<ReferenceCurvatureDefaultParameter>(another_replaced)
              .get()(dummy_index) == Vec3{1.0, 1.0, 1.0});
    CHECK(
        std::get<ReferenceShearStretchStrainDefaultParameter>(another_replaced)
            .get()(dummy_index) == Vec3{2.0, 1.0, 2.0});
    CHECK(std::get<DilatationDefaultParameter>(replaced).get()(dummy_index) ==
          1.0);
  }

  void test_straight_rod_error() {
    SECTION("Radius not properly initialized") {
      TestStraightRod initializer{
          TestStraightRod::NElement{20UL},
          TestStraightRod::Material{::elastica::iron},
          //
          TestStraightRod::Radius{-0.2},
          //
          TestStraightRod::Length{1.0},
          TestStraightRod::Origin{Vec3{0.0, 0.0, 0.0}},
          TestStraightRod::Direction{Vec3{0.0, 0.0, 1.0}},
          TestStraightRod::Normal{Vec3{0.0, 1.0, 0.0}}};

      // a tuple of functions depending on index
      CHECK_THROWS(initializer.get_required_parameters());
    }
    SECTION("Length not properly initialized") {
      TestStraightRod initializer{
          TestStraightRod::NElement{20UL},
          TestStraightRod::Material{::elastica::iron},
          TestStraightRod::Radius{0.2},
          TestStraightRod::Length{-1.0},
          TestStraightRod::Origin{Vec3{0.0, 0.0, 0.0}},
          TestStraightRod::Direction{Vec3{0.0, 0.0, 1.0}},
          TestStraightRod::Normal{Vec3{0.0, 1.0, 0.0}}};
      CHECK_THROWS(initializer.get_required_parameters());
    }
    SECTION("Direction not properly initialized") {
      TestStraightRod initializer{
          TestStraightRod::NElement{20UL},
          TestStraightRod::Material{::elastica::iron},
          TestStraightRod::Radius{0.2}, TestStraightRod::Length{1.0},
          TestStraightRod::Origin{Vec3{0.0, 0.0, 0.0}},
          //
          TestStraightRod::Direction{Vec3{0.0, 0.0, 0.0}},
          //
          TestStraightRod::Normal{Vec3{0.0, 1.0, 0.0}}};
      CHECK_THROWS(initializer.get_required_parameters());
    }
    SECTION("Normal not properly initialized") {
      TestStraightRod initializer{
          TestStraightRod::NElement{20UL},
          TestStraightRod::Material{::elastica::iron},
          TestStraightRod::Radius{0.2}, TestStraightRod::Length{1.0},
          TestStraightRod::Origin{Vec3{0.0, 0.0, 0.0}},
          TestStraightRod::Direction{Vec3{1.0, 0.0, 0.0}},
          //
          TestStraightRod::Normal{Vec3{0.0, 0.0, 0.0}}};
      CHECK_THROWS(initializer.get_required_parameters());
    }
    SECTION("Direction and normal are not perpendicular") {
      TestStraightRod initializer{
          TestStraightRod::NElement{20UL},
          TestStraightRod::Material{::elastica::iron},
          TestStraightRod::Radius{0.2},
          TestStraightRod::Length{1.0},
          TestStraightRod::Origin{Vec3{0.0, 0.0, 0.0}},
          TestStraightRod::Direction{Vec3{0.8, 0.6, 0.0}},
          TestStraightRod::Normal{Vec3{0.6, 0.8, 0.0}}};
      CHECK_THROWS(initializer.get_required_parameters());
    }
  }

  ELASTICA_TEST_CASE(
      "Unit.Systems.CosseratRods.Initializers.Geometry.StraightRod",
      "[Systems][Unit]") {
    SECTION("Correct initialization") { test_straight_rod(); }
    SECTION("Error testing") { test_straight_rod_error(); }
  }

}  // namespace