//
// Created by Seung Hyun Kim on 11/9/21.
//

#include <iostream>

#include "Framework/TestingFramework.hpp"
#include "Utilities/MakeArray.hpp"
///
#include "Simulator/Materials.hpp"
#include "Simulator/Materials/Materials.hpp"
#include "Systems/Block.hpp"
#include "Systems/Block/Protocols.hpp"
#include "Systems/CosseratRods.hpp"
#include "Systems/CosseratRods/CosseratRodPlugin.hpp"
#include "Systems/CosseratRods/Initializers/StraightCosseratRod.hpp"  // TODO : replace later
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

  template<class A, class B>
  void rel_comp(A a, B b){
    auto aa = blaze::evaluate(blaze::ravel(a));
    auto bb = blaze::evaluate(blaze::ravel(b));
    std::cout << blaze::max(blaze::abs(((aa - bb) / bb))) << '\n';
  }

  ELASTICA_TEST_CASE("Unit.z.Integrated.CosseratRods.SimpleInitialization",
                     "[Systems][Integrated]") {
    Model simple_rod{};

    auto initializer = elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
        // Variables
        blocks::initialize<elastica::tags::NElement>([](...) { return 5UL; }),
        // Geometry Variables
        blocks::initialize<elastica::tags::Position>(
            [](std::size_t index) { return 1.0 * index; }),
        blocks::initialize<elastica::tags::Director>([](...) { return 0.5; }),
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
        blocks::initialize<elastica::tags::ForceDampingRate>(
            [](...) { return 4.0; }),
        blocks::initialize<elastica::tags::TorqueDampingRate>(
            [](...) { return 1.0; }),
        // Kinematics Variables
        // blocks::initialize<tags::Mass>([](...) { return 1.0; }),
        blocks::initialize<elastica::tags::Material>(
            [](...) { return ::elastica::iron; }),
        blocks::initialize<elastica::tags::Velocity>([](...) { return 0.0; }),
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
     * rng_seed=150
     * NROD=1, N=5, n_step=1, time=0.0, time_step=2e-05
     * tag=150
     */

    Approx approx =
        Approx::custom() //.epsilon(std::numeric_limits<double>::epsilon() * 100)
            .epsilon(1.0e-12)
            .scale(0.0);

    elastica::MaterialID myMaterial = elastica::create_material(
        7.874, 0.24, 200, 200.0 / (2*(1.00+0.50)), 200, 1e-10, 1e-10, 0.1, 0.1);

    using Init = ::elastica::cosserat_rod::StraightCosseratRodInitializer;
    Init straight_initializer{
        // Initialization Wrapper
        Init::NElement{5UL},
        Init::Material{myMaterial},
        Init::Radius{0.007522},
        Init::Length{0.18},
        Init::Origin{::elastica::Vec3{0.0, 0.0, 0.0}},
        Init::Direction{::elastica::Vec3{0.0, 1.0, 0.0}},
        Init::Normal{::elastica::Vec3{0.0, 0.0, 1.0}},
        Init::ForceDampingRate{0.2},
        Init::TorqueDampingRate{0.2},
    };

    Model simple_rod{};
    auto slicedrod_00000 = simple_rod.emplace_back(
        straight_initializer.initialize<Plugin>());
    //**************************************************************************
    // clang-format off
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalLoads_REPLACED( 3UL, 6UL );     ROD00000_InternalLoads_REPLACED = {{0.004822044056611019,0.9307870177032551,0.7845822109817221,0.3343249592744213,0.6862689176861808,0.7322625609228085},{0.8246472414522372,0.10745443906835128,0.3089916869203323,0.31294344042724076,0.5940727124966699,0.6105506387346894},{0.9165420505570774,0.738817174285074,0.37747080022423485,0.7949533183809576,0.48693548220111915,0.5788914075188061}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalTorques_REPLACED( 3UL, 5UL );     ROD00000_InternalTorques_REPLACED = {{0.9744748566188305,0.688389675725899,0.5848087428374977,0.2739643248027279,0.8377858251400324},{0.30222935408131235,0.1728462163878618,0.801743366769752,0.3441099204558331,0.4375847094010139},{0.15268115598708532,0.2761357842472395,0.5638672039037047,0.86467466140162,0.08278860289302525}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ExternalLoads_REPLACED( 3UL, 6UL );     ROD00000_ExternalLoads_REPLACED = {{0.9748236222955216,0.9228236343831954,0.4073736921398182,0.7807688857822744,0.6465955762463057,0.22546091065284335},{0.824480688125953,0.3453562771880586,0.5685671364094176,0.6863696968075128,0.2470087962580596,0.8753744062638658},{0.15593308505796288,0.6952022537494705,0.16522045602868496,0.48096625789258196,0.5220324730992747,0.5747132880427898}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ExternalTorques_REPLACED( 3UL, 5UL );     ROD00000_ExternalTorques_REPLACED = {{0.8415277735547125,0.4884155772097232,0.9164990050965889,0.12455725715642718,0.45378529009583934},{0.9589130366491199,0.6706482598702904,0.7928513073170474,0.7638068902765369,0.4694302963114608},{0.2684748849053611,0.45322327766269055,0.2527348132181251,0.9472151712532533,0.0497407763497818}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalStress_REPLACED( 3UL, 5UL );     ROD00000_InternalStress_REPLACED = {{0.4375196365597337,0.7683879888762261,0.16663239947009578,0.8244444091919032,0.8696022625042859},{0.44593293278653,0.16041064218431744,0.41984957138538315,0.4522866548422185,0.8544544784625221},{0.7439885668390492,0.02976576715291579,0.18617391272004868,0.7383720961472324,0.8817988526289666}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalCouple_REPLACED( 3UL, 4UL );     ROD00000_InternalCouple_REPLACED = {{0.6536435340498628,0.2525396458442313,0.6183595148406859,0.19936517150220823},{0.8575809295973004,0.021480917433798763,0.9080569690386844,0.25119605094289243},{0.908052828476898,0.7615298850236448,0.7495712349893845,0.46584477600408136}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ReferenceShearStretchStrain_REPLACED( 3UL, 5UL );     ROD00000_ReferenceShearStretchStrain_REPLACED = {{0.8330628756074665,0.42173609990468275,0.5729168526354368,0.25708185818524776,0.7142533332135373},{0.8957865325349527,0.1461385058562097,0.19592065162059813,0.6849354450730554,0.3365366346874151},{0.41888599874152754,0.2251580498219029,0.893616157352747,0.42505738060020026,0.785763682498953}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ReferenceCurvature_REPLACED( 3UL, 4UL );     ROD00000_ReferenceCurvature_REPLACED = {{0.42296459317398183,0.35168182761748235,0.5263809158705643,0.3073262083073959},{0.357345091966899,0.026933120150035905,0.7107949934487903,0.2763092450283908},{0.2914274522809438,0.14947801345388878,0.28186903372731364,0.8952340780705201}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ReferenceElementLength_REPLACED( 5UL ); ROD00000_ReferenceElementLength_REPLACED={0.036,0.036,0.03599999999999999,0.036000000000000004,0.036000000000000004};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Position_REPLACED( 3UL, 6UL );     ROD00000_Position_REPLACED = {{0.,0.,0.,0.,0.,0.},{0.,0.036,0.072,0.10799999999999998,0.144,0.18},{0.,0.,0.,0.,0.,0.}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_Director_REPLACED( 3UL, 3UL, 5UL);     ROD00000_Director_REPLACED = {{{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{1.,1.,1.,1.,1.}},{{1.,1.,1.,1.,1.},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{1.,1.,1.,1.,1.},{0.,0.,0.,0.,0.}}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Velocity_REPLACED( 3UL, 6UL );     ROD00000_Velocity_REPLACED = {{0.,0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.,0.}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Acceleration_REPLACED( 3UL, 6UL );     ROD00000_Acceleration_REPLACED = {{0.,0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.,0.}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularVelocity_REPLACED( 3UL, 5UL );     ROD00000_AngularVelocity_REPLACED = {{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularAcceleration_REPLACED( 3UL, 5UL );     ROD00000_AngularAcceleration_REPLACED = {{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementDimension_PYEL( 5UL ); ROD00000_ElementDimension_PYEL={0.007521999999998955,0.007521999999998955,0.007521999999998956,0.007521999999998956,0.007521999999998956};
    blaze::DynamicTensor<elastica::real_t> ROD00000_MassSecondMomentOfInertia_PYEL( 3UL, 3UL, 5UL);     ROD00000_MassSecondMomentOfInertia_PYEL = {{{0.0000000007127235497145191,0.0000000007127235497145191,0.000000000712723549714519,0.0000000007127235497145191,0.0000000007127235497145191},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.0000000007127235497145191,0.0000000007127235497145191,0.000000000712723549714519,0.0000000007127235497145191,0.0000000007127235497145191},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{0.0000000014254470994290381,0.0000000014254470994290381,0.000000001425447099429038,0.0000000014254470994290381,0.0000000014254470994290381}}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_InvMassSecondMomentOfInertia_PYEL( 3UL, 3UL, 5UL);     ROD00000_InvMassSecondMomentOfInertia_PYEL = {{{1403068553.579504,1403068553.579504,1403068553.5795043,1403068553.579504,1403068553.579504},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{1403068553.579504,1403068553.579504,1403068553.5795043,1403068553.579504,1403068553.579504},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{701534276.789752,701534276.789752,701534276.7897521,701534276.789752,701534276.789752}}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_ShearStretchRigidityMatrix_PYEL( 3UL, 3UL, 5UL);     ROD00000_ShearStretchRigidityMatrix_PYEL = {{{0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{0.035550566574190964,0.035550566574190964,0.035550566574190964,0.035550566574190964,0.035550566574190964}}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_BendingTwistRigidityMatrix_PYEL( 3UL, 3UL, 4UL);     ROD00000_BendingTwistRigidityMatrix_PYEL = {{{0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868},{0.,0.,0.,0.},{0.,0.,0.,0.}},{{0.,0.,0.,0.},{0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868},{0.,0.,0.,0.}},{{0.,0.,0.,0.},{0.,0.,0.,0.},{0.00000033524471054032456,0.00000033524471054032456,0.00000033524471054032456,0.00000033524471054032456}}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementVolume_PYEL( 5UL ); ROD00000_ElementVolume_PYEL={0.0000063991019833543736,0.0000063991019833543736,0.000006399101983354373,0.000006399101983354375,0.000006399101983354375};
    blaze::DynamicVector<elastica::real_t> ROD00000_Mass_PYEL( 6UL ); ROD00000_Mass_PYEL={0.000025193264508466167,0.00005038652901693233,0.000050386529016932326,0.00005038652901693234,0.00005038652901693235,0.000025193264508466173};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalLoads_PYEL( 3UL, 6UL );     ROD00000_InternalLoads_PYEL = {{-0.01023612388750886,0.0085662038827434,-0.0005688589780293682,-0.005587956311911392,0.003981141997386493,0.003845593297319726},{-0.01489163458525301,0.006887138345346669,-0.023764064453826377,0.01665752998685827,-0.01282331339937827,0.02793434410625272},{-0.009519382677780637,0.0047002178325102,-0.0017275375975787611,0.003609036320354216,-0.005224083736760286,0.008161749859255267}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalTorques_PYEL( 3UL, 5UL );     ROD00000_InternalTorques_PYEL = {{0.00036828776498640795,0.00006015296592671471,0.00008050819296270421,0.00028187262600746805,0.00013859590293212825},{-0.0003428774734779819,-0.00017332378113095966,-0.00023602517955621617,-0.00010553749183637491,-0.00029368404811388603},{-0.00000009769951188334782,0.000000047587798530897935,-0.00000004438338926966561,-0.00000020562738674622027,0.00000030012248936833576}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ExternalLoads_PYEL( 3UL, 6UL );     ROD00000_ExternalLoads_PYEL = {{0.9748236222955216,0.9228236343831954,0.4073736921398182,0.7807688857822744,0.6465955762463057,0.22546091065284335},{0.824480688125953,0.3453562771880586,0.5685671364094176,0.6863696968075128,0.2470087962580596,0.8753744062638658},{0.15593308505796288,0.6952022537494705,0.16522045602868496,0.48096625789258196,0.5220324730992747,0.5747132880427898}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ExternalTorques_PYEL( 3UL, 5UL );     ROD00000_ExternalTorques_PYEL = {{0.8415277735547125,0.4884155772097232,0.9164990050965889,0.12455725715642718,0.45378529009583934},{0.9589130366491199,0.6706482598702904,0.7928513073170474,0.7638068902765369,0.4694302963114608},{0.2684748849053611,0.45322327766269055,0.2527348132181251,0.9472151712532533,0.0497407763497818}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementLength_PYEL( 5UL ); ROD00000_ElementLength_PYEL={0.036000000000009996,0.036000000000009996,0.03600000000000999,0.03600000000001,0.03600000000001};
    blaze::DynamicVector<elastica::real_t> ROD00000_ReferenceElementLength_PYEL( 5UL ); ROD00000_ReferenceElementLength_PYEL={0.036,0.036,0.03599999999999999,0.036000000000000004,0.036000000000000004};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Tangent_PYEL( 3UL, 5UL );     ROD00000_Tangent_PYEL = {{0.,0.,0.,0.,0.},{0.9999999999997222,0.9999999999997222,0.9999999999997222,0.9999999999997222,0.9999999999997222},{0.,0.,0.,0.,0.}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementDilatation_PYEL( 5UL ); ROD00000_ElementDilatation_PYEL={1.0000000000002778,1.0000000000002778,1.0000000000002778,1.0000000000002778,1.0000000000002778};
    blaze::DynamicVector<elastica::real_t> ROD00000_VoronoiDilatation_PYEL( 4UL ); ROD00000_VoronoiDilatation_PYEL={1.0000000000002778,1.000000000000278,1.0000000000002778,1.0000000000002778};
    blaze::DynamicVector<elastica::real_t> ROD00000_ReferenceVoronoiLength_PYEL( 4UL ); ROD00000_ReferenceVoronoiLength_PYEL={0.036,0.03599999999999999,0.036,0.036000000000000004};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ShearStretchStrain_PYEL( 3UL, 5UL );     ROD00000_ShearStretchStrain_PYEL = {{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Curvature_PYEL( 3UL, 4UL );     ROD00000_Curvature_PYEL = {{-0.,-0.,-0.,-0.},{-0.,-0.,-0.,-0.},{-0.,-0.,-0.,-0.}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ReferenceShearStretchStrain_PYEL( 3UL, 5UL );     ROD00000_ReferenceShearStretchStrain_PYEL = {{0.8330628756074665,0.42173609990468275,0.5729168526354368,0.25708185818524776,0.7142533332135373},{0.8957865325349527,0.1461385058562097,0.19592065162059813,0.6849354450730554,0.3365366346874151},{0.41888599874152754,0.2251580498219029,0.893616157352747,0.42505738060020026,0.785763682498953}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ReferenceCurvature_PYEL( 3UL, 4UL );     ROD00000_ReferenceCurvature_PYEL = {{0.42296459317398183,0.35168182761748235,0.5263809158705643,0.3073262083073959},{0.357345091966899,0.026933120150035905,0.7107949934487903,0.2763092450283908},{0.2914274522809438,0.14947801345388878,0.28186903372731364,0.8952340780705201}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalStress_PYEL( 3UL, 5UL );     ROD00000_InternalStress_PYEL = {{-0.00951938267778328,-0.004819164845271775,-0.006546702442851017,-0.002937666122495798,-0.008161749859257534},{-0.010236123887511703,-0.001669920004765922,-0.002238778982795448,-0.007826735294708393,-0.0038455932973207943},{-0.014891634585257147,-0.008004496239908565,-0.031768560693741545,-0.015111030706878647,-0.02793434410626048}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalCouple_PYEL( 3UL, 4UL );     ROD00000_InternalCouple_PYEL = {{-0.0000002126949639111265,-0.00000017684920875287278,-0.0000002646996266624674,-0.00000015454422861820262},{-0.00000017969707787917307,-0.000000013543779102969854,-0.00000035743539274837734,-0.00000013894681930373774},{-0.00000009769951188342925,-0.000000050111713352491686,-0.00000009449510262219424,-0.00000030012248936858585}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Position_PYEL( 3UL, 6UL );     ROD00000_Position_PYEL = {{0.,0.,0.,0.,0.,0.},{0.,0.036,0.072,0.10799999999999998,0.144,0.18},{0.,0.,0.,0.,0.,0.}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_Director_PYEL( 3UL, 3UL, 5UL);     ROD00000_Director_PYEL = {{{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{1.,1.,1.,1.,1.}},{{1.,1.,1.,1.,1.},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{1.,1.,1.,1.,1.},{0.,0.,0.,0.,0.}}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Velocity_PYEL( 3UL, 6UL );     ROD00000_Velocity_PYEL = {{0.,0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.,0.}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularVelocity_PYEL( 3UL, 5UL );     ROD00000_AngularVelocity_PYEL = {{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Acceleration_PYEL( 3UL, 6UL );     ROD00000_Acceleration_PYEL = {{38287.51522391488,18484.89777799432,8073.682412715561,15384.6860380056,12911.719281656939,9101.897210387517},{32135.138868908336,6990.825175019186,10812.474734517054,13952.68220516082,4647.779623398617,35855.16875221004},{5811.620892996225,13890.666518163602,3244.7743795998454,9617.15965888611,10256.876181902537,23136.145683151557}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularAcceleration_PYEL( 3UL, 5UL );     ROD00000_AngularAcceleration_PYEL = {{1181237889.0204394,685364936.1964315,1286023891.9521344,175157857.25401324,636886330.1636609},{1344939646.7394164,940722298.7901887,1112093577.4539695,1071525352.418638,658230827.9997057},{188344265.6787903,317951697.70393676,177302103.27415082,664503765.8749566,34895070.109728195}};

    blocks::get<elastica::tags::InternalLoads>(slicedrod_00000) = ROD00000_InternalLoads_REPLACED;
    blocks::get<elastica::tags::InternalTorques>(slicedrod_00000) = ROD00000_InternalTorques_REPLACED;
    blocks::get<elastica::tags::ExternalLoads>(slicedrod_00000) = ROD00000_ExternalLoads_REPLACED;
    blocks::get<elastica::tags::ExternalTorques>(slicedrod_00000) = ROD00000_ExternalTorques_REPLACED;
    blocks::get<elastica::tags::InternalStress>(slicedrod_00000) = ROD00000_InternalStress_REPLACED;
    blocks::get<elastica::tags::InternalCouple>(slicedrod_00000) = ROD00000_InternalCouple_REPLACED;
    blocks::get<elastica::tags::ReferenceShearStretchStrain>(slicedrod_00000) = ROD00000_ReferenceShearStretchStrain_REPLACED;
    blocks::get<elastica::tags::ReferenceCurvature>(slicedrod_00000) = ROD00000_ReferenceCurvature_REPLACED;
    blocks::get<elastica::tags::ReferenceElementLength>(slicedrod_00000) = ROD00000_ReferenceElementLength_REPLACED;
    blocks::get<elastica::tags::Position>(slicedrod_00000) = ROD00000_Position_REPLACED;
    blocks::get<elastica::tags::Director>(slicedrod_00000) = ROD00000_Director_REPLACED;
    blocks::get<elastica::tags::Velocity>(slicedrod_00000) = ROD00000_Velocity_REPLACED;
    blocks::get<elastica::tags::Acceleration>(slicedrod_00000) = ROD00000_Acceleration_REPLACED;
    blocks::get<elastica::tags::AngularVelocity>(slicedrod_00000) = ROD00000_AngularVelocity_REPLACED;
    blocks::get<elastica::tags::AngularAcceleration>(slicedrod_00000) = ROD00000_AngularAcceleration_REPLACED;

    slicedrod_00000.compute_internal_forces();
    slicedrod_00000.compute_internal_torques();
    slicedrod_00000.update_acceleration();

    auto ROD00000_ElementDimension(blocks::get<elastica::tags::ElementDimension>(slicedrod_00000));
    auto ROD00000_MassSecondMomentOfInertia(blocks::get<elastica::tags::MassSecondMomentOfInertia>(slicedrod_00000));
    auto ROD00000_InvMassSecondMomentOfInertia(blocks::get<elastica::tags::InvMassSecondMomentOfInertia>(slicedrod_00000));
    auto ROD00000_ShearStretchRigidityMatrix(blocks::get<elastica::tags::ShearStretchRigidityMatrix>(slicedrod_00000));
    auto ROD00000_BendingTwistRigidityMatrix(blocks::get<elastica::tags::BendingTwistRigidityMatrix>(slicedrod_00000));
    auto ROD00000_ElementVolume(blocks::get<elastica::tags::ElementVolume>(slicedrod_00000));
    auto ROD00000_Mass(blocks::get<elastica::tags::Mass>(slicedrod_00000));
    auto ROD00000_InternalLoads(blocks::get<elastica::tags::InternalLoads>(slicedrod_00000));
    auto ROD00000_InternalTorques(blocks::get<elastica::tags::InternalTorques>(slicedrod_00000));
    auto ROD00000_ElementLength(blocks::get<elastica::tags::ElementLength>(slicedrod_00000));
    auto ROD00000_ReferenceElementLength(blocks::get<elastica::tags::ReferenceElementLength>(slicedrod_00000));
    auto ROD00000_Tangent(blocks::get<elastica::tags::Tangent>(slicedrod_00000));
    auto ROD00000_ElementDilatation(blocks::get<elastica::tags::ElementDilatation>(slicedrod_00000));
    auto ROD00000_VoronoiDilatation(blocks::get<elastica::tags::VoronoiDilatation>(slicedrod_00000));
    auto ROD00000_ReferenceVoronoiLength(blocks::get<elastica::tags::ReferenceVoronoiLength>(slicedrod_00000));
    auto ROD00000_ShearStretchStrain(blocks::get<elastica::tags::ShearStretchStrain>(slicedrod_00000));
    auto ROD00000_Curvature(blocks::get<elastica::tags::Curvature>(slicedrod_00000));
    auto ROD00000_ReferenceShearStretchStrain(blocks::get<elastica::tags::ReferenceShearStretchStrain>(slicedrod_00000));
    auto ROD00000_ReferenceCurvature(blocks::get<elastica::tags::ReferenceCurvature>(slicedrod_00000));
    auto ROD00000_InternalStress(blocks::get<elastica::tags::InternalStress>(slicedrod_00000));
    auto ROD00000_InternalCouple(blocks::get<elastica::tags::InternalCouple>(slicedrod_00000));
    auto ROD00000_Position(blocks::get<elastica::tags::Position>(slicedrod_00000));
    auto ROD00000_Director(blocks::get<elastica::tags::Director>(slicedrod_00000));
    auto ROD00000_Velocity(blocks::get<elastica::tags::Velocity>(slicedrod_00000));
    auto ROD00000_AngularVelocity(blocks::get<elastica::tags::AngularVelocity>(slicedrod_00000));
    auto ROD00000_Acceleration(blocks::get<elastica::tags::Acceleration>(slicedrod_00000));
    auto ROD00000_AngularAcceleration(blocks::get<elastica::tags::AngularAcceleration>(slicedrod_00000));


    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ElementDimension , ROD00000_ElementDimension_PYEL , approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_MassSecondMomentOfInertia)),
                                 blaze::evaluate(blaze::ravel(ROD00000_MassSecondMomentOfInertia_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_InvMassSecondMomentOfInertia)),
                                 blaze::evaluate(blaze::ravel(ROD00000_InvMassSecondMomentOfInertia_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_ShearStretchRigidityMatrix)),
                                 blaze::evaluate(blaze::ravel(ROD00000_ShearStretchRigidityMatrix_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_BendingTwistRigidityMatrix)),
                                 blaze::evaluate(blaze::ravel(ROD00000_BendingTwistRigidityMatrix_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ElementVolume , ROD00000_ElementVolume_PYEL , approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_Mass , ROD00000_Mass_PYEL , approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_InternalLoads)),
                                 blaze::evaluate(blaze::ravel(ROD00000_InternalLoads_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_InternalTorques)),
                                 blaze::evaluate(blaze::ravel(ROD00000_InternalTorques_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ElementLength , ROD00000_ElementLength_PYEL , approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ReferenceElementLength , ROD00000_ReferenceElementLength_PYEL , approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_Tangent)),
                                 blaze::evaluate(blaze::ravel(ROD00000_Tangent_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ElementDilatation , ROD00000_ElementDilatation_PYEL , approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_VoronoiDilatation , ROD00000_VoronoiDilatation_PYEL , approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ReferenceVoronoiLength , ROD00000_ReferenceVoronoiLength_PYEL , approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_ShearStretchStrain)),
                                 blaze::evaluate(blaze::ravel(ROD00000_ShearStretchStrain_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_Curvature)),
                                 blaze::evaluate(blaze::ravel(ROD00000_Curvature_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_ReferenceShearStretchStrain)),
                                 blaze::evaluate(blaze::ravel(ROD00000_ReferenceShearStretchStrain_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_ReferenceCurvature)),
                                 blaze::evaluate(blaze::ravel(ROD00000_ReferenceCurvature_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_InternalStress)),
                                 blaze::evaluate(blaze::ravel(ROD00000_InternalStress_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_InternalCouple)),
                                 blaze::evaluate(blaze::ravel(ROD00000_InternalCouple_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_Position)),
                                 blaze::evaluate(blaze::ravel(ROD00000_Position_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_Director)),
                                 blaze::evaluate(blaze::ravel(ROD00000_Director_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_Velocity)),
                                 blaze::evaluate(blaze::ravel(ROD00000_Velocity_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_AngularVelocity)),
                                 blaze::evaluate(blaze::ravel(ROD00000_AngularVelocity_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_Acceleration)),
                                 blaze::evaluate(blaze::ravel(ROD00000_Acceleration_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_AngularAcceleration)),
                                 blaze::evaluate(blaze::ravel(ROD00000_AngularAcceleration_PYEL)),
                                 approx);
    // clang-format on
  }

  ELASTICA_TEST_CASE(
      "Unit.z.Integrated.CosseratRods.PerturbedInternalKinematics",
      "[Systems][Integrated]") {
    /*
     * rng_seed = 150
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
     * NROD=1, N=5, n_step=1, time=0.0, time_step=2e-05
     * tag=150
     */

    Approx approx =
        Approx::custom() //.epsilon(std::numeric_limits<double>::epsilon() * 100)
            .epsilon(1.0e-12)
            .scale(0.0);

    elastica::MaterialID myMaterial = elastica::create_material(
        7.874, 0.24, 200, 200.0 / (2.*(1.+0.50)), 200, 1e-10, 1e-10, 0.1, 0.1);

    using Init = ::elastica::cosserat_rod::StraightCosseratRodInitializer;
    Init straight_initializer{
        // Initialization Wrapper
        Init::NElement{5UL},
        Init::Material{myMaterial},
        Init::Radius{0.007522},
        Init::Length{0.18},
        Init::Origin{::elastica::Vec3{0.0, 0.0, 0.0}},
        Init::Direction{::elastica::Vec3{0.0, 1.0, 0.0}},
        Init::Normal{::elastica::Vec3{0.0, 0.0, 1.0}},
        Init::ForceDampingRate{0.2},
        Init::TorqueDampingRate{0.2},
    };

    Model simple_rod{};
    auto slicedrod_00000 = simple_rod.emplace_back(
        straight_initializer.initialize<Plugin>());
    //**************************************************************************
    // clang-format off
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalLoads_REPLACED( 3UL, 6UL );     ROD00000_InternalLoads_REPLACED = {{0.004822044056611019,0.9307870177032551,0.7845822109817221,0.3343249592744213,0.6862689176861808,0.7322625609228085},{0.8246472414522372,0.10745443906835128,0.3089916869203323,0.31294344042724076,0.5940727124966699,0.6105506387346894},{0.9165420505570774,0.738817174285074,0.37747080022423485,0.7949533183809576,0.48693548220111915,0.5788914075188061}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalTorques_REPLACED( 3UL, 5UL );     ROD00000_InternalTorques_REPLACED = {{0.9744748566188305,0.688389675725899,0.5848087428374977,0.2739643248027279,0.8377858251400324},{0.30222935408131235,0.1728462163878618,0.801743366769752,0.3441099204558331,0.4375847094010139},{0.15268115598708532,0.2761357842472395,0.5638672039037047,0.86467466140162,0.08278860289302525}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalStress_REPLACED( 3UL, 5UL );     ROD00000_InternalStress_REPLACED = {{0.9748236222955216,0.9228236343831954,0.4073736921398182,0.7807688857822744,0.6465955762463057},{0.22546091065284335,0.824480688125953,0.3453562771880586,0.5685671364094176,0.6863696968075128},{0.2470087962580596,0.8753744062638658,0.15593308505796288,0.6952022537494705,0.16522045602868496}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalCouple_REPLACED( 3UL, 4UL );     ROD00000_InternalCouple_REPLACED = {{0.48096625789258196,0.5220324730992747,0.5747132880427898,0.8415277735547125},{0.4884155772097232,0.9164990050965889,0.12455725715642718,0.45378529009583934},{0.9589130366491199,0.6706482598702904,0.7928513073170474,0.7638068902765369}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ReferenceShearStretchStrain_REPLACED( 3UL, 5UL );     ROD00000_ReferenceShearStretchStrain_REPLACED = {{0.4694302963114608,0.2684748849053611,0.45322327766269055,0.2527348132181251,0.9472151712532533},{0.0497407763497818,0.4375196365597337,0.7683879888762261,0.16663239947009578,0.8244444091919032},{0.8696022625042859,0.44593293278653,0.16041064218431744,0.41984957138538315,0.4522866548422185}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ReferenceCurvature_REPLACED( 3UL, 4UL );     ROD00000_ReferenceCurvature_REPLACED = {{0.8544544784625221,0.7439885668390492,0.02976576715291579,0.18617391272004868},{0.7383720961472324,0.8817988526289666,0.6536435340498628,0.2525396458442313},{0.6183595148406859,0.19936517150220823,0.8575809295973004,0.021480917433798763}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ReferenceElementLength_REPLACED( 5UL ); ROD00000_ReferenceElementLength_REPLACED={0.12680569690386845,0.061119605094289244,0.1268052828476898,0.11215298850236449,0.11095712349893845};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementDimension_PYEL( 5UL ); ROD00000_ElementDimension_PYEL={0.007521999999998955,0.007521999999998955,0.007521999999998956,0.007521999999998956,0.007521999999998956};
    blaze::DynamicTensor<elastica::real_t> ROD00000_MassSecondMomentOfInertia_PYEL( 3UL, 3UL, 5UL);     ROD00000_MassSecondMomentOfInertia_PYEL = {{{0.0000000007127235497145191,0.0000000007127235497145191,0.000000000712723549714519,0.0000000007127235497145191,0.0000000007127235497145191},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.0000000007127235497145191,0.0000000007127235497145191,0.000000000712723549714519,0.0000000007127235497145191,0.0000000007127235497145191},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{0.0000000014254470994290381,0.0000000014254470994290381,0.000000001425447099429038,0.0000000014254470994290381,0.0000000014254470994290381}}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_InvMassSecondMomentOfInertia_PYEL( 3UL, 3UL, 5UL);     ROD00000_InvMassSecondMomentOfInertia_PYEL = {{{1403068553.579504,1403068553.579504,1403068553.5795043,1403068553.579504,1403068553.579504},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{1403068553.579504,1403068553.579504,1403068553.5795043,1403068553.579504,1403068553.579504},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{701534276.789752,701534276.789752,701534276.7897521,701534276.789752,701534276.789752}}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_ShearStretchRigidityMatrix_PYEL( 3UL, 3UL, 5UL);     ROD00000_ShearStretchRigidityMatrix_PYEL = {{{0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{0.035550566574190964,0.035550566574190964,0.035550566574190964,0.035550566574190964,0.035550566574190964}}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_BendingTwistRigidityMatrix_PYEL( 3UL, 3UL, 4UL);     ROD00000_BendingTwistRigidityMatrix_PYEL = {{{0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868},{0.,0.,0.,0.},{0.,0.,0.,0.}},{{0.,0.,0.,0.},{0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868},{0.,0.,0.,0.}},{{0.,0.,0.,0.},{0.,0.,0.,0.},{0.00000033524471054032456,0.00000033524471054032456,0.00000033524471054032456,0.00000033524471054032456}}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementVolume_PYEL( 5UL ); ROD00000_ElementVolume_PYEL={0.0000063991019833543736,0.0000063991019833543736,0.000006399101983354373,0.000006399101983354375,0.000006399101983354375};
    blaze::DynamicVector<elastica::real_t> ROD00000_Mass_PYEL( 6UL ); ROD00000_Mass_PYEL={0.000025193264508466167,0.00005038652901693233,0.000050386529016932326,0.00005038652901693234,0.00005038652901693235,0.000025193264508466173};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalLoads_PYEL( 3UL, 6UL );     ROD00000_InternalLoads_PYEL = {{-0.0020020726298942263,-0.006485951255046142,-0.022439590264980942,0.024995646109236283,-0.02310458573286417,0.029036553773549197},{-0.19856592967907175,0.14684494117301133,-0.05803763293729508,-0.011943186131277941,-0.0018774422702858007,0.12357924984491923},{-0.018894629655140778,0.01368612917844915,-0.013033735032239858,0.009245096659907887,-0.02436334482770507,0.03336048367672867}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalTorques_PYEL( 3UL, 5UL );     ROD00000_InternalTorques_PYEL = {{0.00007164493765973941,0.0003056244095267034,0.0011137532685207797,0.00021347219695943097,0.0010454095565769908},{-0.0006805779705945335,-0.00018757814175308923,-0.0006566057465259305,-0.00032369529662950263,-0.0012008504184915258},{-0.0000002073017565624485,0.0000001404656373504243,-0.00000022066335129548562,0.0000002802981065602814,0.000000007201363947228476}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementLength_PYEL( 5UL ); ROD00000_ElementLength_PYEL={0.036000000000009996,0.036000000000009996,0.03600000000000999,0.03600000000001,0.03600000000001};
    blaze::DynamicVector<elastica::real_t> ROD00000_ReferenceElementLength_PYEL( 5UL ); ROD00000_ReferenceElementLength_PYEL={0.12680569690386845,0.061119605094289244,0.1268052828476898,0.11215298850236449,0.11095712349893845};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Tangent_PYEL( 3UL, 5UL );     ROD00000_Tangent_PYEL = {{0.,0.,0.,0.,0.},{0.9999999999997222,0.9999999999997222,0.9999999999997222,0.9999999999997222,0.9999999999997222},{0.,0.,0.,0.,0.}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementDilatation_PYEL( 5UL ); ROD00000_ElementDilatation_PYEL={0.2838989168388991,0.5890090412801715,0.2838998438515439,0.3209901089639803,0.32444965104339984};
    blaze::DynamicVector<elastica::real_t> ROD00000_VoronoiDilatation_PYEL( 4UL ); ROD00000_VoronoiDilatation_PYEL={1.0000000000002778,1.000000000000278,1.0000000000002778,1.0000000000002778};
    blaze::DynamicVector<elastica::real_t> ROD00000_ReferenceVoronoiLength_PYEL( 4UL ); ROD00000_ReferenceVoronoiLength_PYEL={0.036,0.03599999999999999,0.036,0.036000000000000004};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ShearStretchStrain_PYEL( 3UL, 5UL );     ROD00000_ShearStretchStrain_PYEL = {{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{-0.7161010831611798,-0.41099095871999214,-0.716100156148535,-0.6790098910361089,-0.6755503489566903}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Curvature_PYEL( 3UL, 4UL );     ROD00000_Curvature_PYEL = {{-0.,-0.,-0.,-0.},{-0.,-0.,-0.,-0.},{-0.,-0.,-0.,-0.}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ReferenceShearStretchStrain_PYEL( 3UL, 5UL );     ROD00000_ReferenceShearStretchStrain_PYEL = {{0.4694302963114608,0.2684748849053611,0.45322327766269055,0.2527348132181251,0.9472151712532533},{0.0497407763497818,0.4375196365597337,0.7683879888762261,0.16663239947009578,0.8244444091919032},{0.8696022625042859,0.44593293278653,0.16041064218431744,0.41984957138538315,0.4522866548422185}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ReferenceCurvature_PYEL( 3UL, 4UL );     ROD00000_ReferenceCurvature_PYEL = {{0.8544544784625221,0.7439885668390492,0.02976576715291579,0.18617391272004868},{0.7383720961472324,0.8817988526289666,0.6536435340498628,0.2525396458442313},{0.6183595148406859,0.19936517150220823,0.8575809295973004,0.021480917433798763}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalStress_PYEL( 3UL, 5UL );     ROD00000_InternalStress_PYEL = {{-0.005364164893166609,-0.0030678538722834527,-0.005178967812488738,-0.0028879925795121456,-0.010823797287553654},{-0.000568386251059777,-0.0049995228108319235,-0.008780344827863459,-0.0019041030677503362,-0.009420899739330952},{-0.05637265235699748,-0.030464129854017412,-0.031160455489129338,-0.03906507647449495,-0.04009524448838917}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalCouple_PYEL( 3UL, 4UL );     ROD00000_InternalCouple_PYEL = {{-0.0000004296770164530783,-0.00000037412734760290195,-0.000000014968223989784932,-0.00000009362072921998855},{-0.0000003713030094658974,-0.00000044342760165658233,-0.0000003286958060536515,-0.00000012699387070650807},{-0.00000020730175656262125,-0.00000006683611921207996,-0.00000028749947050774944,-0.000000007201363947234478}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Position_PYEL( 3UL, 6UL );     ROD00000_Position_PYEL = {{0.,0.,0.,0.,0.,0.},{0.,0.036,0.072,0.10799999999999998,0.144,0.18},{0.,0.,0.,0.,0.,0.}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_Director_PYEL( 3UL, 3UL, 5UL);     ROD00000_Director_PYEL = {{{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{1.,1.,1.,1.,1.}},{{1.,1.,1.,1.,1.},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{1.,1.,1.,1.,1.},{0.,0.,0.,0.,0.}}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Velocity_PYEL( 3UL, 6UL );     ROD00000_Velocity_PYEL = {{0.,0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.,0.}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularVelocity_PYEL( 3UL, 5UL );     ROD00000_AngularVelocity_PYEL = {{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Acceleration_PYEL( 3UL, 6UL );     ROD00000_Acceleration_PYEL = {{-79.46856705376281,-128.72391453808112,-445.3489990834683,496.0779517246866,-458.54688115349035,1152.5522531544686},{-7881.707017855661,2914.369059310759,-1151.8482036695088,-237.03133286407655,-37.260797814727184,4905.249567930768},{-749.9873487531264,271.6227818322223,-258.6749928311174,183.48349926626383,-483.5289372585635,1324.1826467355158}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularAcceleration_PYEL( 3UL, 5UL );     ROD00000_AngularAcceleration_PYEL = {{28538.30241295887,252574.14395700436,443642.39002464205,96141.71411935895,475896.67265056627},{-271094.37979648134,-155018.33983647372,-261546.38637295566,-145783.01583811876,-546657.2549636621},{-41.287217302173964,58.04181047597019,-43.94852043379209,63.11908719953246,1.6391208208643275}};


    blocks::get<elastica::tags::InternalLoads>(slicedrod_00000) = ROD00000_InternalLoads_REPLACED;
    blocks::get<elastica::tags::InternalTorques>(slicedrod_00000) = ROD00000_InternalTorques_REPLACED;
    blocks::get<elastica::tags::InternalStress>(slicedrod_00000) = ROD00000_InternalStress_REPLACED;
    blocks::get<elastica::tags::InternalCouple>(slicedrod_00000) = ROD00000_InternalCouple_REPLACED;
    blocks::get<elastica::tags::ReferenceShearStretchStrain>(slicedrod_00000) = ROD00000_ReferenceShearStretchStrain_REPLACED;
    blocks::get<elastica::tags::ReferenceCurvature>(slicedrod_00000) = ROD00000_ReferenceCurvature_REPLACED;
    blocks::get<elastica::tags::ReferenceElementLength>(slicedrod_00000) = ROD00000_ReferenceElementLength_REPLACED;


    slicedrod_00000.compute_internal_forces();
    slicedrod_00000.compute_internal_torques();
    slicedrod_00000.update_acceleration();


    auto ROD00000_ElementDimension(blocks::get<elastica::tags::ElementDimension>(slicedrod_00000));
    auto ROD00000_MassSecondMomentOfInertia(blocks::get<elastica::tags::MassSecondMomentOfInertia>(slicedrod_00000));
    auto ROD00000_InvMassSecondMomentOfInertia(blocks::get<elastica::tags::InvMassSecondMomentOfInertia>(slicedrod_00000));
    auto ROD00000_ShearStretchRigidityMatrix(blocks::get<elastica::tags::ShearStretchRigidityMatrix>(slicedrod_00000));
    auto ROD00000_BendingTwistRigidityMatrix(blocks::get<elastica::tags::BendingTwistRigidityMatrix>(slicedrod_00000));
    auto ROD00000_ElementVolume(blocks::get<elastica::tags::ElementVolume>(slicedrod_00000));
    auto ROD00000_Mass(blocks::get<elastica::tags::Mass>(slicedrod_00000));
    auto ROD00000_InternalLoads(blocks::get<elastica::tags::InternalLoads>(slicedrod_00000));
    auto ROD00000_InternalTorques(blocks::get<elastica::tags::InternalTorques>(slicedrod_00000));
    auto ROD00000_ElementLength(blocks::get<elastica::tags::ElementLength>(slicedrod_00000));
    auto ROD00000_ReferenceElementLength(blocks::get<elastica::tags::ReferenceElementLength>(slicedrod_00000));
    auto ROD00000_Tangent(blocks::get<elastica::tags::Tangent>(slicedrod_00000));
    auto ROD00000_ElementDilatation(blocks::get<elastica::tags::ElementDilatation>(slicedrod_00000));
    auto ROD00000_VoronoiDilatation(blocks::get<elastica::tags::VoronoiDilatation>(slicedrod_00000));
    auto ROD00000_ReferenceVoronoiLength(blocks::get<elastica::tags::ReferenceVoronoiLength>(slicedrod_00000));
    auto ROD00000_ShearStretchStrain(blocks::get<elastica::tags::ShearStretchStrain>(slicedrod_00000));
    auto ROD00000_Curvature(blocks::get<elastica::tags::Curvature>(slicedrod_00000));
    auto ROD00000_ReferenceShearStretchStrain(blocks::get<elastica::tags::ReferenceShearStretchStrain>(slicedrod_00000));
    auto ROD00000_ReferenceCurvature(blocks::get<elastica::tags::ReferenceCurvature>(slicedrod_00000));
    auto ROD00000_InternalStress(blocks::get<elastica::tags::InternalStress>(slicedrod_00000));
    auto ROD00000_InternalCouple(blocks::get<elastica::tags::InternalCouple>(slicedrod_00000));
    auto ROD00000_Position(blocks::get<elastica::tags::Position>(slicedrod_00000));
    auto ROD00000_Director(blocks::get<elastica::tags::Director>(slicedrod_00000));
    auto ROD00000_Velocity(blocks::get<elastica::tags::Velocity>(slicedrod_00000));
    auto ROD00000_AngularVelocity(blocks::get<elastica::tags::AngularVelocity>(slicedrod_00000));
    auto ROD00000_Acceleration(blocks::get<elastica::tags::Acceleration>(slicedrod_00000));
    auto ROD00000_AngularAcceleration(blocks::get<elastica::tags::AngularAcceleration>(slicedrod_00000));


    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ElementDimension , ROD00000_ElementDimension_PYEL , approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_MassSecondMomentOfInertia)),
                                 blaze::evaluate(blaze::ravel(ROD00000_MassSecondMomentOfInertia_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_InvMassSecondMomentOfInertia)),
                                 blaze::evaluate(blaze::ravel(ROD00000_InvMassSecondMomentOfInertia_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_ShearStretchRigidityMatrix)),
                                 blaze::evaluate(blaze::ravel(ROD00000_ShearStretchRigidityMatrix_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_BendingTwistRigidityMatrix)),
                                 blaze::evaluate(blaze::ravel(ROD00000_BendingTwistRigidityMatrix_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ElementVolume , ROD00000_ElementVolume_PYEL , approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_Mass , ROD00000_Mass_PYEL , approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_InternalLoads)),
                                 blaze::evaluate(blaze::ravel(ROD00000_InternalLoads_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_InternalTorques)),
                                 blaze::evaluate(blaze::ravel(ROD00000_InternalTorques_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ElementLength , ROD00000_ElementLength_PYEL , approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ReferenceElementLength , ROD00000_ReferenceElementLength_PYEL , approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_Tangent)),
                                 blaze::evaluate(blaze::ravel(ROD00000_Tangent_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ElementDilatation , ROD00000_ElementDilatation_PYEL , approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_VoronoiDilatation , ROD00000_VoronoiDilatation_PYEL , approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ReferenceVoronoiLength , ROD00000_ReferenceVoronoiLength_PYEL , approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_ShearStretchStrain)),
                                 blaze::evaluate(blaze::ravel(ROD00000_ShearStretchStrain_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_Curvature)),
                                 blaze::evaluate(blaze::ravel(ROD00000_Curvature_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_ReferenceShearStretchStrain)),
                                 blaze::evaluate(blaze::ravel(ROD00000_ReferenceShearStretchStrain_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_ReferenceCurvature)),
                                 blaze::evaluate(blaze::ravel(ROD00000_ReferenceCurvature_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_InternalStress)),
                                 blaze::evaluate(blaze::ravel(ROD00000_InternalStress_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_InternalCouple)),
                                 blaze::evaluate(blaze::ravel(ROD00000_InternalCouple_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_Position)),
                                 blaze::evaluate(blaze::ravel(ROD00000_Position_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_Director)),
                                 blaze::evaluate(blaze::ravel(ROD00000_Director_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_Velocity)),
                                 blaze::evaluate(blaze::ravel(ROD00000_Velocity_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_AngularVelocity)),
                                 blaze::evaluate(blaze::ravel(ROD00000_AngularVelocity_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_Acceleration)),
                                 blaze::evaluate(blaze::ravel(ROD00000_Acceleration_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_AngularAcceleration)),
                                 blaze::evaluate(blaze::ravel(ROD00000_AngularAcceleration_PYEL)),
                                 approx);
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


    /*
     * rng_seed=150
     * NROD=1, N=5, n_step=0, time=0.0, time_step=2e-05
     * tag=150
     */

    Approx approx =
        Approx::custom() //.epsilon(std::numeric_limits<double>::epsilon() * 100)
            .epsilon(2.0e-11)
            .scale(0.0);

    elastica::MaterialID myMaterial = elastica::create_material(
        7.874, 0.24, 200, 200.0 / (2.*(1.+0.50)), 200, 1e-10, 1e-10, 0.1, 0.1);

    using Init = ::elastica::cosserat_rod::StraightCosseratRodInitializer;
    Init straight_initializer{
        // Initialization Wrapper
        Init::NElement{5UL},
        Init::Material{myMaterial},
        Init::Radius{0.007522},
        Init::Length{0.18},
        Init::Origin{::elastica::Vec3{0.0, 0.0, 0.0}},
        Init::Direction{::elastica::Vec3{0.0, 1.0, 0.0}},
        Init::Normal{::elastica::Vec3{0.0, 0.0, 1.0}},
        Init::ForceDampingRate{0.2},
        Init::TorqueDampingRate{0.2},
    };



    Model simple_rod{};


    auto slicedrod_00000 = simple_rod.emplace_back(
        straight_initializer.initialize<Plugin>());
    //**************************************************************************
    // clang-format off


    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalLoads_REPLACED( 3UL, 6UL );     ROD00000_InternalLoads_REPLACED = {{0.004822044056611019,0.9307870177032551,0.7845822109817221,0.3343249592744213,0.6862689176861808,0.7322625609228085},{0.8246472414522372,0.10745443906835128,0.3089916869203323,0.31294344042724076,0.5940727124966699,0.6105506387346894},{0.9165420505570774,0.738817174285074,0.37747080022423485,0.7949533183809576,0.48693548220111915,0.5788914075188061}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalTorques_REPLACED( 3UL, 5UL );     ROD00000_InternalTorques_REPLACED = {{0.9744748566188305,0.688389675725899,0.5848087428374977,0.2739643248027279,0.8377858251400324},{0.30222935408131235,0.1728462163878618,0.801743366769752,0.3441099204558331,0.4375847094010139},{0.15268115598708532,0.2761357842472395,0.5638672039037047,0.86467466140162,0.08278860289302525}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ExternalLoads_REPLACED( 3UL, 6UL );     ROD00000_ExternalLoads_REPLACED = {{0.9748236222955216,0.9228236343831954,0.4073736921398182,0.7807688857822744,0.6465955762463057,0.22546091065284335},{0.824480688125953,0.3453562771880586,0.5685671364094176,0.6863696968075128,0.2470087962580596,0.8753744062638658},{0.15593308505796288,0.6952022537494705,0.16522045602868496,0.48096625789258196,0.5220324730992747,0.5747132880427898}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ExternalTorques_REPLACED( 3UL, 5UL );     ROD00000_ExternalTorques_REPLACED = {{0.8415277735547125,0.4884155772097232,0.9164990050965889,0.12455725715642718,0.45378529009583934},{0.9589130366491199,0.6706482598702904,0.7928513073170474,0.7638068902765369,0.4694302963114608},{0.2684748849053611,0.45322327766269055,0.2527348132181251,0.9472151712532533,0.0497407763497818}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalStress_REPLACED( 3UL, 5UL );     ROD00000_InternalStress_REPLACED = {{0.4375196365597337,0.7683879888762261,0.16663239947009578,0.8244444091919032,0.8696022625042859},{0.44593293278653,0.16041064218431744,0.41984957138538315,0.4522866548422185,0.8544544784625221},{0.7439885668390492,0.02976576715291579,0.18617391272004868,0.7383720961472324,0.8817988526289666}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalCouple_REPLACED( 3UL, 4UL );     ROD00000_InternalCouple_REPLACED = {{0.6536435340498628,0.2525396458442313,0.6183595148406859,0.19936517150220823},{0.8575809295973004,0.021480917433798763,0.9080569690386844,0.25119605094289243},{0.908052828476898,0.7615298850236448,0.7495712349893845,0.46584477600408136}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ReferenceShearStretchStrain_REPLACED( 3UL, 5UL );     ROD00000_ReferenceShearStretchStrain_REPLACED = {{0.8330628756074665,0.42173609990468275,0.5729168526354368,0.25708185818524776,0.7142533332135373},{0.8957865325349527,0.1461385058562097,0.19592065162059813,0.6849354450730554,0.3365366346874151},{0.41888599874152754,0.2251580498219029,0.893616157352747,0.42505738060020026,0.785763682498953}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ReferenceCurvature_REPLACED( 3UL, 4UL );     ROD00000_ReferenceCurvature_REPLACED = {{0.42296459317398183,0.35168182761748235,0.5263809158705643,0.3073262083073959},{0.357345091966899,0.026933120150035905,0.7107949934487903,0.2763092450283908},{0.2914274522809438,0.14947801345388878,0.28186903372731364,0.8952340780705201}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ReferenceElementLength_REPLACED( 5UL ); ROD00000_ReferenceElementLength_REPLACED={0.0426999984824372,0.06021312422305085,0.12769661319401673,0.12159671964324305,0.0678179495567241};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Position_REPLACED( 3UL, 6UL );     ROD00000_Position_REPLACED = {{0.05374345637055803,0.028996065607452062,0.07415154940462247,0.024859356918426626,0.04064159265168045,0.06737567066158216},{0.02963858757601048,0.06973946292340193,0.08828160600644791,0.20771545928276713,0.21467524757067283,0.22514507962648403},{0.04836319070419701,0.018704045242927358,0.0165739712548571,0.06855920051506412,0.015482774970845548,0.038441072012352875}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Velocity_REPLACED( 3UL, 6UL );     ROD00000_Velocity_REPLACED = {{0.018197333695852358,0.08766855523127529,0.06310417859392764,0.005886377952147127,0.04651766498643496,0.013806238174047048},{0.03903477571221334,0.008541891267936252,0.025397791978663778,0.016728381899616587,0.03177615374075308,0.06777741149744103},{0.04691392627870822,0.03263778989996056,0.05654957730089996,0.05000081261681551,0.03606695351052915,0.04624814200030012}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Acceleration_REPLACED( 3UL, 6UL );     ROD00000_Acceleration_REPLACED = {{0.025521678254808857,0.08879391958437341,0.010899786996980654,0.01968499323761388,0.06758684467024345,0.03979854625346502},{0.05467244975907565,0.030489744465855475,0.0501465843868291,0.025331464086950153,0.07832008548993236,0.015947804239189612},{0.019968834201570376,0.06961861647008744,0.07038236907405,0.043850613884863304,0.046868672010347126,0.07523022215578842}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularVelocity_REPLACED( 3UL, 5UL );     ROD00000_AngularVelocity_REPLACED = {{0.08770114933878452,0.009577245914128951,0.09875343648612099,0.01932221298301469,0.0726677364112708},{0.001063980046126911,0.0838166367595207,0.014960843780453748,0.07960981813312598,0.044280498457266185},{0.018307202694590942,0.005622099491997146,0.01090490002706256,0.0710350804397759,0.06736100309564207}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularAcceleration_REPLACED( 3UL, 5UL );     ROD00000_AngularAcceleration_REPLACED = {{0.001194391204051315,0.01409282051105949,0.01463816812234734,0.03931543293728981,0.0235708323000189},{0.09073814929269795,0.04078499055234254,0.09258798531315957,0.014499827620907391,0.0493737051779979},{0.05973131578797437,0.08073493167581633,0.09944518992838293,0.06111602004198833,0.02882443602400854}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementDimension_PYEL( 5UL ); ROD00000_ElementDimension_PYEL={0.006048363804440265,0.0064566140709191545,0.003824309314689266,0.00604133826466519,0.007443679400016551};
    blaze::DynamicTensor<elastica::real_t> ROD00000_MassSecondMomentOfInertia_PYEL( 3UL, 3UL, 5UL);     ROD00000_MassSecondMomentOfInertia_PYEL = {{{0.0000000007127235497145191,0.0000000007127235497145191,0.000000000712723549714519,0.0000000007127235497145191,0.0000000007127235497145191},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.0000000007127235497145191,0.0000000007127235497145191,0.000000000712723549714519,0.0000000007127235497145191,0.0000000007127235497145191},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{0.0000000014254470994290381,0.0000000014254470994290381,0.000000001425447099429038,0.0000000014254470994290381,0.0000000014254470994290381}}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_InvMassSecondMomentOfInertia_PYEL( 3UL, 3UL, 5UL);     ROD00000_InvMassSecondMomentOfInertia_PYEL = {{{1403068553.579504,1403068553.579504,1403068553.5795043,1403068553.579504,1403068553.579504},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{1403068553.579504,1403068553.579504,1403068553.5795043,1403068553.579504,1403068553.579504},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{701534276.789752,701534276.789752,701534276.7897521,701534276.789752,701534276.789752}}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_ShearStretchRigidityMatrix_PYEL( 3UL, 3UL, 5UL);     ROD00000_ShearStretchRigidityMatrix_PYEL = {{{0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{0.035550566574190964,0.035550566574190964,0.035550566574190964,0.035550566574190964,0.035550566574190964}}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_BendingTwistRigidityMatrix_PYEL( 3UL, 3UL, 4UL);     ROD00000_BendingTwistRigidityMatrix_PYEL = {{{0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868},{0.,0.,0.,0.},{0.,0.,0.,0.}},{{0.,0.,0.,0.},{0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868},{0.,0.,0.,0.}},{{0.,0.,0.,0.},{0.,0.,0.,0.},{0.00000033524471054032456,0.00000033524471054032456,0.00000033524471054032456,0.00000033524471054032456}}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementVolume_PYEL( 5UL ); ROD00000_ElementVolume_PYEL={0.0000063991019833543736,0.0000063991019833543736,0.000006399101983354373,0.000006399101983354375,0.000006399101983354375};
    blaze::DynamicVector<elastica::real_t> ROD00000_Mass_PYEL( 6UL ); ROD00000_Mass_PYEL={0.000025193264508466167,0.00005038652901693233,0.000050386529016932326,0.00005038652901693234,0.00005038652901693235,0.000025193264508466173};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalLoads_PYEL( 3UL, 6UL );     ROD00000_InternalLoads_PYEL = {{-0.012928968444664258,0.021430518513509467,-0.014600195097012181,-0.0077245362919761236,0.015036692597398276,-0.0012157200066570425},{-0.013079990740954516,-0.02710398850905906,0.008946076440090654,-0.07471128101665034,-0.0010442796032015364,0.10699209442463233},{-0.013387481669229602,0.006949894600862993,0.004699142451749671,-0.015531309800791883,0.009347264308766026,0.007920254597744971}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalTorques_PYEL( 3UL, 5UL );     ROD00000_InternalTorques_PYEL = {{0.0008420805135067476,-0.0019721097253553653,0.0022679470201509883,-0.001575968451866091,-0.00287299910959954},{-0.0009248344760349428,-0.00020489251830664194,0.001416370759266229,-0.005743596225394128,0.0023735033112509945},{0.000052128107152514014,0.0002725849292001289,-0.0004025934441411028,0.001005989460644408,0.00023979734272641496}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementLength_PYEL( 5UL ); ROD00000_ElementLength_PYEL={0.055679246247616845,0.04886067951087357,0.13927178325046535,0.055808821581400166,0.03676155210764667};
    blaze::DynamicVector<elastica::real_t> ROD00000_ReferenceElementLength_PYEL( 5UL ); ROD00000_ReferenceElementLength_PYEL={0.0426999984824372,0.06021312422305085,0.12769661319401673,0.12159671964324305,0.0678179495567241};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Tangent_PYEL( 3UL, 5UL );     ROD00000_Tangent_PYEL = {{-0.44446346584954344,0.924168150120004,-0.3539280630703861,0.28279105858263975,0.7272293055423203},{0.7202122523184807,0.3794900780886515,0.857559589522382,0.12470767327983209,0.284803863154445},{-0.5326786452778014,-0.04359484987506622,0.3732646200610296,-0.9510400692980724,0.6245192524591973}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementDilatation_PYEL( 5UL ); ROD00000_ElementDilatation_PYEL={1.3039636586993815,0.8114622873557634,1.09064586575105,0.45896650621118446,0.5420622762547351};
    blaze::DynamicVector<elastica::real_t> ROD00000_VoronoiDilatation_PYEL( 4UL ); ROD00000_VoronoiDilatation_PYEL={1.451943413312367,2.6129508716852636,2.709452844887021,1.2856996345700946};
    blaze::DynamicVector<elastica::real_t> ROD00000_ReferenceVoronoiLength_PYEL( 4UL ); ROD00000_ReferenceVoronoiLength_PYEL={0.036,0.03599999999999999,0.036,0.036000000000000004};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ShearStretchStrain_PYEL( 3UL, 5UL );     ROD00000_ShearStretchStrain_PYEL = {{-0.6945935952074719,-0.03537557659655235,0.4070995147006984,-0.43649553787257905,0.33852832755293805},{-0.5795642070873782,0.7499276009977229,-0.38601017876099347,0.12979162414543655,0.3942035727214204},{-0.060869396426671774,-0.6920581132053656,-0.06470617905224663,-0.9427633548970296,-0.8456185696523595}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Curvature_PYEL( 3UL, 4UL );     ROD00000_Curvature_PYEL = {{-0.,-0.,-0.,-0.},{-0.,-0.,-0.,-0.},{-0.,-0.,-0.,-0.}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ReferenceShearStretchStrain_PYEL( 3UL, 5UL );     ROD00000_ReferenceShearStretchStrain_PYEL = {{0.8330628756074665,0.42173609990468275,0.5729168526354368,0.25708185818524776,0.7142533332135373},{0.8957865325349527,0.1461385058562097,0.19592065162059813,0.6849354450730554,0.3365366346874151},{0.41888599874152754,0.2251580498219029,0.893616157352747,0.42505738060020026,0.785763682498953}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ReferenceCurvature_PYEL( 3UL, 4UL );     ROD00000_ReferenceCurvature_PYEL = {{0.42296459317398183,0.35168182761748235,0.5263809158705643,0.3073262083073959},{0.357345091966899,0.026933120150035905,0.7107949934487903,0.2763092450283908},{0.2914274522809438,0.14947801345388878,0.28186903372731364,0.8952340780705201}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalStress_PYEL( 3UL, 5UL );     ROD00000_InternalStress_PYEL = {{-0.017456481343350032,-0.005223400420916958,-0.0018947893858084412,-0.007925486590577504,-0.004293397551640312},{-0.0168587854358225,0.006899478564728215,-0.0066497048765533934,-0.006343610581330091,0.0006589582456203163},{-0.017055576115254364,-0.0326075542666249,-0.034068902019899955,-0.04862680211885312,-0.05799656336305883}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalCouple_PYEL( 3UL, 4UL );     ROD00000_InternalCouple_PYEL = {{-0.0000002126949639111265,-0.00000017684920875287278,-0.0000002646996266624674,-0.00000015454422861820262},{-0.00000017969707787917307,-0.000000013543779102969854,-0.00000035743539274837734,-0.00000013894681930373774},{-0.00000009769951188342925,-0.000000050111713352491686,-0.00000009449510262219424,-0.00000030012248936858585}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Position_PYEL( 3UL, 6UL );     ROD00000_Position_PYEL = {{0.05374345637055803,0.028996065607452062,0.07415154940462247,0.024859356918426626,0.04064159265168045,0.06737567066158216},{0.02963858757601048,0.06973946292340193,0.08828160600644791,0.20771545928276713,0.21467524757067283,0.22514507962648403},{0.04836319070419701,0.018704045242927358,0.0165739712548571,0.06855920051506412,0.015482774970845548,0.038441072012352875}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_Director_PYEL( 3UL, 3UL, 5UL);     ROD00000_Director_PYEL = {{{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{1.,1.,1.,1.,1.}},{{1.,1.,1.,1.,1.},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{1.,1.,1.,1.,1.},{0.,0.,0.,0.,0.}}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Velocity_PYEL( 3UL, 6UL );     ROD00000_Velocity_PYEL = {{0.018197333695852358,0.08766855523127529,0.06310417859392764,0.005886377952147127,0.04651766498643496,0.013806238174047048},{0.03903477571221334,0.008541891267936252,0.025397791978663778,0.016728381899616587,0.03177615374075308,0.06777741149744103},{0.04691392627870822,0.03263778989996056,0.05654957730089996,0.05000081261681551,0.03606695351052915,0.04624814200030012}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularVelocity_PYEL( 3UL, 5UL );     ROD00000_AngularVelocity_PYEL = {{0.08770114933878452,0.009577245914128951,0.09875343648612099,0.01932221298301469,0.0726677364112708},{0.001063980046126911,0.0838166367595207,0.014960843780453748,0.07960981813312598,0.044280498457266185},{0.018307202694590942,0.005622099491997146,0.01090490002706256,0.0710350804397759,0.06736100309564207}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Acceleration_PYEL( 3UL, 6UL );     ROD00000_Acceleration_PYEL = {{38180.627743880264,18740.210356212257,7795.208455633351,15342.282244337917,13131.13408985491,8900.997747665013},{32207.048717816153,6316.217744866912,11461.658981420125,12139.324294104785,4881.55309472105,38993.21980914282},{5658.083863678366,13935.31489566142,3372.3219637402167,9237.289354370514,10546.067525895087,23126.559975772143}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularAcceleration_PYEL( 3UL, 5UL );     ROD00000_AngularAcceleration_PYEL = {{1541158107.275718,553833992.0888584,1405943971.286182,79195211.70401922,342941583.64928716},{1752687702.8222477,763324726.1604527,1215428842.9793,488163554.71480656,358830631.6966226},{245641852.64235044,258160958.84062666,193065805.3901223,305308947.22272855,19006376.00310138}};


    blocks::get<elastica::tags::InternalLoads>(slicedrod_00000) = ROD00000_InternalLoads_REPLACED;
    blocks::get<elastica::tags::InternalTorques>(slicedrod_00000) = ROD00000_InternalTorques_REPLACED;
    blocks::get<elastica::tags::ExternalLoads>(slicedrod_00000) = ROD00000_ExternalLoads_REPLACED;
    blocks::get<elastica::tags::ExternalTorques>(slicedrod_00000) = ROD00000_ExternalTorques_REPLACED;
    blocks::get<elastica::tags::InternalStress>(slicedrod_00000) = ROD00000_InternalStress_REPLACED;
    blocks::get<elastica::tags::InternalCouple>(slicedrod_00000) = ROD00000_InternalCouple_REPLACED;
    blocks::get<elastica::tags::ReferenceShearStretchStrain>(slicedrod_00000) = ROD00000_ReferenceShearStretchStrain_REPLACED;
    blocks::get<elastica::tags::ReferenceCurvature>(slicedrod_00000) = ROD00000_ReferenceCurvature_REPLACED;
    blocks::get<elastica::tags::ReferenceElementLength>(slicedrod_00000) = ROD00000_ReferenceElementLength_REPLACED;
    blocks::get<elastica::tags::Position>(slicedrod_00000) = ROD00000_Position_REPLACED;
    blocks::get<elastica::tags::Velocity>(slicedrod_00000) = ROD00000_Velocity_REPLACED;
    blocks::get<elastica::tags::Acceleration>(slicedrod_00000) = ROD00000_Acceleration_REPLACED;
    blocks::get<elastica::tags::AngularVelocity>(slicedrod_00000) = ROD00000_AngularVelocity_REPLACED;
    blocks::get<elastica::tags::AngularAcceleration>(slicedrod_00000) = ROD00000_AngularAcceleration_REPLACED;


    slicedrod_00000.compute_internal_forces();
    slicedrod_00000.compute_internal_torques();
    slicedrod_00000.update_acceleration();


    auto ROD00000_ElementDimension(blocks::get<elastica::tags::ElementDimension>(slicedrod_00000));
    auto ROD00000_MassSecondMomentOfInertia(blocks::get<elastica::tags::MassSecondMomentOfInertia>(slicedrod_00000));
    auto ROD00000_InvMassSecondMomentOfInertia(blocks::get<elastica::tags::InvMassSecondMomentOfInertia>(slicedrod_00000));
    auto ROD00000_ShearStretchRigidityMatrix(blocks::get<elastica::tags::ShearStretchRigidityMatrix>(slicedrod_00000));
    auto ROD00000_BendingTwistRigidityMatrix(blocks::get<elastica::tags::BendingTwistRigidityMatrix>(slicedrod_00000));
    auto ROD00000_ElementVolume(blocks::get<elastica::tags::ElementVolume>(slicedrod_00000));
    auto ROD00000_Mass(blocks::get<elastica::tags::Mass>(slicedrod_00000));
    auto ROD00000_InternalLoads(blocks::get<elastica::tags::InternalLoads>(slicedrod_00000));
    auto ROD00000_InternalTorques(blocks::get<elastica::tags::InternalTorques>(slicedrod_00000));
    auto ROD00000_ElementLength(blocks::get<elastica::tags::ElementLength>(slicedrod_00000));
    auto ROD00000_ReferenceElementLength(blocks::get<elastica::tags::ReferenceElementLength>(slicedrod_00000));
    auto ROD00000_Tangent(blocks::get<elastica::tags::Tangent>(slicedrod_00000));
    auto ROD00000_ElementDilatation(blocks::get<elastica::tags::ElementDilatation>(slicedrod_00000));
    auto ROD00000_VoronoiDilatation(blocks::get<elastica::tags::VoronoiDilatation>(slicedrod_00000));
    auto ROD00000_ReferenceVoronoiLength(blocks::get<elastica::tags::ReferenceVoronoiLength>(slicedrod_00000));
    auto ROD00000_ShearStretchStrain(blocks::get<elastica::tags::ShearStretchStrain>(slicedrod_00000));
    auto ROD00000_Curvature(blocks::get<elastica::tags::Curvature>(slicedrod_00000));
    auto ROD00000_ReferenceShearStretchStrain(blocks::get<elastica::tags::ReferenceShearStretchStrain>(slicedrod_00000));
    auto ROD00000_ReferenceCurvature(blocks::get<elastica::tags::ReferenceCurvature>(slicedrod_00000));
    auto ROD00000_InternalStress(blocks::get<elastica::tags::InternalStress>(slicedrod_00000));
    auto ROD00000_InternalCouple(blocks::get<elastica::tags::InternalCouple>(slicedrod_00000));
    auto ROD00000_Position(blocks::get<elastica::tags::Position>(slicedrod_00000));
    auto ROD00000_Director(blocks::get<elastica::tags::Director>(slicedrod_00000));
    auto ROD00000_Velocity(blocks::get<elastica::tags::Velocity>(slicedrod_00000));
    auto ROD00000_AngularVelocity(blocks::get<elastica::tags::AngularVelocity>(slicedrod_00000));
    auto ROD00000_Acceleration(blocks::get<elastica::tags::Acceleration>(slicedrod_00000));
    auto ROD00000_AngularAcceleration(blocks::get<elastica::tags::AngularAcceleration>(slicedrod_00000));


    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ElementDimension , ROD00000_ElementDimension_PYEL , approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_MassSecondMomentOfInertia)),
                                 blaze::evaluate(blaze::ravel(ROD00000_MassSecondMomentOfInertia_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_InvMassSecondMomentOfInertia)),
                                 blaze::evaluate(blaze::ravel(ROD00000_InvMassSecondMomentOfInertia_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_ShearStretchRigidityMatrix)),
                                 blaze::evaluate(blaze::ravel(ROD00000_ShearStretchRigidityMatrix_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_BendingTwistRigidityMatrix)),
                                 blaze::evaluate(blaze::ravel(ROD00000_BendingTwistRigidityMatrix_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ElementVolume , ROD00000_ElementVolume_PYEL , approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_Mass , ROD00000_Mass_PYEL , approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_InternalLoads)),
                                 blaze::evaluate(blaze::ravel(ROD00000_InternalLoads_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_InternalTorques)),
                                 blaze::evaluate(blaze::ravel(ROD00000_InternalTorques_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ElementLength , ROD00000_ElementLength_PYEL , approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ReferenceElementLength , ROD00000_ReferenceElementLength_PYEL , approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_Tangent)),
                                 blaze::evaluate(blaze::ravel(ROD00000_Tangent_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ElementDilatation , ROD00000_ElementDilatation_PYEL , approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_VoronoiDilatation , ROD00000_VoronoiDilatation_PYEL , approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ReferenceVoronoiLength , ROD00000_ReferenceVoronoiLength_PYEL , approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_ShearStretchStrain)),
                                 blaze::evaluate(blaze::ravel(ROD00000_ShearStretchStrain_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_Curvature)),
                                 blaze::evaluate(blaze::ravel(ROD00000_Curvature_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_ReferenceShearStretchStrain)),
                                 blaze::evaluate(blaze::ravel(ROD00000_ReferenceShearStretchStrain_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_ReferenceCurvature)),
                                 blaze::evaluate(blaze::ravel(ROD00000_ReferenceCurvature_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_InternalStress)),
                                 blaze::evaluate(blaze::ravel(ROD00000_InternalStress_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_InternalCouple)),
                                 blaze::evaluate(blaze::ravel(ROD00000_InternalCouple_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_Position)),
                                 blaze::evaluate(blaze::ravel(ROD00000_Position_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_Director)),
                                 blaze::evaluate(blaze::ravel(ROD00000_Director_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_Velocity)),
                                 blaze::evaluate(blaze::ravel(ROD00000_Velocity_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_AngularVelocity)),
                                 blaze::evaluate(blaze::ravel(ROD00000_AngularVelocity_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_Acceleration)),
                                 blaze::evaluate(blaze::ravel(ROD00000_Acceleration_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_AngularAcceleration)),
                                 blaze::evaluate(blaze::ravel(ROD00000_AngularAcceleration_PYEL)),
                                 approx);
    // clang-format on
  }

  ELASTICA_TEST_CASE("Unit.z.Integrated.CosseratRods.FullWithDirector",
                     "[Systems][Integrated]") {

     /*
      * rng_seed=150
      * NROD=1, N=5, n_step=0, time=0.0, time_step=2e-05
      * tag=150
      */


     Approx approx =
         Approx::custom() //.epsilon(std::numeric_limits<double>::epsilon() * 100)
             .epsilon(6.1e-11)
             .scale(0.0);

     elastica::MaterialID myMaterial = elastica::create_material(
         7.874, 0.24, 200, 200.0 / (2.*(1.+0.50)), 200, 1e-10, 1e-10, 0.1, 0.1);

     using Init = ::elastica::cosserat_rod::StraightCosseratRodInitializer;
     Init straight_initializer{
         // Initialization Wrapper
         Init::NElement{5UL},
         Init::Material{myMaterial},
         Init::Radius{0.007522},
         Init::Length{0.18},
         Init::Origin{::elastica::Vec3{0.0, 0.0, 0.0}},
         Init::Direction{::elastica::Vec3{0.0, 1.0, 0.0}},
         Init::Normal{::elastica::Vec3{0.0, 0.0, 1.0}},
         Init::ForceDampingRate{0.2},
         Init::TorqueDampingRate{0.2},
     };

     Model simple_rod{};
     auto slicedrod_00000 = simple_rod.emplace_back(
         straight_initializer.initialize<Plugin>());

     //**************************************************************************
     // clang-format off
     blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalLoads_REPLACED( 3UL, 6UL );     ROD00000_InternalLoads_REPLACED = {{0.004822044056611019,0.9307870177032551,0.7845822109817221,0.3343249592744213,0.6862689176861808,0.7322625609228085},{0.8246472414522372,0.10745443906835128,0.3089916869203323,0.31294344042724076,0.5940727124966699,0.6105506387346894},{0.9165420505570774,0.738817174285074,0.37747080022423485,0.7949533183809576,0.48693548220111915,0.5788914075188061}};
     blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalTorques_REPLACED( 3UL, 5UL );     ROD00000_InternalTorques_REPLACED = {{0.9744748566188305,0.688389675725899,0.5848087428374977,0.2739643248027279,0.8377858251400324},{0.30222935408131235,0.1728462163878618,0.801743366769752,0.3441099204558331,0.4375847094010139},{0.15268115598708532,0.2761357842472395,0.5638672039037047,0.86467466140162,0.08278860289302525}};
     blaze::DynamicMatrix<elastica::real_t> ROD00000_ExternalLoads_REPLACED( 3UL, 6UL );     ROD00000_ExternalLoads_REPLACED = {{0.9748236222955216,0.9228236343831954,0.4073736921398182,0.7807688857822744,0.6465955762463057,0.22546091065284335},{0.824480688125953,0.3453562771880586,0.5685671364094176,0.6863696968075128,0.2470087962580596,0.8753744062638658},{0.15593308505796288,0.6952022537494705,0.16522045602868496,0.48096625789258196,0.5220324730992747,0.5747132880427898}};
     blaze::DynamicMatrix<elastica::real_t> ROD00000_ExternalTorques_REPLACED( 3UL, 5UL );     ROD00000_ExternalTorques_REPLACED = {{0.8415277735547125,0.4884155772097232,0.9164990050965889,0.12455725715642718,0.45378529009583934},{0.9589130366491199,0.6706482598702904,0.7928513073170474,0.7638068902765369,0.4694302963114608},{0.2684748849053611,0.45322327766269055,0.2527348132181251,0.9472151712532533,0.0497407763497818}};
     blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalStress_REPLACED( 3UL, 5UL );     ROD00000_InternalStress_REPLACED = {{0.4375196365597337,0.7683879888762261,0.16663239947009578,0.8244444091919032,0.8696022625042859},{0.44593293278653,0.16041064218431744,0.41984957138538315,0.4522866548422185,0.8544544784625221},{0.7439885668390492,0.02976576715291579,0.18617391272004868,0.7383720961472324,0.8817988526289666}};
     blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalCouple_REPLACED( 3UL, 4UL );     ROD00000_InternalCouple_REPLACED = {{0.6536435340498628,0.2525396458442313,0.6183595148406859,0.19936517150220823},{0.8575809295973004,0.021480917433798763,0.9080569690386844,0.25119605094289243},{0.908052828476898,0.7615298850236448,0.7495712349893845,0.46584477600408136}};
     blaze::DynamicMatrix<elastica::real_t> ROD00000_ReferenceShearStretchStrain_REPLACED( 3UL, 5UL );     ROD00000_ReferenceShearStretchStrain_REPLACED = {{0.8330628756074665,0.42173609990468275,0.5729168526354368,0.25708185818524776,0.7142533332135373},{0.8957865325349527,0.1461385058562097,0.19592065162059813,0.6849354450730554,0.3365366346874151},{0.41888599874152754,0.2251580498219029,0.893616157352747,0.42505738060020026,0.785763682498953}};
     blaze::DynamicMatrix<elastica::real_t> ROD00000_ReferenceCurvature_REPLACED( 3UL, 4UL );     ROD00000_ReferenceCurvature_REPLACED = {{0.42296459317398183,0.35168182761748235,0.5263809158705643,0.3073262083073959},{0.357345091966899,0.026933120150035905,0.7107949934487903,0.2763092450283908},{0.2914274522809438,0.14947801345388878,0.28186903372731364,0.8952340780705201}};
     blaze::DynamicVector<elastica::real_t> ROD00000_ReferenceElementLength_REPLACED( 5UL ); ROD00000_ReferenceElementLength_REPLACED={0.0426999984824372,0.06021312422305085,0.12769661319401673,0.12159671964324305,0.0678179495567241};
     blaze::DynamicMatrix<elastica::real_t> ROD00000_Position_REPLACED( 3UL, 6UL );     ROD00000_Position_REPLACED = {{0.05374345637055803,0.028996065607452062,0.07415154940462247,0.024859356918426626,0.04064159265168045,0.06737567066158216},{0.02963858757601048,0.06973946292340193,0.08828160600644791,0.20771545928276713,0.21467524757067283,0.22514507962648403},{0.04836319070419701,0.018704045242927358,0.0165739712548571,0.06855920051506412,0.015482774970845548,0.038441072012352875}};
     blaze::DynamicTensor<elastica::real_t> ROD00000_Director_REPLACED( 3UL, 3UL, 5UL);     ROD00000_Director_REPLACED = {{{0.027989491107628155,0.03323424292011635,0.00656565551820349,0.014575406270250519,0.014417391356387824},{-0.005516804929133914,-0.004039109173336363,-0.028713681937887404,-0.0073883408736775845,-0.020190406250985567},{0.9995929937984309,0.99943942822695,0.9995661141901452,0.9998664760612747,0.9996921957892324}},{{0.999407638620116,0.9993996617357986,0.9998048854052993,0.9994198020920843,0.9995982434141826},{0.020185977126543624,0.009927444148141009,0.01881138633619778,0.030893462155792993,0.024691273023532488},{-0.027872893556475445,-0.03319280004845875,-0.006026845261556988,-0.01434061303598674,-0.01391735606205962}},{{-0.020023991992106797,-0.00978780976000858,-0.018630171424673592,-0.030783383801597716,-0.024402675872894034},{0.9997810216196448,0.999942564075443,0.9994106544427472,0.9994953758848327,0.9994912148347767},{0.006078532214235719,0.0043666154319415285,0.02883158856197704,0.007834338935775,0.02053827836857159}}};
     blaze::DynamicMatrix<elastica::real_t> ROD00000_Velocity_REPLACED( 3UL, 6UL );     ROD00000_Velocity_REPLACED = {{0.05000081261681551,0.03606695351052915,0.04624814200030012,0.025521678254808857,0.08879391958437341,0.010899786996980654},{0.01968499323761388,0.06758684467024345,0.03979854625346502,0.05467244975907565,0.030489744465855475,0.0501465843868291},{0.025331464086950153,0.07832008548993236,0.015947804239189612,0.019968834201570376,0.06961861647008744,0.07038236907405}};
     blaze::DynamicMatrix<elastica::real_t> ROD00000_Acceleration_REPLACED( 3UL, 6UL );     ROD00000_Acceleration_REPLACED = {{0.043850613884863304,0.046868672010347126,0.07523022215578842,0.08770114933878452,0.009577245914128951,0.09875343648612099},{0.01932221298301469,0.0726677364112708,0.001063980046126911,0.0838166367595207,0.014960843780453748,0.07960981813312598},{0.044280498457266185,0.018307202694590942,0.005622099491997146,0.01090490002706256,0.0710350804397759,0.06736100309564207}};
     blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularVelocity_REPLACED( 3UL, 5UL );     ROD00000_AngularVelocity_REPLACED = {{0.001194391204051315,0.01409282051105949,0.01463816812234734,0.03931543293728981,0.0235708323000189},{0.09073814929269795,0.04078499055234254,0.09258798531315957,0.014499827620907391,0.0493737051779979},{0.05973131578797437,0.08073493167581633,0.09944518992838293,0.06111602004198833,0.02882443602400854}};
     blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularAcceleration_REPLACED( 3UL, 5UL );     ROD00000_AngularAcceleration_REPLACED = {{0.08926565941377294,0.08584207275989887,0.08885434917188945,0.04558153086157374,0.06831109257680112},{0.07905526092391878,0.0004386444369899212,0.0732637876081235,0.0638572284293879,0.06747098268735854},{0.020611557335144727,0.04025138027428276,0.004734782466892584,0.040794854777998435,0.09965710897628582}};
     blaze::DynamicVector<elastica::real_t> ROD00000_ElementDimension_PYEL( 5UL ); ROD00000_ElementDimension_PYEL={0.006048363804440265,0.0064566140709191545,0.003824309314689266,0.00604133826466519,0.007443679400016551};
     blaze::DynamicTensor<elastica::real_t> ROD00000_MassSecondMomentOfInertia_PYEL( 3UL, 3UL, 5UL);     ROD00000_MassSecondMomentOfInertia_PYEL = {{{0.0000000007127235497145191,0.0000000007127235497145191,0.000000000712723549714519,0.0000000007127235497145191,0.0000000007127235497145191},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.0000000007127235497145191,0.0000000007127235497145191,0.000000000712723549714519,0.0000000007127235497145191,0.0000000007127235497145191},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{0.0000000014254470994290381,0.0000000014254470994290381,0.000000001425447099429038,0.0000000014254470994290381,0.0000000014254470994290381}}};
     blaze::DynamicTensor<elastica::real_t> ROD00000_InvMassSecondMomentOfInertia_PYEL( 3UL, 3UL, 5UL);     ROD00000_InvMassSecondMomentOfInertia_PYEL = {{{1403068553.579504,1403068553.579504,1403068553.5795043,1403068553.579504,1403068553.579504},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{1403068553.579504,1403068553.579504,1403068553.5795043,1403068553.579504,1403068553.579504},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{701534276.789752,701534276.789752,701534276.7897521,701534276.789752,701534276.789752}}};
     blaze::DynamicTensor<elastica::real_t> ROD00000_ShearStretchRigidityMatrix_PYEL( 3UL, 3UL, 5UL);     ROD00000_ShearStretchRigidityMatrix_PYEL = {{{0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{0.035550566574190964,0.035550566574190964,0.035550566574190964,0.035550566574190964,0.035550566574190964}}};
     blaze::DynamicTensor<elastica::real_t> ROD00000_BendingTwistRigidityMatrix_PYEL( 3UL, 3UL, 4UL);     ROD00000_BendingTwistRigidityMatrix_PYEL = {{{0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868},{0.,0.,0.,0.},{0.,0.,0.,0.}},{{0.,0.,0.,0.},{0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868},{0.,0.,0.,0.}},{{0.,0.,0.,0.},{0.,0.,0.,0.},{0.00000033524471054032456,0.00000033524471054032456,0.00000033524471054032456,0.00000033524471054032456}}};
     blaze::DynamicVector<elastica::real_t> ROD00000_ElementVolume_PYEL( 5UL ); ROD00000_ElementVolume_PYEL={0.0000063991019833543736,0.0000063991019833543736,0.000006399101983354373,0.000006399101983354375,0.000006399101983354375};
     blaze::DynamicVector<elastica::real_t> ROD00000_Mass_PYEL( 6UL ); ROD00000_Mass_PYEL={0.000025193264508466167,0.00005038652901693233,0.000050386529016932326,0.00005038652901693234,0.00005038652901693235,0.000025193264508466173};
     blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalLoads_PYEL( 3UL, 6UL );     ROD00000_InternalLoads_PYEL = {{-0.012704766429325904,0.021448478376042156,-0.014123890506958477,-0.005208704245954657,0.014281369908107855,-0.003694775466521151},{-0.013060657511705088,-0.027341021495478235,0.009727307335704023,-0.07609225123642302,-0.00016504235638283398,0.10692937304630667},{-0.013294323406403562,0.0067326444065524465,0.0036668349962333878,-0.014973411466203073,0.007782366490654369,0.010083553943894437}};
     blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalTorques_PYEL( 3UL, 5UL );     ROD00000_InternalTorques_PYEL = {{0.0008061943984918901,-0.001993257481579183,0.0021737833367582375,-0.0017018379350442299,-0.0028696279734918466},{-0.0009422381656041703,-0.00013880739511641,0.0012277167445807675,-0.005738459149613437,0.0023974564080564793},{0.00007127529732270515,0.00027097059030043775,-0.0003831621172438921,0.001009006146853488,0.00023753532355623558}};
     blaze::DynamicVector<elastica::real_t> ROD00000_ElementLength_PYEL( 5UL ); ROD00000_ElementLength_PYEL={0.055679246247616845,0.04886067951087357,0.13927178325046535,0.055808821581400166,0.03676155210764667};
     blaze::DynamicVector<elastica::real_t> ROD00000_ReferenceElementLength_PYEL( 5UL ); ROD00000_ReferenceElementLength_PYEL={0.0426999984824372,0.06021312422305085,0.12769661319401673,0.12159671964324305,0.0678179495567241};
     blaze::DynamicMatrix<elastica::real_t> ROD00000_Tangent_PYEL( 3UL, 5UL );     ROD00000_Tangent_PYEL = {{-0.44446346584954344,0.924168150120004,-0.3539280630703861,0.28279105858263975,0.7272293055423203},{0.7202122523184807,0.3794900780886515,0.857559589522382,0.12470767327983209,0.284803863154445},{-0.5326786452778014,-0.04359484987506622,0.3732646200610296,-0.9510400692980724,0.6245192524591973}};
     blaze::DynamicVector<elastica::real_t> ROD00000_ElementDilatation_PYEL( 5UL ); ROD00000_ElementDilatation_PYEL={1.3039636586993815,0.8114622873557634,1.09064586575105,0.45896650621118446,0.5420622762547351};
     blaze::DynamicVector<elastica::real_t> ROD00000_VoronoiDilatation_PYEL( 4UL ); ROD00000_VoronoiDilatation_PYEL={1.451943413312367,2.6129508716852636,2.709452844887021,1.2856996345700946};
     blaze::DynamicVector<elastica::real_t> ROD00000_ReferenceVoronoiLength_PYEL( 4UL ); ROD00000_ReferenceVoronoiLength_PYEL={0.036,0.03599999999999999,0.036,0.036000000000000004};
     blaze::DynamicMatrix<elastica::real_t> ROD00000_ShearStretchStrain_PYEL( 3UL, 5UL );     ROD00000_ShearStretchStrain_PYEL = {{-0.715713598870118,-0.011676280882605257,0.37753274084464095,-0.4349683734612173,0.34099049049393704},{-0.5409032934071408,0.7537086810860585,-0.37079421491886355,0.13774417104662112,0.3931456536147561},{-0.05369196622782546,-0.699570420363369,-0.04632862879673083,-0.9502073172563688,-0.8483639496200347}};
     blaze::DynamicMatrix<elastica::real_t> ROD00000_Curvature_PYEL( 3UL, 4UL );     ROD00000_Curvature_PYEL = {{-0.2856947816654389,0.25912304367927697,0.3314966906884659,-0.17217152187208679},{-0.03884842645187527,0.6747570422266704,-0.5867671247094117,0.35539228644007403},{0.14635225327044962,-0.7505933669350083,0.23706983854593255,-0.01420255223507034}};
     blaze::DynamicMatrix<elastica::real_t> ROD00000_ReferenceShearStretchStrain_PYEL( 3UL, 5UL );     ROD00000_ReferenceShearStretchStrain_PYEL = {{0.8330628756074665,0.42173609990468275,0.5729168526354368,0.25708185818524776,0.7142533332135373},{0.8957865325349527,0.1461385058562097,0.19592065162059813,0.6849354450730554,0.3365366346874151},{0.41888599874152754,0.2251580498219029,0.893616157352747,0.42505738060020026,0.785763682498953}};
     blaze::DynamicMatrix<elastica::real_t> ROD00000_ReferenceCurvature_PYEL( 3UL, 4UL );     ROD00000_ReferenceCurvature_PYEL = {{0.42296459317398183,0.35168182761748235,0.5263809158705643,0.3073262083073959},{0.357345091966899,0.026933120150035905,0.7107949934487903,0.2763092450283908},{0.2914274522809438,0.14947801345388878,0.28186903372731364,0.8952340780705201}};
     blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalStress_PYEL( 3UL, 5UL );     ROD00000_InternalStress_PYEL = {{-0.017697818945718046,-0.004952589331261207,-0.002232647959422169,-0.007908035731981695,-0.004265262494927654},{-0.016417008419019823,0.006942684845250518,-0.006475832547266212,-0.006252737083744218,0.0006468694380244519},{-0.01680041440513867,-0.03287462104237127,-0.0334155696960707,-0.04889143919828458,-0.058094163176371116}};
     blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalCouple_PYEL( 3UL, 4UL );     ROD00000_InternalCouple_PYEL = {{-0.0000003563614604845934,-0.00000004654476409399215,-0.00000009800085849007204,-0.00000024112361663814495},{-0.00000019923267210038208,0.00000032576931485651864,-0.0000006525012550650552,0.00000003976825699006235},{-0.0000000486356930988531,-0.0000003017441693841062,-0.000000015018693221021606,-0.00000030488381988156584}};
     blaze::DynamicMatrix<elastica::real_t> ROD00000_Position_PYEL( 3UL, 6UL );     ROD00000_Position_PYEL = {{0.05374345637055803,0.028996065607452062,0.07415154940462247,0.024859356918426626,0.04064159265168045,0.06737567066158216},{0.02963858757601048,0.06973946292340193,0.08828160600644791,0.20771545928276713,0.21467524757067283,0.22514507962648403},{0.04836319070419701,0.018704045242927358,0.0165739712548571,0.06855920051506412,0.015482774970845548,0.038441072012352875}};
     blaze::DynamicTensor<elastica::real_t> ROD00000_Director_PYEL( 3UL, 3UL, 5UL);     ROD00000_Director_PYEL = {{{0.027989491107628155,0.03323424292011635,0.00656565551820349,0.014575406270250519,0.014417391356387824},{-0.005516804929133914,-0.004039109173336363,-0.028713681937887404,-0.0073883408736775845,-0.020190406250985567},{0.9995929937984309,0.99943942822695,0.9995661141901452,0.9998664760612747,0.9996921957892324}},{{0.999407638620116,0.9993996617357986,0.9998048854052993,0.9994198020920843,0.9995982434141826},{0.020185977126543624,0.009927444148141009,0.01881138633619778,0.030893462155792993,0.024691273023532488},{-0.027872893556475445,-0.03319280004845875,-0.006026845261556988,-0.01434061303598674,-0.01391735606205962}},{{-0.020023991992106797,-0.00978780976000858,-0.018630171424673592,-0.030783383801597716,-0.024402675872894034},{0.9997810216196448,0.999942564075443,0.9994106544427472,0.9994953758848327,0.9994912148347767},{0.006078532214235719,0.0043666154319415285,0.02883158856197704,0.007834338935775,0.02053827836857159}}};
     blaze::DynamicMatrix<elastica::real_t> ROD00000_Velocity_PYEL( 3UL, 6UL );     ROD00000_Velocity_PYEL = {{0.05000081261681551,0.03606695351052915,0.04624814200030012,0.025521678254808857,0.08879391958437341,0.010899786996980654},{0.01968499323761388,0.06758684467024345,0.03979854625346502,0.05467244975907565,0.030489744465855475,0.0501465843868291},{0.025331464086950153,0.07832008548993236,0.015947804239189612,0.019968834201570376,0.06961861647008744,0.07038236907405}};
     blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularVelocity_PYEL( 3UL, 5UL );     ROD00000_AngularVelocity_PYEL = {{0.001194391204051315,0.01409282051105949,0.01463816812234734,0.03931543293728981,0.0235708323000189},{0.09073814929269795,0.04078499055234254,0.09258798531315957,0.014499827620907391,0.0493737051779979},{0.05973131578797437,0.08073493167581633,0.09944518992838293,0.06111602004198833,0.02882443602400854}};
     blaze::DynamicMatrix<elastica::real_t> ROD00000_Acceleration_PYEL( 3UL, 6UL );     ROD00000_Acceleration_PYEL = {{38189.527027864,18740.566797961335,7804.6614701462895,15392.212892372356,13116.143521859312,8802.596230107372},{32207.816114563924,6311.51345205207,11477.163738561681,12111.916765808708,4899.002942209517,38990.73020013227},{5661.78160847816,13931.003223503221,3351.8341969569683,9248.361725209976,10515.009664823017,23212.428138884654}};
     blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularAcceleration_PYEL( 3UL, 5UL );     ROD00000_AngularAcceleration_PYEL = {{1541092451.8194754,553809914.5813245,1405799877.2232914,79114156.60635054,342944147.5686537},{1752655861.894956,763399966.5327481,1215140155.0002701,488166862.7936881,358848849.2371865},{245659368.0171112,258160039.84816694,193080672.793027,305309918.5375744,19005515.8131576}};


     blocks::get<elastica::tags::InternalLoads>(slicedrod_00000) = ROD00000_InternalLoads_REPLACED;
     blocks::get<elastica::tags::InternalTorques>(slicedrod_00000) = ROD00000_InternalTorques_REPLACED;
     blocks::get<elastica::tags::ExternalLoads>(slicedrod_00000) = ROD00000_ExternalLoads_REPLACED;
     blocks::get<elastica::tags::ExternalTorques>(slicedrod_00000) = ROD00000_ExternalTorques_REPLACED;
     blocks::get<elastica::tags::InternalStress>(slicedrod_00000) = ROD00000_InternalStress_REPLACED;
     blocks::get<elastica::tags::InternalCouple>(slicedrod_00000) = ROD00000_InternalCouple_REPLACED;
     blocks::get<elastica::tags::ReferenceShearStretchStrain>(slicedrod_00000) = ROD00000_ReferenceShearStretchStrain_REPLACED;
     blocks::get<elastica::tags::ReferenceCurvature>(slicedrod_00000) = ROD00000_ReferenceCurvature_REPLACED;
     blocks::get<elastica::tags::ReferenceElementLength>(slicedrod_00000) = ROD00000_ReferenceElementLength_REPLACED;
     blocks::get<elastica::tags::Position>(slicedrod_00000) = ROD00000_Position_REPLACED;
     blocks::get<elastica::tags::Director>(slicedrod_00000) = ROD00000_Director_REPLACED;
     blocks::get<elastica::tags::Velocity>(slicedrod_00000) = ROD00000_Velocity_REPLACED;
     blocks::get<elastica::tags::Acceleration>(slicedrod_00000) = ROD00000_Acceleration_REPLACED;
     blocks::get<elastica::tags::AngularVelocity>(slicedrod_00000) = ROD00000_AngularVelocity_REPLACED;
     blocks::get<elastica::tags::AngularAcceleration>(slicedrod_00000) = ROD00000_AngularAcceleration_REPLACED;


     slicedrod_00000.compute_internal_forces();
     slicedrod_00000.compute_internal_torques();
     slicedrod_00000.update_acceleration();


     auto ROD00000_ElementDimension(blocks::get<elastica::tags::ElementDimension>(slicedrod_00000));
     auto ROD00000_MassSecondMomentOfInertia(blocks::get<elastica::tags::MassSecondMomentOfInertia>(slicedrod_00000));
     auto ROD00000_InvMassSecondMomentOfInertia(blocks::get<elastica::tags::InvMassSecondMomentOfInertia>(slicedrod_00000));
     auto ROD00000_ShearStretchRigidityMatrix(blocks::get<elastica::tags::ShearStretchRigidityMatrix>(slicedrod_00000));
     auto ROD00000_BendingTwistRigidityMatrix(blocks::get<elastica::tags::BendingTwistRigidityMatrix>(slicedrod_00000));
     auto ROD00000_ElementVolume(blocks::get<elastica::tags::ElementVolume>(slicedrod_00000));
     auto ROD00000_Mass(blocks::get<elastica::tags::Mass>(slicedrod_00000));
     auto ROD00000_InternalLoads(blocks::get<elastica::tags::InternalLoads>(slicedrod_00000));
     auto ROD00000_InternalTorques(blocks::get<elastica::tags::InternalTorques>(slicedrod_00000));
     auto ROD00000_ElementLength(blocks::get<elastica::tags::ElementLength>(slicedrod_00000));
     auto ROD00000_ReferenceElementLength(blocks::get<elastica::tags::ReferenceElementLength>(slicedrod_00000));
     auto ROD00000_Tangent(blocks::get<elastica::tags::Tangent>(slicedrod_00000));
     auto ROD00000_ElementDilatation(blocks::get<elastica::tags::ElementDilatation>(slicedrod_00000));
     auto ROD00000_VoronoiDilatation(blocks::get<elastica::tags::VoronoiDilatation>(slicedrod_00000));
     auto ROD00000_ReferenceVoronoiLength(blocks::get<elastica::tags::ReferenceVoronoiLength>(slicedrod_00000));
     auto ROD00000_ShearStretchStrain(blocks::get<elastica::tags::ShearStretchStrain>(slicedrod_00000));
     auto ROD00000_Curvature(blocks::get<elastica::tags::Curvature>(slicedrod_00000));
     auto ROD00000_ReferenceShearStretchStrain(blocks::get<elastica::tags::ReferenceShearStretchStrain>(slicedrod_00000));
     auto ROD00000_ReferenceCurvature(blocks::get<elastica::tags::ReferenceCurvature>(slicedrod_00000));
     auto ROD00000_InternalStress(blocks::get<elastica::tags::InternalStress>(slicedrod_00000));
     auto ROD00000_InternalCouple(blocks::get<elastica::tags::InternalCouple>(slicedrod_00000));
     auto ROD00000_Position(blocks::get<elastica::tags::Position>(slicedrod_00000));
     auto ROD00000_Director(blocks::get<elastica::tags::Director>(slicedrod_00000));
     auto ROD00000_Velocity(blocks::get<elastica::tags::Velocity>(slicedrod_00000));
     auto ROD00000_AngularVelocity(blocks::get<elastica::tags::AngularVelocity>(slicedrod_00000));
     auto ROD00000_Acceleration(blocks::get<elastica::tags::Acceleration>(slicedrod_00000));
     auto ROD00000_AngularAcceleration(blocks::get<elastica::tags::AngularAcceleration>(slicedrod_00000));


     CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ElementDimension , ROD00000_ElementDimension_PYEL , approx);
     CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_MassSecondMomentOfInertia)),
                                  blaze::evaluate(blaze::ravel(ROD00000_MassSecondMomentOfInertia_PYEL)),
                                  approx);
     CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_InvMassSecondMomentOfInertia)),
                                  blaze::evaluate(blaze::ravel(ROD00000_InvMassSecondMomentOfInertia_PYEL)),
                                  approx);
     CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_ShearStretchRigidityMatrix)),
                                  blaze::evaluate(blaze::ravel(ROD00000_ShearStretchRigidityMatrix_PYEL)),
                                  approx);
     CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_BendingTwistRigidityMatrix)),
                                  blaze::evaluate(blaze::ravel(ROD00000_BendingTwistRigidityMatrix_PYEL)),
                                  approx);
     CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ElementVolume , ROD00000_ElementVolume_PYEL , approx);
     CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_Mass , ROD00000_Mass_PYEL , approx);
     CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_InternalLoads)),
                                  blaze::evaluate(blaze::ravel(ROD00000_InternalLoads_PYEL)),
                                  approx);
     CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_InternalTorques)),
                                  blaze::evaluate(blaze::ravel(ROD00000_InternalTorques_PYEL)),
                                  approx);
     CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ElementLength , ROD00000_ElementLength_PYEL , approx);
     CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ReferenceElementLength , ROD00000_ReferenceElementLength_PYEL , approx);
     CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_Tangent)),
                                  blaze::evaluate(blaze::ravel(ROD00000_Tangent_PYEL)),
                                  approx);
     CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ElementDilatation , ROD00000_ElementDilatation_PYEL , approx);
     CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_VoronoiDilatation , ROD00000_VoronoiDilatation_PYEL , approx);
     CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ReferenceVoronoiLength , ROD00000_ReferenceVoronoiLength_PYEL , approx);
     CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_ShearStretchStrain)),
                                  blaze::evaluate(blaze::ravel(ROD00000_ShearStretchStrain_PYEL)),
                                  approx);
     CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_Curvature)),
                                  blaze::evaluate(blaze::ravel(ROD00000_Curvature_PYEL)),
                                  approx);
     CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_ReferenceShearStretchStrain)),
                                  blaze::evaluate(blaze::ravel(ROD00000_ReferenceShearStretchStrain_PYEL)),
                                  approx);
     CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_ReferenceCurvature)),
                                  blaze::evaluate(blaze::ravel(ROD00000_ReferenceCurvature_PYEL)),
                                  approx);
     CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_InternalStress)),
                                  blaze::evaluate(blaze::ravel(ROD00000_InternalStress_PYEL)),
                                  approx);
     CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_InternalCouple)),
                                  blaze::evaluate(blaze::ravel(ROD00000_InternalCouple_PYEL)),
                                  approx);
     CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_Position)),
                                  blaze::evaluate(blaze::ravel(ROD00000_Position_PYEL)),
                                  approx);
     CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_Director)),
                                  blaze::evaluate(blaze::ravel(ROD00000_Director_PYEL)),
                                  approx);
     CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_Velocity)),
                                  blaze::evaluate(blaze::ravel(ROD00000_Velocity_PYEL)),
                                  approx);
     CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_AngularVelocity)),
                                  blaze::evaluate(blaze::ravel(ROD00000_AngularVelocity_PYEL)),
                                  approx);
     CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_Acceleration)),
                                  blaze::evaluate(blaze::ravel(ROD00000_Acceleration_PYEL)),
                                  approx);
     CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_AngularAcceleration)),
                                  blaze::evaluate(blaze::ravel(ROD00000_AngularAcceleration_PYEL)),
                                  approx);
     // clang-format on
    rel_comp(ROD00000_InternalLoads, ROD00000_InternalLoads_PYEL);
  }

  ELASTICA_TEST_CASE("Unit.z.Integrated.CosseratRods.JustDirector",
                     "[Systems][Integrated]") {
    /*
     * rng_seed = 150
     * shuffle_variables = {
     * }
     * perturb_mag = 0.1
     * perturb_variables = {
     *  'director_collection':'Director',
     *  }
     * director_perturb_ang = 2 # in degree
     * NROD=1, N=5, n_step=1, time=0.0, time_step=2e-05
     * tag=150
     */

    Approx approx =
        Approx::custom() //.epsilon(std::numeric_limits<double>::epsilon() * 100)
            .epsilon(1.0e-12)
            .scale(0.0);

    elastica::MaterialID myMaterial = elastica::create_material(
        7.874, 0.24, 200, 200.0 / (2.*(1.+0.50)), 200, 1e-10, 1e-10, 0.1, 0.1);

    using Init = ::elastica::cosserat_rod::StraightCosseratRodInitializer;
    Init straight_initializer{
        // Initialization Wrapper
        Init::NElement{5UL},
        Init::Material{myMaterial},
        Init::Radius{0.007522},
        Init::Length{0.18},
        Init::Origin{::elastica::Vec3{0.0, 0.0, 0.0}},
        Init::Direction{::elastica::Vec3{0.0, 1.0, 0.0}},
        Init::Normal{::elastica::Vec3{0.0, 0.0, 1.0}},
        Init::ForceDampingRate{0.2},
        Init::TorqueDampingRate{0.2},
    };

    Model simple_rod{};
    auto slicedrod_00000 = simple_rod.emplace_back(
        straight_initializer.initialize<Plugin>());
    //**************************************************************************
    // clang-format off
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalLoads_REPLACED( 3UL, 6UL );     ROD00000_InternalLoads_REPLACED = {{0.004822044056611019,0.9307870177032551,0.7845822109817221,0.3343249592744213,0.6862689176861808,0.7322625609228085},{0.8246472414522372,0.10745443906835128,0.3089916869203323,0.31294344042724076,0.5940727124966699,0.6105506387346894},{0.9165420505570774,0.738817174285074,0.37747080022423485,0.7949533183809576,0.48693548220111915,0.5788914075188061}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalTorques_REPLACED( 3UL, 5UL );     ROD00000_InternalTorques_REPLACED = {{0.9744748566188305,0.688389675725899,0.5848087428374977,0.2739643248027279,0.8377858251400324},{0.30222935408131235,0.1728462163878618,0.801743366769752,0.3441099204558331,0.4375847094010139},{0.15268115598708532,0.2761357842472395,0.5638672039037047,0.86467466140162,0.08278860289302525}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ExternalLoads_REPLACED( 3UL, 6UL );     ROD00000_ExternalLoads_REPLACED = {{0.9748236222955216,0.9228236343831954,0.4073736921398182,0.7807688857822744,0.6465955762463057,0.22546091065284335},{0.824480688125953,0.3453562771880586,0.5685671364094176,0.6863696968075128,0.2470087962580596,0.8753744062638658},{0.15593308505796288,0.6952022537494705,0.16522045602868496,0.48096625789258196,0.5220324730992747,0.5747132880427898}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ExternalTorques_REPLACED( 3UL, 5UL );     ROD00000_ExternalTorques_REPLACED = {{0.8415277735547125,0.4884155772097232,0.9164990050965889,0.12455725715642718,0.45378529009583934},{0.9589130366491199,0.6706482598702904,0.7928513073170474,0.7638068902765369,0.4694302963114608},{0.2684748849053611,0.45322327766269055,0.2527348132181251,0.9472151712532533,0.0497407763497818}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalStress_REPLACED( 3UL, 5UL );     ROD00000_InternalStress_REPLACED = {{0.4375196365597337,0.7683879888762261,0.16663239947009578,0.8244444091919032,0.8696022625042859},{0.44593293278653,0.16041064218431744,0.41984957138538315,0.4522866548422185,0.8544544784625221},{0.7439885668390492,0.02976576715291579,0.18617391272004868,0.7383720961472324,0.8817988526289666}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalCouple_REPLACED( 3UL, 4UL );     ROD00000_InternalCouple_REPLACED = {{0.6536435340498628,0.2525396458442313,0.6183595148406859,0.19936517150220823},{0.8575809295973004,0.021480917433798763,0.9080569690386844,0.25119605094289243},{0.908052828476898,0.7615298850236448,0.7495712349893845,0.46584477600408136}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ReferenceShearStretchStrain_REPLACED( 3UL, 5UL );     ROD00000_ReferenceShearStretchStrain_REPLACED = {{0.8330628756074665,0.42173609990468275,0.5729168526354368,0.25708185818524776,0.7142533332135373},{0.8957865325349527,0.1461385058562097,0.19592065162059813,0.6849354450730554,0.3365366346874151},{0.41888599874152754,0.2251580498219029,0.893616157352747,0.42505738060020026,0.785763682498953}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ReferenceCurvature_REPLACED( 3UL, 4UL );     ROD00000_ReferenceCurvature_REPLACED = {{0.42296459317398183,0.35168182761748235,0.5263809158705643,0.3073262083073959},{0.357345091966899,0.026933120150035905,0.7107949934487903,0.2763092450283908},{0.2914274522809438,0.14947801345388878,0.28186903372731364,0.8952340780705201}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_Director_REPLACED( 3UL, 3UL, 5UL);     ROD00000_Director_REPLACED = {{{0.008929306869093294,0.010729301673494002,0.03108882919338686,0.028069308307168833,0.005514995179358402},{-0.0023097416714801125,-0.028099746190731512,-0.011970825330580848,-0.016696973779586885,-0.010966956545234918},{0.9999574653815275,0.999547540815151,0.9994449379732179,0.9994665201985312,0.999924652507531}},{{0.9993938521821771,0.9997883573647111,0.9994644449419705,0.9995331526697828,0.9994519424176602},{0.03366998110935982,0.017848959655636928,0.0105896543962556,0.012537990055759245,0.03269944469674037},{-0.008846501713348878,-0.010230108366213303,-0.030962598673349405,-0.027861721399915856,-0.005153747577348094}},{{-0.03364811583590522,-0.01755342028130646,-0.010213128620723952,-0.01206609485964493,-0.03264045994985146},{0.9994303364744871,0.9994457558583297,0.9998722711230034,0.9997819811698777,0.9994050591130005},{0.0026089912202082585,0.028285305959100105,0.012293634251268455,0.0170411115503825,0.01114128330141533}}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementDimension_PYEL( 5UL ); ROD00000_ElementDimension_PYEL={0.007521999999998955,0.007521999999998955,0.007521999999998956,0.007521999999998956,0.007521999999998956};
    blaze::DynamicTensor<elastica::real_t> ROD00000_MassSecondMomentOfInertia_PYEL( 3UL, 3UL, 5UL);     ROD00000_MassSecondMomentOfInertia_PYEL = {{{0.0000000007127235497145191,0.0000000007127235497145191,0.000000000712723549714519,0.0000000007127235497145191,0.0000000007127235497145191},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.0000000007127235497145191,0.0000000007127235497145191,0.000000000712723549714519,0.0000000007127235497145191,0.0000000007127235497145191},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{0.0000000014254470994290381,0.0000000014254470994290381,0.000000001425447099429038,0.0000000014254470994290381,0.0000000014254470994290381}}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_InvMassSecondMomentOfInertia_PYEL( 3UL, 3UL, 5UL);     ROD00000_InvMassSecondMomentOfInertia_PYEL = {{{1403068553.579504,1403068553.579504,1403068553.5795043,1403068553.579504,1403068553.579504},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{1403068553.579504,1403068553.579504,1403068553.5795043,1403068553.579504,1403068553.579504},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{701534276.789752,701534276.789752,701534276.7897521,701534276.789752,701534276.789752}}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_ShearStretchRigidityMatrix_PYEL( 3UL, 3UL, 5UL);     ROD00000_ShearStretchRigidityMatrix_PYEL = {{{0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{0.035550566574190964,0.035550566574190964,0.035550566574190964,0.035550566574190964,0.035550566574190964}}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_BendingTwistRigidityMatrix_PYEL( 3UL, 3UL, 4UL);     ROD00000_BendingTwistRigidityMatrix_PYEL = {{{0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868},{0.,0.,0.,0.},{0.,0.,0.,0.}},{{0.,0.,0.,0.},{0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868},{0.,0.,0.,0.}},{{0.,0.,0.,0.},{0.,0.,0.,0.},{0.00000033524471054032456,0.00000033524471054032456,0.00000033524471054032456,0.00000033524471054032456}}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementVolume_PYEL( 5UL ); ROD00000_ElementVolume_PYEL={0.0000063991019833543736,0.0000063991019833543736,0.000006399101983354373,0.000006399101983354375,0.000006399101983354375};
    blaze::DynamicVector<elastica::real_t> ROD00000_Mass_PYEL( 6UL ); ROD00000_Mass_PYEL={0.000025193264508466167,0.00005038652901693233,0.000050386529016932326,0.00005038652901693234,0.00005038652901693235,0.000025193264508466173};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalLoads_PYEL( 3UL, 6UL );     ROD00000_InternalLoads_PYEL = {{-0.009428886986208394,0.008048937485335058,-0.0006199668484017113,-0.005585349842945949,0.0049820082757246,0.0026032579164963973},{-0.015213039126287421,0.0073115606828787864,-0.023809984284846153,0.016551878157571823,-0.01280192466762266,0.02796150923830562},{-0.009497124743213809,0.004147220744758922,-0.001654914112365789,0.0038344588727695283,-0.005409651865028734,0.008580011103079882}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalTorques_PYEL( 3UL, 5UL );     ROD00000_InternalTorques_PYEL = {{0.0003359373260999311,0.00004773934380722986,0.00006416498499732098,0.00027008696083972365,0.00009187429432813102},{-0.0003445119077386708,-0.0001934832633708005,-0.00025432659620234794,-0.00012161781262103619,-0.0003089642732522567},{0.000012306846743418546,0.000005017396899442458,0.000003194794311825254,0.000005643076259837776,0.000011633170891887118}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementLength_PYEL( 5UL ); ROD00000_ElementLength_PYEL={0.036000000000009996,0.036000000000009996,0.03600000000000999,0.03600000000001,0.03600000000001};
    blaze::DynamicVector<elastica::real_t> ROD00000_ReferenceElementLength_PYEL( 5UL ); ROD00000_ReferenceElementLength_PYEL={0.036,0.036,0.03599999999999999,0.036000000000000004,0.036000000000000004};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Tangent_PYEL( 3UL, 5UL );     ROD00000_Tangent_PYEL = {{0.,0.,0.,0.,0.},{0.9999999999997222,0.9999999999997222,0.9999999999997222,0.9999999999997222,0.9999999999997222},{0.,0.,0.,0.,0.}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementDilatation_PYEL( 5UL ); ROD00000_ElementDilatation_PYEL={1.0000000000002778,1.0000000000002778,1.0000000000002778,1.0000000000002778,1.0000000000002778};
    blaze::DynamicVector<elastica::real_t> ROD00000_VoronoiDilatation_PYEL( 4UL ); ROD00000_VoronoiDilatation_PYEL={1.0000000000002778,1.000000000000278,1.0000000000002778,1.0000000000002778};
    blaze::DynamicVector<elastica::real_t> ROD00000_ReferenceVoronoiLength_PYEL( 4UL ); ROD00000_ReferenceVoronoiLength_PYEL={0.036,0.03599999999999999,0.036,0.036000000000000004};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ShearStretchStrain_PYEL( 3UL, 5UL );     ROD00000_ShearStretchStrain_PYEL = {{-0.0023097416714801125,-0.028099746190731512,-0.011970825330580848,-0.016696973779586885,-0.010966956545234918},{0.03366998110935982,0.017848959655636928,0.0105896543962556,0.012537990055759245,0.03269944469674037},{-0.000569663525512909,-0.0005542441416702681,-0.0001277288769966356,-0.00021801883012229073,-0.0005949408869995176}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Curvature_PYEL( 3UL, 4UL );     ROD00000_Curvature_PYEL = {{-0.4401675542432311,-0.21316451204570688,0.055353526519164274,0.568842710021838},{0.7175523464982391,-0.44001719838044995,0.1303168409997054,-0.17331405503176842},{0.03163906950232023,0.5718303722213512,-0.08536979902115586,-0.622900471922686}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ReferenceShearStretchStrain_PYEL( 3UL, 5UL );     ROD00000_ReferenceShearStretchStrain_PYEL = {{0.8330628756074665,0.42173609990468275,0.5729168526354368,0.25708185818524776,0.7142533332135373},{0.8957865325349527,0.1461385058562097,0.19592065162059813,0.6849354450730554,0.3365366346874151},{0.41888599874152754,0.2251580498219029,0.893616157352747,0.42505738060020026,0.785763682498953}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ReferenceCurvature_PYEL( 3UL, 4UL );     ROD00000_ReferenceCurvature_PYEL = {{0.42296459317398183,0.35168182761748235,0.5263809158705643,0.3073262083073959},{0.357345091966899,0.026933120150035905,0.7107949934487903,0.2763092450283908},{0.2914274522809438,0.14947801345388878,0.28186903372731364,0.8952340780705201}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalStress_PYEL( 3UL, 5UL );     ROD00000_InternalStress_PYEL = {{-0.009545776021552932,-0.0051402597409518904,-0.00668349267877121,-0.0031284619046903885,-0.008287068918864632},{-0.009851378096625259,-0.0014659605170280674,-0.0021177713427059544,-0.007683464085720738,-0.0034719377947966907},{-0.014911886446345782,-0.008024199933165369,-0.031773101527686656,-0.015118781399813336,-0.027955494591871466}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalCouple_PYEL( 3UL, 4UL );     ROD00000_InternalCouple_PYEL = {{-0.0000004340407303783984,-0.00000028404262146022156,-0.0000002368641611995123,0.00000013150803587816462},{0.00000018113636516982615,-0.00000023481393655869762,-0.00000029190334528916373,-0.00000022610074962128036},{-0.0000000870926811863587,0.00000014159139426102126,-0.00000012311487618392733,-0.0000005089465777737383}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Position_PYEL( 3UL, 6UL );     ROD00000_Position_PYEL = {{0.,0.,0.,0.,0.,0.},{0.,0.036,0.072,0.10799999999999998,0.144,0.18},{0.,0.,0.,0.,0.,0.}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_Director_PYEL( 3UL, 3UL, 5UL);     ROD00000_Director_PYEL = {{{0.008929306869093294,0.010729301673494002,0.03108882919338686,0.028069308307168833,0.005514995179358402},{-0.0023097416714801125,-0.028099746190731512,-0.011970825330580848,-0.016696973779586885,-0.010966956545234918},{0.9999574653815275,0.999547540815151,0.9994449379732179,0.9994665201985312,0.999924652507531}},{{0.9993938521821771,0.9997883573647111,0.9994644449419705,0.9995331526697828,0.9994519424176602},{0.03366998110935982,0.017848959655636928,0.0105896543962556,0.012537990055759245,0.03269944469674037},{-0.008846501713348878,-0.010230108366213303,-0.030962598673349405,-0.027861721399915856,-0.005153747577348094}},{{-0.03364811583590522,-0.01755342028130646,-0.010213128620723952,-0.01206609485964493,-0.03264045994985146},{0.9994303364744871,0.9994457558583297,0.9998722711230034,0.9997819811698777,0.9994050591130005},{0.0026089912202082585,0.028285305959100105,0.012293634251268455,0.0170411115503825,0.01114128330141533}}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Velocity_PYEL( 3UL, 6UL );     ROD00000_Velocity_PYEL = {{0.,0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.,0.}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularVelocity_PYEL( 3UL, 5UL );     ROD00000_AngularVelocity_PYEL = {{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Acceleration_PYEL( 3UL, 6UL );     ROD00000_Acceleration_PYEL = {{38319.556998454624,18474.63181192163,8072.668096560639,15384.737767486007,12931.583048776158,9052.585007103744},{32122.381310596415,6999.248504544217,10811.563383171453,13950.585378263873,4648.204116456046,35856.24702184214},{5812.504380507712,13879.691420284455,3246.215707006791,9621.633524357963,10253.193290227142,23152.747788991557}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularAcceleration_PYEL( 3UL, 5UL );     ROD00000_AngularAcceleration_PYEL = {{1181192499.1369433,685347519.0335997,1286000961.3109734,175141321.15783334,636820776.5438559},{1344937353.5161023,940694013.6546016,1112067899.3117874,1071502790.6262137,658209388.7963223},{188352967.89317644,317955184.19537014,177304375.66833675,664507868.9410394,34903020.63164308}};


    blocks::get<elastica::tags::InternalLoads>(slicedrod_00000) = ROD00000_InternalLoads_REPLACED;
    blocks::get<elastica::tags::InternalTorques>(slicedrod_00000) = ROD00000_InternalTorques_REPLACED;
    blocks::get<elastica::tags::ExternalLoads>(slicedrod_00000) = ROD00000_ExternalLoads_REPLACED;
    blocks::get<elastica::tags::ExternalTorques>(slicedrod_00000) = ROD00000_ExternalTorques_REPLACED;
    blocks::get<elastica::tags::InternalStress>(slicedrod_00000) = ROD00000_InternalStress_REPLACED;
    blocks::get<elastica::tags::InternalCouple>(slicedrod_00000) = ROD00000_InternalCouple_REPLACED;
    blocks::get<elastica::tags::ReferenceShearStretchStrain>(slicedrod_00000) = ROD00000_ReferenceShearStretchStrain_REPLACED;
    blocks::get<elastica::tags::ReferenceCurvature>(slicedrod_00000) = ROD00000_ReferenceCurvature_REPLACED;
    blocks::get<elastica::tags::Director>(slicedrod_00000) = ROD00000_Director_REPLACED;


    slicedrod_00000.compute_internal_forces();
    slicedrod_00000.compute_internal_torques();
    slicedrod_00000.update_acceleration();


    auto ROD00000_ElementDimension(blocks::get<elastica::tags::ElementDimension>(slicedrod_00000));
    auto ROD00000_MassSecondMomentOfInertia(blocks::get<elastica::tags::MassSecondMomentOfInertia>(slicedrod_00000));
    auto ROD00000_InvMassSecondMomentOfInertia(blocks::get<elastica::tags::InvMassSecondMomentOfInertia>(slicedrod_00000));
    auto ROD00000_ShearStretchRigidityMatrix(blocks::get<elastica::tags::ShearStretchRigidityMatrix>(slicedrod_00000));
    auto ROD00000_BendingTwistRigidityMatrix(blocks::get<elastica::tags::BendingTwistRigidityMatrix>(slicedrod_00000));
    auto ROD00000_ElementVolume(blocks::get<elastica::tags::ElementVolume>(slicedrod_00000));
    auto ROD00000_Mass(blocks::get<elastica::tags::Mass>(slicedrod_00000));
    auto ROD00000_InternalLoads(blocks::get<elastica::tags::InternalLoads>(slicedrod_00000));
    auto ROD00000_InternalTorques(blocks::get<elastica::tags::InternalTorques>(slicedrod_00000));
    auto ROD00000_ElementLength(blocks::get<elastica::tags::ElementLength>(slicedrod_00000));
    auto ROD00000_ReferenceElementLength(blocks::get<elastica::tags::ReferenceElementLength>(slicedrod_00000));
    auto ROD00000_Tangent(blocks::get<elastica::tags::Tangent>(slicedrod_00000));
    auto ROD00000_ElementDilatation(blocks::get<elastica::tags::ElementDilatation>(slicedrod_00000));
    auto ROD00000_VoronoiDilatation(blocks::get<elastica::tags::VoronoiDilatation>(slicedrod_00000));
    auto ROD00000_ReferenceVoronoiLength(blocks::get<elastica::tags::ReferenceVoronoiLength>(slicedrod_00000));
    auto ROD00000_ShearStretchStrain(blocks::get<elastica::tags::ShearStretchStrain>(slicedrod_00000));
    auto ROD00000_Curvature(blocks::get<elastica::tags::Curvature>(slicedrod_00000));
    auto ROD00000_ReferenceShearStretchStrain(blocks::get<elastica::tags::ReferenceShearStretchStrain>(slicedrod_00000));
    auto ROD00000_ReferenceCurvature(blocks::get<elastica::tags::ReferenceCurvature>(slicedrod_00000));
    auto ROD00000_InternalStress(blocks::get<elastica::tags::InternalStress>(slicedrod_00000));
    auto ROD00000_InternalCouple(blocks::get<elastica::tags::InternalCouple>(slicedrod_00000));
    auto ROD00000_Position(blocks::get<elastica::tags::Position>(slicedrod_00000));
    auto ROD00000_Director(blocks::get<elastica::tags::Director>(slicedrod_00000));
    auto ROD00000_Velocity(blocks::get<elastica::tags::Velocity>(slicedrod_00000));
    auto ROD00000_AngularVelocity(blocks::get<elastica::tags::AngularVelocity>(slicedrod_00000));
    auto ROD00000_Acceleration(blocks::get<elastica::tags::Acceleration>(slicedrod_00000));
    auto ROD00000_AngularAcceleration(blocks::get<elastica::tags::AngularAcceleration>(slicedrod_00000));


    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ElementDimension , ROD00000_ElementDimension_PYEL , approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_MassSecondMomentOfInertia)),
                                 blaze::evaluate(blaze::ravel(ROD00000_MassSecondMomentOfInertia_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_InvMassSecondMomentOfInertia)),
                                 blaze::evaluate(blaze::ravel(ROD00000_InvMassSecondMomentOfInertia_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_ShearStretchRigidityMatrix)),
                                 blaze::evaluate(blaze::ravel(ROD00000_ShearStretchRigidityMatrix_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_BendingTwistRigidityMatrix)),
                                 blaze::evaluate(blaze::ravel(ROD00000_BendingTwistRigidityMatrix_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ElementVolume , ROD00000_ElementVolume_PYEL , approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_Mass , ROD00000_Mass_PYEL , approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_InternalLoads)),
                                 blaze::evaluate(blaze::ravel(ROD00000_InternalLoads_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_InternalTorques)),
                                 blaze::evaluate(blaze::ravel(ROD00000_InternalTorques_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ElementLength , ROD00000_ElementLength_PYEL , approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ReferenceElementLength , ROD00000_ReferenceElementLength_PYEL , approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_Tangent)),
                                 blaze::evaluate(blaze::ravel(ROD00000_Tangent_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ElementDilatation , ROD00000_ElementDilatation_PYEL , approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_VoronoiDilatation , ROD00000_VoronoiDilatation_PYEL , approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ReferenceVoronoiLength , ROD00000_ReferenceVoronoiLength_PYEL , approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_ShearStretchStrain)),
                                 blaze::evaluate(blaze::ravel(ROD00000_ShearStretchStrain_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_Curvature)),
                                 blaze::evaluate(blaze::ravel(ROD00000_Curvature_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_ReferenceShearStretchStrain)),
                                 blaze::evaluate(blaze::ravel(ROD00000_ReferenceShearStretchStrain_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_ReferenceCurvature)),
                                 blaze::evaluate(blaze::ravel(ROD00000_ReferenceCurvature_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_InternalStress)),
                                 blaze::evaluate(blaze::ravel(ROD00000_InternalStress_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_InternalCouple)),
                                 blaze::evaluate(blaze::ravel(ROD00000_InternalCouple_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_Position)),
                                 blaze::evaluate(blaze::ravel(ROD00000_Position_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_Director)),
                                 blaze::evaluate(blaze::ravel(ROD00000_Director_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_Velocity)),
                                 blaze::evaluate(blaze::ravel(ROD00000_Velocity_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_AngularVelocity)),
                                 blaze::evaluate(blaze::ravel(ROD00000_AngularVelocity_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_Acceleration)),
                                 blaze::evaluate(blaze::ravel(ROD00000_Acceleration_PYEL)),
                                 approx);
    CHECK_ITERABLE_CUSTOM_APPROX(blaze::evaluate(blaze::ravel(ROD00000_AngularAcceleration)),
                                 blaze::evaluate(blaze::ravel(ROD00000_AngularAcceleration_PYEL)),
                                 approx);
    // clang-format on

  }

}  // namespace
