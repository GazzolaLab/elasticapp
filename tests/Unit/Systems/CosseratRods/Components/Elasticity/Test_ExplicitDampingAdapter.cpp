//
// Created by Tejaswin Parthasarathy on 4/29/21.
//
#include "Framework/TestingFramework.hpp"
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

    auto compute_internal_modeled_loads_impl() const {
      // blocks::get<InternalLoads>(self()) = 3.14;
      return 3.14;
    }

    auto compute_internal_modeled_torques_impl() const {
      // blocks::get<InternalTorques>(self()) = 42.0;
      return 42.0;
    }
  };

  template <typename CRT,                 // Cosserat Rod Traits
            typename ComputationalBlock>  // Block
  class TestElasticityModelWithInterface
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

  template <typename B>
  using TestGeometry =
      component::WithCircularCosseratRod<mock::BasicCosseratRodTraits, B>;

  template <typename B>
  using TestElasticity =
      TestElasticityModelWithInterface<mock::BasicCosseratRodTraits, B>;

  using Plugin = mock::MockPlugin<TestGeometry, TestElasticity>;
  using Model = typename Plugin::Model;

  template <typename B>
  using TestDampedElasticity =
      TestElasticityModelWithDamping<mock::BasicCosseratRodTraits, B>;

  using DampedPlugin = mock::MockPlugin<TestGeometry, TestDampedElasticity>;
  using DampedModel = typename DampedPlugin::Model;

}  // namespace

namespace {

  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.ExplicitDampingAdapter.data",
                     "[Systems][Unit]") {
    Model model{};
    DampedModel damped_model{};

    static_assert(
        sizeof(Model) == sizeof(double) * tmpl::size<Plugin::Variables>::value,
        "EBO failure!");
    static_assert(
        sizeof(DampedModel) ==
            sizeof(double) * tmpl::size<DampedPlugin::Variables>::value,
        "EBO failure!");

    CHECK(approx(model.compute_internal_forces()) == 3.14);
    CHECK(approx(model.compute_internal_torques()) == 42.0);

    CHECK(approx(damped_model.compute_internal_forces()) == 4.14);
    CHECK(approx(damped_model.compute_internal_torques()) == 43.0);

    // DampedModel::initialize(damped_model, []() {});
  }

}  // namespace
