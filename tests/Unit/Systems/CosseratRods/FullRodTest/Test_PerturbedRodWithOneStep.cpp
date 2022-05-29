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

  template<class BlazeTypeA, class BlazeTypeB>
  void evaluate_relative_difference(BlazeTypeA a_raw, BlazeTypeB b_raw){
    auto a = blaze::evaluate(blaze::ravel(a_raw));
    auto b = blaze::evaluate(blaze::ravel(b_raw));
    std::cout << std::setprecision(std::numeric_limits<double>::max_digits10)
              << blaze::max(blaze::abs((a-b)/b))
              << '\n';
  }

  ELASTICA_TEST_CASE("Unit.z.Integrated.CosseratRods.Stepper:SingleRod,OneStep",
                     "[Systems][Integrated]") {
    /*
     * rng_seed=150
     * NROD=1, N=5, n_step=1, time=0.0, time_step=2e-05
     * tag=150
     * perturb_mag=0.1, perturb_angle_mag=2
     */

    namespace ec = elastica::configuration;
    namespace et = elastica::timestepper;

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
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementDimension_PYEL( 5UL ); ROD00000_ElementDimension_PYEL={0.006048388373046336,0.006456625532588278,0.003824307890629453,0.006041323857364528,0.007443686665948013};
    blaze::DynamicTensor<elastica::real_t> ROD00000_MassSecondMomentOfInertia_PYEL( 3UL, 3UL, 5UL);     ROD00000_MassSecondMomentOfInertia_PYEL = {{{0.0000000007127235497145191,0.0000000007127235497145191,0.000000000712723549714519,0.0000000007127235497145191,0.0000000007127235497145191},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.0000000007127235497145191,0.0000000007127235497145191,0.000000000712723549714519,0.0000000007127235497145191,0.0000000007127235497145191},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{0.0000000014254470994290381,0.0000000014254470994290381,0.000000001425447099429038,0.0000000014254470994290381,0.0000000014254470994290381}}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_InvMassSecondMomentOfInertia_PYEL( 3UL, 3UL, 5UL);     ROD00000_InvMassSecondMomentOfInertia_PYEL = {{{1403068553.579504,1403068553.579504,1403068553.5795043,1403068553.579504,1403068553.579504},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{1403068553.579504,1403068553.579504,1403068553.5795043,1403068553.579504,1403068553.579504},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{701534276.789752,701534276.789752,701534276.7897521,701534276.789752,701534276.789752}}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_ShearStretchRigidityMatrix_PYEL( 3UL, 3UL, 5UL);     ROD00000_ShearStretchRigidityMatrix_PYEL = {{{0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526,0.011426967827418526},{0.,0.,0.,0.,0.}},{{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{0.035550566574190964,0.035550566574190964,0.035550566574190964,0.035550566574190964,0.035550566574190964}}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_BendingTwistRigidityMatrix_PYEL( 3UL, 3UL, 4UL);     ROD00000_BendingTwistRigidityMatrix_PYEL = {{{0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868},{0.,0.,0.,0.},{0.,0.,0.,0.}},{{0.,0.,0.,0.},{0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868,0.0000005028670658104868},{0.,0.,0.,0.}},{{0.,0.,0.,0.},{0.,0.,0.,0.},{0.00000033524471054032456,0.00000033524471054032456,0.00000033524471054032456,0.00000033524471054032456}}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementVolume_PYEL( 5UL ); ROD00000_ElementVolume_PYEL={0.0000063991019833543736,0.0000063991019833543736,0.000006399101983354373,0.000006399101983354375,0.000006399101983354375};
    blaze::DynamicVector<elastica::real_t> ROD00000_Mass_PYEL( 6UL ); ROD00000_Mass_PYEL={0.000025193264508466167,0.00005038652901693233,0.000050386529016932326,0.00005038652901693234,0.00005038652901693235,0.000025193264508466173};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalLoads_PYEL( 3UL, 6UL );     ROD00000_InternalLoads_PYEL = {{-0.012928913551264895,0.02143044028861121,-0.014600174300295006,-0.007724368441451481,0.01503649794586193,-0.0012156906708636172},{-0.013080289237734227,-0.027103708272687967,0.008946104484437707,-0.07471070739955656,-0.0010447282906941133,0.10699195971109268},{-0.01338761851664972,0.00695002731716246,0.004699136588005944,-0.015531319340106639,0.009347255988484428,0.007920282452205704}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalTorques_PYEL( 3UL, 5UL );     ROD00000_InternalTorques_PYEL = {{0.0008420724952909421,-0.0019721019140757456,0.002267964398890227,-0.0015760062396697441,-0.0028729591512297497},{-0.0009248467077364181,-0.000204883976465658,0.0014163665180723605,-0.005743581446160042,0.0023735100798321983},{0.00005213505766110705,0.0002725839849856356,-0.0004025957224855113,0.0010059897922540903,0.0002397919490639605}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementLength_PYEL( 5UL ); ROD00000_ElementLength_PYEL={0.05567879390937243,0.04886050603807675,0.1392718869719189,0.05580908776657741,0.036761480340308884};
    blaze::DynamicVector<elastica::real_t> ROD00000_ReferenceElementLength_PYEL( 5UL ); ROD00000_ReferenceElementLength_PYEL={0.0426999984824372,0.06021312422305085,0.12769661319401673,0.12159671964324305,0.0678179495567241};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Tangent_PYEL( 3UL, 5UL );     ROD00000_Tangent_PYEL = {{-0.4444545995585044,0.9241664038070908,-0.3539319078382349,0.28279699019871773,0.7272218269817627},{0.720212626800396,0.37949487521891745,0.8575583283818048,0.12470977477242053,0.28481421236206894},{-0.532685536811545,-0.043590110764233095,0.37326387186124527,-0.9510380299495915,0.6245232411987063}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementDilatation_PYEL( 5UL ); ROD00000_ElementDilatation_PYEL={1.3039530652974964,0.8114594063759263,1.0906466780001063,0.4589686952930777,0.5420612180195886};
    blaze::DynamicVector<elastica::real_t> ROD00000_VoronoiDilatation_PYEL( 4UL ); ROD00000_VoronoiDilatation_PYEL={1.4519347214923497,2.612949902916607,2.7094579824791154,1.2857023348178651};
    blaze::DynamicVector<elastica::real_t> ROD00000_ReferenceVoronoiLength_PYEL( 4UL ); ROD00000_ReferenceVoronoiLength_PYEL={0.036,0.03599999999999999,0.036,0.036000000000000004};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ShearStretchStrain_PYEL( 3UL, 5UL );     ROD00000_ShearStretchStrain_PYEL = {{-0.6945970546560727,-0.03537182135199204,0.40709881984068264,-0.436496637146817,0.3385300259787157},{-0.5795469866963238,0.7499235529071241,-0.3860137802814115,0.12979528674965526,0.39419863345630984},{-0.0608760367382557,-0.6920554153015517,-0.06470641585132508,-0.9427624899558934,-0.8456133976902526}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Curvature_PYEL( 3UL, 4UL );     ROD00000_Curvature_PYEL = {{-0.00002170108190675127,0.000024771162636942715,-0.000022064228782815196,0.000014818197726483301},{0.0000229868492408391,-0.00001912660954005579,0.000017958057707005898,-0.000009813705174401249},{-0.0000035236499331583556,0.0000014674558774897467,0.00001670281721178342,-0.0000010205701832946277}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ReferenceShearStretchStrain_PYEL( 3UL, 5UL );     ROD00000_ReferenceShearStretchStrain_PYEL = {{0.8330628756074665,0.42173609990468275,0.5729168526354368,0.25708185818524776,0.7142533332135373},{0.8957865325349527,0.1461385058562097,0.19592065162059813,0.6849354450730554,0.3365366346874151},{0.41888599874152754,0.2251580498219029,0.893616157352747,0.42505738060020026,0.785763682498953}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ReferenceCurvature_PYEL( 3UL, 4UL );     ROD00000_ReferenceCurvature_PYEL = {{0.42296459317398183,0.35168182761748235,0.5263809158705643,0.3073262083073959},{0.357345091966899,0.026933120150035905,0.7107949934487903,0.2763092450283908},{0.2914274522809438,0.14947801345388878,0.28186903372731364,0.8952340780705201}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalStress_PYEL( 3UL, 5UL );     ROD00000_InternalStress_PYEL = {{-0.01745652087435789,-0.005223357509858183,-0.0018947973259514858,-0.007925499151948854,-0.004293378143783593},{-0.016858588658967947,0.00689943230732718,-0.006649746031011342,-0.00634356872886952,0.0006589018047968066},{-0.017055812182093403,-0.03260745835461575,-0.03406891043824136,-0.04862677136970568,-0.057996379496875625}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalCouple_PYEL( 3UL, 4UL );     ROD00000_InternalCouple_PYEL = {{-0.00000021270587667050985,-0.0000001768367521510008,-0.0000002647107220364548,-0.0000001545367770345913},{-0.00000017968551854974308,-0.000000013553397244988165,-0.00000035742636223259057,-0.00000013895175429286351},{-0.00000009770069316843115,-0.00000005011122139567081,-0.00000009448950309107287,-0.0000003001228315093415}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Position_PYEL( 3UL, 6UL );     ROD00000_Position_PYEL = {{0.05375145644321651,0.02900156702031743,0.07415437052996803,0.024862543103100786,0.04064514923102552,0.06737772698612805},{0.02964580967889863,0.0697408970058886,0.0882844062941951,0.2077182217175408,0.2146768594025856,0.22515423381760635},{0.048365260598408934,0.018707485062231282,0.016575776710772594,0.06856204798914946,0.015485605523387912,0.03844662228740916}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_Director_PYEL( 3UL, 3UL, 5UL);     ROD00000_Director_PYEL = {{{0.10038277860908605,0.059731866484270646,0.07150197183608205,0.061698452453128876,0.006255581432657199},{-0.33037212386780374,-0.14884444200648278,-0.23210178005162851,-0.09693197941885699,-0.07151859954064366},{0.9384975000125443,0.9870549306953915,0.970059911407753,0.9933768128614796,0.9974196497065236}},{{0.9521777441534682,0.9925562099227864,0.9601868969984432,0.9980124190710667,0.9976424420641261},{0.3054916097178139,0.1139906649019998,0.2793306258586445,0.01878144762343393,0.0686131895701763},{0.005693850264215572,-0.04287538290107145,-0.003940087552204983,-0.06015370815739972,-0.001337164662159853}},{{-0.28858420139975033,-0.10613328540104072,-0.2700529408394828,-0.012826236580010643,-0.06834051136233962},{0.8930448679451077,0.9822685276435252,0.9317205402663845,0.9951137867556206,0.9950765398383946},{0.34523908026087213,0.154545350402406,0.24283377851868826,0.09789810551835568,0.07177920569344462}}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Velocity_PYEL( 3UL, 6UL );     ROD00000_Velocity_PYEL = {{0.7818099321512952,0.4624727313055952,0.2190083559614666,0.3127320894640642,0.30914026952020895,0.1918262164159477},{0.683175513102994,0.1348663573999146,0.25463098273875073,0.2595150954683996,0.12940703753697946,0.8476417007362057},{0.160075494914175,0.3113441404924674,0.12399601424819977,0.23474659591777136,0.24698830072584912,0.5087793636283682}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularVelocity_PYEL( 3UL, 5UL );     ROD00000_AngularVelocity_PYEL = {{30822.999145875696,11076.650270736598,28118.999652328916,1583.930624186479,6858.891558449088},{35053.46989613024,15266.52433286185,24308.60979422981,9763.397461256985,7176.643006924196},{4912.815575319286,5163.206456896688,3861.3298535197678,6106.279105638295,380.19409794388383}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Acceleration_PYEL( 3UL, 6UL );     ROD00000_Acceleration_PYEL = {{38180.62992277214,18740.208803715992,7795.208868376946,15342.285575595852,13131.1302266887,8900.99891209503},{32207.036869539024,6316.223306598917,11461.659538004347,12139.33567843915,4881.544189811319,38993.21446193823},{5658.078431773339,13935.31752962534,3372.32184736499,9237.289165047792,10546.067360765997,23126.5610814034}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularAcceleration_PYEL( 3UL, 5UL );     ROD00000_AngularAcceleration_PYEL = {{1541145572.2363176,553832034.6745342,1405945044.9446213,79195565.09867479,342940944.5356338},{1752673441.6075096,763322025.8112545,1215429741.6693015,488165892.57194257,358829936.3212869},{245639863.40582952,258160041.73985976,193065947.43098703,305310403.5278927,19006336.84703941}};


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


    std::size_t step = 1;
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
