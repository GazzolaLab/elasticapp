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

  ELASTICA_TEST_CASE("Unit.z.Integrated.CosseratRods.Stepper:SingleRod,TwoStep",
                     "[Systems][Integrated]") {

    /*
     * rng_seed=150
     * NROD=1, N=5, n_step=2, time=0.0, time_step=2e-05
     * tag=150
     * perturb_mag=0.1, perturb_angle_mag=2
     */


    namespace ec = elastica::configuration;
    namespace et = elastica::timestepper;

    Approx approx =
        Approx::custom() //.epsilon(std::numeric_limits<double>::epsilon() * 100)
            .epsilon(6.0e-9)
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
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementDimension_PYEL( 5UL ); ROD00000_ElementDimension_PYEL={0.006048750676346524,0.00645685199814755,0.003824304497410618,0.006041355121147986,0.00744311392810424};
    blaze::DynamicTensor<elastica::real_t> ROD00000_MassSecondMomentOfInertia_PYEL( 3UL, 3UL, 5UL);     ROD00000_MassSecondMomentOfInertia_PYEL = {{{0.0000000007127235497145191,0.0000000007127235497145191,0.000000000712723549714519,0.0000000007127235497145191,0.0000000007127235497145191},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.0000000007127235497145191,0.0000000007127235497145191,0.000000000712723549714519,0.0000000007127235497145191,0.0000000007127235497145191},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{0.0000000014254470994290381,0.0000000014254470994290381,0.000000001425447099429038,0.0000000014254470994290381,0.0000000014254470994290381}}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_InvMassSecondMomentOfInertia_PYEL( 3UL, 3UL, 5UL);     ROD00000_InvMassSecondMomentOfInertia_PYEL = {{{1403068553.579504,1403068553.579504,1403068553.5795043,1403068553.579504,1403068553.579504},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{1403068553.579504,1403068553.579504,1403068553.5795043,1403068553.579504,1403068553.579504},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{701534276.789752,701534276.789752,701534276.7897521,701534276.789752,701534276.789752}}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_ShearStretchRigidityMatrix_PYEL( 3UL, 3UL, 5UL);     ROD00000_ShearStretchRigidityMatrix_PYEL = {{{0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{0.035550566574190964,0.035550566574190964,0.035550566574190964,0.035550566574190964,0.035550566574190964}}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_BendingTwistRigidityMatrix_PYEL( 3UL, 3UL, 4UL);     ROD00000_BendingTwistRigidityMatrix_PYEL = {{{0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868},{0.,0.,0.,0.},{0.,0.,0.,0.}},{{0.,0.,0.,0.},{0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868},{0.,0.,0.,0.}},{{0.,0.,0.,0.},{0.,0.,0.,0.},{0.00000033524471054032456,0.00000033524471054032456,0.00000033524471054032456,0.00000033524471054032456}}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementVolume_PYEL( 5UL ); ROD00000_ElementVolume_PYEL={0.0000063991019833543736,0.0000063991019833543736,0.000006399101983354373,0.000006399101983354375,0.000006399101983354375};
    blaze::DynamicVector<elastica::real_t> ROD00000_Mass_PYEL( 6UL ); ROD00000_Mass_PYEL={0.000025193264508466167,0.00005038652901693233,0.000050386529016932326,0.00005038652901693234,0.00005038652901693235,0.000025193264508466173};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalLoads_PYEL( 3UL, 6UL );     ROD00000_InternalLoads_PYEL = {{0.001892087803708688,0.015875507187305966,-0.005955307972443644,-0.024119330176620275,0.028319315091742143,-0.016030312039545826},{-0.010512989940295554,-0.030378469761098148,0.02406595787271911,-0.09113967680150041,0.0039730643586716905,0.10397655664383265},{-0.032224236398868865,0.010793138042058273,0.002618988650744983,-0.018182474595034304,0.013380712728907413,0.023601259798981238}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalTorques_PYEL( 3UL, 5UL );     ROD00000_InternalTorques_PYEL = {{-0.09407134777022401,-0.07166705114798344,-0.062000585008050536,-0.09513680335516357,-0.0065421084790653295},{0.08088626323782241,0.05007913429454338,0.06958987771943452,0.009312963885937406,0.0057495955546263125},{0.0003727809316216659,0.00025570062827971247,0.00017661541753249757,0.000998566806838058,0.00027516301331706054}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementLength_PYEL( 5UL ); ROD00000_ElementLength_PYEL={0.05567212410011044,0.04885707866269926,0.13927213411761555,0.05580851014824999,0.03676713805326419};
    blaze::DynamicVector<elastica::real_t> ROD00000_ReferenceElementLength_PYEL( 5UL ); ROD00000_ReferenceElementLength_PYEL={0.0426999984824372,0.06021312422305085,0.12769661319401673,0.12159671964324305,0.0678179495567241};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Tangent_PYEL( 3UL, 5UL );     ROD00000_Tangent_PYEL = {{-0.44462256820659796,0.9241315711405387,-0.35391782068842265,0.2827986299544711,0.7270461077456171},{0.7201019340189576,0.37957052370219063,0.8575575079765125,0.12466443891771983,0.28516107907185684},{-0.5326950126133779,-0.04366986097247048,0.37327911368321626,-0.951043486158675,0.624569544721786}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementDilatation_PYEL( 5UL ); ROD00000_ElementDilatation_PYEL={1.3037968636698842,0.8114024856394305,1.0906486134131959,0.4589639450142123,0.5421446430271608};
    blaze::DynamicVector<elastica::real_t> ROD00000_VoronoiDilatation_PYEL( 4UL ); ROD00000_VoronoiDilatation_PYEL={1.4517944828168015,2.6129057330599283,2.709453392581466,1.2857728916876967};
    blaze::DynamicVector<elastica::real_t> ROD00000_ReferenceVoronoiLength_PYEL( 4UL ); ROD00000_ReferenceVoronoiLength_PYEL={0.036,0.03599999999999999,0.036,0.036000000000000004};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ShearStretchStrain_PYEL( 3UL, 5UL );     ROD00000_ShearStretchStrain_PYEL = {{-1.2375020162688504,-0.021285360814104072,-0.11159076441039986,-0.41982334424834855,0.32000231540384694},{-0.028058129293276862,0.8014083568680952,0.19299589144289353,0.18272598970395706,0.412373258408092},{-0.5905030368538652,-0.8748378145821596,0.06762094636833593,-1.0317407487904005,-0.8534816467163326}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Curvature_PYEL( 3UL, 4UL );     ROD00000_Curvature_PYEL = {{-11.446373027962057,9.696242972616588,-15.170813323452201,3.1303889304043033},{-10.471722616825828,4.5714125918797315,-7.279368186528146,-1.6619217394923573},{-0.48212308116986485,0.2667751886828608,-0.21164569209940573,-2.8541131356151968}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ReferenceShearStretchStrain_PYEL( 3UL, 5UL );     ROD00000_ReferenceShearStretchStrain_PYEL = {{0.8330628756074665,0.42173609990468275,0.5729168526354368,0.25708185818524776,0.7142533332135373},{0.8957865325349527,0.1461385058562097,0.19592065162059813,0.6849354450730554,0.3365366346874151},{0.41888599874152754,0.2251580498219029,0.893616157352747,0.42505738060020026,0.785763682498953}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ReferenceCurvature_PYEL( 3UL, 4UL );     ROD00000_ReferenceCurvature_PYEL = {{0.42296459317398183,0.35168182761748235,0.5263809158705643,0.3073262083073959},{0.357345091966899,0.026933120150035905,0.7107949934487903,0.2763092450283908},{0.2914274522809438,0.14947801345388878,0.28186903372731364,0.8952340780705201}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalStress_PYEL( 3UL, 5UL );     ROD00000_InternalStress_PYEL = {{-0.023660278404052992,-0.005062391978489537,-0.007821846517605697,-0.007734973970420929,-0.00450509369643834},{-0.010556743228243527,0.007487747505790146,-0.00003342114045354535,-0.0057387312891280665,0.0008665826593962196},{-0.03588435210919242,-0.039105476208831355,-0.029364597738064217,-0.051789998884057414,-0.05827610020769959}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalCouple_PYEL( 3UL, 4UL );     ROD00000_InternalCouple_PYEL = {{-0.000005968698982654703,0.000004699072044272383,-0.000007893602008585516,0.0000014196252676598376},{-0.000005445581504183689,0.000002285269057584703,-0.00000401798991366199,-0.0000009746725280489198},{-0.00000025932872467503,0.00000003932325755683446,-0.00000016544820140716617,-0.0000012569488213672406}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Position_PYEL( 3UL, 6UL );     ROD00000_Position_PYEL = {{0.05376710766244413,0.029010879489830645,0.07415872705859464,0.0248687020076736,0.040651444444695053,0.06738143625174375},{0.029659389730425624,0.06974347375132417,0.08828959443921486,0.20772305025737378,0.21467946331367985,0.22517201208301055},{0.04836820629202435,0.018713754786404697,0.016578267026648212,0.06856667074910062,0.015490598401664734,0.03845698523634572}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_Director_PYEL( 3UL, 3UL, 5UL);     ROD00000_Director_PYEL = {{{0.5242656664701675,0.22155363846743298,0.37953710951016995,0.18587611963949757,0.03335800249996833},{-0.6917550299844296,-0.41772466785203155,-0.5549137718762996,-0.28423298053743573,-0.2118081128100405},{0.4966089905064455,0.8811470292463978,0.7402852749357395,0.9405645543622025,0.9767418118505325}},{{0.6530929787572574,0.9394083804273672,0.6872594230584147,0.9825427539126551,0.9791226059664206},{0.7008638917452351,0.3338201039975072,0.7047811698038774,0.06130150886713448,0.202995013517486},{0.28680893700457677,-0.07794891243579961,0.175948822409509,-0.1756469804589539,0.010580499636648517}},{{-0.5464568345794233,-0.2615834093834555,-0.6193755467582491,-0.007733361591097847,-0.2005147529603184},{0.17396776637918443,0.8450267688274753,0.44198892347195284,0.956793466629773,0.9559970438421266},{0.8192192284135382,0.46637311232566153,0.6488603267314343,0.2906655420463234,0.21415761954779383}}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Velocity_PYEL( 3UL, 6UL );     ROD00000_Velocity_PYEL = {{0.7833119906106687,0.46877422001579033,0.21664450669912014,0.30315836781774286,0.32038109743364807,0.1791003451544771},{0.6748296395963964,0.1228081861570058,0.2641835192370008,0.22333888782731642,0.13098407188968772,0.93018483968423},{0.13449386662774157,0.3156282768490048,0.12503557331362572,0.22752939919881338,0.25229952695634056,0.5275155300279628}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularVelocity_PYEL( 3UL, 5UL );     ROD00000_AngularVelocity_PYEL = {{27381.27399920012,9444.85843607043,26221.46583022011,358.6487431144433,6759.364355582044},{38012.80122075649,16406.77945875293,26438.414644166256,9883.340590583266,7264.113455480639},{4919.63491796173,5166.117489167879,3864.0325193755034,6112.709455262777,382.2871697062636}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Acceleration_PYEL( 3UL, 6UL );     ROD00000_Acceleration_PYEL = {{75.10292296867101,315.0744355097574,-118.1924631173219,-478.686082316069,562.0413956719556,-636.2935630735292},{-417.2936753298754,-602.9085621454396,477.6268249125035,-1808.81038205416,78.85171763541295,4127.156947401216},{-1279.0814143216712,214.20681782686898,51.97795327129748,-360.8598359478999,265.5613115245711,936.8083199797334}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularAcceleration_PYEL( 3UL, 5UL );     ROD00000_AngularAcceleration_PYEL = {{-172086257.3337788,-81589591.73330842,-94876691.10544024,-61264094.05360178,-4976360.143352203},{147966566.23131248,57012756.29455405,106490242.49682231,5997156.466314035,4373522.4278221065},{340967.13212222664,145551.61355958224,135133.29278678045,321517.48122411146,104653.5881189901}};


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


    std::size_t step = 2;
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
