//
// Created by Seung Hyun Kim on 11/9/21.
//

#include <iostream>

#include "Framework/TestingFramework.hpp"
#include "Utilities/MakeArray.hpp"
///
#include "Systems/Block.hpp"
#include "Systems/Block/Protocols.hpp"
#include "Systems/CosseratRods.hpp"
#include "Systems/CosseratRods/CosseratRodPlugin.hpp"
#include "Systems/CosseratRods/Initializers/StraightCosseratRod.hpp" // TODO : replace later
#include "Simulator/Materials.hpp"
#include "Simulator/Materials/Materials.hpp"

#include "blaze/Blaze.h"
#include "blaze_tensor/Blaze.h"

namespace {

  using Traits = elastica::cosserat_rod::DefaultCosseratRodTraits;
  using Plugin = elastica::cosserat_rod::CosseratRodPlugin<
      Traits, blocks::Block,
      elastica::cosserat_rod::component::WithCircularCosseratRod,
      elastica::cosserat_rod::component::
          WithExplicitlyDampedDiagonalLinearHyperElasticModel,
      elastica::cosserat_rod::component::WithRodKinematics>;
  using Model = blocks::Block<Plugin>;

  using real_type = typename Traits::DataType::real_type;

  ELASTICA_TEST_CASE("Unit.z.Integrated.CosseratRods.SimpleInitialization",
                     "[Systems][Integrated]") {
    Model simple_rod{};

    auto initializer = elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
        // Variables
        blocks::initialize<elastica::tags::NElement>(
            [](...) { return 5; }),
        // Geometry Variables
        blocks::initialize<elastica::tags::Position>(
            [](std::size_t index) { return 1.0 * index; }),
        blocks::initialize<elastica::tags::Director>(
            [](...) { return 0.5; }),
        blocks::initialize<elastica::tags::ElementDimension>(
            [](...) { return 4.0; }),
        blocks::initialize<elastica::tags::ReferenceCurvature>(
            [](...) { return 1.0; }),
        blocks::initialize<elastica::tags::ReferenceShearStretchStrain>(
            [](...) { return 1.0; }),
        blocks::initialize<elastica::tags::ShearStretchRigidityMatrix>(
            [](...) { return 4.0; }),
        blocks::initialize<elastica::tags::BendingTwistRigidityMatrix>(
            [](...) { return 4.0; }),
        blocks::initialize<elastica::tags::ElementDilatation>(
            [](...) { return 1.0; }),
        blocks::initialize<elastica::tags::ForceDampingCoefficient>(
            [](...) { return 4.0; }),
        blocks::initialize<elastica::tags::TorqueDampingCoefficient>(
            [](...) { return 1.0; }),
        // Kinematics Variables
        blocks::initialize<elastica::tags::Mass>(
            [](...) { return 1.0; }),
        blocks::initialize<elastica::tags::Velocity>(
            [](...) { return 0.0; }),
        blocks::initialize<elastica::tags::AngularVelocity>(
            [](...) { return 0.0; }),
        blocks::initialize<elastica::tags::Acceleration>(
            [](...) { return 0.0; }),
        blocks::initialize<elastica::tags::AngularAcceleration>(
            [](...) { return 0.0; }));

    auto slicedrod = simple_rod.emplace_back(std::move(initializer));

    CHECK(slicedrod.get_element_dimension() == 4);
  }

  ELASTICA_TEST_CASE("Unit.z.Integrated.CosseratRods.InitializeStraightRod",
                     "[Systems][Integrated]") {
    /*
     * rng_seed = 100
     *  Test initialization step without any additional perturbation
     */
    elastica::MaterialID myMaterial = elastica::create_material(
        7.874, 0.24, 200, 200.0 / 1.24, 200, 1e-10, 1e-10, 0.1, 0.1);
    using Init = ::elastica::cosserat_rod::StraightCosseratRodInitializer;
    Model simple_rod{};
    Init straight_initializer{
        // Initialization Wrapper
        Init::NElement{5UL},
        Init::Material{myMaterial},
        Init::Radius{0.007522},
        Init::Length{0.18},
        Init::Origin{::elastica::Vec3{0.0, 0.0, 0.0}},
        Init::Direction{::elastica::Vec3{0.0, 1.0, 0.0}},
        Init::Normal{::elastica::Vec3{0.0, 0.0, 1.0}},
        Init::ForceDampingCoefficient{0.2},
        Init::TorqueDampingCoefficient{0.2},
    };
    auto slicedrod =
        simple_rod.emplace_back(straight_initializer.initialize<Plugin>());

    // clang-format off
    blaze::DynamicVector<elastica::real_t> ElementDimension_PYEL( 5UL ); ElementDimension_PYEL={0.007522,0.007522,0.007522,0.007522,0.007522};
    blaze::DynamicTensor<elastica::real_t> MassSecondMomentOfInertia_PYEL( 3UL, 3UL, 5UL);     MassSecondMomentOfInertia_PYEL = {{{7.127235497145191e-10,7.127235497145191e-10,7.12723549714519e-10,7.127235497145191e-10,7.127235497145191e-10},{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0,0.0},{7.127235497145191e-10,7.127235497145191e-10,7.12723549714519e-10,7.127235497145191e-10,7.127235497145191e-10},{0.0,0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},{1.4254470994290381e-09,1.4254470994290381e-09,1.425447099429038e-09,1.4254470994290381e-09,1.4254470994290381e-09}}};
    blaze::DynamicTensor<elastica::real_t> InvMassSecondMomentOfInertia_PYEL( 3UL, 3UL, 5UL);     InvMassSecondMomentOfInertia_PYEL = {{{1403068553.579504,1403068553.579504,1403068553.5795043,1403068553.579504,1403068553.579504},{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0,0.0},{1403068553.579504,1403068553.579504,1403068553.5795043,1403068553.579504,1403068553.579504},{0.0,0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},{701534276.789752,701534276.789752,701534276.7897521,701534276.789752,701534276.789752}}};
    blaze::DynamicTensor<elastica::real_t> ShearStretchRigidityMatrix_PYEL( 3UL, 3UL, 5UL);     ShearStretchRigidityMatrix_PYEL = {{{0.03822641567117308,0.03822641567117308,0.03822641567117308,0.03822641567117308,0.03822641567117308},{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0,0.0},{0.03822641567117308,0.03822641567117308,0.03822641567117308,0.03822641567117308,0.03822641567117308},{0.0,0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},{0.035550566574190964,0.035550566574190964,0.035550566574190964,0.035550566574190964,0.035550566574190964}}};
    blaze::DynamicTensor<elastica::real_t> BendingTwistRigidityMatrix_PYEL( 3UL, 3UL, 4UL);     BendingTwistRigidityMatrix_PYEL = {{{5.028670658104868e-07,5.028670658104868e-07,5.028670658104868e-07,5.028670658104868e-07},{0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0},{5.028670658104868e-07,5.028670658104868e-07,5.028670658104868e-07,5.028670658104868e-07},{0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0},{8.110759125975593e-07,8.110759125975594e-07,8.110759125975593e-07,8.110759125975593e-07}}};
    blaze::DynamicVector<elastica::real_t> ElementVolume_PYEL( 5UL ); ElementVolume_PYEL={6.3991019833543736e-06,6.3991019833543736e-06,6.399101983354373e-06,6.399101983354375e-06,6.399101983354375e-06};
    blaze::DynamicVector<elastica::real_t> Mass_PYEL( 6UL ); Mass_PYEL={2.5193264508466167e-05,5.038652901693233e-05,5.0386529016932326e-05,5.038652901693234e-05,5.038652901693235e-05,2.5193264508466173e-05};
    blaze::DynamicMatrix<elastica::real_t> InternalLoads_PYEL( 3UL, 6UL );     InternalLoads_PYEL = {{0.0,0.0,0.0,0.0,0.0,-0.0},{0.0,0.0,0.0,0.0,0.0,-0.0},{0.0,0.0,0.0,0.0,0.0,-0.0}};
    blaze::DynamicMatrix<elastica::real_t> InternalTorques_PYEL( 3UL, 5UL );     InternalTorques_PYEL = {{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0}};
    blaze::DynamicMatrix<elastica::real_t> ExternalLoads_PYEL( 3UL, 6UL );     ExternalLoads_PYEL = {{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}};
    blaze::DynamicMatrix<elastica::real_t> ExternalTorques_PYEL( 3UL, 5UL );     ExternalTorques_PYEL = {{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0}};
    blaze::DynamicVector<elastica::real_t> ElementLength_PYEL( 5UL ); ElementLength_PYEL={0.036,0.036,0.03599999999999999,0.036000000000000004,0.036000000000000004};
    blaze::DynamicVector<elastica::real_t> ReferenceElementLength_PYEL( 5UL ); ReferenceElementLength_PYEL={0.036,0.036,0.03599999999999999,0.036000000000000004,0.036000000000000004};
    blaze::DynamicMatrix<elastica::real_t> Tangent_PYEL( 3UL, 5UL );     Tangent_PYEL = {{0.0,0.0,0.0,0.0,0.0},{1.0,1.0,1.0,1.0,1.0},{0.0,0.0,0.0,0.0,0.0}};
    blaze::DynamicVector<elastica::real_t> ElementDilatation_PYEL( 5UL ); ElementDilatation_PYEL={1.0,1.0,1.0,1.0,1.0};
    blaze::DynamicVector<elastica::real_t> VoronoiDilatation_PYEL( 4UL ); VoronoiDilatation_PYEL={1.0,1.0,1.0,1.0};
    blaze::DynamicVector<elastica::real_t> ReferenceVoronoiLength_PYEL( 4UL ); ReferenceVoronoiLength_PYEL={0.036,0.03599999999999999,0.036,0.036000000000000004};
    blaze::DynamicMatrix<elastica::real_t> ShearStretchStrain_PYEL( 3UL, 5UL );     ShearStretchStrain_PYEL = {{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0}};
    blaze::DynamicMatrix<elastica::real_t> Curvature_PYEL( 3UL, 4UL );     Curvature_PYEL = {{-0.0,-0.0,-0.0,-0.0},{-0.0,-0.0,-0.0,-0.0},{-0.0,-0.0,-0.0,-0.0}};
    blaze::DynamicMatrix<elastica::real_t> ReferenceShearStretchStrain_PYEL( 3UL, 5UL );     ReferenceShearStretchStrain_PYEL = {{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0}};
    blaze::DynamicMatrix<elastica::real_t> ReferenceCurvature_PYEL( 3UL, 4UL );     ReferenceCurvature_PYEL = {{0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0}};
    blaze::DynamicMatrix<elastica::real_t> InternalStress_PYEL( 3UL, 5UL );     InternalStress_PYEL = {{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0}};
    blaze::DynamicMatrix<elastica::real_t> InternalCouple_PYEL( 3UL, 4UL );     InternalCouple_PYEL = {{0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0}};
    blaze::DynamicMatrix<elastica::real_t> Position_PYEL( 3UL, 6UL );     Position_PYEL = {{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.036,0.072,0.10799999999999998,0.144,0.18},{0.0,0.0,0.0,0.0,0.0,0.0}};
    blaze::DynamicTensor<elastica::real_t> Director_PYEL( 3UL, 3UL, 5UL);     Director_PYEL = {{{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},{1.0,1.0,1.0,1.0,1.0}},{{1.0,1.0,1.0,1.0,1.0},{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0,0.0},{1.0,1.0,1.0,1.0,1.0},{0.0,0.0,0.0,0.0,0.0}}};
    blaze::DynamicMatrix<elastica::real_t> Velocity_PYEL( 3UL, 6UL );     Velocity_PYEL = {{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}};
    blaze::DynamicMatrix<elastica::real_t> AngularVelocity_PYEL( 3UL, 5UL );     AngularVelocity_PYEL = {{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0}};
    blaze::DynamicMatrix<elastica::real_t> Acceleration_PYEL( 3UL, 6UL );     Acceleration_PYEL = {{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}};
    blaze::DynamicMatrix<elastica::real_t> AngularAcceleration_PYEL( 3UL, 5UL );     AngularAcceleration_PYEL = {{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0}};

    slicedrod.compute_internal_forces();
    slicedrod.compute_internal_torques();
    slicedrod.update_acceleration();

    auto ElementDimension(blocks::get<elastica::tags::ElementDimension>(slicedrod));
    auto MassSecondMomentOfInertia(blocks::get<elastica::tags::MassSecondMomentOfInertia>(slicedrod));
    auto InvMassSecondMomentOfInertia(blocks::get<elastica::tags::InvMassSecondMomentOfInertia>(slicedrod));
    auto ShearStretchRigidityMatrix(blocks::get<elastica::tags::ShearStretchRigidityMatrix>(slicedrod));
    auto BendingTwistRigidityMatrix(blocks::get<elastica::tags::BendingTwistRigidityMatrix>(slicedrod));
    auto ElementVolume(blocks::get<elastica::tags::ElementVolume>(slicedrod));
    auto Mass(blocks::get<elastica::tags::Mass>(slicedrod));
    auto InternalLoads(blocks::get<elastica::tags::InternalLoads>(slicedrod));
    auto InternalTorques(blocks::get<elastica::tags::InternalTorques>(slicedrod));
    auto ExternalLoads(blocks::get<elastica::tags::ExternalLoads>(slicedrod));
    auto ExternalTorques(blocks::get<elastica::tags::ExternalTorques>(slicedrod));
    auto ElementLength(blocks::get<elastica::tags::ElementLength>(slicedrod));
    auto ReferenceElementLength(blocks::get<elastica::tags::ReferenceElementLength>(slicedrod));
    auto Tangent(blocks::get<elastica::tags::Tangent>(slicedrod));
    auto ElementDilatation(blocks::get<elastica::tags::ElementDilatation>(slicedrod));
    auto VoronoiDilatation(blocks::get<elastica::tags::VoronoiDilatation>(slicedrod));
    auto ReferenceVoronoiLength(blocks::get<elastica::tags::ReferenceVoronoiLength>(slicedrod));
    auto ShearStretchStrain(blocks::get<elastica::tags::ShearStretchStrain>(slicedrod));
    auto Curvature(blocks::get<elastica::tags::Curvature>(slicedrod));
    auto ReferenceShearStretchStrain(blocks::get<elastica::tags::ReferenceShearStretchStrain>(slicedrod));
    auto ReferenceCurvature(blocks::get<elastica::tags::ReferenceCurvature>(slicedrod));
    auto InternalStress(blocks::get<elastica::tags::InternalStress>(slicedrod));
    auto InternalCouple(blocks::get<elastica::tags::InternalCouple>(slicedrod));
    auto Position(blocks::get<elastica::tags::Position>(slicedrod));
    auto Director(blocks::get<elastica::tags::Director>(slicedrod));
    auto Velocity(blocks::get<elastica::tags::Velocity>(slicedrod));
    auto AngularVelocity(blocks::get<elastica::tags::AngularVelocity>(slicedrod));
    auto Acceleration(blocks::get<elastica::tags::Acceleration>(slicedrod));
    auto AngularAcceleration(blocks::get<elastica::tags::AngularAcceleration>(slicedrod));


    CHECK_ITERABLE_APPROX(ElementDimension , ElementDimension_PYEL);
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::pageslice(MassSecondMomentOfInertia, i) - blaze::pageslice(MassSecondMomentOfInertia_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::pageslice(InvMassSecondMomentOfInertia, i) - blaze::pageslice(InvMassSecondMomentOfInertia_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::pageslice(ShearStretchRigidityMatrix, i) - blaze::pageslice(ShearStretchRigidityMatrix_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::pageslice(BendingTwistRigidityMatrix, i) - blaze::pageslice(BendingTwistRigidityMatrix_PYEL, i)) == Approx(0.0).margin(1e-6));
    CHECK_ITERABLE_APPROX(ElementVolume , ElementVolume_PYEL);
    CHECK_ITERABLE_APPROX(Mass , Mass_PYEL);
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(InternalLoads, i) - blaze::row(InternalLoads_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(InternalTorques, i) - blaze::row(InternalTorques_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(ExternalLoads, i) - blaze::row(ExternalLoads_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(ExternalTorques, i) - blaze::row(ExternalTorques_PYEL, i)) == Approx(0.0).margin(1e-6));
    CHECK_ITERABLE_APPROX(ElementLength , ElementLength_PYEL);
    CHECK_ITERABLE_APPROX(ReferenceElementLength , ReferenceElementLength_PYEL);
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(Tangent, i) - blaze::row(Tangent_PYEL, i)) == Approx(0.0).margin(1e-6));
    CHECK_ITERABLE_APPROX(ElementDilatation , ElementDilatation_PYEL);
    CHECK_ITERABLE_APPROX(VoronoiDilatation , VoronoiDilatation_PYEL);
    CHECK_ITERABLE_APPROX(ReferenceVoronoiLength , ReferenceVoronoiLength_PYEL);
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(ShearStretchStrain, i) - blaze::row(ShearStretchStrain_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(Curvature, i) - blaze::row(Curvature_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(ReferenceShearStretchStrain, i) - blaze::row(ReferenceShearStretchStrain_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(ReferenceCurvature, i) - blaze::row(ReferenceCurvature_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(InternalStress, i) - blaze::row(InternalStress_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(InternalCouple, i) - blaze::row(InternalCouple_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(Position, i) - blaze::row(Position_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::pageslice(Director, i) - blaze::pageslice(Director_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(Velocity, i) - blaze::row(Velocity_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(AngularVelocity, i) - blaze::row(AngularVelocity_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(Acceleration, i) - blaze::row(Acceleration_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(AngularAcceleration, i) - blaze::row(AngularAcceleration_PYEL, i)) == Approx(0.0).margin(1e-6));
    // clang-format on
  }

  ELASTICA_TEST_CASE("Unit.z.Integrated.CosseratRods.PerturbedInternalKinematics",
                     "[Systems][Integrated]") {
    /*
     * rng_seed = 100
     * shuffle_variables = {
     * 'internal_forces':'InternalLoads',
     * 'internal_torques':'InternalTorques',
     * 'internal_stress':'InternalStress',
     * 'internal_couple':'InternalCouple',
     * 'rest_sigma':'ReferenceShearStretchStrain',
     * 'rest_kappa':'ReferenceCurvature',
     * }
     * perturb_mag = 0.1
     * perturb_variables = {
     *  'rest_lengths':'ReferenceElementLength',
     *  }
     */
    elastica::MaterialID myMaterial = elastica::create_material(
        7.874, 0.24, 200, 200.0 / 1.24, 200, 1e-10, 1e-10, 0.1, 0.1);

    using Init = ::elastica::cosserat_rod::StraightCosseratRodInitializer;
    Model simple_rod{};
    Init straight_initializer{
        // Initialization Wrapper
        Init::NElement{5UL},
        Init::Material{myMaterial},
        Init::Radius{0.007522},
        Init::Length{0.18},
        Init::Origin{::elastica::Vec3{0.0, 0.0, 0.0}},
        Init::Direction{::elastica::Vec3{0.0, 1.0, 0.0}},
        Init::Normal{::elastica::Vec3{0.0, 0.0, 1.0}},
        Init::ForceDampingCoefficient{0.2},
        Init::TorqueDampingCoefficient{0.2},
    };
    auto slicedrod =
        simple_rod.emplace_back(straight_initializer.initialize<Plugin>());

    // clang-format off
    blaze::DynamicMatrix<elastica::real_t> InternalLoads_REPLACED( 3UL, 6UL );     InternalLoads_REPLACED = {{0.004822044056611019,0.9307870177032551,0.7845822109817221,0.3343249592744213,0.6862689176861808,0.7322625609228085},{0.8246472414522372,0.10745443906835128,0.3089916869203323,0.31294344042724076,0.5940727124966699,0.6105506387346894},{0.9165420505570774,0.738817174285074,0.37747080022423485,0.7949533183809576,0.48693548220111915,0.5788914075188061}};
    blaze::DynamicMatrix<elastica::real_t> InternalTorques_REPLACED( 3UL, 5UL );     InternalTorques_REPLACED = {{0.9744748566188305,0.688389675725899,0.5848087428374977,0.2739643248027279,0.8377858251400324},{0.30222935408131235,0.1728462163878618,0.801743366769752,0.3441099204558331,0.4375847094010139},{0.15268115598708532,0.2761357842472395,0.5638672039037047,0.86467466140162,0.08278860289302525}};
    blaze::DynamicMatrix<elastica::real_t> InternalStress_REPLACED( 3UL, 5UL );     InternalStress_REPLACED = {{0.9748236222955216,0.9228236343831954,0.4073736921398182,0.7807688857822744,0.6465955762463057},{0.22546091065284335,0.824480688125953,0.3453562771880586,0.5685671364094176,0.6863696968075128},{0.2470087962580596,0.8753744062638658,0.15593308505796288,0.6952022537494705,0.16522045602868496}};
    blaze::DynamicMatrix<elastica::real_t> InternalCouple_REPLACED( 3UL, 4UL );     InternalCouple_REPLACED = {{0.48096625789258196,0.5220324730992747,0.5747132880427898,0.8415277735547125},{0.4884155772097232,0.9164990050965889,0.12455725715642718,0.45378529009583934},{0.9589130366491199,0.6706482598702904,0.7928513073170474,0.7638068902765369}};
    blaze::DynamicMatrix<elastica::real_t> ReferenceShearStretchStrain_REPLACED( 3UL, 5UL );     ReferenceShearStretchStrain_REPLACED = {{0.4694302963114608,0.2684748849053611,0.45322327766269055,0.2527348132181251,0.9472151712532533},{0.0497407763497818,0.4375196365597337,0.7683879888762261,0.16663239947009578,0.8244444091919032},{0.8696022625042859,0.44593293278653,0.16041064218431744,0.41984957138538315,0.4522866548422185}};
    blaze::DynamicMatrix<elastica::real_t> ReferenceCurvature_REPLACED( 3UL, 4UL );     ReferenceCurvature_REPLACED = {{0.8544544784625221,0.7439885668390492,0.02976576715291579,0.18617391272004868},{0.7383720961472324,0.8817988526289666,0.6536435340498628,0.2525396458442313},{0.6183595148406859,0.19936517150220823,0.8575809295973004,0.021480917433798763}};
    blaze::DynamicVector<elastica::real_t> ReferenceElementLength_REPLACED( 5UL ); ReferenceElementLength_REPLACED={0.12680569690386845,0.061119605094289244,0.1268052828476898,0.11215298850236449,0.11095712349893845};
    blaze::DynamicVector<elastica::real_t> ElementDimension_PYEL( 5UL ); ElementDimension_PYEL={0.007522,0.007522,0.007522,0.007522,0.007522};
    blaze::DynamicTensor<elastica::real_t> MassSecondMomentOfInertia_PYEL( 3UL, 3UL, 5UL);     MassSecondMomentOfInertia_PYEL = {{{7.127235497145191e-10,7.127235497145191e-10,7.12723549714519e-10,7.127235497145191e-10,7.127235497145191e-10},{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0,0.0},{7.127235497145191e-10,7.127235497145191e-10,7.12723549714519e-10,7.127235497145191e-10,7.127235497145191e-10},{0.0,0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},{1.4254470994290381e-09,1.4254470994290381e-09,1.425447099429038e-09,1.4254470994290381e-09,1.4254470994290381e-09}}};
    blaze::DynamicTensor<elastica::real_t> InvMassSecondMomentOfInertia_PYEL( 3UL, 3UL, 5UL);     InvMassSecondMomentOfInertia_PYEL = {{{1403068553.579504,1403068553.579504,1403068553.5795043,1403068553.579504,1403068553.579504},{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0,0.0},{1403068553.579504,1403068553.579504,1403068553.5795043,1403068553.579504,1403068553.579504},{0.0,0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},{701534276.789752,701534276.789752,701534276.7897521,701534276.789752,701534276.789752}}};
    blaze::DynamicTensor<elastica::real_t> ShearStretchRigidityMatrix_PYEL( 3UL, 3UL, 5UL);     ShearStretchRigidityMatrix_PYEL = {{{0.03822641567117308,0.03822641567117308,0.03822641567117308,0.03822641567117308,0.03822641567117308},{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0,0.0},{0.03822641567117308,0.03822641567117308,0.03822641567117308,0.03822641567117308,0.03822641567117308},{0.0,0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},{0.035550566574190964,0.035550566574190964,0.035550566574190964,0.035550566574190964,0.035550566574190964}}};
    blaze::DynamicTensor<elastica::real_t> BendingTwistRigidityMatrix_PYEL( 3UL, 3UL, 4UL);     BendingTwistRigidityMatrix_PYEL = {{{5.028670658104868e-07,5.028670658104868e-07,5.028670658104868e-07,5.028670658104868e-07},{0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0},{5.028670658104868e-07,5.028670658104868e-07,5.028670658104868e-07,5.028670658104868e-07},{0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0},{8.110759125975593e-07,8.110759125975594e-07,8.110759125975593e-07,8.110759125975593e-07}}};
    blaze::DynamicVector<elastica::real_t> ElementVolume_PYEL( 5UL ); ElementVolume_PYEL={6.3991019833543736e-06,6.3991019833543736e-06,6.399101983354373e-06,6.399101983354375e-06,6.399101983354375e-06};
    blaze::DynamicVector<elastica::real_t> Mass_PYEL( 6UL ); Mass_PYEL={2.5193264508466167e-05,5.038652901693233e-05,5.0386529016932326e-05,5.038652901693234e-05,5.038652901693235e-05,2.5193264508466173e-05};
    blaze::DynamicMatrix<elastica::real_t> InternalLoads_PYEL( 3UL, 6UL );     InternalLoads_PYEL = {{-0.0066974950581904285,-0.02169732797387603,-0.07506672967976592,0.08361745412889009,-0.07729132622704615,0.09713542480998845},{-0.1985659296791269,0.14684494117305213,-0.05803763293731121,-0.011943186131281286,-0.0018774422702863003,0.12357924984495355},{-0.06320784114027343,0.04578394468299672,-0.043601503094721314,0.0309274440235953,-0.08150222881432871,0.11160018434273143}};
    blaze::DynamicMatrix<elastica::real_t> InternalTorques_PYEL( 3UL, 5UL );     InternalTorques_PYEL = {{0.00024068014507840237,0.0010222691788232428,0.0037249750567495783,0.0007143088964806924,0.003496968913888804},{-0.002275853584059309,-0.0006273323970541523,-0.0021967996520763254,-0.001083324697087151,-0.004017479642467625},{-5.015365078127933e-07,3.398362193964708e-07,-5.338629466830712e-07,6.781405803883425e-07,1.7422654711051154e-08}};
    blaze::DynamicMatrix<elastica::real_t> ExternalLoads_PYEL( 3UL, 6UL );     ExternalLoads_PYEL = {{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}};
    blaze::DynamicMatrix<elastica::real_t> ExternalTorques_PYEL( 3UL, 5UL );     ExternalTorques_PYEL = {{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0}};
    blaze::DynamicVector<elastica::real_t> ElementLength_PYEL( 5UL ); ElementLength_PYEL={0.036,0.036,0.03599999999999999,0.036000000000000004,0.036000000000000004};
    blaze::DynamicVector<elastica::real_t> ReferenceElementLength_PYEL( 5UL ); ReferenceElementLength_PYEL={0.12680569690386845,0.061119605094289244,0.1268052828476898,0.11215298850236449,0.11095712349893845};
    blaze::DynamicMatrix<elastica::real_t> Tangent_PYEL( 3UL, 5UL );     Tangent_PYEL = {{0.0,0.0,0.0,0.0,0.0},{1.0,1.0,1.0,1.0,1.0},{0.0,0.0,0.0,0.0,0.0}};
    blaze::DynamicVector<elastica::real_t> ElementDilatation_PYEL( 5UL ); ElementDilatation_PYEL={0.2838989168388203,0.589009041280008,0.28389984385146505,0.3209901089638911,0.32444965104330975};
    blaze::DynamicVector<elastica::real_t> VoronoiDilatation_PYEL( 4UL ); VoronoiDilatation_PYEL={1.0,1.0,1.0,1.0};
    blaze::DynamicVector<elastica::real_t> ReferenceVoronoiLength_PYEL( 4UL ); ReferenceVoronoiLength_PYEL={0.036,0.03599999999999999,0.036,0.036000000000000004};
    blaze::DynamicMatrix<elastica::real_t> ShearStretchStrain_PYEL( 3UL, 5UL );     ShearStretchStrain_PYEL = {{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},{-0.7161010831611797,-0.41099095871999203,-0.716100156148535,-0.6790098910361089,-0.6755503489566903}};
    blaze::DynamicMatrix<elastica::real_t> Curvature_PYEL( 3UL, 4UL );     Curvature_PYEL = {{-0.0,-0.0,-0.0,-0.0},{-0.0,-0.0,-0.0,-0.0},{-0.0,-0.0,-0.0,-0.0}};
    blaze::DynamicMatrix<elastica::real_t> ReferenceShearStretchStrain_PYEL( 3UL, 5UL );     ReferenceShearStretchStrain_PYEL = {{0.4694302963114608,0.2684748849053611,0.45322327766269055,0.2527348132181251,0.9472151712532533},{0.0497407763497818,0.4375196365597337,0.7683879888762261,0.16663239947009578,0.8244444091919032},{0.8696022625042859,0.44593293278653,0.16041064218431744,0.41984957138538315,0.4522866548422185}};
    blaze::DynamicMatrix<elastica::real_t> ReferenceCurvature_PYEL( 3UL, 4UL );     ReferenceCurvature_PYEL = {{0.8544544784625221,0.7439885668390492,0.02976576715291579,0.18617391272004868},{0.7383720961472324,0.8817988526289666,0.6536435340498628,0.2525396458442313},{0.6183595148406859,0.19936517150220823,0.8575809295973004,0.021480917433798763}};
    blaze::DynamicMatrix<elastica::real_t> InternalStress_PYEL( 3UL, 5UL );     InternalStress_PYEL = {{-0.01794463763544385,-0.010262832547662684,-0.017325101403785504,-0.009661146024652338,-0.036208640866368255},{-0.0019014115925536145,-0.016724807491432955,-0.029372718659519334,-0.006369759366428843,-0.0315155546835444},{-0.05637265235699748,-0.03046412985401741,-0.031160455489129338,-0.03906507647449495,-0.04009524448838917}};
    blaze::DynamicMatrix<elastica::real_t> InternalCouple_PYEL( 3UL, 4UL );     InternalCouple_PYEL = {{-4.296770164530783e-07,-3.7412734760290195e-07,-1.4968223989784932e-08,-9.362072921998855e-08},{-3.713030094658974e-07,-4.4342760165658233e-07,-3.286958060536515e-07,-1.2699387070650807e-07},{-5.015365078127933e-07,-1.6170028841632248e-07,-6.955632350993937e-07,-1.7422654711051154e-08}};
    blaze::DynamicMatrix<elastica::real_t> Position_PYEL( 3UL, 6UL );     Position_PYEL = {{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.036,0.072,0.10799999999999998,0.144,0.18},{0.0,0.0,0.0,0.0,0.0,0.0}};
    blaze::DynamicTensor<elastica::real_t> Director_PYEL( 3UL, 3UL, 5UL);     Director_PYEL = {{{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},{1.0,1.0,1.0,1.0,1.0}},{{1.0,1.0,1.0,1.0,1.0},{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0,0.0},{1.0,1.0,1.0,1.0,1.0},{0.0,0.0,0.0,0.0,0.0}}};
    blaze::DynamicMatrix<elastica::real_t> Velocity_PYEL( 3UL, 6UL );     Velocity_PYEL = {{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}};
    blaze::DynamicMatrix<elastica::real_t> AngularVelocity_PYEL( 3UL, 5UL );     AngularVelocity_PYEL = {{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0}};
    blaze::DynamicMatrix<elastica::real_t> Acceleration_PYEL( 3UL, 6UL );     Acceleration_PYEL = {{-265.8446687581811,-430.6176352529596,-1489.8174401840588,1659.5200296648839,-1533.968061206845,3855.6108827161393},{-7881.707017857851,2914.3690593115684,-1151.8482036698288,-237.03133286414294,-37.2607978147371,4905.24956793213},{-2508.918251504584,908.6544673003986,-865.34047781043,613.8038207236336,-1617.540052956217,4429.762736989813}};
    blaze::DynamicMatrix<elastica::real_t> AngularAcceleration_PYEL( 3UL, 5UL );     AngularAcceleration_PYEL = {{95870.03617284377,844823.7597732814,1483772.8280276363,321704.10337508086,1591908.0326095354},{-906539.9447752003,-518440.0794680635,-875053.2775958261,-487898.1657061345,-1828857.582430559},{-99.88842895692798,140.42373502258647,-106.32706556568509,152.7074690312117,3.965614889190092}};


    blocks::get<elastica::tags::InternalLoads>(slicedrod) = InternalLoads_REPLACED;
    blocks::get<elastica::tags::InternalTorques>(slicedrod) = InternalTorques_REPLACED;
    blocks::get<elastica::tags::InternalStress>(slicedrod) = InternalStress_REPLACED;
    blocks::get<elastica::tags::InternalCouple>(slicedrod) = InternalCouple_REPLACED;
    blocks::get<elastica::tags::ReferenceShearStretchStrain>(slicedrod) = ReferenceShearStretchStrain_REPLACED;
    blocks::get<elastica::tags::ReferenceCurvature>(slicedrod) = ReferenceCurvature_REPLACED;
    blocks::get<elastica::tags::ReferenceElementLength>(slicedrod) = ReferenceElementLength_REPLACED;


    slicedrod.compute_internal_forces();
    slicedrod.compute_internal_torques();
    slicedrod.update_acceleration();


    auto ElementDimension(blocks::get<elastica::tags::ElementDimension>(slicedrod));
    auto MassSecondMomentOfInertia(blocks::get<elastica::tags::MassSecondMomentOfInertia>(slicedrod));
    auto InvMassSecondMomentOfInertia(blocks::get<elastica::tags::InvMassSecondMomentOfInertia>(slicedrod));
    auto ShearStretchRigidityMatrix(blocks::get<elastica::tags::ShearStretchRigidityMatrix>(slicedrod));
    auto BendingTwistRigidityMatrix(blocks::get<elastica::tags::BendingTwistRigidityMatrix>(slicedrod));
    auto ElementVolume(blocks::get<elastica::tags::ElementVolume>(slicedrod));
    auto Mass(blocks::get<elastica::tags::Mass>(slicedrod));
    auto InternalLoads(blocks::get<elastica::tags::InternalLoads>(slicedrod));
    auto InternalTorques(blocks::get<elastica::tags::InternalTorques>(slicedrod));
    auto ExternalLoads(blocks::get<elastica::tags::ExternalLoads>(slicedrod));
    auto ExternalTorques(blocks::get<elastica::tags::ExternalTorques>(slicedrod));
    auto ElementLength(blocks::get<elastica::tags::ElementLength>(slicedrod));
    auto ReferenceElementLength(blocks::get<elastica::tags::ReferenceElementLength>(slicedrod));
    auto Tangent(blocks::get<elastica::tags::Tangent>(slicedrod));
    auto ElementDilatation(blocks::get<elastica::tags::ElementDilatation>(slicedrod));
    auto VoronoiDilatation(blocks::get<elastica::tags::VoronoiDilatation>(slicedrod));
    auto ReferenceVoronoiLength(blocks::get<elastica::tags::ReferenceVoronoiLength>(slicedrod));
    auto ShearStretchStrain(blocks::get<elastica::tags::ShearStretchStrain>(slicedrod));
    auto Curvature(blocks::get<elastica::tags::Curvature>(slicedrod));
    auto ReferenceShearStretchStrain(blocks::get<elastica::tags::ReferenceShearStretchStrain>(slicedrod));
    auto ReferenceCurvature(blocks::get<elastica::tags::ReferenceCurvature>(slicedrod));
    auto InternalStress(blocks::get<elastica::tags::InternalStress>(slicedrod));
    auto InternalCouple(blocks::get<elastica::tags::InternalCouple>(slicedrod));
    auto Position(blocks::get<elastica::tags::Position>(slicedrod));
    auto Director(blocks::get<elastica::tags::Director>(slicedrod));
    auto Velocity(blocks::get<elastica::tags::Velocity>(slicedrod));
    auto AngularVelocity(blocks::get<elastica::tags::AngularVelocity>(slicedrod));
    auto Acceleration(blocks::get<elastica::tags::Acceleration>(slicedrod));
    auto AngularAcceleration(blocks::get<elastica::tags::AngularAcceleration>(slicedrod));


    CHECK_ITERABLE_APPROX(ElementDimension , ElementDimension_PYEL);
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::pageslice(MassSecondMomentOfInertia, i) - blaze::pageslice(MassSecondMomentOfInertia_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::pageslice(InvMassSecondMomentOfInertia, i) - blaze::pageslice(InvMassSecondMomentOfInertia_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::pageslice(ShearStretchRigidityMatrix, i) - blaze::pageslice(ShearStretchRigidityMatrix_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::pageslice(BendingTwistRigidityMatrix, i) - blaze::pageslice(BendingTwistRigidityMatrix_PYEL, i)) == Approx(0.0).margin(1e-6));
    CHECK_ITERABLE_APPROX(ElementVolume , ElementVolume_PYEL);
    CHECK_ITERABLE_APPROX(Mass , Mass_PYEL);
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(InternalLoads, i) - blaze::row(InternalLoads_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(InternalTorques, i) - blaze::row(InternalTorques_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(ExternalLoads, i) - blaze::row(ExternalLoads_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(ExternalTorques, i) - blaze::row(ExternalTorques_PYEL, i)) == Approx(0.0).margin(1e-6));
    CHECK_ITERABLE_APPROX(ElementLength , ElementLength_PYEL);
    CHECK_ITERABLE_APPROX(ReferenceElementLength , ReferenceElementLength_PYEL);
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(Tangent, i) - blaze::row(Tangent_PYEL, i)) == Approx(0.0).margin(1e-6));
    CHECK_ITERABLE_APPROX(ElementDilatation , ElementDilatation_PYEL);
    CHECK_ITERABLE_APPROX(VoronoiDilatation , VoronoiDilatation_PYEL);
    CHECK_ITERABLE_APPROX(ReferenceVoronoiLength , ReferenceVoronoiLength_PYEL);
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(ShearStretchStrain, i) - blaze::row(ShearStretchStrain_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(Curvature, i) - blaze::row(Curvature_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(ReferenceShearStretchStrain, i) - blaze::row(ReferenceShearStretchStrain_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(ReferenceCurvature, i) - blaze::row(ReferenceCurvature_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(InternalStress, i) - blaze::row(InternalStress_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(InternalCouple, i) - blaze::row(InternalCouple_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(Position, i) - blaze::row(Position_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::pageslice(Director, i) - blaze::pageslice(Director_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(Velocity, i) - blaze::row(Velocity_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(AngularVelocity, i) - blaze::row(AngularVelocity_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(Acceleration, i) - blaze::row(Acceleration_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(AngularAcceleration, i) - blaze::row(AngularAcceleration_PYEL, i)) == Approx(0.0).margin(1e-6));
    // clang-format on
  }

  ELASTICA_TEST_CASE("Unit.z.Integrated.CosseratRods.WithDynamicAndExternal",
                     "[Systems][Integrated]") {
    /*
     * rng_seed = 150
     * shuffle_variables = {
     * 'internal_forces':'InternalLoads',
     * 'internal_torques':'InternalTorques',
     *  'external_forces':'ExternalLoads',
     *  'external_torques':'ExternalTorques',
     * 'internal_stress':'InternalStress',
     * 'internal_couple':'InternalCouple',
     * 'rest_sigma':'ReferenceShearStretchStrain',
     * 'rest_kappa':'ReferenceCurvature',
     * }
     * perturb_mag = 0.1
     * perturb_variables = {
     *  'rest_lengths':'ReferenceElementLength',
     *  'position_collection':'Position',
     *  'velocity_collection':'Velocity',
     *  'acceleration_collection':'Acceleration',
     *  'omega_collection':'AngularVelocity',
     *  'alpha_collection':'AngularAcceleration',
     *  }
     */
    elastica::MaterialID myMaterial = elastica::create_material(
        7.874, 0.24, 200, 200.0 / 1.24, 200, 1e-10, 1e-10, 0.1, 0.1);

    using Init = ::elastica::cosserat_rod::StraightCosseratRodInitializer;

    Model simple_rod{};
    Init straight_initializer{
        // Initialization Wrapper
        Init::NElement{5UL},
        Init::Material{myMaterial},
        Init::Radius{0.007522},
        Init::Length{0.18},
        Init::Origin{::elastica::Vec3{0.0, 0.0, 0.0}},
        Init::Direction{::elastica::Vec3{0.0, 1.0, 0.0}},
        Init::Normal{::elastica::Vec3{0.0, 0.0, 1.0}},
        Init::ForceDampingCoefficient{0.2},
        Init::TorqueDampingCoefficient{0.2},
    };
    auto slicedrod =
        simple_rod.emplace_back(straight_initializer.initialize<Plugin>());

    // clang-format off
    blaze::DynamicMatrix<elastica::real_t> InternalLoads_REPLACED( 3UL, 6UL );     InternalLoads_REPLACED = {{0.004822044056611019,0.9307870177032551,0.7845822109817221,0.3343249592744213,0.6862689176861808,0.7322625609228085},{0.8246472414522372,0.10745443906835128,0.3089916869203323,0.31294344042724076,0.5940727124966699,0.6105506387346894},{0.9165420505570774,0.738817174285074,0.37747080022423485,0.7949533183809576,0.48693548220111915,0.5788914075188061}};
    blaze::DynamicMatrix<elastica::real_t> InternalTorques_REPLACED( 3UL, 5UL );     InternalTorques_REPLACED = {{0.9744748566188305,0.688389675725899,0.5848087428374977,0.2739643248027279,0.8377858251400324},{0.30222935408131235,0.1728462163878618,0.801743366769752,0.3441099204558331,0.4375847094010139},{0.15268115598708532,0.2761357842472395,0.5638672039037047,0.86467466140162,0.08278860289302525}};
    blaze::DynamicMatrix<elastica::real_t> ExternalLoads_REPLACED( 3UL, 6UL );     ExternalLoads_REPLACED = {{0.9748236222955216,0.9228236343831954,0.4073736921398182,0.7807688857822744,0.6465955762463057,0.22546091065284335},{0.824480688125953,0.3453562771880586,0.5685671364094176,0.6863696968075128,0.2470087962580596,0.8753744062638658},{0.15593308505796288,0.6952022537494705,0.16522045602868496,0.48096625789258196,0.5220324730992747,0.5747132880427898}};
    blaze::DynamicMatrix<elastica::real_t> ExternalTorques_REPLACED( 3UL, 5UL );     ExternalTorques_REPLACED = {{0.8415277735547125,0.4884155772097232,0.9164990050965889,0.12455725715642718,0.45378529009583934},{0.9589130366491199,0.6706482598702904,0.7928513073170474,0.7638068902765369,0.4694302963114608},{0.2684748849053611,0.45322327766269055,0.2527348132181251,0.9472151712532533,0.0497407763497818}};
    blaze::DynamicMatrix<elastica::real_t> InternalStress_REPLACED( 3UL, 5UL );     InternalStress_REPLACED = {{0.4375196365597337,0.7683879888762261,0.16663239947009578,0.8244444091919032,0.8696022625042859},{0.44593293278653,0.16041064218431744,0.41984957138538315,0.4522866548422185,0.8544544784625221},{0.7439885668390492,0.02976576715291579,0.18617391272004868,0.7383720961472324,0.8817988526289666}};
    blaze::DynamicMatrix<elastica::real_t> InternalCouple_REPLACED( 3UL, 4UL );     InternalCouple_REPLACED = {{0.6536435340498628,0.2525396458442313,0.6183595148406859,0.19936517150220823},{0.8575809295973004,0.021480917433798763,0.9080569690386844,0.25119605094289243},{0.908052828476898,0.7615298850236448,0.7495712349893845,0.46584477600408136}};
    blaze::DynamicMatrix<elastica::real_t> ReferenceShearStretchStrain_REPLACED( 3UL, 5UL );     ReferenceShearStretchStrain_REPLACED = {{0.8330628756074665,0.42173609990468275,0.5729168526354368,0.25708185818524776,0.7142533332135373},{0.8957865325349527,0.1461385058562097,0.19592065162059813,0.6849354450730554,0.3365366346874151},{0.41888599874152754,0.2251580498219029,0.893616157352747,0.42505738060020026,0.785763682498953}};
    blaze::DynamicMatrix<elastica::real_t> ReferenceCurvature_REPLACED( 3UL, 4UL );     ReferenceCurvature_REPLACED = {{0.42296459317398183,0.35168182761748235,0.5263809158705643,0.3073262083073959},{0.357345091966899,0.026933120150035905,0.7107949934487903,0.2763092450283908},{0.2914274522809438,0.14947801345388878,0.28186903372731364,0.8952340780705201}};
    blaze::DynamicVector<elastica::real_t> ReferenceElementLength_REPLACED( 5UL ); ReferenceElementLength_REPLACED={0.0426999984824372,0.06021312422305085,0.12769661319401673,0.12159671964324305,0.0678179495567241};
    blaze::DynamicMatrix<elastica::real_t> Position_REPLACED( 3UL, 6UL );     Position_REPLACED = {{0.05374345637055803,0.028996065607452062,0.07415154940462247,0.024859356918426626,0.04064159265168045,0.06737567066158216},{0.02963858757601048,0.06973946292340193,0.08828160600644791,0.20771545928276713,0.21467524757067283,0.22514507962648403},{0.04836319070419701,0.018704045242927358,0.0165739712548571,0.06855920051506412,0.015482774970845548,0.038441072012352875}};
    blaze::DynamicMatrix<elastica::real_t> Velocity_REPLACED( 3UL, 6UL );     Velocity_REPLACED = {{0.018197333695852358,0.08766855523127529,0.06310417859392764,0.005886377952147127,0.04651766498643496,0.013806238174047048},{0.03903477571221334,0.008541891267936252,0.025397791978663778,0.016728381899616587,0.03177615374075308,0.06777741149744103},{0.04691392627870822,0.03263778989996056,0.05654957730089996,0.05000081261681551,0.03606695351052915,0.04624814200030012}};
    blaze::DynamicMatrix<elastica::real_t> Acceleration_REPLACED( 3UL, 6UL );     Acceleration_REPLACED = {{0.025521678254808857,0.08879391958437341,0.010899786996980654,0.01968499323761388,0.06758684467024345,0.03979854625346502},{0.05467244975907565,0.030489744465855475,0.0501465843868291,0.025331464086950153,0.07832008548993236,0.015947804239189612},{0.019968834201570376,0.06961861647008744,0.07038236907405,0.043850613884863304,0.046868672010347126,0.07523022215578842}};
    blaze::DynamicMatrix<elastica::real_t> AngularVelocity_REPLACED( 3UL, 5UL );     AngularVelocity_REPLACED = {{0.08770114933878452,0.009577245914128951,0.09875343648612099,0.01932221298301469,0.0726677364112708},{0.001063980046126911,0.0838166367595207,0.014960843780453748,0.07960981813312598,0.044280498457266185},{0.018307202694590942,0.005622099491997146,0.01090490002706256,0.0710350804397759,0.06736100309564207}};
    blaze::DynamicMatrix<elastica::real_t> AngularAcceleration_REPLACED( 3UL, 5UL );     AngularAcceleration_REPLACED = {{0.001194391204051315,0.01409282051105949,0.01463816812234734,0.03931543293728981,0.0235708323000189},{0.09073814929269795,0.04078499055234254,0.09258798531315957,0.014499827620907391,0.0493737051779979},{0.05973131578797437,0.08073493167581633,0.09944518992838293,0.06111602004198833,0.02882443602400854}};
    blaze::DynamicVector<elastica::real_t> ElementDimension_PYEL( 5UL ); ElementDimension_PYEL={0.0060483638044408075,0.0064566140709198154,0.003824309314689403,0.0060413382646657314,0.007443679400017562};
    blaze::DynamicTensor<elastica::real_t> MassSecondMomentOfInertia_PYEL( 3UL, 3UL, 5UL);     MassSecondMomentOfInertia_PYEL = {{{7.127235497145191e-10,7.127235497145191e-10,7.12723549714519e-10,7.127235497145191e-10,7.127235497145191e-10},{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0,0.0},{7.127235497145191e-10,7.127235497145191e-10,7.12723549714519e-10,7.127235497145191e-10,7.127235497145191e-10},{0.0,0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},{1.4254470994290381e-09,1.4254470994290381e-09,1.425447099429038e-09,1.4254470994290381e-09,1.4254470994290381e-09}}};
    blaze::DynamicTensor<elastica::real_t> InvMassSecondMomentOfInertia_PYEL( 3UL, 3UL, 5UL);     InvMassSecondMomentOfInertia_PYEL = {{{1403068553.579504,1403068553.579504,1403068553.5795043,1403068553.579504,1403068553.579504},{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0,0.0},{1403068553.579504,1403068553.579504,1403068553.5795043,1403068553.579504,1403068553.579504},{0.0,0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},{701534276.789752,701534276.789752,701534276.7897521,701534276.789752,701534276.789752}}};
    blaze::DynamicTensor<elastica::real_t> ShearStretchRigidityMatrix_PYEL( 3UL, 3UL, 5UL);     ShearStretchRigidityMatrix_PYEL = {{{0.03822641567117308,0.03822641567117308,0.03822641567117308,0.03822641567117308,0.03822641567117308},{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0,0.0},{0.03822641567117308,0.03822641567117308,0.03822641567117308,0.03822641567117308,0.03822641567117308},{0.0,0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},{0.035550566574190964,0.035550566574190964,0.035550566574190964,0.035550566574190964,0.035550566574190964}}};
    blaze::DynamicTensor<elastica::real_t> BendingTwistRigidityMatrix_PYEL( 3UL, 3UL, 4UL);     BendingTwistRigidityMatrix_PYEL = {{{5.028670658104868e-07,5.028670658104868e-07,5.028670658104868e-07,5.028670658104868e-07},{0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0},{5.028670658104868e-07,5.028670658104868e-07,5.028670658104868e-07,5.028670658104868e-07},{0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0},{8.110759125975593e-07,8.110759125975594e-07,8.110759125975593e-07,8.110759125975593e-07}}};
    blaze::DynamicVector<elastica::real_t> ElementVolume_PYEL( 5UL ); ElementVolume_PYEL={6.3991019833543736e-06,6.3991019833543736e-06,6.399101983354373e-06,6.399101983354375e-06,6.399101983354375e-06};
    blaze::DynamicVector<elastica::real_t> Mass_PYEL( 6UL ); Mass_PYEL={2.5193264508466167e-05,5.038652901693233e-05,5.0386529016932326e-05,5.038652901693234e-05,5.038652901693235e-05,2.5193264508466173e-05};
    blaze::DynamicMatrix<elastica::real_t> InternalLoads_PYEL( 3UL, 6UL );     InternalLoads_PYEL = {{-0.043545449360508914,0.07103098723441001,-0.04968838928987469,-0.02646719652114665,0.05004641627664524,-0.004177572054994125},{-0.013212245706092359,-0.02731926987692651,0.008570067214245167,-0.07513981085629651,-0.0013622956127654748,0.1068094487527123},{-0.04500556312724803,0.02281109224268177,0.014761996311766688,-0.052937046498616865,0.030878970798373425,0.026344953368423147}};
    blaze::DynamicMatrix<elastica::real_t> InternalTorques_PYEL( 3UL, 5UL );     InternalTorques_PYEL = {{0.0010813863102305075,-0.0024354452032655616,0.0012250488903254456,-0.0015660346428867271,-0.003437124772928622},{-0.0021957239198040534,-0.0013038838195032936,0.0005130154188824035,-0.006914043762841922,0.0018534541889307931},{-2.945348948838112e-05,0.0008569062615869237,-0.0016505352742172282,0.002572566622393443,0.00030679987821769677}};
    blaze::DynamicMatrix<elastica::real_t> ExternalLoads_PYEL( 3UL, 6UL );     ExternalLoads_PYEL = {{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}};
    blaze::DynamicMatrix<elastica::real_t> ExternalTorques_PYEL( 3UL, 5UL );     ExternalTorques_PYEL = {{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0}};
    blaze::DynamicVector<elastica::real_t> ElementLength_PYEL( 5UL ); ElementLength_PYEL={0.055679246247606846,0.04886067951086357,0.13927178325045536,0.05580882158139017,0.03676155210763667};
    blaze::DynamicVector<elastica::real_t> ReferenceElementLength_PYEL( 5UL ); ReferenceElementLength_PYEL={0.0426999984824372,0.06021312422305085,0.12769661319401673,0.12159671964324305,0.0678179495567241};
    blaze::DynamicMatrix<elastica::real_t> Tangent_PYEL( 3UL, 5UL );     Tangent_PYEL = {{-0.44446346584962326,0.9241681501201932,-0.3539280630704115,0.28279105858269044,0.7272293055425182},{0.7202122523186101,0.37949007808872914,0.8575595895224435,0.12470767327985444,0.28480386315452244},{-0.5326786452778971,-0.04359484987507514,0.3732646200610564,-0.9510400692982427,0.6245192524593671}};
    blaze::DynamicVector<elastica::real_t> ElementDilatation_PYEL( 5UL ); ElementDilatation_PYEL={1.3039636586991474,0.8114622873555974,1.0906458657509719,0.45896650621110224,0.5420622762545877};
    blaze::DynamicVector<elastica::real_t> VoronoiDilatation_PYEL( 4UL ); VoronoiDilatation_PYEL={1.4519434133120892,2.6129508716849856,2.7094528448867434,1.285699634569817};
    blaze::DynamicVector<elastica::real_t> ReferenceVoronoiLength_PYEL( 4UL ); ReferenceVoronoiLength_PYEL={0.036,0.03599999999999999,0.036,0.036000000000000004};
    blaze::DynamicMatrix<elastica::real_t> ShearStretchStrain_PYEL( 3UL, 5UL );     ShearStretchStrain_PYEL = {{-0.694593595207472,-0.035375576596552355,0.40709951470069844,-0.436495537872579,0.33852832755293805},{-0.5795642070873783,0.749927600997723,-0.3860101787609936,0.12979162414543657,0.3942035727214205},{-0.06086939642667166,-0.6920581132053656,-0.06470617905224652,-0.9427633548970296,-0.8456185696523595}};
    blaze::DynamicMatrix<elastica::real_t> Curvature_PYEL( 3UL, 4UL );     Curvature_PYEL = {{-0.0,-0.0,-0.0,-0.0},{-0.0,-0.0,-0.0,-0.0},{-0.0,-0.0,-0.0,-0.0}};
    blaze::DynamicMatrix<elastica::real_t> ReferenceShearStretchStrain_PYEL( 3UL, 5UL );     ReferenceShearStretchStrain_PYEL = {{0.8330628756074665,0.42173609990468275,0.5729168526354368,0.25708185818524776,0.7142533332135373},{0.8957865325349527,0.1461385058562097,0.19592065162059813,0.6849354450730554,0.3365366346874151},{0.41888599874152754,0.2251580498219029,0.893616157352747,0.42505738060020026,0.785763682498953}};
    blaze::DynamicMatrix<elastica::real_t> ReferenceCurvature_PYEL( 3UL, 4UL );     ReferenceCurvature_PYEL = {{0.42296459317398183,0.35168182761748235,0.5263809158705643,0.3073262083073959},{0.357345091966899,0.026933120150035905,0.7107949934487903,0.2763092450283908},{0.2914274522809438,0.14947801345388878,0.28186903372731364,0.8952340780705201}};
    blaze::DynamicMatrix<elastica::real_t> InternalStress_PYEL( 3UL, 5UL );     InternalStress_PYEL = {{-0.05839683125612913,-0.017473740954083012,-0.006338602485380685,-0.02651297784183633,-0.014362620244435928},{-0.056397370633575875,0.02308069292860096,-0.022245129814037645,-0.02122115845606243,0.002204400343771669},{-0.01705557611525436,-0.0326075542666249,-0.034068902019899955,-0.04862680211885312,-0.05799656336305883}};
    blaze::DynamicMatrix<elastica::real_t> InternalCouple_PYEL( 3UL, 4UL );     InternalCouple_PYEL = {{-2.126949639111265e-07,-1.7684920875287278e-07,-2.646996266624674e-07,-1.5454422861820262e-07},{-1.7969707787917307e-07,-1.3543779102969854e-08,-3.5743539274837734e-07,-1.3894681930373774e-07},{-2.3636978681474816e-07,-1.2123801617538312e-07,-2.2861718376337313e-07,-7.261027968594818e-07}};
    blaze::DynamicMatrix<elastica::real_t> Position_PYEL( 3UL, 6UL );     Position_PYEL = {{0.05374345637055803,0.028996065607452062,0.07415154940462247,0.024859356918426626,0.04064159265168045,0.06737567066158216},{0.02963858757601048,0.06973946292340193,0.08828160600644791,0.20771545928276713,0.21467524757067283,0.22514507962648403},{0.04836319070419701,0.018704045242927358,0.0165739712548571,0.06855920051506412,0.015482774970845548,0.038441072012352875}};
    blaze::DynamicTensor<elastica::real_t> Director_PYEL( 3UL, 3UL, 5UL);     Director_PYEL = {{{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},{1.0,1.0,1.0,1.0,1.0}},{{1.0,1.0,1.0,1.0,1.0},{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0,0.0},{1.0,1.0,1.0,1.0,1.0},{0.0,0.0,0.0,0.0,0.0}}};
    blaze::DynamicMatrix<elastica::real_t> Velocity_PYEL( 3UL, 6UL );     Velocity_PYEL = {{0.018197333695852358,0.08766855523127529,0.06310417859392764,0.005886377952147127,0.04651766498643496,0.013806238174047048},{0.03903477571221334,0.008541891267936252,0.025397791978663778,0.016728381899616587,0.03177615374075308,0.06777741149744103},{0.04691392627870822,0.03263778989996056,0.05654957730089996,0.05000081261681551,0.03606695351052915,0.04624814200030012}};
    blaze::DynamicMatrix<elastica::real_t> AngularVelocity_PYEL( 3UL, 5UL );     AngularVelocity_PYEL = {{0.08770114933878452,0.009577245914128951,0.09875343648612099,0.01932221298301469,0.0726677364112708},{0.001063980046126911,0.0838166367595207,0.014960843780453748,0.07960981813312598,0.044280498457266185},{0.018307202694590942,0.005622099491997146,0.01090490002706256,0.0710350804397759,0.06736100309564207}};
    blaze::DynamicMatrix<elastica::real_t> Acceleration_PYEL( 3UL, 6UL );     Acceleration_PYEL = {{36965.36320737861,19724.609752015698,7098.8279968589195,14970.304642489773,13825.957177737824,8783.432513221427},{32201.799101789085,6311.945147169318,11454.196486321107,12130.819444733197,4875.241566307234,38985.9700272871},{4403.062647694497,14250.105335711152,3572.0351421700193,8494.913615703244,10973.398142028056,23857.894287944615}};
    blaze::DynamicMatrix<elastica::real_t> AngularAcceleration_PYEL( 3UL, 5UL );     AngularAcceleration_PYEL = {{1541595929.292644,553306467.4028152,1404348075.4662375,79201608.69425806,342512537.5747653},{1750362546.5259712,762073483.661841,1214046482.6409187,487409831.5955925,358435108.058893},{245567223.82073802,258493594.63261354,192110973.32498634,305813354.8902232,19031855.4101663}};


    blocks::get<elastica::tags::InternalLoads>(slicedrod) = InternalLoads_REPLACED;
    blocks::get<elastica::tags::InternalTorques>(slicedrod) = InternalTorques_REPLACED;
    blocks::get<elastica::tags::ExternalLoads>(slicedrod) = ExternalLoads_REPLACED;
    blocks::get<elastica::tags::ExternalTorques>(slicedrod) = ExternalTorques_REPLACED;
    blocks::get<elastica::tags::InternalStress>(slicedrod) = InternalStress_REPLACED;
    blocks::get<elastica::tags::InternalCouple>(slicedrod) = InternalCouple_REPLACED;
    blocks::get<elastica::tags::ReferenceShearStretchStrain>(slicedrod) = ReferenceShearStretchStrain_REPLACED;
    blocks::get<elastica::tags::ReferenceCurvature>(slicedrod) = ReferenceCurvature_REPLACED;
    blocks::get<elastica::tags::ReferenceElementLength>(slicedrod) = ReferenceElementLength_REPLACED;
    blocks::get<elastica::tags::Position>(slicedrod) = Position_REPLACED;
    blocks::get<elastica::tags::Velocity>(slicedrod) = Velocity_REPLACED;
    blocks::get<elastica::tags::Acceleration>(slicedrod) = Acceleration_REPLACED;
    blocks::get<elastica::tags::AngularVelocity>(slicedrod) = AngularVelocity_REPLACED;
    blocks::get<elastica::tags::AngularAcceleration>(slicedrod) = AngularAcceleration_REPLACED;


    slicedrod.compute_internal_forces();
    slicedrod.compute_internal_torques();
    slicedrod.update_acceleration();


    auto ElementDimension(blocks::get<elastica::tags::ElementDimension>(slicedrod));
    auto MassSecondMomentOfInertia(blocks::get<elastica::tags::MassSecondMomentOfInertia>(slicedrod));
    auto InvMassSecondMomentOfInertia(blocks::get<elastica::tags::InvMassSecondMomentOfInertia>(slicedrod));
    auto ShearStretchRigidityMatrix(blocks::get<elastica::tags::ShearStretchRigidityMatrix>(slicedrod));
    auto BendingTwistRigidityMatrix(blocks::get<elastica::tags::BendingTwistRigidityMatrix>(slicedrod));
    auto ElementVolume(blocks::get<elastica::tags::ElementVolume>(slicedrod));
    auto Mass(blocks::get<elastica::tags::Mass>(slicedrod));
    auto InternalLoads(blocks::get<elastica::tags::InternalLoads>(slicedrod));
    auto InternalTorques(blocks::get<elastica::tags::InternalTorques>(slicedrod));
    auto ExternalLoads(blocks::get<elastica::tags::ExternalLoads>(slicedrod));
    auto ExternalTorques(blocks::get<elastica::tags::ExternalTorques>(slicedrod));
    auto ElementLength(blocks::get<elastica::tags::ElementLength>(slicedrod));
    auto ReferenceElementLength(blocks::get<elastica::tags::ReferenceElementLength>(slicedrod));
    auto Tangent(blocks::get<elastica::tags::Tangent>(slicedrod));
    auto ElementDilatation(blocks::get<elastica::tags::ElementDilatation>(slicedrod));
    auto VoronoiDilatation(blocks::get<elastica::tags::VoronoiDilatation>(slicedrod));
    auto ReferenceVoronoiLength(blocks::get<elastica::tags::ReferenceVoronoiLength>(slicedrod));
    auto ShearStretchStrain(blocks::get<elastica::tags::ShearStretchStrain>(slicedrod));
    auto Curvature(blocks::get<elastica::tags::Curvature>(slicedrod));
    auto ReferenceShearStretchStrain(blocks::get<elastica::tags::ReferenceShearStretchStrain>(slicedrod));
    auto ReferenceCurvature(blocks::get<elastica::tags::ReferenceCurvature>(slicedrod));
    auto InternalStress(blocks::get<elastica::tags::InternalStress>(slicedrod));
    auto InternalCouple(blocks::get<elastica::tags::InternalCouple>(slicedrod));
    auto Position(blocks::get<elastica::tags::Position>(slicedrod));
    auto Director(blocks::get<elastica::tags::Director>(slicedrod));
    auto Velocity(blocks::get<elastica::tags::Velocity>(slicedrod));
    auto AngularVelocity(blocks::get<elastica::tags::AngularVelocity>(slicedrod));
    auto Acceleration(blocks::get<elastica::tags::Acceleration>(slicedrod));
    auto AngularAcceleration(blocks::get<elastica::tags::AngularAcceleration>(slicedrod));


    CHECK_ITERABLE_APPROX(ElementDimension , ElementDimension_PYEL);
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::pageslice(MassSecondMomentOfInertia, i) - blaze::pageslice(MassSecondMomentOfInertia_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::pageslice(InvMassSecondMomentOfInertia, i) - blaze::pageslice(InvMassSecondMomentOfInertia_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::pageslice(ShearStretchRigidityMatrix, i) - blaze::pageslice(ShearStretchRigidityMatrix_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::pageslice(BendingTwistRigidityMatrix, i) - blaze::pageslice(BendingTwistRigidityMatrix_PYEL, i)) == Approx(0.0).margin(1e-6));
    CHECK_ITERABLE_APPROX(ElementVolume , ElementVolume_PYEL);
    CHECK_ITERABLE_APPROX(Mass , Mass_PYEL);
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(InternalLoads, i) - blaze::row(InternalLoads_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(InternalTorques, i) - blaze::row(InternalTorques_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(ExternalLoads, i) - blaze::row(ExternalLoads_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(ExternalTorques, i) - blaze::row(ExternalTorques_PYEL, i)) == Approx(0.0).margin(1e-6));
    CHECK_ITERABLE_APPROX(ElementLength , ElementLength_PYEL);
    CHECK_ITERABLE_APPROX(ReferenceElementLength , ReferenceElementLength_PYEL);
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(Tangent, i) - blaze::row(Tangent_PYEL, i)) == Approx(0.0).margin(1e-6));
    CHECK_ITERABLE_APPROX(ElementDilatation , ElementDilatation_PYEL);
    CHECK_ITERABLE_APPROX(VoronoiDilatation , VoronoiDilatation_PYEL);
    CHECK_ITERABLE_APPROX(ReferenceVoronoiLength , ReferenceVoronoiLength_PYEL);
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(ShearStretchStrain, i) - blaze::row(ShearStretchStrain_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(Curvature, i) - blaze::row(Curvature_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(ReferenceShearStretchStrain, i) - blaze::row(ReferenceShearStretchStrain_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(ReferenceCurvature, i) - blaze::row(ReferenceCurvature_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(InternalStress, i) - blaze::row(InternalStress_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(InternalCouple, i) - blaze::row(InternalCouple_PYEL, i)) == Approx(0.0).margin(1e-6));
    CHECK_MATRIX_APPROX(Position, Position_PYEL);
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(Position, i) - blaze::row(Position_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::pageslice(Director, i) - blaze::pageslice(Director_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(Velocity, i) - blaze::row(Velocity_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(AngularVelocity, i) - blaze::row(AngularVelocity_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(Acceleration, i) - blaze::row(Acceleration_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::row(AngularAcceleration, i) - blaze::row(AngularAcceleration_PYEL, i)) == Approx(0.0).margin(1e-6));
    // clang-format on
  }
  ELASTICA_TEST_CASE("Unit.z.Integrated.CosseratRods.FullWithDirector",
                     "[Systems][Integrated]") {
    /*
     * rng_seed = 250
     * shuffle_variables = {
     * 'internal_forces':'InternalLoads',
     * 'internal_torques':'InternalTorques',
     *  'external_forces':'ExternalLoads',
     *  'external_torques':'ExternalTorques',
     * 'internal_stress':'InternalStress',
     * 'internal_couple':'InternalCouple',
     * 'rest_sigma':'ReferenceShearStretchStrain',
     * 'rest_kappa':'ReferenceCurvature',
     * }
     * perturb_mag = 0.1
     * perturb_variables = {
     *  'rest_lengths':'ReferenceElementLength',
     *  'position_collection':'Position',
     *  'director_collection':'Director',
     *  'velocity_collection':'Velocity',
     *  'acceleration_collection':'Acceleration',
     *  'omega_collection':'AngularVelocity',
     *  'alpha_collection':'AngularAcceleration',
     *  }
     * director_perturb_ang = 3 # in degree
     */
    elastica::MaterialID myMaterial = elastica::create_material(
        7.874, 0.24, 200, 200.0 / 1.24, 200, 1e-10, 1e-10, 0.1, 0.1);

    using Init = ::elastica::cosserat_rod::StraightCosseratRodInitializer;

    Model simple_rod{};
    Init straight_initializer{
        // Initialization Wrapper
        Init::NElement{5UL},
        Init::Material{myMaterial},
        Init::Radius{0.007522},
        Init::Length{0.18},
        Init::Origin{::elastica::Vec3{0.0, 0.0, 0.0}},
        Init::Direction{::elastica::Vec3{0.0, 1.0, 0.0}},
        Init::Normal{::elastica::Vec3{0.0, 0.0, 1.0}},
        Init::ForceDampingCoefficient{0.2},
        Init::TorqueDampingCoefficient{0.2},
    };
    auto slicedrod =
        simple_rod.emplace_back(straight_initializer.initialize<Plugin>());

    // clang-format off
    blaze::DynamicMatrix<elastica::real_t> InternalLoads_REPLACED( 3UL, 6UL );     InternalLoads_REPLACED = {{0.203957429946802,0.6553728672957231,0.07586006916499055,0.2266804768321402,0.023150782148356863,0.8267331529503883},{0.15482201584295285,0.5341137261531649,0.8217521666658159,0.08207419303514252,0.2903135892954033,0.2571735009896835},{0.14465615688052758,0.6064242289540898,0.5539788198297757,0.5294675598323154,0.04860584329765816,0.4773015165169989}};
    blaze::DynamicMatrix<elastica::real_t> InternalTorques_REPLACED( 3UL, 5UL );     InternalTorques_REPLACED = {{0.7350951313834501,0.7091748026745681,0.15380848652812207,0.04987070974939489,0.4252467292498079},{0.7562052074199033,0.8984525966266456,0.2720759351923555,0.8572655759809178,0.5079311479102631},{0.05713725081795762,0.9519014792912509,0.35573031120251863,0.7560601197761251,0.6850910122901672}};
    blaze::DynamicMatrix<elastica::real_t> ExternalLoads_REPLACED( 3UL, 6UL );     ExternalLoads_REPLACED = {{0.1791075438221752,0.9899214316476954,0.7463701769482871,0.5090261497759466,0.9544198771087045,0.9468784978450715},{0.682594016831527,0.4165951128802907,0.3851920271071624,0.8590050248260485,0.8718064970595327,0.7653391073424969},{0.023139976989049904,0.5033417877320233,0.4084747987104368,0.2787462316282022,0.28563869362760064,0.9108284397359842}};
    blaze::DynamicMatrix<elastica::real_t> ExternalTorques_REPLACED( 3UL, 5UL );     ExternalTorques_REPLACED = {{0.2011706275871653,0.35044024901157045,0.3469039633761569,0.730724784965762,0.5070191807639516},{0.9066304124156365,0.8407550791041166,0.013837335101241854,0.33352982437168033,0.03454364086259509},{0.4341406093148956,0.8678259663477438,0.8695583275771691,0.8563534969675165,0.38881434776187773}};
    blaze::DynamicMatrix<elastica::real_t> InternalStress_REPLACED( 3UL, 5UL );     InternalStress_REPLACED = {{0.7596379212252692,0.3721425106646684,0.37886398601387816,0.2242284930736973,0.8432162709655906},{0.006054751261704738,0.48548428952997214,0.519602535005509,0.7863058184251852,0.862033563570232},{0.3951042311864247,0.2986331591155028,0.5703814493289531,0.909277466665711,0.9909018868251039}};
    blaze::DynamicMatrix<elastica::real_t> InternalCouple_REPLACED( 3UL, 4UL );     InternalCouple_REPLACED = {{0.4311385046965821,0.6832921248581908,0.8865673324660336,0.1672225452678583},{0.2938057843773313,0.3857070159278747,0.8153466251274433,0.2208439831421144},{0.09631586788311886,0.8574195723473468,0.9543850950940012,0.6935530433914594}};
    blaze::DynamicMatrix<elastica::real_t> ReferenceShearStretchStrain_REPLACED( 3UL, 5UL );     ReferenceShearStretchStrain_REPLACED = {{0.7184804985324387,0.6583747305005937,0.364722264668331,0.9093049831040217,0.45994236798265253},{0.3060060659583749,0.7782948087672685,0.9241066144615926,0.2995753702543501,0.9044553750545788},{0.9642671548467786,0.45804198361957094,0.33837614563409246,0.41542573531258753,0.9118989166105574}};
    blaze::DynamicMatrix<elastica::real_t> ReferenceCurvature_REPLACED( 3UL, 4UL );     ReferenceCurvature_REPLACED = {{0.8711649329104771,0.18219259883358419,0.604351208520874,0.6482932130639908},{0.8154536694563059,0.7224160802374119,0.929567645128783,0.1977950177866734},{0.23536632862462836,0.28433022419255993,0.44594753847623225,0.02922136066212011}};
    blaze::DynamicMatrix<elastica::real_t> ShearStretchStrain_REPLACED( 3UL, 5UL );     ShearStretchStrain_REPLACED = {{0.17015537742449616,0.24188911472131192,0.12951526410122371,0.6410513197951541,0.6697892199549795},{0.17516497260853625,0.31704036797809554,0.2762522726764465,0.3198716032399259,0.07718805919824767},{0.18329113443805112,0.30371975879604374,0.9994931973914124,0.24676623009938614,0.6209416000520357}};
    blaze::DynamicMatrix<elastica::real_t> Curvature_REPLACED( 3UL, 4UL );     Curvature_REPLACED = {{0.11181426579214082,0.2518362108425869,0.14002481988864546,0.9348913021909042},{0.04802068166268403,0.40716714832443246,0.6487104333959989,0.23171343732239702},{0.938828576197833,0.5867739522581421,0.030625780555092907,0.795681353275379}};
    blaze::DynamicVector<elastica::real_t> ReferenceElementLength_REPLACED( 5UL ); ReferenceElementLength_REPLACED={0.10619813470026732,0.06846781592702295,0.08932266024355683,0.05339723069495667,0.06191007336705552};
    blaze::DynamicMatrix<elastica::real_t> Position_REPLACED( 3UL, 6UL );     Position_REPLACED = {{0.08870522268171689,0.024612313410292953,0.01793587216973136,0.0010584322218191832,0.013381251500294357,0.07315923638365135},{0.09792447196272275,0.03980292966236467,0.09801932960152229,0.16346768585830138,0.19246603367494378,0.18873080125423047},{0.007504401265764327,0.011626806829448355,0.038039767086985435,0.01195050117458274,0.08671144050250446,0.07553838100418793}};
    blaze::DynamicTensor<elastica::real_t> Director_REPLACED( 3UL, 3UL, 5UL);     Director_REPLACED = {{{0.02943969992717696,0.04541644313472981,0.02015957643685662,0.03603802609541951,0.010014438658309128},{-0.02517894880786191,-0.013269490062532379,-0.029224170189883124,-0.0175290305472582,-0.008636467209964098},{0.9992493805878133,0.9988800064704819,0.9993695709569104,0.9991966742154514,0.9999125574031411}},{{0.9989597345335528,0.9987244914988865,0.9990679485990466,0.9987940132621695,0.9986690289624184},{0.03556977895212262,0.022686080161656193,0.03874866740700368,0.03399167192369593,0.0506823055525266},{-0.02853488401852649,-0.045108002030561936,-0.019020380024440578,-0.035427183228149105,-0.00956422998157405}},{{-0.034824601201613216,-0.02206211171397978,-0.03816838429877878,-0.03334336136017018,-0.050595272601489875},{0.9990499543877813,0.999654571539779,0.9988215499531906,0.9992683820315266,0.998677483143588},{0.026199919635762153,0.014282885784763959,0.02997808848353377,0.018732883528704277,0.009132527162258071}}};
    blaze::DynamicMatrix<elastica::real_t> Velocity_REPLACED( 3UL, 6UL );     Velocity_REPLACED = {{0.07054612126439226,0.012027841764838499,0.0010865578899558637,0.09268345190901274,0.06483176946365873,0.08031282746334137},{0.032171779450055886,0.08025912067963979,0.004325978162144239,0.02231566995787714,0.08681653073682254,0.0937261112329596},{0.09683642462618614,0.013000786308630176,0.004212250850022337,0.003862820481706364,0.011986298087795312,0.0344416863076018}};
    blaze::DynamicMatrix<elastica::real_t> Acceleration_REPLACED( 3UL, 6UL );     Acceleration_REPLACED = {{0.0640429219321413,0.0007288269619492249,0.0781986334077072,0.023669101353811985,0.009110851975167334,0.06301371619343733},{0.048571724002000216,0.053397382524431394,0.08479892010142935,0.03772285187398736,0.004565207003110849,0.09596209846086073},{0.07471448935660593,0.026858259125849195,0.072565897173148,0.08977362823417845,0.04147658039054533,0.048065276441950605}};
    blaze::DynamicMatrix<elastica::real_t> AngularVelocity_REPLACED( 3UL, 5UL );     AngularVelocity_REPLACED = {{0.06082509135226907,0.0007186504236994629,0.09601940820517396,0.05070126100005872,0.07224977770147603},{0.022207577772058774,0.00844527284372777,0.06436320476061112,0.03337980231245532,0.05555948511703849},{0.06683155389284953,0.09849435093158453,0.027355999551702884,0.034268572069556025,0.00784288732251256}};
    blaze::DynamicMatrix<elastica::real_t> AngularAcceleration_REPLACED( 3UL, 5UL );     AngularAcceleration_REPLACED = {{0.08855921743181473,0.08941407668705303,0.016310657640645633,0.01935356614221394,0.019692299947268677},{0.0952422444147717,0.006880887603691888,0.04015261047111745,0.07433375528438327,0.06016618072288619},{0.06857668566963625,0.023246115331522012,0.08842427315391167,0.03348525452550784,0.07693202348730308}};
    blaze::DynamicVector<elastica::real_t> ElementDimension_PYEL( 5UL ); ElementDimension_PYEL={0.004849262720391566,0.005629386852070082,0.005302317679887516,0.005010670759604674,0.005781983320205748};
    blaze::DynamicTensor<elastica::real_t> MassSecondMomentOfInertia_PYEL( 3UL, 3UL, 5UL);     MassSecondMomentOfInertia_PYEL = {{{7.127235497145191e-10,7.127235497145191e-10,7.12723549714519e-10,7.127235497145191e-10,7.127235497145191e-10},{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0,0.0},{7.127235497145191e-10,7.127235497145191e-10,7.12723549714519e-10,7.127235497145191e-10,7.127235497145191e-10},{0.0,0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},{1.4254470994290381e-09,1.4254470994290381e-09,1.425447099429038e-09,1.4254470994290381e-09,1.4254470994290381e-09}}};
    blaze::DynamicTensor<elastica::real_t> InvMassSecondMomentOfInertia_PYEL( 3UL, 3UL, 5UL);     InvMassSecondMomentOfInertia_PYEL = {{{1403068553.579504,1403068553.579504,1403068553.5795043,1403068553.579504,1403068553.579504},{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0,0.0},{1403068553.579504,1403068553.579504,1403068553.5795043,1403068553.579504,1403068553.579504},{0.0,0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},{701534276.789752,701534276.789752,701534276.7897521,701534276.789752,701534276.789752}}};
    blaze::DynamicTensor<elastica::real_t> ShearStretchRigidityMatrix_PYEL( 3UL, 3UL, 5UL);     ShearStretchRigidityMatrix_PYEL = {{{0.03822641567117308,0.03822641567117308,0.03822641567117308,0.03822641567117308,0.03822641567117308},{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0,0.0},{0.03822641567117308,0.03822641567117308,0.03822641567117308,0.03822641567117308,0.03822641567117308},{0.0,0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},{0.035550566574190964,0.035550566574190964,0.035550566574190964,0.035550566574190964,0.035550566574190964}}};
    blaze::DynamicTensor<elastica::real_t> BendingTwistRigidityMatrix_PYEL( 3UL, 3UL, 4UL);     BendingTwistRigidityMatrix_PYEL = {{{5.028670658104868e-07,5.028670658104868e-07,5.028670658104868e-07,5.028670658104868e-07},{0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0},{5.028670658104868e-07,5.028670658104868e-07,5.028670658104868e-07,5.028670658104868e-07},{0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0},{8.110759125975593e-07,8.110759125975594e-07,8.110759125975593e-07,8.110759125975593e-07}}};
    blaze::DynamicVector<elastica::real_t> ElementVolume_PYEL( 5UL ); ElementVolume_PYEL={6.3991019833543736e-06,6.3991019833543736e-06,6.399101983354373e-06,6.399101983354375e-06,6.399101983354375e-06};
    blaze::DynamicVector<elastica::real_t> Mass_PYEL( 6UL ); Mass_PYEL={2.5193264508466167e-05,5.038652901693233e-05,5.0386529016932326e-05,5.038652901693234e-05,5.038652901693235e-05,2.5193264508466173e-05};
    blaze::DynamicMatrix<elastica::real_t> InternalLoads_PYEL( 3UL, 6UL );     InternalLoads_PYEL = {{-0.041038906491795285,0.004713987421622501,-0.015246413145440504,0.04804437406791679,0.006048465950325292,-0.0061626622338915435},{-0.1096120526496636,0.08498634657125886,-0.00463961937961776,0.006838055937436686,-0.05336124278622377,0.07209255372955521},{-0.03409259765322157,0.022621639556013026,-0.021566229266761775,0.043846812186692896,-0.0373660534321958,0.025024419644009154}};
    blaze::DynamicMatrix<elastica::real_t> InternalTorques_PYEL( 3UL, 5UL );     InternalTorques_PYEL = {{0.003701650458195074,0.002228835058144662,0.0022942392764492784,-0.0009598517205025257,-0.005220700130289879},{0.001801725521722019,-0.0002235303230338732,-0.0037838147923481276,0.00131849242461081,-0.0012793427596023803},{-0.0034402836042479205,-0.002243636061512467,0.000599663580632446,-0.0008751321486323427,0.001339898753784716}};
    blaze::DynamicMatrix<elastica::real_t> ExternalLoads_PYEL( 3UL, 6UL );     ExternalLoads_PYEL = {{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}};
    blaze::DynamicMatrix<elastica::real_t> ExternalTorques_PYEL( 3UL, 5UL );     ExternalTorques_PYEL = {{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0}};
    blaze::DynamicVector<elastica::real_t> ElementLength_PYEL( 5UL ); ElementLength_PYEL={0.08661991067808132,0.06427572293705343,0.07244987999822851,0.08112924318800294,0.060927799045300546};
    blaze::DynamicVector<elastica::real_t> ReferenceElementLength_PYEL( 5UL ); ReferenceElementLength_PYEL={0.10619813470026732,0.06846781592702295,0.08932266024355683,0.05339723069495667,0.06191007336705552};
    blaze::DynamicMatrix<elastica::real_t> Tangent_PYEL( 3UL, 5UL );     Tangent_PYEL = {{-0.7399327564492894,-0.10387189650282072,-0.2329533181880336,0.1518912144899365,0.9811282504872915},{-0.6709951770368819,0.9057292128191257,0.9033604508161971,0.3574339742014327,-0.061305881375037244},{0.04759189349668978,0.410932138148082,-0.3601008850951942,0.9215042121701569,-0.1833819647745559}};
    blaze::DynamicVector<elastica::real_t> ElementDilatation_PYEL( 5UL ); ElementDilatation_PYEL={0.8156443700499693,0.9387727951708332,0.811103025824341,1.5193530101115438,0.9841338530495479};
    blaze::DynamicVector<elastica::real_t> VoronoiDilatation_PYEL( 4UL ); VoronoiDilatation_PYEL={2.095772689099094,1.8989667074344718,2.133043377586548,1.973014475462548};
    blaze::DynamicVector<elastica::real_t> ReferenceVoronoiLength_PYEL( 4UL ); ReferenceVoronoiLength_PYEL={0.036,0.03599999999999999,0.036,0.036000000000000004};
    blaze::DynamicMatrix<elastica::real_t> ShearStretchStrain_PYEL( 3UL, 5UL );     ShearStretchStrain_PYEL = {{0.034801689674478956,0.3696284950218688,-0.317117004204837,1.397762732717163,-0.1702659962850838},{-0.6234689394004518,-0.09549975041059774,-0.15482571754483973,0.19935661105907845,0.9629446452618708},{-1.5247390421846734,-0.14235850479804735,-0.26968916115651365,-0.4387960673791599,-1.1107544192738452}};
    blaze::DynamicMatrix<elastica::real_t> Curvature_PYEL( 3UL, 4UL );     Curvature_PYEL = {{-0.36681621270129505,0.4616826432514418,-0.1428502579128258,0.4733903392453568},{-0.3178034905260345,0.42150080107524307,-0.3086883588523472,-0.2774261021905808},{0.45334159787266465,-0.7149940790980792,0.4525740099463648,-0.7120742047812789}};
    blaze::DynamicMatrix<elastica::real_t> ReferenceShearStretchStrain_PYEL( 3UL, 5UL );     ReferenceShearStretchStrain_PYEL = {{0.7184804985324387,0.6583747305005937,0.364722264668331,0.9093049831040217,0.45994236798265253},{0.3060060659583749,0.7782948087672685,0.9241066144615926,0.2995753702543501,0.9044553750545788},{0.9642671548467786,0.45804198361957094,0.33837614563409246,0.41542573531258753,0.9118989166105574}};
    blaze::DynamicMatrix<elastica::real_t> ReferenceCurvature_PYEL( 3UL, 4UL );     ReferenceCurvature_PYEL = {{0.8711649329104771,0.18219259883358419,0.604351208520874,0.6482932130639908},{0.8154536694563059,0.7224160802374119,0.929567645128783,0.1977950177866734},{0.23536632862462836,0.28433022419255993,0.44594753847623225,0.02922136066212011}};
    blaze::DynamicMatrix<elastica::real_t> InternalStress_PYEL( 3UL, 5UL );     InternalStress_PYEL = {{-0.026134590332976857,-0.011037733620896164,-0.026064271312874466,0.018671988974517717,-0.02409060689194855},{-0.035530497910812336,-0.033402034030342566,-0.04124371580434601,-0.0038310039470476538,0.002235835155247504},{-0.0884855805111405,-0.02134457753466731,-0.021617066170515297,-0.030368069065718383,-0.0719064720738679}};
    blaze::DynamicMatrix<elastica::real_t> InternalCouple_PYEL( 3UL, 4UL );     InternalCouple_PYEL = {{-6.22539946222497e-07,1.4054633855965068e-07,-3.7574300899480754e-07,-8.79528949589983e-08},{-5.69877702849045e-07,-1.5132038348982335e-07,-6.226781634442094e-07,-2.3897305021413514e-07},{1.7679449042904973e-07,-8.105278712723754e-07,5.374571394939462e-09,-6.012469772465286e-07}};
    blaze::DynamicMatrix<elastica::real_t> Position_PYEL( 3UL, 6UL );     Position_PYEL = {{0.08870522268171689,0.024612313410292953,0.01793587216973136,0.0010584322218191832,0.013381251500294357,0.07315923638365135},{0.09792447196272275,0.03980292966236467,0.09801932960152229,0.16346768585830138,0.19246603367494378,0.18873080125423047},{0.007504401265764327,0.011626806829448355,0.038039767086985435,0.01195050117458274,0.08671144050250446,0.07553838100418793}};
    blaze::DynamicTensor<elastica::real_t> Director_PYEL( 3UL, 3UL, 5UL);     Director_PYEL = {{{0.02943969992717696,0.04541644313472981,0.02015957643685662,0.03603802609541951,0.010014438658309128},{-0.02517894880786191,-0.013269490062532379,-0.029224170189883124,-0.0175290305472582,-0.008636467209964098},{0.9992493805878133,0.9988800064704819,0.9993695709569104,0.9991966742154514,0.9999125574031411}},{{0.9989597345335528,0.9987244914988865,0.9990679485990466,0.9987940132621695,0.9986690289624184},{0.03556977895212262,0.022686080161656193,0.03874866740700368,0.03399167192369593,0.0506823055525266},{-0.02853488401852649,-0.045108002030561936,-0.019020380024440578,-0.035427183228149105,-0.00956422998157405}},{{-0.034824601201613216,-0.02206211171397978,-0.03816838429877878,-0.03334336136017018,-0.050595272601489875},{0.9990499543877813,0.999654571539779,0.9988215499531906,0.9992683820315266,0.998677483143588},{0.026199919635762153,0.014282885784763959,0.02997808848353377,0.018732883528704277,0.009132527162258071}}};
    blaze::DynamicMatrix<elastica::real_t> Velocity_PYEL( 3UL, 6UL );     Velocity_PYEL = {{0.07054612126439226,0.012027841764838499,0.0010865578899558637,0.09268345190901274,0.06483176946365873,0.08031282746334137},{0.032171779450055886,0.08025912067963979,0.004325978162144239,0.02231566995787714,0.08681653073682254,0.0937261112329596},{0.09683642462618614,0.013000786308630176,0.004212250850022337,0.003862820481706364,0.011986298087795312,0.0344416863076018}};
    blaze::DynamicMatrix<elastica::real_t> AngularVelocity_PYEL( 3UL, 5UL );     AngularVelocity_PYEL = {{0.06082509135226907,0.0007186504236994629,0.09601940820517396,0.05070126100005872,0.07224977770147603},{0.022207577772058774,0.00844527284372777,0.06436320476061112,0.03337980231245532,0.05555948511703849},{0.06683155389284953,0.09849435093158453,0.027355999551702884,0.034268572069556025,0.00784288732251256}};
    blaze::DynamicMatrix<elastica::real_t> Acceleration_PYEL( 3UL, 6UL );     Acceleration_PYEL = {{5480.378983199344,19740.105906780598,14510.302219014797,11055.941631872687,19062.006488604628,37339.97375747209},{22743.458434666674,9954.673783601836,7552.661696535209,17184.01917450038,16243.334681741446,33240.29963606479},{-434.7440031239719,10438.572323790888,7678.809733324852,6402.366864891369,4927.361440435261,37146.94691771687}};
    blaze::DynamicMatrix<elastica::real_t> AngularAcceleration_PYEL( 3UL, 5UL );     AngularAcceleration_PYEL = {{234456852.4976382,464522523.4315273,397399130.2762484,1555681089.033963,692886985.9824619},{1039614250.4030539,1107116709.9705732,11441239.37815219,713815052.4968753,45931584.37685913},{246447795.91279826,570056331.0829066,495134319.51001614,911835752.5814157,269363908.3911978}};


    blocks::get<elastica::tags::InternalLoads>(slicedrod) = InternalLoads_REPLACED;
    blocks::get<elastica::tags::InternalTorques>(slicedrod) = InternalTorques_REPLACED;
    blocks::get<elastica::tags::ExternalLoads>(slicedrod) = ExternalLoads_REPLACED;
    blocks::get<elastica::tags::ExternalTorques>(slicedrod) = ExternalTorques_REPLACED;
    blocks::get<elastica::tags::InternalStress>(slicedrod) = InternalStress_REPLACED;
    blocks::get<elastica::tags::InternalCouple>(slicedrod) = InternalCouple_REPLACED;
    blocks::get<elastica::tags::ReferenceShearStretchStrain>(slicedrod) = ReferenceShearStretchStrain_REPLACED;
    blocks::get<elastica::tags::ReferenceCurvature>(slicedrod) = ReferenceCurvature_REPLACED;
    blocks::get<elastica::tags::ShearStretchStrain>(slicedrod) = ShearStretchStrain_REPLACED;
    blocks::get<elastica::tags::Curvature>(slicedrod) = Curvature_REPLACED;
    blocks::get<elastica::tags::ReferenceElementLength>(slicedrod) = ReferenceElementLength_REPLACED;
    blocks::get<elastica::tags::Position>(slicedrod) = Position_REPLACED;
    blocks::get<elastica::tags::Director>(slicedrod) = Director_REPLACED;
    blocks::get<elastica::tags::Velocity>(slicedrod) = Velocity_REPLACED;
    blocks::get<elastica::tags::Acceleration>(slicedrod) = Acceleration_REPLACED;
    blocks::get<elastica::tags::AngularVelocity>(slicedrod) = AngularVelocity_REPLACED;
    blocks::get<elastica::tags::AngularAcceleration>(slicedrod) = AngularAcceleration_REPLACED;


    slicedrod.compute_internal_forces();
    slicedrod.compute_internal_torques();
    slicedrod.update_acceleration();


    auto ElementDimension(blocks::get<elastica::tags::ElementDimension>(slicedrod));
    auto MassSecondMomentOfInertia(blocks::get<elastica::tags::MassSecondMomentOfInertia>(slicedrod));
    auto InvMassSecondMomentOfInertia(blocks::get<elastica::tags::InvMassSecondMomentOfInertia>(slicedrod));
    auto ShearStretchRigidityMatrix(blocks::get<elastica::tags::ShearStretchRigidityMatrix>(slicedrod));
    auto BendingTwistRigidityMatrix(blocks::get<elastica::tags::BendingTwistRigidityMatrix>(slicedrod));
    auto ElementVolume(blocks::get<elastica::tags::ElementVolume>(slicedrod));
    auto Mass(blocks::get<elastica::tags::Mass>(slicedrod));
    auto InternalLoads(blocks::get<elastica::tags::InternalLoads>(slicedrod));
    auto InternalTorques(blocks::get<elastica::tags::InternalTorques>(slicedrod));
    auto ExternalLoads(blocks::get<elastica::tags::ExternalLoads>(slicedrod));
    auto ExternalTorques(blocks::get<elastica::tags::ExternalTorques>(slicedrod));
    auto ElementLength(blocks::get<elastica::tags::ElementLength>(slicedrod));
    auto ReferenceElementLength(blocks::get<elastica::tags::ReferenceElementLength>(slicedrod));
    auto Tangent(blocks::get<elastica::tags::Tangent>(slicedrod));
    auto ElementDilatation(blocks::get<elastica::tags::ElementDilatation>(slicedrod));
    auto VoronoiDilatation(blocks::get<elastica::tags::VoronoiDilatation>(slicedrod));
    auto ReferenceVoronoiLength(blocks::get<elastica::tags::ReferenceVoronoiLength>(slicedrod));
    auto ShearStretchStrain(blocks::get<elastica::tags::ShearStretchStrain>(slicedrod));
    auto Curvature(blocks::get<elastica::tags::Curvature>(slicedrod));
    auto ReferenceShearStretchStrain(blocks::get<elastica::tags::ReferenceShearStretchStrain>(slicedrod));
    auto ReferenceCurvature(blocks::get<elastica::tags::ReferenceCurvature>(slicedrod));
    auto InternalStress(blocks::get<elastica::tags::InternalStress>(slicedrod));
    auto InternalCouple(blocks::get<elastica::tags::InternalCouple>(slicedrod));
    auto Position(blocks::get<elastica::tags::Position>(slicedrod));
    auto Director(blocks::get<elastica::tags::Director>(slicedrod));
    auto Velocity(blocks::get<elastica::tags::Velocity>(slicedrod));
    auto AngularVelocity(blocks::get<elastica::tags::AngularVelocity>(slicedrod));
    auto Acceleration(blocks::get<elastica::tags::Acceleration>(slicedrod));
    auto AngularAcceleration(blocks::get<elastica::tags::AngularAcceleration>(slicedrod));


    CHECK_ITERABLE_APPROX(ElementDimension , ElementDimension_PYEL);
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::pageslice(MassSecondMomentOfInertia, i) - blaze::pageslice(MassSecondMomentOfInertia_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::pageslice(InvMassSecondMomentOfInertia, i) - blaze::pageslice(InvMassSecondMomentOfInertia_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::pageslice(ShearStretchRigidityMatrix, i) - blaze::pageslice(ShearStretchRigidityMatrix_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::pageslice(BendingTwistRigidityMatrix, i) - blaze::pageslice(BendingTwistRigidityMatrix_PYEL, i)) == Approx(0.0).margin(1e-6));
    CHECK_ITERABLE_APPROX(ElementVolume , ElementVolume_PYEL);
    CHECK_ITERABLE_APPROX(Mass , Mass_PYEL);
    CHECK_MATRIX_APPROX(InternalLoads , InternalLoads_PYEL);
    CHECK_MATRIX_APPROX(InternalTorques , InternalTorques_PYEL);
    CHECK_MATRIX_APPROX(ExternalLoads , ExternalLoads_PYEL);
    CHECK_MATRIX_APPROX(ExternalTorques , ExternalTorques_PYEL);
    CHECK_ITERABLE_APPROX(ElementLength , ElementLength_PYEL);
    CHECK_ITERABLE_APPROX(ReferenceElementLength , ReferenceElementLength_PYEL);
    CHECK_MATRIX_APPROX(Tangent , Tangent_PYEL);
    CHECK_ITERABLE_APPROX(ElementDilatation , ElementDilatation_PYEL);
    CHECK_ITERABLE_APPROX(VoronoiDilatation , VoronoiDilatation_PYEL);
    CHECK_ITERABLE_APPROX(ReferenceVoronoiLength , ReferenceVoronoiLength_PYEL);
    CHECK_MATRIX_APPROX(ShearStretchStrain , ShearStretchStrain_PYEL);
    CHECK_MATRIX_APPROX(Curvature , Curvature_PYEL);
    CHECK_MATRIX_APPROX(ReferenceShearStretchStrain , ReferenceShearStretchStrain_PYEL);
    CHECK_MATRIX_APPROX(ReferenceCurvature , ReferenceCurvature_PYEL);
    CHECK_MATRIX_APPROX(InternalStress , InternalStress_PYEL);
    CHECK_MATRIX_APPROX(InternalCouple , InternalCouple_PYEL);
    CHECK_MATRIX_APPROX(Position , Position_PYEL);
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::pageslice(Director, i) - blaze::pageslice(Director_PYEL, i)) == Approx(0.0).margin(1e-6));
    CHECK_MATRIX_APPROX(Velocity , Velocity_PYEL);
    CHECK_MATRIX_APPROX(AngularVelocity , AngularVelocity_PYEL);
    CHECK_MATRIX_APPROX(Acceleration , Acceleration_PYEL);
    CHECK_MATRIX_APPROX(AngularAcceleration , AngularAcceleration_PYEL);
    // clang-format on
  }

  ELASTICA_TEST_CASE("Unit.z.Integrated.CosseratRods.JustDirector",
                     "[Systems][Integrated]") {
    /*
     * rng_seed = 250
     * shuffle_variables = {
     * }
     * perturb_mag = 0.1
     * perturb_variables = {
     *  'director_collection':'Director',
     *  }
     * director_perturb_ang = 3 # in degree
     */
    elastica::MaterialID myMaterial = elastica::create_material(
        7.874, 0.24, 200, 200.0 / 1.24, 200, 1e-10, 1e-10, 0.1, 0.1);

    using Init = ::elastica::cosserat_rod::StraightCosseratRodInitializer;

    Model simple_rod{};
    Init straight_initializer{
        // Initialization Wrapper
        Init::NElement{5UL},
        Init::Material{myMaterial},
        Init::Radius{0.007522},
        Init::Length{0.18},
        Init::Origin{::elastica::Vec3{0.0, 0.0, 0.0}},
        Init::Direction{::elastica::Vec3{0.0, 1.0, 0.0}},
        Init::Normal{::elastica::Vec3{0.0, 0.0, 1.0}},
        Init::ForceDampingCoefficient{0.2},
        Init::TorqueDampingCoefficient{0.2},
    };
    auto slicedrod =
        simple_rod.emplace_back(straight_initializer.initialize<Plugin>());

    // clang-format off
    blaze::DynamicTensor<elastica::real_t> Director_REPLACED( 3UL, 3UL, 5UL);     Director_REPLACED = {{{0.04971398932038238,0.0017621047613692155,0.028349298452394353,0.038510774335923696,0.03821246174013585},{-0.015314684238161978,-0.013798409812089229,-0.0075552298434086035,-0.010184223579856029,-0.0084156112261818},{0.9986460732975112,0.999903244755945,0.9995695252353736,0.9992062859340523,0.9992341994024465}},{{0.9987490848254921,0.9987252912155221,0.9986629009873723,0.9986882768317997,0.9986707636882262},{0.006133445547642621,0.05046445653036852,0.04345899798566483,0.034159394781008254,0.03493633455617835},{-0.049625058246374414,-0.0010636322507983514,-0.02799510109943551,-0.03814264630208201,-0.037896679038589016}},{{-0.005365149214593786,-0.05044489739587496,-0.043228780560440355,-0.03374382875101853,-0.03459065657274675},{0.9998639113861443,0.9986305335376803,0.9990266432864016,0.9993645067432956,0.9993541064184749},{0.015600444913708315,0.013869744517022007,0.008777159857382425,0.011486369441699338,0.00973942823772274}}};
    blaze::DynamicVector<elastica::real_t> ElementDimension_PYEL( 5UL ); ElementDimension_PYEL={0.007522,0.007522,0.007522,0.007522,0.007522};
    blaze::DynamicTensor<elastica::real_t> MassSecondMomentOfInertia_PYEL( 3UL, 3UL, 5UL);     MassSecondMomentOfInertia_PYEL = {{{7.127235497145191e-10,7.127235497145191e-10,7.12723549714519e-10,7.127235497145191e-10,7.127235497145191e-10},{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0,0.0},{7.127235497145191e-10,7.127235497145191e-10,7.12723549714519e-10,7.127235497145191e-10,7.127235497145191e-10},{0.0,0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},{1.4254470994290381e-09,1.4254470994290381e-09,1.425447099429038e-09,1.4254470994290381e-09,1.4254470994290381e-09}}};
    blaze::DynamicTensor<elastica::real_t> InvMassSecondMomentOfInertia_PYEL( 3UL, 3UL, 5UL);     InvMassSecondMomentOfInertia_PYEL = {{{1403068553.579504,1403068553.579504,1403068553.5795043,1403068553.579504,1403068553.579504},{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0,0.0},{1403068553.579504,1403068553.579504,1403068553.5795043,1403068553.579504,1403068553.579504},{0.0,0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},{701534276.789752,701534276.789752,701534276.7897521,701534276.789752,701534276.789752}}};
    blaze::DynamicTensor<elastica::real_t> ShearStretchRigidityMatrix_PYEL( 3UL, 3UL, 5UL);     ShearStretchRigidityMatrix_PYEL = {{{0.03822641567117308,0.03822641567117308,0.03822641567117308,0.03822641567117308,0.03822641567117308},{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0,0.0},{0.03822641567117308,0.03822641567117308,0.03822641567117308,0.03822641567117308,0.03822641567117308},{0.0,0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},{0.035550566574190964,0.035550566574190964,0.035550566574190964,0.035550566574190964,0.035550566574190964}}};
    blaze::DynamicTensor<elastica::real_t> BendingTwistRigidityMatrix_PYEL( 3UL, 3UL, 4UL);     BendingTwistRigidityMatrix_PYEL = {{{5.028670658104868e-07,5.028670658104868e-07,5.028670658104868e-07,5.028670658104868e-07},{0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0},{5.028670658104868e-07,5.028670658104868e-07,5.028670658104868e-07,5.028670658104868e-07},{0.0,0.0,0.0,0.0}},{{0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0},{8.110759125975593e-07,8.110759125975594e-07,8.110759125975593e-07,8.110759125975593e-07}}};
    blaze::DynamicVector<elastica::real_t> ElementVolume_PYEL( 5UL ); ElementVolume_PYEL={6.3991019833543736e-06,6.3991019833543736e-06,6.399101983354373e-06,6.399101983354375e-06,6.399101983354375e-06};
    blaze::DynamicVector<elastica::real_t> Mass_PYEL( 6UL ); Mass_PYEL={2.5193264508466167e-05,5.038652901693233e-05,5.0386529016932326e-05,5.038652901693234e-05,5.038652901693235e-05,2.5193264508466173e-05};
    blaze::DynamicMatrix<elastica::real_t> InternalLoads_PYEL( 3UL, 6UL );     InternalLoads_PYEL = {{0.0002050884702819235,0.0017230542914625428,-0.00027577401884923,-0.0003625204995647371,3.2368789650499136e-05,-0.0013222170329809985},{5.566282958831594e-06,5.044297848394425e-05,-1.6199302591079558e-05,-1.3817926053756082e-05,4.2536117888526524e-07,-2.6417393976825466e-05},{-0.0005963434109841614,6.620361730251491e-05,0.00019464329316259987,-0.00010356669992390316,6.677660101600687e-05,0.00037228659942694286}};
    blaze::DynamicMatrix<elastica::real_t> InternalTorques_PYEL( 3UL, 5UL );     InternalTorques_PYEL = {{-7.810645364284036e-06,-7.017124273100338e-05,-5.9831787832668335e-05,-4.6864119002227225e-05,-4.808638554137016e-05},{-2.1106020523688212e-05,-1.900632082117581e-05,-1.0283307513410038e-05,-1.4081045119008567e-05,-1.1555791481459718e-05},{-1.0795168233272579e-06,1.6845364206651774e-06,-3.784064832617711e-07,-2.319724854400688e-07,5.359371363920199e-09}};
    blaze::DynamicMatrix<elastica::real_t> ExternalLoads_PYEL( 3UL, 6UL );     ExternalLoads_PYEL = {{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}};
    blaze::DynamicMatrix<elastica::real_t> ExternalTorques_PYEL( 3UL, 5UL );     ExternalTorques_PYEL = {{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0}};
    blaze::DynamicVector<elastica::real_t> ElementLength_PYEL( 5UL ); ElementLength_PYEL={0.036,0.036,0.03599999999999999,0.036000000000000004,0.036000000000000004};
    blaze::DynamicVector<elastica::real_t> ReferenceElementLength_PYEL( 5UL ); ReferenceElementLength_PYEL={0.036,0.036,0.03599999999999999,0.036000000000000004,0.036000000000000004};
    blaze::DynamicMatrix<elastica::real_t> Tangent_PYEL( 3UL, 5UL );     Tangent_PYEL = {{0.0,0.0,0.0,0.0,0.0},{1.0,1.0,1.0,1.0,1.0},{0.0,0.0,0.0,0.0,0.0}};
    blaze::DynamicVector<elastica::real_t> ElementDilatation_PYEL( 5UL ); ElementDilatation_PYEL={1.0,1.0,1.0,1.0,1.0};
    blaze::DynamicVector<elastica::real_t> VoronoiDilatation_PYEL( 4UL ); VoronoiDilatation_PYEL={1.0,1.0,1.0,1.0};
    blaze::DynamicVector<elastica::real_t> ReferenceVoronoiLength_PYEL( 4UL ); ReferenceVoronoiLength_PYEL={0.036,0.03599999999999999,0.036,0.036000000000000004};
    blaze::DynamicMatrix<elastica::real_t> ShearStretchStrain_PYEL( 3UL, 5UL );     ShearStretchStrain_PYEL = {{-0.015314684238161978,-0.013798409812089229,-0.0075552298434086035,-0.010184223579856029,-0.0084156112261818},{0.006133445547642621,0.05046445653036852,0.04345899798566483,0.034159394781008254,0.03493633455617835},{-0.00013608861385572268,-0.001369466462319724,-0.0009733567135984389,-0.0006354932567044225,-0.0006458935815251143}};
    blaze::DynamicMatrix<elastica::real_t> Curvature_PYEL( 3UL, 4UL );     Curvature_PYEL = {{1.2512885210626974,-0.20278499442246198,-0.26100618139447257,0.021656980378066265},{-0.0798286408606425,-0.13854144634260093,0.08393713426275719,-0.04938803085366383},{-1.3309689100123652,0.745946942747046,0.2793981556552399,-0.006607730892606791}};
    blaze::DynamicMatrix<elastica::real_t> ReferenceShearStretchStrain_PYEL( 3UL, 5UL );     ReferenceShearStretchStrain_PYEL = {{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0}};
    blaze::DynamicMatrix<elastica::real_t> ReferenceCurvature_PYEL( 3UL, 4UL );     ReferenceCurvature_PYEL = {{0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0}};
    blaze::DynamicMatrix<elastica::real_t> InternalStress_PYEL( 3UL, 5UL );     InternalStress_PYEL = {{-0.0005854254855607424,-0.0005274637490781161,-0.0002888093564853892,-0.0003893063638517389,-0.0003216986528590161},{0.00023445963900069265,0.001929075291949712,0.0016612817216526974,0.0013057912239745218,0.001335490846771642},{-4.838027326867236e-06,-4.8685308639819125e-05,-3.460338264721703e-05,-2.259214532992e-05,-2.2961882769851216e-05}};
    blaze::DynamicMatrix<elastica::real_t> InternalCouple_PYEL( 3UL, 4UL );     InternalCouple_PYEL = {{6.292317870691422e-07,-1.0197389513561938e-07,-1.3125141259623809e-07,1.089058217703347e-08},{-4.014319439723043e-08,-6.966793061544473e-08,4.2209220419253583e-08,-2.483561416153972e-08},{-1.0795168233272579e-06,6.050195973379197e-07,2.2661311407614864e-07,-5.359371363920138e-09}};
    blaze::DynamicMatrix<elastica::real_t> Position_PYEL( 3UL, 6UL );     Position_PYEL = {{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.036,0.072,0.10799999999999998,0.144,0.18},{0.0,0.0,0.0,0.0,0.0,0.0}};
    blaze::DynamicTensor<elastica::real_t> Director_PYEL( 3UL, 3UL, 5UL);     Director_PYEL = {{{0.04971398932038238,0.0017621047613692155,0.028349298452394353,0.038510774335923696,0.03821246174013585},{-0.015314684238161978,-0.013798409812089229,-0.0075552298434086035,-0.010184223579856029,-0.0084156112261818},{0.9986460732975112,0.999903244755945,0.9995695252353736,0.9992062859340523,0.9992341994024465}},{{0.9987490848254921,0.9987252912155221,0.9986629009873723,0.9986882768317997,0.9986707636882262},{0.006133445547642621,0.05046445653036852,0.04345899798566483,0.034159394781008254,0.03493633455617835},{-0.049625058246374414,-0.0010636322507983514,-0.02799510109943551,-0.03814264630208201,-0.037896679038589016}},{{-0.005365149214593786,-0.05044489739587496,-0.043228780560440355,-0.03374382875101853,-0.03459065657274675},{0.9998639113861443,0.9986305335376803,0.9990266432864016,0.9993645067432956,0.9993541064184749},{0.015600444913708315,0.013869744517022007,0.008777159857382425,0.011486369441699338,0.00973942823772274}}};
    blaze::DynamicMatrix<elastica::real_t> Velocity_PYEL( 3UL, 6UL );     Velocity_PYEL = {{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}};
    blaze::DynamicMatrix<elastica::real_t> AngularVelocity_PYEL( 3UL, 5UL );     AngularVelocity_PYEL = {{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0}};
    blaze::DynamicMatrix<elastica::real_t> Acceleration_PYEL( 3UL, 6UL );     Acceleration_PYEL = {{8.14060719336328,34.19672529702358,-5.47316959968718,-7.194790088495925,0.6424095940329931,-52.482957599110215},{0.2209432984344308,1.001120328550374,-0.3215006651010988,-0.2742384983318177,0.008441962309853156,-1.0485895532890517},{-23.6707478216553,1.3139150204267347,3.8630026112175786,-2.0554442217899087,1.325286784361881,14.777227433222729}};
    blaze::DynamicMatrix<elastica::real_t> AngularAcceleration_PYEL( 3UL, 5UL );     AngularAcceleration_PYEL = {{-10958.870893788462,-98455.06404146519,-83948.10001245774,-65753.5716632327,-67468.4954083966},{-29613.193687990544,-26667.171063435155,-14428.185398853471,-19756.671608015084,-16213.56763935804},{-757.3180539352584,1181.7600395973427,-265.46511856760003,-162.73664980831992,3.759782713835464}};


    blocks::get<elastica::tags::Director>(slicedrod) = Director_REPLACED;


    slicedrod.compute_internal_forces();
    slicedrod.compute_internal_torques();
    slicedrod.update_acceleration();


    auto ElementDimension(blocks::get<elastica::tags::ElementDimension>(slicedrod));
    auto MassSecondMomentOfInertia(blocks::get<elastica::tags::MassSecondMomentOfInertia>(slicedrod));
    auto InvMassSecondMomentOfInertia(blocks::get<elastica::tags::InvMassSecondMomentOfInertia>(slicedrod));
    auto ShearStretchRigidityMatrix(blocks::get<elastica::tags::ShearStretchRigidityMatrix>(slicedrod));
    auto BendingTwistRigidityMatrix(blocks::get<elastica::tags::BendingTwistRigidityMatrix>(slicedrod));
    auto ElementVolume(blocks::get<elastica::tags::ElementVolume>(slicedrod));
    auto Mass(blocks::get<elastica::tags::Mass>(slicedrod));
    auto InternalLoads(blocks::get<elastica::tags::InternalLoads>(slicedrod));
    auto InternalTorques(blocks::get<elastica::tags::InternalTorques>(slicedrod));
    auto ExternalLoads(blocks::get<elastica::tags::ExternalLoads>(slicedrod));
    auto ExternalTorques(blocks::get<elastica::tags::ExternalTorques>(slicedrod));
    auto ElementLength(blocks::get<elastica::tags::ElementLength>(slicedrod));
    auto ReferenceElementLength(blocks::get<elastica::tags::ReferenceElementLength>(slicedrod));
    auto Tangent(blocks::get<elastica::tags::Tangent>(slicedrod));
    auto ElementDilatation(blocks::get<elastica::tags::ElementDilatation>(slicedrod));
    auto VoronoiDilatation(blocks::get<elastica::tags::VoronoiDilatation>(slicedrod));
    auto ReferenceVoronoiLength(blocks::get<elastica::tags::ReferenceVoronoiLength>(slicedrod));
    auto ShearStretchStrain(blocks::get<elastica::tags::ShearStretchStrain>(slicedrod));
    auto Curvature(blocks::get<elastica::tags::Curvature>(slicedrod));
    auto ReferenceShearStretchStrain(blocks::get<elastica::tags::ReferenceShearStretchStrain>(slicedrod));
    auto ReferenceCurvature(blocks::get<elastica::tags::ReferenceCurvature>(slicedrod));
    auto InternalStress(blocks::get<elastica::tags::InternalStress>(slicedrod));
    auto InternalCouple(blocks::get<elastica::tags::InternalCouple>(slicedrod));
    auto Position(blocks::get<elastica::tags::Position>(slicedrod));
    auto Director(blocks::get<elastica::tags::Director>(slicedrod));
    auto Velocity(blocks::get<elastica::tags::Velocity>(slicedrod));
    auto AngularVelocity(blocks::get<elastica::tags::AngularVelocity>(slicedrod));
    auto Acceleration(blocks::get<elastica::tags::Acceleration>(slicedrod));
    auto AngularAcceleration(blocks::get<elastica::tags::AngularAcceleration>(slicedrod));


    CHECK_ITERABLE_APPROX(ElementDimension , ElementDimension_PYEL);
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::pageslice(MassSecondMomentOfInertia, i) - blaze::pageslice(MassSecondMomentOfInertia_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::pageslice(InvMassSecondMomentOfInertia, i) - blaze::pageslice(InvMassSecondMomentOfInertia_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::pageslice(ShearStretchRigidityMatrix, i) - blaze::pageslice(ShearStretchRigidityMatrix_PYEL, i)) == Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::pageslice(BendingTwistRigidityMatrix, i) - blaze::pageslice(BendingTwistRigidityMatrix_PYEL, i)) == Approx(0.0).margin(1e-6));
    CHECK_ITERABLE_APPROX(ElementVolume , ElementVolume_PYEL);
    CHECK_ITERABLE_APPROX(Mass , Mass_PYEL);
    CHECK_MATRIX_APPROX(InternalLoads , InternalLoads_PYEL);
    CHECK_MATRIX_APPROX(InternalTorques , InternalTorques_PYEL);
    CHECK_MATRIX_APPROX(ExternalLoads , ExternalLoads_PYEL);
    CHECK_MATRIX_APPROX(ExternalTorques , ExternalTorques_PYEL);
    CHECK_ITERABLE_APPROX(ElementLength , ElementLength_PYEL);
    CHECK_ITERABLE_APPROX(ReferenceElementLength , ReferenceElementLength_PYEL);
    CHECK_MATRIX_APPROX(Tangent , Tangent_PYEL);
    CHECK_ITERABLE_APPROX(ElementDilatation , ElementDilatation_PYEL);
    CHECK_ITERABLE_APPROX(VoronoiDilatation , VoronoiDilatation_PYEL);
    CHECK_ITERABLE_APPROX(ReferenceVoronoiLength , ReferenceVoronoiLength_PYEL);
    CHECK_MATRIX_APPROX(ShearStretchStrain , ShearStretchStrain_PYEL);
    CHECK_MATRIX_APPROX(Curvature , Curvature_PYEL);
    CHECK_MATRIX_APPROX(ReferenceShearStretchStrain , ReferenceShearStretchStrain_PYEL);
    CHECK_MATRIX_APPROX(ReferenceCurvature , ReferenceCurvature_PYEL);
    CHECK_MATRIX_APPROX(InternalStress , InternalStress_PYEL);
    CHECK_MATRIX_APPROX(InternalCouple , InternalCouple_PYEL);
    CHECK_MATRIX_APPROX(Position , Position_PYEL);
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK( (blaze::pageslice(Director, i) - blaze::pageslice(Director_PYEL, i)) == Approx(0.0).margin(1e-6));
    CHECK_MATRIX_APPROX(Velocity , Velocity_PYEL);
    CHECK_MATRIX_APPROX(AngularVelocity , AngularVelocity_PYEL);
    CHECK_MATRIX_APPROX(Acceleration , Acceleration_PYEL);
    CHECK_MATRIX_APPROX(AngularAcceleration , AngularAcceleration_PYEL);
    // clang-format on
  }

}  // namespace
