//
// Created by Tejaswin Parthasarathy on 10/29/21.
//
#include "Framework/TestingFramework.hpp"
#include "Systems/CosseratRods/Initializers/ComposeInitializers.hpp"
#include "Systems/CosseratRods/Initializers/detail/Common.hpp"
#include "Systems/CosseratRods/Initializers/detail/Geometry.hpp"
#include "Systems/CosseratRods/Initializers/detail/Kinematics.hpp"
#include "Systems/CosseratRods/Initializers/detail/Transformations.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"

// depends on Affordances passing

namespace {
  using namespace elastica::cosserat_rod;
  using namespace elastica::cosserat_rod::detail;
}  // namespace

namespace {

  struct VelocityDefaultParameter {
    using TagType = ::elastica::tags::Velocity;
    static auto initialize(...) -> double { return 0.0; }
    using type = decltype(blocks::initialize<TagType>(initialize));
    type data = blocks::initialize<TagType>(initialize);
  };

  class TestInitializer {
   public:
    using Radius = option_tags::Radius;

    using RequiredParameters = tmpl::list<Radius>;
    using DefaultParameters = tmpl::list<VelocityDefaultParameter>;

   protected:
    template <typename OptionsTuple>
    decltype(auto) get_required_parameters(
        OptionsTuple const& options_cache) const noexcept {
      // for things that don't need to be processed, pass as wrapped
      return pass_as_wrapped(options_cache, tmpl::list<Radius>{});
    }
  };

  class TestComposeInitializers
      : public ComposeInitializers<CommonInitializerToAllCosseratRods,
                                   TestInitializer> {
    using P = ComposeInitializers<CommonInitializerToAllCosseratRods,
                                  TestInitializer>;
    using P::P;

   public:
    using P::get_required_parameters;
  };

  void test_compose_initializer() {
    TestComposeInitializers initializer{
        TestComposeInitializers::NElement{20UL},
        TestComposeInitializers::Material{::elastica::iron},
        TestComposeInitializers::Radius{0.5}};
    static_assert(
        cpp17::is_same_v<typename TestComposeInitializers::RequiredParameters,
                         tmpl::list<TestComposeInitializers::NElement,
                                    TestComposeInitializers::Material,
                                    TestComposeInitializers::Radius>>,
        "!");
    static_assert(
        cpp17::is_same_v<typename TestComposeInitializers::DefaultParameters,
                         tmpl::list<VelocityDefaultParameter>>,
        "!");

    const std::size_t dummy_index = 1UL;
    // a tuple of functions depending on idnex
    auto required_params = initializer.get_required_parameters();

    constexpr std::size_t nelems = 0UL;
    constexpr std::size_t material = 1UL;
    constexpr std::size_t other = 2UL;

    CHECK(std::get<nelems>(required_params).get()(dummy_index) == 20UL);
    CHECK(std::get<material>(required_params).get()(dummy_index) ==
          ::elastica::iron);
    CHECK(std::get<other>(required_params).get()(dummy_index) == approx(0.5));

    // velocity is nil
    CHECK(
        std::get<0UL>(initializer.default_initializers()).get()(dummy_index) ==
        0.0);
  }

}  // namespace

namespace {

  class AnotherTestInitializer {
   public:
    using Radius = option_tags::Radius;

    using RequiredParameters = tmpl::list<Radius>;
    using DefaultParameters = tmpl::list<>;

   protected:
    // This time don't pass as wrapped but do it differently
    template <typename OptionsTuple>
    decltype(auto) get_required_parameters(
        OptionsTuple const& options_cache) const noexcept {
      // for things that don't need to be processed, pass as wrapped
      return std::make_tuple(blocks::initialize<typename Radius::TagType>(
          [r = std::get<Radius>(options_cache).value()](std::size_t index) {
            return static_cast<double>(index) + r;
          }));
    }
  };

  class AnotherTestComposeInitializers
      : public ComposeInitializers<CommonInitializerToAllCosseratRods,
                                   AnotherTestInitializer> {
    using P = ComposeInitializers<CommonInitializerToAllCosseratRods,
                                  AnotherTestInitializer>;
    using P::P;

   public:
    using P::get_required_parameters;
  };

  void another_test_compose_initializer() {
    AnotherTestComposeInitializers initializer{
        AnotherTestComposeInitializers::NElement{20UL},
        AnotherTestComposeInitializers::Material{::elastica::iron},
        AnotherTestComposeInitializers::Radius{0.5}};
    static_assert(
        cpp17::is_same_v<
            typename AnotherTestComposeInitializers::RequiredParameters,
            tmpl::list<AnotherTestComposeInitializers::NElement,
                       AnotherTestComposeInitializers::Material,
                       AnotherTestComposeInitializers::Radius>>,
        "!");
    static_assert(
        cpp17::is_same_v<
            typename AnotherTestComposeInitializers::DefaultParameters,
            tmpl::list<>>,
        "!");

    const std::size_t dummy_index = 1UL;

    constexpr std::size_t nelems = 0UL;
    constexpr std::size_t material = 1UL;
    constexpr std::size_t other = 2UL;

    // a tuple of functions depending on idnex
    CHECK(std::get<nelems>(initializer.get_required_parameters())
              .get()(dummy_index) == 20UL);

    CHECK(std::get<other>(initializer.get_required_parameters()).get()(0UL) ==
          approx(0.5));
    CHECK(std::get<other>(initializer.get_required_parameters())
              .get()(dummy_index) == approx(1.5));
    CHECK(std::get<other>(initializer.get_required_parameters()).get()(10UL) ==
          approx(10.5));
  }

}  // namespace

namespace {

  ELASTICA_TEST_CASE(
      "Unit.Systems.CosseratRods.Initializers.ComposeInitializers",
      "[Systems][Unit]") {
    // using T = typename StraightCosseratRod::NElement;

    SECTION("Compose Initializer without custom lambdas") {
      test_compose_initializer();
    }
    SECTION("Compose Initializer with custom lambdas") {
      another_test_compose_initializer();
    }
  }

}  // namespace
