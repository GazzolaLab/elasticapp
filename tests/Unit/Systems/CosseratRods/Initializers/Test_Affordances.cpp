//
// Created by Tejaswin Parthasarathy on 10/28/21.
//
#include <iostream>

#include "Framework/TestingFramework.hpp"
#include "Systems/CosseratRods/Initializers/detail/Affordances.hpp"

// for testing whether it makes the correct initializer
#include "Systems/CosseratRods/Block.hpp"
#include "Systems/CosseratRods/BlockInitializer.hpp"
#include "Systems/CosseratRods/CosseratRodPlugin.hpp"
#include "Systems/CosseratRods/Mocks.hpp"
//

namespace {
  using namespace elastica;
  using namespace elastica::cosserat_rod::detail;
}  // namespace

namespace {

  struct MockInitializer : public EnableCopyInitialization<MockInitializer> {
    using Cache = std::tuple<int, float>;
    MockInitializer(int i, float f) : cache_(i, f){};
    auto data() const& noexcept -> Cache const& { return cache_; }
    auto data() & noexcept -> Cache& { return cache_; }

    Cache cache_;
  };

  ELASTICA_TEST_CASE(
      "Unit.Systems.CosseratRods.Initializers.EnableCopyInitialization",
      "[Systems][Unit]") {
    MockInitializer mockInitializer(1, 2.0F);
    CHECK(mockInitializer.data() == std::make_tuple(1, 2.0F));

    auto anotherMockInitializer = mockInitializer.but_with(56);
    CHECK(anotherMockInitializer.data() == std::make_tuple(56, 2.0F));

    auto yetanotherMockInitializer = mockInitializer.but_with(5.6F);
    CHECK(yetanotherMockInitializer.data() == std::make_tuple(1, 5.6F));
  }

  template <typename Tag, typename DataType>
  struct TagInitializer {
    using TagType = Tag;
    DataType d;

    friend bool operator==(const TagInitializer& a, const TagInitializer& b) {
      return a.d == b.d;
    }
  };

  template <typename Tag, typename DataType>
  std::ostream& operator<<(std::ostream& os,
                           const TagInitializer<Tag, DataType>& t) noexcept {
    os << t.d;
    return os;
  }

  struct FirstTag;
  struct SecondTag;
  struct ThirdTag;

  struct AnotherMockInitializer
      : public EnableOptionalParameterInitialization<AnotherMockInitializer> {
    using P = EnableOptionalParameterInitialization<AnotherMockInitializer>;
    using D1 = TagInitializer<FirstTag, int>;
    using D2 = TagInitializer<SecondTag, float>;
    using D3 = TagInitializer<ThirdTag, char>;

    using DefaultInitializers = tmpl::list<D1, D2, D3>;
    using Cache = tmpl::as_tuple<DefaultInitializers>;
    // using default_initializers = tmpl::list<>;

    AnotherMockInitializer() : cache_(D1{0}, D2{0.0F}, D3{'a'}){};
    auto default_initializers() const& noexcept -> Cache const& {
      return cache_;
    }
    auto default_initializers() & noexcept -> Cache& { return cache_; }

   public:
    using P::get_optional_parameters;

    Cache cache_;
  };

  ELASTICA_TEST_CASE(
      "Unit.Systems.CosseratRods.Initializers.EnableOptionalParameters",
      "[Systems][Unit]") {
    AnotherMockInitializer another_mock_initializer;
    CHECK(another_mock_initializer.default_initializers() ==
          std::make_tuple(AnotherMockInitializer::D1{0},
                          AnotherMockInitializer::D2{0.0F},
                          AnotherMockInitializer::D3{'a'}));

    INFO(std::get<0UL>(another_mock_initializer.get_optional_parameters()));
    INFO(std::get<1UL>(another_mock_initializer.get_optional_parameters()));
    INFO(std::get<2UL>(another_mock_initializer.get_optional_parameters()));
    CHECK(another_mock_initializer.get_optional_parameters() ==
          std::make_tuple(AnotherMockInitializer::D1{0},
                          AnotherMockInitializer::D2{0.0F},
                          AnotherMockInitializer::D3{'a'}));

    CHECK(another_mock_initializer.get_optional_parameters(
              AnotherMockInitializer::D1{5}) ==
          std::make_tuple(AnotherMockInitializer::D1{5},
                          AnotherMockInitializer::D2{0.0F},
                          AnotherMockInitializer::D3{'a'}));

    CHECK(another_mock_initializer.get_optional_parameters(
              AnotherMockInitializer::D2{5.0F}) ==
          std::make_tuple(AnotherMockInitializer::D2{5.0F},
                          AnotherMockInitializer::D1{0},
                          AnotherMockInitializer::D3{'a'}));

    CHECK(another_mock_initializer.get_optional_parameters(
              AnotherMockInitializer::D3{'e'}) ==
          std::make_tuple(AnotherMockInitializer::D3{'e'},
                          AnotherMockInitializer::D1{0},
                          AnotherMockInitializer::D2{0.0F}));
  }

  // copied from Test_BlockInitializer in cosserat rod
  namespace {

    struct SomeVectorVariableTag;
    struct SomeScalarVariableTag;

    template <typename Traits, typename>
    struct NonEmptyComponent {
      struct SomeVectorVariable
          : public Traits::template CosseratRodInitializedVariable<
                SomeVectorVariableTag,              //
                typename Traits::DataType::Vector,  //
                typename Traits::Place::OnNode> {};

      struct SomeScalarVariable
          : public Traits::template CosseratRodInitializedVariable<
                SomeScalarVariableTag,              //
                typename Traits::DataType::Scalar,  //
                typename Traits::Place::OnNode> {};

      using InitializedVariables =
          tmpl::list<SomeVectorVariable, SomeScalarVariable>;
      using ComputedVariables = tmpl::list<>;
      using Variables = tmpl::list<>;

      template <class DownstreamBlock, class CosseratInitializer>
      static void initialize(DownstreamBlock&, CosseratInitializer&&) {}
    };

    template <template <typename, typename> class... T>
    using GenerateTestPlugin = ::elastica::cosserat_rod::CosseratRodPlugin<
        mock::MinimalCosseratRodTraits, blocks::Block, T...>;

    template <typename Plugin>
    using GenerateBlock = blocks::Block<Plugin>;
  }  // namespace

  struct MockInterfaceInitializer
      : public InitializationInterface<MockInterfaceInitializer> {
    decltype(auto) get_required_parameters() const noexcept {
      // pass directly as references
      return std::forward_as_tuple(
          blocks::initialize<tags::NElement>([](...) { return 20UL; }),
          blocks::initialize<SomeVectorVariableTag>([](...) { return 1.0; }));
    }

    decltype(auto) default_initializers() const noexcept {
      return std::forward_as_tuple(
          blocks::initialize<SomeScalarVariableTag>([](...) { return 2.0; }));
    }

    template <typename ExtraInitializer>
    decltype(auto) get_optional_parameters(
        ExtraInitializer&& extra_initializers) const noexcept {
      return std::forward_as_tuple(
          std::forward<ExtraInitializer>(extra_initializers));
    }
  };

  ELASTICA_TEST_CASE(
      "Unit.Systems.CosseratRods.Initializers.InitializationInterface",
      "[Systems][Unit]") {
    using TestCosseratPlugin = GenerateTestPlugin<NonEmptyComponent>;

    MockInterfaceInitializer mock_initializer;
    SECTION("Without custom initialization") {
      auto initializer = mock_initializer.initialize<TestCosseratPlugin>();

      CHECK(initializer.n_elems() == 20UL);
      std::size_t dummy_index = 1UL;
      // initializer should always be moved1
      CHECK(blocks::get<SomeVectorVariableTag>(std::move(initializer))(
                dummy_index) == approx(1.0));
      CHECK(blocks::get<SomeScalarVariableTag>(std::move(initializer))(
                dummy_index) == approx(2.0));
    }
    SECTION("With custom initialization") {
      auto initializer = mock_initializer.initialize<TestCosseratPlugin>(
          blocks::initialize<SomeScalarVariableTag>([](...) { return 4.0; }));

      CHECK(initializer.n_elems() == 20UL);
      std::size_t dummy_index = 1UL;
      // initializer should always be moved1
      CHECK(blocks::get<SomeVectorVariableTag>(std::move(initializer))(
                dummy_index) == approx(1.0));
      CHECK(blocks::get<SomeScalarVariableTag>(std::move(initializer))(
                dummy_index) == approx(4.0));
    }

//    struct VelocityInitializer {
//      using TagType = tags::Velocity;
//      static double initialize(std::size_t index) { return 1.0; }
//      using type = decltype(blocks::initialize<TagType>(initialize));
//      type data = blocks::initialize<TagType>(initialize);
//    };
//    VelocityInitializer v{};
//    CHECK(v.data.get()(2UL) == 1.0);
  }

}  // namespace