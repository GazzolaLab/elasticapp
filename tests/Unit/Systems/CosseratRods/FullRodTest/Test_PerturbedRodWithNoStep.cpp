//
// Created by Seung Hyun Kim on 11/9/21.
//

#include "Framework/TestingFramework.hpp"
#include "Utilities/MakeArray.hpp"
///
#include "Simulator/Materials.hpp"
#include "Simulator/Materials/Materials.hpp"
#include "Simulator/Simulator.hpp"
#include "Systems/Block.hpp"
#include "Systems/Block/Protocols.hpp"
#include "Systems/CosseratRods.hpp"
#include "Systems/CosseratRods/CosseratRodPlugin.hpp"
#include "Systems/CosseratRods/Initializers/StraightCosseratRod.hpp"  // TODO : replace later
#include "Systems/common/SymplecticStepperAdapter.hpp"
#include "Time/SimulationTime.hpp"
#include "Time/SymplecticSteppers/Algorithms.hpp"
#include "Time/SymplecticSteppers/SymplecticStepper.hpp"
#include "blaze/Blaze.h"
#include "blaze_tensor/Blaze.h"

namespace {

  // TODO: Refactor >-
  template <typename T, typename B>
  using ComponentsWithStepper = ::elastica::SymplecticPolicy<
      T, B, elastica::cosserat_rod::component::WithCircularCosseratRod,
      elastica::cosserat_rod::component::
          WithExplicitlyDampedDiagonalLinearHyperElasticModel,
      elastica::cosserat_rod::component::WithRodKinematics>;

  using Traits = elastica::cosserat_rod::DefaultCosseratRodTraits;
  using Plugin =
      elastica::cosserat_rod::CosseratRodPlugin<Traits, blocks::Block,
                                                ComponentsWithStepper>;
  using Model = blocks::Block<Plugin>;
  // -<

  using real_type = typename Traits::DataType::real_type;

  ELASTICA_TEST_CASE("Unit.z.Integrated.CosseratRods.Stepper:SingleRod,NoStep",
                     "[Systems][Integrated]") {
    /*
     * Threshold set to machine-epsilon: There is no timestep operation, only
     * the floating-point value transfer.
     *
     * rng_seed=150
     * NROD=1, N=5, n_step=0, time=0.0, time_step=2e-05
     * tag=300
     */

    namespace ec = elastica::configuration;
    namespace et = elastica::timestepper;

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

    auto system_config = ec::make_systems_configuration<tmpl::list<Plugin>>(
        ec::make_blocking_policy(ec::RestrictSizeAcrossBlockTypes{8UL}),
        ec::make_iteration_policy(ec::iterate_across_system_types_in(ec::seq),
                                  ec::iterate_over_systems_in(ec::seq)));

    auto simulator_config = ec::make_simulator_configuration(
        ec::DefaultDomainConfiguration{}, system_config,
        ec::DefaultParallelConfiguration{});

    auto simulator = elastica::make_simulator<>(simulator_config);
    auto& ref = simulator->get_system_collection_container();
    auto& cr = std::get<0UL>(ref);

    simulator->finalize();

    using Algorithm = et::symplectic::PositionVerlet;
    using TimeStepper = et::symplectic::SymplecticStepper<Algorithm>;
    const TimeStepper time_stepper{};

    ::elastica::Time time_v{ 0.0 };
    ::elastica::TimeDelta time_delta{ 2e-05 };

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
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementDimension_PYEL( 5UL ); ROD00000_ElementDimension_PYEL={0.007521999999998955,0.007521999999998955,0.007521999999998956,0.007521999999998956,0.007521999999998956};
    blaze::DynamicTensor<elastica::real_t> ROD00000_MassSecondMomentOfInertia_PYEL( 3UL, 3UL, 5UL);     ROD00000_MassSecondMomentOfInertia_PYEL = {{{0.0000000007127235497145191,0.0000000007127235497145191,0.000000000712723549714519,0.0000000007127235497145191,0.0000000007127235497145191},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.0000000007127235497145191,0.0000000007127235497145191,0.000000000712723549714519,0.0000000007127235497145191,0.0000000007127235497145191},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{0.0000000014254470994290381,0.0000000014254470994290381,0.000000001425447099429038,0.0000000014254470994290381,0.0000000014254470994290381}}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_InvMassSecondMomentOfInertia_PYEL( 3UL, 3UL, 5UL);     ROD00000_InvMassSecondMomentOfInertia_PYEL = {{{1403068553.579504,1403068553.579504,1403068553.5795043,1403068553.579504,1403068553.579504},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{1403068553.579504,1403068553.579504,1403068553.5795043,1403068553.579504,1403068553.579504},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{701534276.789752,701534276.789752,701534276.7897521,701534276.789752,701534276.789752}}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_ShearStretchRigidityMatrix_PYEL( 3UL, 3UL, 5UL);     ROD00000_ShearStretchRigidityMatrix_PYEL = {{{0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{0.035550566574190964,0.035550566574190964,0.035550566574190964,0.035550566574190964,0.035550566574190964}}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_BendingTwistRigidityMatrix_PYEL( 3UL, 3UL, 4UL);     ROD00000_BendingTwistRigidityMatrix_PYEL = {{{0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868},{0.,0.,0.,0.},{0.,0.,0.,0.}},{{0.,0.,0.,0.},{0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868},{0.,0.,0.,0.}},{{0.,0.,0.,0.},{0.,0.,0.,0.},{0.00000033524471054032456,0.00000033524471054032456,0.00000033524471054032456,0.00000033524471054032456}}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementVolume_PYEL( 5UL ); ROD00000_ElementVolume_PYEL={0.0000063991019833543736,0.0000063991019833543736,0.000006399101983354373,0.000006399101983354375,0.000006399101983354375};
    blaze::DynamicVector<elastica::real_t> ROD00000_Mass_PYEL( 6UL ); ROD00000_Mass_PYEL={0.000025193264508466167,0.00005038652901693233,0.000050386529016932326,0.00005038652901693234,0.00005038652901693235,0.000025193264508466173};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalLoads_PYEL( 3UL, 6UL );     ROD00000_InternalLoads_PYEL = {{0.004822044056611019,0.9307870177032551,0.7845822109817221,0.3343249592744213,0.6862689176861808,0.7322625609228085},{0.8246472414522372,0.10745443906835128,0.3089916869203323,0.31294344042724076,0.5940727124966699,0.6105506387346894},{0.9165420505570774,0.738817174285074,0.37747080022423485,0.7949533183809576,0.48693548220111915,0.5788914075188061}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalTorques_PYEL( 3UL, 5UL );     ROD00000_InternalTorques_PYEL = {{0.9744748566188305,0.688389675725899,0.5848087428374977,0.2739643248027279,0.8377858251400324},{0.30222935408131235,0.1728462163878618,0.801743366769752,0.3441099204558331,0.4375847094010139},{0.15268115598708532,0.2761357842472395,0.5638672039037047,0.86467466140162,0.08278860289302525}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementLength_PYEL( 5UL ); ROD00000_ElementLength_PYEL={0.036000000000009996,0.036000000000009996,0.03600000000000999,0.03600000000001,0.03600000000001};
    blaze::DynamicVector<elastica::real_t> ROD00000_ReferenceElementLength_PYEL( 5UL ); ROD00000_ReferenceElementLength_PYEL={0.0426999984824372,0.06021312422305085,0.12769661319401673,0.12159671964324305,0.0678179495567241};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Tangent_PYEL( 3UL, 5UL );     ROD00000_Tangent_PYEL = {{0.,0.,0.,0.,0.},{0.9999999999997222,0.9999999999997222,0.9999999999997222,0.9999999999997222,0.9999999999997222},{0.,0.,0.,0.,0.}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementDilatation_PYEL( 5UL ); ROD00000_ElementDilatation_PYEL={1.0000000000002778,1.0000000000002778,1.0000000000002778,1.0000000000002778,1.0000000000002778};
    blaze::DynamicVector<elastica::real_t> ROD00000_VoronoiDilatation_PYEL( 4UL ); ROD00000_VoronoiDilatation_PYEL={1.0000000000002778,1.000000000000278,1.0000000000002778,1.0000000000002778};
    blaze::DynamicVector<elastica::real_t> ROD00000_ReferenceVoronoiLength_PYEL( 4UL ); ROD00000_ReferenceVoronoiLength_PYEL={0.036,0.03599999999999999,0.036,0.036000000000000004};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ShearStretchStrain_PYEL( 3UL, 5UL );     ROD00000_ShearStretchStrain_PYEL = {{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Curvature_PYEL( 3UL, 4UL );     ROD00000_Curvature_PYEL = {{-0.,-0.,-0.,-0.},{-0.,-0.,-0.,-0.},{-0.,-0.,-0.,-0.}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ReferenceShearStretchStrain_PYEL( 3UL, 5UL );     ROD00000_ReferenceShearStretchStrain_PYEL = {{0.8330628756074665,0.42173609990468275,0.5729168526354368,0.25708185818524776,0.7142533332135373},{0.8957865325349527,0.1461385058562097,0.19592065162059813,0.6849354450730554,0.3365366346874151},{0.41888599874152754,0.2251580498219029,0.893616157352747,0.42505738060020026,0.785763682498953}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ReferenceCurvature_PYEL( 3UL, 4UL );     ROD00000_ReferenceCurvature_PYEL = {{0.42296459317398183,0.35168182761748235,0.5263809158705643,0.3073262083073959},{0.357345091966899,0.026933120150035905,0.7107949934487903,0.2763092450283908},{0.2914274522809438,0.14947801345388878,0.28186903372731364,0.8952340780705201}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalStress_PYEL( 3UL, 5UL );     ROD00000_InternalStress_PYEL = {{0.4375196365597337,0.7683879888762261,0.16663239947009578,0.8244444091919032,0.8696022625042859},{0.44593293278653,0.16041064218431744,0.41984957138538315,0.4522866548422185,0.8544544784625221},{0.7439885668390492,0.02976576715291579,0.18617391272004868,0.7383720961472324,0.8817988526289666}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalCouple_PYEL( 3UL, 4UL );     ROD00000_InternalCouple_PYEL = {{0.6536435340498628,0.2525396458442313,0.6183595148406859,0.19936517150220823},{0.8575809295973004,0.021480917433798763,0.9080569690386844,0.25119605094289243},{0.908052828476898,0.7615298850236448,0.7495712349893845,0.46584477600408136}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Position_PYEL( 3UL, 6UL );     ROD00000_Position_PYEL = {{0.05374345637055803,0.028996065607452062,0.07415154940462247,0.024859356918426626,0.04064159265168045,0.06737567066158216},{0.02963858757601048,0.06973946292340193,0.08828160600644791,0.20771545928276713,0.21467524757067283,0.22514507962648403},{0.04836319070419701,0.018704045242927358,0.0165739712548571,0.06855920051506412,0.015482774970845548,0.038441072012352875}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_Director_PYEL( 3UL, 3UL, 5UL);     ROD00000_Director_PYEL = {{{0.027989491107628155,0.03323424292011635,0.00656565551820349,0.014575406270250519,0.014417391356387824},{-0.005516804929133914,-0.004039109173336363,-0.028713681937887404,-0.0073883408736775845,-0.020190406250985567},{0.9995929937984309,0.99943942822695,0.9995661141901452,0.9998664760612747,0.9996921957892324}},{{0.999407638620116,0.9993996617357986,0.9998048854052993,0.9994198020920843,0.9995982434141826},{0.020185977126543624,0.009927444148141009,0.01881138633619778,0.030893462155792993,0.024691273023532488},{-0.027872893556475445,-0.03319280004845875,-0.006026845261556988,-0.01434061303598674,-0.01391735606205962}},{{-0.020023991992106797,-0.00978780976000858,-0.018630171424673592,-0.030783383801597716,-0.024402675872894034},{0.9997810216196448,0.999942564075443,0.9994106544427472,0.9994953758848327,0.9994912148347767},{0.006078532214235719,0.0043666154319415285,0.02883158856197704,0.007834338935775,0.02053827836857159}}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Velocity_PYEL( 3UL, 6UL );     ROD00000_Velocity_PYEL = {{0.05000081261681551,0.03606695351052915,0.04624814200030012,0.025521678254808857,0.08879391958437341,0.010899786996980654},{0.01968499323761388,0.06758684467024345,0.03979854625346502,0.05467244975907565,0.030489744465855475,0.0501465843868291},{0.025331464086950153,0.07832008548993236,0.015947804239189612,0.019968834201570376,0.06961861647008744,0.07038236907405}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularVelocity_PYEL( 3UL, 5UL );     ROD00000_AngularVelocity_PYEL = {{0.001194391204051315,0.01409282051105949,0.01463816812234734,0.03931543293728981,0.0235708323000189},{0.09073814929269795,0.04078499055234254,0.09258798531315957,0.014499827620907391,0.0493737051779979},{0.05973131578797437,0.08073493167581633,0.09944518992838293,0.06111602004198833,0.02882443602400854}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Acceleration_PYEL( 3UL, 6UL );     ROD00000_Acceleration_PYEL = {{0.043850613884863304,0.046868672010347126,0.07523022215578842,0.08770114933878452,0.009577245914128951,0.09875343648612099},{0.01932221298301469,0.0726677364112708,0.001063980046126911,0.0838166367595207,0.014960843780453748,0.07960981813312598},{0.044280498457266185,0.018307202694590942,0.005622099491997146,0.01090490002706256,0.0710350804397759,0.06736100309564207}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularAcceleration_PYEL( 3UL, 5UL );     ROD00000_AngularAcceleration_PYEL = {{0.08926565941377294,0.08584207275989887,0.08885434917188945,0.04558153086157374,0.06831109257680112},{0.07905526092391878,0.0004386444369899212,0.0732637876081235,0.0638572284293879,0.06747098268735854},{0.020611557335144727,0.04025138027428276,0.004734782466892584,0.040794854777998435,0.09965710897628582}};


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
