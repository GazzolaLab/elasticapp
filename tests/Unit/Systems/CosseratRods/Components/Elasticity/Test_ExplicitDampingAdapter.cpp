//
// Created by Tejaswin Parthasarathy on 4/29/21.
//
#include "Framework/TestingFramework.hpp"
#include "Systems/CosseratRods/Components/Elasticity/ExplicitDampingAdapter.hpp"
#include "Systems/CosseratRods/Components/Elasticity/detail/ElasticityInterface.hpp"
#include "Systems/CosseratRods/Components/Geometry.hpp"
#include "Systems/CosseratRods/Components/Kinematics.hpp"
#include "Systems/CosseratRods/Components/Tags.hpp"
#include "Systems/CosseratRods/Components/helpers.hpp"
#include "Systems/CosseratRods/CosseratRodTraits.hpp"
///
///
#include "Systems/CosseratRods/Mocks.hpp"
///
#include "Utilities/CRTP.hpp"
#include "Utilities/MakeArray.hpp"

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
        public ElasticityComponent<TestElasticityModel<CRT, B>> {
    using CRTP = elastica::CRTPHelper<B, TestElasticityModel>;
    using CRTP::self;

   protected:
    using InitializedVariables = tmpl::list<>;
    using ComputedVariables = InitializedVariables;
    using Variables = ComputedVariables;

    template <typename DownstreamBlock, typename CosseratInitializer>
    static void initialize(DownstreamBlock& downstream_block,
                           CosseratInitializer&& initializers) {}

    void compute_internal_modeled_loads_impl() {
      auto& internal_load = blocks::get<elastica::tags::InternalLoads>(self());
      internal_load = 3.14;
    }

    void compute_internal_modeled_torques_impl() {
      auto& internal_torque =
          blocks::get<elastica::tags::InternalTorques>(self());
      internal_torque = 42.0;
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
      : public Adapt<TestElasticityModel<CRT, ComputationalBlock>>::
            template with<ElasticityInterface,
                          component::ExplicitDampingAdapter> {};

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

  using Traits = DefaultCosseratRodTraits;  // CRT
  template <typename B>
  struct TestGeometry : public WithCircularCosseratRod<Traits, B> {};

  template <typename B>
  struct TestKinematics : public WithRodKinematics<Traits, B> {};

  template <typename B>
  struct TestElasticity
      : public TestElasticityModelWithInterfaceOnly<Traits, B> {};

  using Plugin =
      mock::MockPlugin<Traits, TestGeometry, TestElasticity, TestKinematics>;
  using Model = typename Plugin::Model;

  template <typename B>
  struct TestDampedElasticity : TestElasticityModelWithDamping<Traits, B> {};

  using DampedPlugin = mock::MockPlugin<Traits, TestGeometry,
                                        TestDampedElasticity, TestKinematics>;
  using DampedModel = typename DampedPlugin::Model;

}  // namespace

namespace {

  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.ExplicitDampingAdapter:EBO",
                     "[Systems][Unit]") {
    CHECK_EBO(Model);
    CHECK_EBO(DampedModel);
  }

  ELASTICA_TEST_CASE(
      "Unit.Systems.CosseratRods.ExplicitDampingAdapter:without.damp",
      "[Systems][Unit]") {
    Model model{};

    auto initializer = mock::initialize_block<Plugin>(
        // Geometry Variables
        blocks::initialize<tags::Position>(
            [](std::size_t index) { return index * 1.0; }),
        blocks::initialize<tags::Director>([](...) { return 0.5; }),
        blocks::initialize<tags::ElementDimension>([](...) { return 4.0; }),
        blocks::initialize<tags::ElementDilatation>([](...) { return 1.0; }),
        blocks::initialize<tags::ReferenceCurvature>([](...) { return 1.0; }),
        blocks::initialize<tags::ReferenceShearStretchStrain>(
            [](...) { return 1.0; }),
        // Elasticity Variables
        blocks::initialize<tags::ShearStretchRigidityMatrix>(
            [](...) { return 4.0; }),
        blocks::initialize<tags::BendingTwistRigidityMatrix>(
            [](...) { return 4.0; }),
        // Kinematics Variables
        // blocks::initialize<tags::Mass>([](...) { return 1.0; }),
        blocks::initialize<tags::Material>(
            [](...) { return ::elastica::iron; }),
        blocks::initialize<tags::Velocity>([](...) { return 0.0; }),
        blocks::initialize<tags::AngularVelocity>([](...) { return 0.0; }),
        blocks::initialize<tags::Acceleration>([](...) { return 0.0; }),
        blocks::initialize<tags::AngularAcceleration>([](...) { return 0.0; }));

    Model::initialize(model, std::move(initializer));

    CHECK(model.compute_internal_forces() == 3.14);
    CHECK(model.compute_internal_torques() == 42.0);
  }

  ELASTICA_TEST_CASE(
      "Unit.Systems.CosseratRods.ExplicitDampingAdapter:with.damp",
      "[Systems][Unit]") {
    DampedModel model{};

    auto initializer = mock::initialize_block<DampedPlugin>(
        // Geometry Variables
        blocks::initialize<tags::Position>(
            [](std::size_t index) { return index * 1.0; }),
        blocks::initialize<tags::Director>([](...) { return 0.5; }),
        blocks::initialize<tags::ElementDimension>([](...) { return 4.0; }),
        blocks::initialize<tags::ElementDilatation>([](...) { return 1.0; }),
        blocks::initialize<tags::ReferenceCurvature>([](...) { return 1.0; }),
        blocks::initialize<tags::ReferenceShearStretchStrain>(
            [](...) { return 1.0; }),
        // Elasticity Variables
        blocks::initialize<tags::ShearStretchRigidityMatrix>(
            [](...) { return 4.0; }),
        blocks::initialize<tags::BendingTwistRigidityMatrix>(
            [](...) { return 4.0; }),
        blocks::initialize<tags::ForceDampingCoefficient>(
            [](...) { return 4.0; }),
        blocks::initialize<tags::TorqueDampingCoefficient>(
            [](...) { return 4.0; }),
        // Kinematics Variables
        // blocks::initialize<tags::Mass>([](...) { return 1.0; }),
        blocks::initialize<tags::Material>(
            [](...) { return ::elastica::iron; }),
        blocks::initialize<tags::Velocity>([](...) { return 0.0; }),
        blocks::initialize<tags::AngularVelocity>([](...) { return 0.0; }),
        blocks::initialize<tags::Acceleration>([](...) { return 0.0; }),
        blocks::initialize<tags::AngularAcceleration>([](...) { return 0.0; }));

    DampedModel::initialize(model, std::move(initializer));

    // TODO: Need to initialize velocity(at least) to resolve
    CHECK(model.compute_internal_forces() == 3.14);
    CHECK(model.compute_internal_torques() == 42.0);
  }

}  // namespace
