//
// Created by Tejaswin Parthasarathy on 4/29/21.
//
#include "Framework/TestingFramework.hpp"
#include "Systems/CosseratRods/CosseratRodTraits.hpp"
#include "Systems/CosseratRods/Components/Tags.hpp"
#include "Systems/CosseratRods/Components/Elasticity/ExplicitDampingAdapter.hpp"
#include "Systems/CosseratRods/Components/Elasticity/detail/ElasticityInterface.hpp"
#include "Systems/CosseratRods/Components/Geometry.hpp"
#include "Systems/CosseratRods/Components/helpers.hpp"
///
///
#include "Systems/CosseratRods/Mocks.hpp"
///
#include "Utilities/CRTP.hpp"

namespace {
  using namespace elastica;
  using namespace elastica::tags;
  using namespace elastica::cosserat_rod;
  using namespace elastica::cosserat_rod::component;
  using elastica::cosserat_rod::component::detail::ElasticityInterface;
  // using namespace elastica::cosserat_rod::detail;
  // using namespace elastica::cosserat_rod::component;



  // no data
  template <typename CRT, typename B>
  class TestElasticityModel
      : public elastica::CRTPHelper<B, TestElasticityModel>,
        public Component<TestElasticityModel<CRT, B>> {
    using CRTP = elastica::CRTPHelper<B, TestElasticityModel>;
    using CRTP::self;

   protected:
    using InitializedVariables = tmpl::list<>;
    using ComputedVariables = InitializedVariables;
    using Variables = ComputedVariables;

    void compute_internal_modeled_loads_impl() const {
//      auto& internal_load = blocks::get<elastica::tags::InternalLoads>(self());
//      internal_load = 3.14;
      //return 3.14;
    }

    void compute_internal_modeled_torques_impl() const {
//      auto& internal_torque = blocks::get<elastica::tags::InternalTorques>(self());
//      internal_torque = 42.0;
      // return 42.0;
    }
  };

  template <typename CRT,                 // Cosserat Rod Traits
            typename ComputationalBlock>  // Block
  class TestElasticityModelWithInterfaceOnly
      : public Adapt<TestElasticityModel<CRT, ComputationalBlock>>::
            template with<ElasticityInterface> {};

  template <typename CRT,                 // Cosserat Rod Traits
            typename ComputationalBlock>  // Block
  class TestElasticityModelWithDamping
      : public Adapt<TestElasticityModel<CRT, ComputationalBlock>>::template with<
            ElasticityInterface, component::ExplicitDampingAdapter>{};


  //  template <typename CRT, typename B>
  //  class TestElasticityModelWithDamping
  //      : public component::ExplicitDampingAdapter<CRT, B,
  //      TestElasticityModel> {
  //   private:
  //    using Parent =
  //        component::ExplicitDampingAdapter<CRT, B, TestElasticityModel>;
  //
  //   protected:
  //    using typename Parent::ComputedVariables;
  //    using typename Parent::InitializedVariables;
  //    using typename Parent::Variables;
  //
  //    using Parent::initialize;
  //  };


  using Traits = DefaultCosseratRodTraits;

  template <typename B>
  struct TestGeometry: public WithCircularCosseratRod<Traits, B> {
    using Traits = Traits;
  };

  template <typename B>
  struct TestElasticity: public TestElasticityModelWithInterfaceOnly<Traits, B> {
    using Traits = Traits;
  };

  using Plugin = mock::MockPlugin<TestGeometry, TestElasticity>;
  using Model = typename Plugin::Model;

  template <typename B>
  struct TestDampedElasticity: TestElasticityModelWithDamping<Traits, B>{
    using Traits = Traits;
  };

  using DampedPlugin = mock::MockPlugin<TestGeometry, TestDampedElasticity>;
  using DampedModel = typename DampedPlugin::Model;

}  // namespace

namespace {

  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.ExplicitDampingAdapter.data",
                     "[Systems][Unit]") {
    Model model{};
    DampedModel damped_model{};

    CHECK_EBO(Model);
    CHECK_EBO(DampedModel);

    CHECK(model.compute_internal_forces() == 3.14);
    CHECK(model.compute_internal_torques() == 42.0);

    CHECK(damped_model.compute_internal_forces() == 4.14);
    CHECK(damped_model.compute_internal_torques() == 43.0);

    // DampedModel::initialize(damped_model, []() {});
  }

}  // namespace
