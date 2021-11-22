//
// Created by Tejaswin Parthasarathy on 4/20/21.
//

#include "Framework/TestingFramework.hpp"
#include "Systems/CosseratRods/CosseratRodTraits.hpp"
#include "Systems/CosseratRods/Components/Elasticity/detail/ElasticityInterface.hpp"
#include "Systems/CosseratRods/Components/helpers.hpp"
///
#include "Systems/CosseratRods/Mocks.hpp"
///
#include "Utilities/MakeArray.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TMPLDebugging.hpp"

namespace {
  using namespace elastica::cosserat_rod::component::detail;
  using namespace elastica::cosserat_rod::component;

  using Traits = typename elastica::cosserat_rod::DefaultCosseratRodTraits;

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
    using Traits = CRT;
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
            TestElasticityInterface> {
      //TypeDisplayer<> rd{};
    };
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
    void compute_internal_modeled_loads_impl() {
      blocks::get<elastica::tags::InternalLoads>(self()) = 3.14;
      //return 3.14;
    }

    void compute_internal_modeled_torques_impl() {
      blocks::get<elastica::tags::InternalTorques>(self()) = 42.0;
      //return 42.0;
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
            TestWithDataElasticityImplementation<Traits,
                                                 B>>::
            template with<TestElasticityInterface> {};

}  // namespace

namespace {

  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.ElasticityInterface.data",
                     "[Systems][Unit]") {
    using Plugin = mock::MockPlugin<Traits, TestDataElasticityModel>;
    using Model = blocks::Block<Plugin>;
    Model model{};
    // using ElasiticityModel = TestDataElasticityModel<Model>;

    CHECK_EBO(Model);
    // static_assert(ElasiticityModel::parent_need_not_be_initialized(),
    //               "Initialization logic error!");

    CHECK(model.compute_internal_forces() == 3.14);
    CHECK(model.compute_internal_torques() == 42.0);

    auto initializer = mock::initialize_block<Plugin>(
        // from before
//        blocks::initialize<elastica::tags::Position>([&i = n_times_called[0]](std::size_t index) {
//          ++i;
//          return index * 1.0;
//          return 2.0;
//        })
    );
    Model::initialize(model, std::move(initializer));

    CHECK(blocks::get<Model::InternalLoads>(model) == 0.0);
    CHECK(blocks::get<Model::InternalTorques>(model) == 0.0);
    // a custom variable gets passed on
    CHECK(blocks::get<Model::TestVar>(model) == 0.0);
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

} // namespace
