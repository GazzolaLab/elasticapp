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

  ELASTICA_TEST_CASE(
      "Unit.z.Integrated.CosseratRods.Stepper:SingleRod,ThreeStep",
      "[Systems][Integrated]") {

    /*
     * rng_seed=150
     * NROD=1, N=5, n_step=3, time=0.0, time_step=2e-05
     * tag=150
     * perturb_mag=0.1, perturb_angle_mag=2
     */


    namespace ec = elastica::configuration;
    namespace et = elastica::timestepper;



    Approx approx =
        Approx::custom().epsilon(5.0e-6).scale(0.0);

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


    auto system_config = ec::make_systems_configuration<tmpl::list<Plugin>>(
        ec::make_blocking_policy(ec::RestrictSizeAcrossBlockTypes{8UL}),
        ec::make_iteration_policy(ec::iterate_across_system_types_in(ec::seq),
                                  ec::iterate_over_systems_in(ec::seq)));

    auto simulator_config = ec::make_simulator_configuration(
        ec::DefaultDomainConfiguration{}, system_config,
        ec::DefaultParallelConfiguration{});

    auto simulator = elastica::make_simulator<>(simulator_config);

    auto slicedrod_00000_it = (simulator->emplace_back<Plugin>(
        straight_initializer.initialize<Plugin>()));

    auto& ref = simulator->get_system_collection_container();
    auto& cr = std::get<0UL>(ref);

    simulator->finalize();

    using Algorithm = et::symplectic::PositionVerlet;
    using TimeStepper = et::symplectic::SymplecticStepper<Algorithm>;
    const TimeStepper time_stepper{};

    auto slicedrod_00000 = slicedrod_00000_it.get();
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
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Velocity_REPLACED( 3UL, 6UL );     ROD00000_Velocity_REPLACED = {{0.018197333695852358,0.08766855523127529,0.06310417859392764,0.005886377952147127,0.04651766498643496,0.013806238174047048},{0.03903477571221334,0.008541891267936252,0.025397791978663778,0.016728381899616587,0.03177615374075308,0.06777741149744103},{0.04691392627870822,0.03263778989996056,0.05654957730089996,0.05000081261681551,0.03606695351052915,0.04624814200030012}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Acceleration_REPLACED( 3UL, 6UL );     ROD00000_Acceleration_REPLACED = {{0.025521678254808857,0.08879391958437341,0.010899786996980654,0.01968499323761388,0.06758684467024345,0.03979854625346502},{0.05467244975907565,0.030489744465855475,0.0501465843868291,0.025331464086950153,0.07832008548993236,0.015947804239189612},{0.019968834201570376,0.06961861647008744,0.07038236907405,0.043850613884863304,0.046868672010347126,0.07523022215578842}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularVelocity_REPLACED( 3UL, 5UL );     ROD00000_AngularVelocity_REPLACED = {{0.08770114933878452,0.009577245914128951,0.09875343648612099,0.01932221298301469,0.0726677364112708},{0.001063980046126911,0.0838166367595207,0.014960843780453748,0.07960981813312598,0.044280498457266185},{0.018307202694590942,0.005622099491997146,0.01090490002706256,0.0710350804397759,0.06736100309564207}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularAcceleration_REPLACED( 3UL, 5UL );     ROD00000_AngularAcceleration_REPLACED = {{0.001194391204051315,0.01409282051105949,0.01463816812234734,0.03931543293728981,0.0235708323000189},{0.09073814929269795,0.04078499055234254,0.09258798531315957,0.014499827620907391,0.0493737051779979},{0.05973131578797437,0.08073493167581633,0.09944518992838293,0.06111602004198833,0.02882443602400854}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementDimension_PYEL( 5UL ); ROD00000_ElementDimension_PYEL={0.006049135435256071,0.006457077996705787,0.003824312017210968,0.006041387812000771,0.007442512249046437};
    blaze::DynamicTensor<elastica::real_t> ROD00000_MassSecondMomentOfInertia_PYEL( 3UL, 3UL, 5UL);     ROD00000_MassSecondMomentOfInertia_PYEL = {{{0.0000000007127235497145191,0.0000000007127235497145191,0.000000000712723549714519,0.0000000007127235497145191,0.0000000007127235497145191},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.0000000007127235497145191,0.0000000007127235497145191,0.000000000712723549714519,0.0000000007127235497145191,0.0000000007127235497145191},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{0.0000000014254470994290381,0.0000000014254470994290381,0.000000001425447099429038,0.0000000014254470994290381,0.0000000014254470994290381}}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_InvMassSecondMomentOfInertia_PYEL( 3UL, 3UL, 5UL);     ROD00000_InvMassSecondMomentOfInertia_PYEL = {{{1403068553.579504,1403068553.579504,1403068553.5795043,1403068553.579504,1403068553.579504},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{1403068553.579504,1403068553.579504,1403068553.5795043,1403068553.579504,1403068553.579504},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{701534276.789752,701534276.789752,701534276.7897521,701534276.789752,701534276.789752}}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_ShearStretchRigidityMatrix_PYEL( 3UL, 3UL, 5UL);     ROD00000_ShearStretchRigidityMatrix_PYEL = {{{0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{0.035550566574190964,0.035550566574190964,0.035550566574190964,0.035550566574190964,0.035550566574190964}}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_BendingTwistRigidityMatrix_PYEL( 3UL, 3UL, 4UL);     ROD00000_BendingTwistRigidityMatrix_PYEL = {{{0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868},{0.,0.,0.,0.},{0.,0.,0.,0.}},{{0.,0.,0.,0.},{0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868},{0.,0.,0.,0.}},{{0.,0.,0.,0.},{0.,0.,0.,0.},{0.00000033524471054032456,0.00000033524471054032456,0.00000033524471054032456,0.00000033524471054032456}}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementVolume_PYEL( 5UL ); ROD00000_ElementVolume_PYEL={0.0000063991019833543736,0.0000063991019833543736,0.000006399101983354373,0.000006399101983354375,0.000006399101983354375};
    blaze::DynamicVector<elastica::real_t> ROD00000_Mass_PYEL( 6UL ); ROD00000_Mass_PYEL={0.000025193264508466167,0.00005038652901693233,0.000050386529016932326,0.00005038652901693234,0.00005038652901693235,0.000025193264508466173};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalLoads_PYEL( 3UL, 6UL );     ROD00000_InternalLoads_PYEL = {{0.008367372928849013,0.01502613893591163,-0.001028038755670305,-0.03842455136771553,0.045920592195370924,-0.02987955397043825},{0.021130987757072787,-0.05433070488369067,0.04071974422218373,-0.11263396457242622,0.008238160105201212,0.096860219806219},{-0.053171255656346325,0.014887130886123779,0.0025757175862280495,-0.021413284239009515,0.018178131722640177,0.03893094797759967}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalTorques_PYEL( 3UL, 5UL );     ROD00000_InternalTorques_PYEL = {{-0.10264084691730078,-0.07713920144955821,-0.07078918942641256,-0.09732079270946808,-0.006748876737295306},{0.07122728444849785,0.042973276455718905,0.062374570020455024,-0.0018946378661181685,0.005615201938381759},{0.00036972598862714434,0.00025075124874055085,0.0006709302754779516,0.0009532060026263722,0.00030124784953821705}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementLength_PYEL( 5UL ); ROD00000_ElementLength_PYEL={0.05566504220724718,0.04885365871450789,0.13927158641250753,0.0558079061734978,0.03677308306496261};
    blaze::DynamicVector<elastica::real_t> ROD00000_ReferenceElementLength_PYEL( 5UL ); ROD00000_ReferenceElementLength_PYEL={0.0426999984824372,0.06021312422305085,0.12769661319401673,0.12159671964324305,0.0678179495567241};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Tangent_PYEL( 3UL, 5UL );     ROD00000_Tangent_PYEL = {{-0.4447921454570628,0.92409304562944,-0.35390678875675785,0.2828078626575072,0.7268517288122871},{0.7199952109467045,0.37965497220197963,0.8575550149769741,0.124632690689437,0.2855496439735174},{-0.5326977037243112,-0.04375094400364954,0.37329530021094287,-0.9510449017949917,0.6246182555358458}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementDilatation_PYEL( 5UL ); ROD00000_ElementDilatation_PYEL={1.3036310113721103,0.8113456882512282,1.0906443243009452,0.4589589779825854,0.5422323043577861};
    blaze::DynamicVector<elastica::real_t> ROD00000_VoronoiDilatation_PYEL( 4UL ); ROD00000_VoronoiDilatation_PYEL={1.451648623913265,2.6128506267641036,2.709437397027852,1.2858470727563944};
    blaze::DynamicVector<elastica::real_t> ROD00000_ReferenceVoronoiLength_PYEL( 4UL ); ROD00000_ReferenceVoronoiLength_PYEL={0.036,0.03599999999999999,0.036,0.036000000000000004};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ShearStretchStrain_PYEL( 3UL, 5UL );     ROD00000_ShearStretchStrain_PYEL = {{-1.164743146819455,0.04625288037296351,-0.5498711021429772,-0.3800542185809803,0.30273380776201375},{0.047917125881571467,0.8096464871963677,0.6900479271108446,0.2313226316136845,0.42909039080127265},{-1.5835504817878405,-1.0248004570799782,-0.358922257875234,-1.112658663727491,-0.8649091044928707}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Curvature_PYEL( 3UL, 4UL );     ROD00000_Curvature_PYEL = {{-22.038135018614735,18.9748980643103,-29.950726518557115,7.196177537050079},{-21.406014224649578,9.379108511189099,-14.6171344899947,-3.7368315714244944},{-3.70475923497298,3.2883689484805463,-4.453588333495545,-4.9636435538753165}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ReferenceShearStretchStrain_PYEL( 3UL, 5UL );     ROD00000_ReferenceShearStretchStrain_PYEL = {{0.8330628756074665,0.42173609990468275,0.5729168526354368,0.25708185818524776,0.7142533332135373},{0.8957865325349527,0.1461385058562097,0.19592065162059813,0.6849354450730554,0.3365366346874151},{0.41888599874152754,0.2251580498219029,0.893616157352747,0.42505738060020026,0.785763682498953}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ReferenceCurvature_PYEL( 3UL, 4UL );     ROD00000_ReferenceCurvature_PYEL = {{0.42296459317398183,0.35168182761748235,0.5263809158705643,0.3073262083073959},{0.357345091966899,0.026933120150035905,0.7107949934487903,0.2763092450283908},{0.2914274522809438,0.14947801345388878,0.28186903372731364,0.8952340780705201}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalStress_PYEL( 3UL, 5UL );     ROD00000_InternalStress_PYEL = {{-0.022828865143695403,-0.004290634669324484,-0.012830061836265982,-0.0072805334508953485,-0.004702420377689099},{-0.009688576431680621,0.007581884356009397,0.0056463764796770175,-0.005183419025505032,0.0010576087934197906},{-0.07118775141164795,-0.04443673311459167,-0.04452845031729399,-0.05466667660607317,-0.058682352806158175}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalCouple_PYEL( 3UL, 4UL );     ROD00000_InternalCouple_PYEL = {{-0.000011294947256657257,0.000009365002104899933,-0.00001532593358994162,0.000003464176454489506},{-0.000010944076641726249,0.0000047029009978368554,-0.00000770791092428928,-0.000002018076347053964},{-0.0000013397004492335402,0.0000010522965829306605,-0.0000015875370343506747,-0.0000019641577358128643}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Position_PYEL( 3UL, 6UL );     ROD00000_Position_PYEL = {{0.05378284032773229,0.029020314617708015,0.07416305586811908,0.02487461265588598,0.040658034339934195,0.06738478105592974},{0.02967305407430634,0.0697457142593713,0.08829503973908676,0.20772706995545737,0.21468211569496923,0.22519138471723013},{0.04837047406244307,0.018720126443652008,0.016580777961948658,0.06857113634101658,0.015495716546932822,0.03846784460532944}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_Director_PYEL( 3UL, 3UL, 5UL);     ROD00000_Director_PYEL = {{{0.9081096904052324,0.4147134425379669,0.762009465895544,0.3015877201392746,0.07909844799962709},{-0.4068202553218708,-0.6185816170962585,-0.5394588612252746,-0.45631253341689243,-0.3442977076761475},{-0.09916687981356986,0.6673601303410975,0.3582257820580719,0.8371521480040839,0.9355225940686842}},{{0.35820779806509045,0.8560907586504061,0.32708777538295875,0.9523421850142109,0.9435428334108863},{0.6321035986907106,0.513810977976949,0.7980719829339704,0.10207992157544239,0.32868006242534403},{0.6871187771612789,-0.05573950003133418,0.5060579979121634,-0.2874439984649355,0.04118662505023188}},{{-0.2168500947601163,-0.3084175311678636,-0.5588874314927957,0.045707873540045935,-0.32166808521052676},{-0.6595015696585549,0.5944367602200836,-0.2684497105630205,0.8839448859843068,0.8794478410076527},{0.71974559117818,0.7426463253577512,0.784588804290512,0.465351725943547,0.3508577174638905}}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Velocity_PYEL( 3UL, 6UL );     ROD00000_Velocity_PYEL = {{0.7899545382059097,0.47473856772105183,0.21623644574484213,0.2879064534202248,0.3386084264802428,0.15538007344423704},{0.6916047484751777,0.10124261855641442,0.2803464679523211,0.17863092053130533,0.1342540570487896,1.0070785822745714},{0.09228317524355975,0.3215374478820486,0.12605795673097225,0.2190297923964422,0.2595149998524916,0.5584213683440071}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularVelocity_PYEL( 3UL, 5UL );     ROD00000_AngularVelocity_PYEL = {{23626.500044319953,7688.593794153912,24054.964451089138,-894.7475162425978,6656.67491908016},{40618.41448834325,17385.172345485556,28347.386805974853,9858.939515046222,7349.5531434971945},{4926.397515671285,5168.971975238541,3874.299421560015,6118.847633985053,384.57903049441023}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Acceleration_PYEL( 3UL, 6UL );     ROD00000_Acceleration_PYEL = {{332.1273797620458,298.21738526307524,-20.403047713900552,-762.5957198759019,911.3664523297358,-1186.0135855120027},{838.7554439390633,-1078.278380029569,808.1474357660143,-2235.398364800554,163.49925795509324,3844.6871295170668},{-2110.534569209091,295.45855165218813,51.119170867326126,-424.98034011855833,360.7736448075523,1545.2919158022162}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularAcceleration_PYEL( 3UL, 5UL );     ROD00000_AngularAcceleration_PYEL = {{-187738697.7440083,-87813232.09582587,-108325068.95654856,-62669812.967852056,-5134471.825094154},{130280663.37933816,48919644.3366312,95448608.0904299,-1220053.7768521935,4271984.400827808},{338129.8854777682,142724.30353307904,513345.10922558827,306908.936113798,114593.03940732934}};


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


    std::size_t step = 3;
    while(step--)
      simulator->step(time_stepper, time_v, time_delta);


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
