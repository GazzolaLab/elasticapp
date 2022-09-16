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

   protected:
    using InitializedVariables = tmpl::list<>;
    using ComputedVariables = InitializedVariables;
    using Variables = ComputedVariables;

    template <typename DownstreamBlock, typename CosseratInitializer>
    static void initialize(DownstreamBlock& downstream_block,
                           CosseratInitializer&& initializers) {}

   public:
    using CRTP::self;
  };

  template <typename CRT, typename B>
  void compute_internal_modeled_loads_impl(
      TestElasticityModel<CRT, B>& block_like) {
    auto& internal_load =
        blocks::get<elastica::tags::InternalLoads>(block_like.self());
    internal_load = 3.14;
  }

  template <typename CRT, typename B>
  void compute_internal_modeled_torques_impl(
      TestElasticityModel<CRT, B>& block_like) {
    auto& internal_torque =
        blocks::get<elastica::tags::InternalTorques>(block_like.self());
    internal_torque = 42.0;
  }
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

  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.ExplicitDampingAdapter.EBO",
                     "[Systems][Unit]") {
    EMPTY_TEST();

    CHECK_EBO(Model);
    CHECK_EBO(DampedModel);
  }

  ELASTICA_TEST_CASE(
      "Unit.Systems.CosseratRods.ExplicitDampingAdapter.compute_forces_torques",
      "[Systems][Unit]") {
    Model model{};

    auto initializer = mock::initialize_block<Plugin>(
        // FIXME : Remove 10UL taken from mock::initialize_block
        blocks::initialize<tags::NElement>([]() { return 10UL; }),
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

    // FIXME : https://github.com/tp5uiuc/elasticapp/issues/460
    // Don't use initialize on model
    Model::initialize(model, std::move(initializer));

    CHECK(blocks::get<::elastica::tags::InternalLoads>(model) != approx(3.14));
    compute_internal_forces(model);
    CHECK(blocks::get<::elastica::tags::InternalLoads>(model) == approx(3.14));

    CHECK(blocks::get<::elastica::tags::InternalTorques>(model) !=
          approx(42.0));
    compute_internal_torques(model);
    CHECK(blocks::get<::elastica::tags::InternalTorques>(model) ==
          approx(42.0));
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Systems.CosseratRods.ExplicitDampingAdapter.compute_forces_torques_with_damping",
      "[Systems][Unit]") {
    // clang-format on
    DampedModel model{};

    auto initializer = mock::initialize_block<DampedPlugin>(
        // FIXME : Remove 10UL taken from mock::initialize_block
        blocks::initialize<tags::NElement>([]() { return 10UL; }),
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
        blocks::initialize<tags::ForceDampingRate>([](...) { return 4.0; }),
        blocks::initialize<tags::TorqueDampingRate>([](...) { return 4.0; }),
        // Kinematics Variables
        // blocks::initialize<tags::Mass>([](...) { return 1.0; }),
        blocks::initialize<tags::Material>(
            [](...) { return ::elastica::iron; }),
        blocks::initialize<tags::Velocity>([](...) { return 0.0; }),
        blocks::initialize<tags::AngularVelocity>([](...) { return 0.0; }),
        blocks::initialize<tags::Acceleration>([](...) { return 0.0; }),
        blocks::initialize<tags::AngularAcceleration>([](...) { return 0.0; }));

    // FIXME : https://github.com/tp5uiuc/elasticapp/issues/460
    // Don't use initialize on model
    DampedModel::initialize(model, std::move(initializer));

    // TODO: Need to initialize velocity(at least) to resolve
    CHECK(blocks::get<::elastica::tags::InternalLoads>(model) != approx(3.14));
    compute_internal_forces(model);
    CHECK(blocks::get<::elastica::tags::InternalLoads>(model) == approx(3.14));

    CHECK(blocks::get<::elastica::tags::InternalTorques>(model) !=
          approx(42.0));
    compute_internal_torques(model);
    CHECK(blocks::get<::elastica::tags::InternalTorques>(model) ==
          approx(42.0));
  }

}  // namespace
