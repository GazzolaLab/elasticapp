//
// Created by Tejaswin Parthasarathy on 4/20/21.
//

#include "Framework/TestingFramework.hpp"
#include "Systems/CosseratRods/Components/Elasticity/detail/ElasticityInterface.hpp"
#include "Systems/CosseratRods/Components/helpers.hpp"
///
#include "Systems/CosseratRods/Mocks.hpp"
///
#include "Utilities/MakeArray.hpp"
#include "Utilities/TMPL.hpp"

namespace {
  using namespace elastica::cosserat_rod::component::detail;
  using namespace elastica::cosserat_rod::component;

  // specialize for testing
  template <typename, typename, typename>
  class TestElasticityInterface;

  template <
      typename CRT,                 // Cosserat Rod Traits
      typename ComputationalBlock,  // Constitutive Model
      template <typename /*CRT*/, typename /* ComputationalBlock*/, typename...>
      class ElasticityModelParam,
      typename... ElasticityModelMetaArgs>
  class TestElasticityInterface<
      CRT, ComputationalBlock,
      ElasticityModelParam<CRT, ComputationalBlock, ElasticityModelMetaArgs...>>
      : public ElasticityInterface<
            CRT, ComputationalBlock,
            ElasticityModelParam<CRT, ComputationalBlock,
                                 ElasticityModelMetaArgs...>> {
   public:
    using Parent =
        ElasticityInterface<CRT, ComputationalBlock,
                            ElasticityModelParam<CRT, ComputationalBlock,
                                                 ElasticityModelMetaArgs...>>;
    TestElasticityInterface() = default;
    /*
    using Parent::initialize;
    using Parent::parent_need_not_be_initialized;
    using Parent::parent_to_be_initialized;
    */
    // brings this to public scope
    using typename Parent::ComputedVariables;
    using typename Parent::InitializedVariables;
    using typename Parent::Variables;

    // brings this to public scope
    using typename Parent::InternalLoads;
    using typename Parent::InternalTorques;
  };

}  // namespace

namespace {

  template <typename C, typename B>
  class TestNoDataElasticityImplementation
      : public Component<TestNoDataElasticityImplementation<C, B>> {
   protected:
    using InitializedVariables = tmpl::list<>;
    using ComputedVariables = InitializedVariables;
    using Variables = ComputedVariables;

    double compute_internal_modeled_loads_impl() {
      ++i;
      return 0.0;
    }

    double compute_internal_modeled_torques_impl() {
      --i;
      return 0.0;
    }

    template <typename DownstreamBlock, typename CosseratInitializer>
    static void initialize(DownstreamBlock&, CosseratInitializer&&) {}

   public:
    int i{0};
  };

  struct DummyBlock {};
  // no data
  template <typename C, typename B>
  class TestNoDataElasticityModel
      : public Adapt<TestNoDataElasticityImplementation<C, B>>::template with<
            TestElasticityInterface> {};

}  // namespace

namespace {

  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.ElasticityInterface.no_data",
                     "[Systems][Unit]") {
    using Model =
        TestNoDataElasticityModel<mock::BasicCosseratRodTraits, DummyBlock>;
    Model model{};

    // static_assert(Model::parent_need_not_be_initialized(),
    //               "Initialization logic error!");

    model.compute_internal_forces();
    CHECK(model.i == 1);
    model.compute_internal_torques();
    CHECK(model.i == 0);
  }

}  // namespace

namespace {

  struct TestVar;

  template <typename C, typename B>
  class TestWithDataElasticityImplementation
      : public ::elastica::CRTPHelper<B, TestWithDataElasticityImplementation>,
        public Component<TestWithDataElasticityImplementation<C, B>> {
   private:
    using Traits = C;
    using CRTP =
        ::elastica::CRTPHelper<B, TestWithDataElasticityImplementation>;
    using CRTP::self;

   public:
    struct TestVar : public Traits::template CosseratRodVariable<
                         TestVar,                            //
                         typename Traits::DataType::Vector,  //
                         typename Traits::Place::OnElement> {};

   protected:
    using InitializedVariables = tmpl::list<>;
    using ComputedVariables = tmpl::list<TestVar>;
    using Variables = ComputedVariables;

   protected:
    auto compute_internal_modeled_loads_impl() {
      // blocks::get<InternalLoads>(self()) = 3.14;
      return 3.14;
    }

    auto compute_internal_modeled_torques_impl() {
      // blocks::get<InternalTorques>(self()) = 42.0;
      return 42.0;
    }

    template <typename DownstreamBlock, typename CosseratInitializer>
    static void initialize(DownstreamBlock& downstream_block,
                           CosseratInitializer&&) {
      blocks::get<TestVar>(downstream_block) = 0.0;
    }
  };

  // no data
  template <typename B>
  class TestDataElasticityModel
      : public elastica::cosserat_rod::component::Adapt<
            TestWithDataElasticityImplementation<mock::BasicCosseratRodTraits,
                                                 B>>::
            template with<TestElasticityInterface> {};

}  // namespace

namespace {

  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.ElasticityInterface.data",
                     "[Systems][Unit]") {
    using Plugin = mock::MockPlugin<TestDataElasticityModel>;
    using Model = blocks::Block<Plugin>;
    Model model{};
    // using ElasiticityModel = TestDataElasticityModel<Model>;

    static_assert(
        sizeof(Model) == sizeof(double) * tmpl::size<Plugin::Variables>::value,
        "EBO failure!");
    // static_assert(ElasiticityModel::parent_need_not_be_initialized(),
    //               "Initialization logic error!");

    CHECK(approx(model.compute_internal_forces()) == 3.14);
    CHECK(approx(model.compute_internal_torques()) == 42.0);

    Model::initialize(model, []() {});
    CHECK(approx(blocks::get<Model::InternalLoads>(model)) == 0.0);
    CHECK(approx(blocks::get<Model::InternalTorques>(model)) == 0.0);
    // a custom variable gets passed on
    CHECK(approx(blocks::get<Model::TestVar>(model)) == 0.0);
  }

}  // namespace

namespace {

  struct TestVar;

  template <typename C, typename B>
  class TestWithInitializerElasticityImplementation
      : public Component<TestWithInitializerElasticityImplementation<C, B>> {
   private:
    using Traits = C;

   public:
    struct TestVariable : public Traits::template CosseratRodVariable<
                              TestVar,                            //
                              typename Traits::DataType::Vector,  //
                              typename Traits::Place::OnElement> {};

   protected:
    // can optionally skip
    using InitializedVariables = tmpl::list<TestVariable>;
    using ComputedVariables = tmpl::list<>;
    using Variables = InitializedVariables;

   protected:
    auto compute_internal_modeled_loads_impl() {
      // blocks::get<InternalLoads>(self()) = 3.14;
      return 3.14;
    }

    auto compute_internal_modeled_torques_impl() {
      // blocks::get<InternalTorques>(self()) = 42.0;
      return 42.0;
    }

    template <typename DownstreamBlock, typename CosseratInitializer>
    static void initialize(DownstreamBlock& downstream_block,
                           CosseratInitializer&& initializer) {
      //      auto&& initializer =
      //          blocks::get<TestVar>(std::forward<CosseratInitializer>(initializers));
      blocks::get<TestVariable>(downstream_block) = initializer(0UL);
    }
  };

  // no data
  template <typename B>
  class TestInitializedElasticityModel
      : public Adapt<TestWithInitializerElasticityImplementation<
            mock::BasicCosseratRodTraits,
            B>>::template with<TestElasticityInterface> {};

}  // namespace

namespace {

  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.ElasticityInterface.initialzed",
                     "[Systems][Unit]") {
    using Plugin = mock::MockPlugin<TestInitializedElasticityModel>;
    using Model = blocks::Block<Plugin>;
    // using ElasticityModel = typename Model::ElasticityModel;
    // using ElasticityModelWithInterface =
    // TestInitializedElasticityModel<Model>;
    Model model{};
    //    using IV = typename ElasticityModel::InitializedVariables;

    // static_assert(ElasticityModelWithInterface::parent_to_be_initialized(),
    //               "Initialization logic error!");

    static_assert(
        sizeof(Model) == sizeof(double) * tmpl::size<Plugin::Variables>::value,
        "EBO failure!");

    CHECK(approx(model.compute_internal_forces()) == 3.14);
    CHECK(approx(model.compute_internal_torques()) == 42.0);

    constexpr auto n_initializers = 1UL;
    auto n_times_called = make_array<n_initializers>(0U);
    auto initializer = [&i = n_times_called[0]](...) {
      ++i;
      return 2.0;
    };
    Model::initialize(model, std::move(initializer));

    // check if initialization is done as expected
    CHECK_ITERABLE_APPROX(n_times_called, make_array<n_initializers>(1U));

    CHECK(approx(blocks::get<Model::InternalLoads>(model)) == 0.0);
    CHECK(approx(blocks::get<Model::InternalTorques>(model)) == 0.0);
    // a custom variable gets passed on
    CHECK(approx(blocks::get<Model::TestVariable>(model)) == 2.0);
  }

}  // namespace
