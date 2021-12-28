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

  auto approx(Catch::Detail::Approx(1e-8));

  ELASTICA_TEST_CASE(
      "Unit.z.Integrated.CosseratRods.Stepper:SingleRod,FiveStep",
      "[Systems][Integrated]") {
    /*
     * rng_seed=300
     * NROD=1, N=5, n_step=5, time=0.0, time_step=2e-05
     * tag=300
     */

    namespace ec = elastica::configuration;
    namespace et = elastica::timestepper;

    Approx approx = Approx::custom()
                        .epsilon(2e-6)
                        .scale(0.0);

    elastica::MaterialID myMaterial = elastica::create_material(
        7.874, 0.24, 200, 200.0 / 1.24, 200, 1e-10, 1e-10, 0.1, 0.1);

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
        Init::ForceDampingCoefficient{0.2},
        Init::TorqueDampingCoefficient{0.2},
    };

    auto system_config = ec::make_systems_configuration<tmpl::list<Plugin>>(
        ec::make_blocking_policy(ec::RestrictSizeAcrossBlockTypes{8UL}),
        ec::make_iteration_policy(ec::iterate_across_system_types_in(ec::seq),
                                  ec::iterate_over_systems_in(ec::seq)));

    auto simulator_config = ec::make_simulator_configuration(
        ec::DefaultDomainConfiguration{}, system_config,
        ec::DefaultParallelConfiguration{});

    auto simulator = elastica::make_simulator<>(simulator_config);

    auto slicedrod_00000 = *(simulator->emplace_back<Plugin>(
        straight_initializer.initialize<Plugin>()));

    simulator->finalize();

    using Algorithm = et::symplectic::PositionVerlet;
    using TimeStepper = et::symplectic::SymplecticStepper<Algorithm>;
    const TimeStepper time_stepper{};

    ::elastica::Time time_v{0.0};
    ::elastica::TimeDelta time_delta{2e-05};

    //**************************************************************************
    // clang-format off
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalLoads_REPLACED(3UL,
                                                                           6UL);
    ROD00000_InternalLoads_REPLACED = {
        {0.6703700886687535, 0.5599270904792274, 0.37978721939595683,
         0.2517851088005173, 0.7532804498119788, 0.831240507511035},
        {0.47781402266344997, 0.6950211678952081, 0.5153977473752267,
         0.5560888125838666, 0.2995297923411575, 0.4569164183770765},
        {0.9061628803336391, 0.9266585640581266, 0.35913849984522517,
         0.4168057452312872, 0.5179665933465903, 0.10411282840829195}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalTorques_REPLACED(
        3UL, 5UL);
    ROD00000_InternalTorques_REPLACED = {
        {0.005446580181096006, 0.4128528364721107, 0.7959898243412399,
         0.6432890641383698, 0.8782875840719243},
        {0.10076240092266242, 0.49634830812601183, 0.32439486332849543,
         0.3715252187945519, 0.9236619681106076},
        {0.8906049364593669, 0.8929947521390232, 0.8007821567530773,
         0.9908026910504705, 0.9343406228935561}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ExternalLoads_REPLACED(3UL,
                                                                           6UL);
    ROD00000_ExternalLoads_REPLACED = {
        {0.1395213064814601, 0.8474314503230967, 0.2696127183057031,
         0.009970972559999569, 0.08155526350714615, 0.44619971524211255},
        {0.12251946914166034, 0.2611447683110364, 0.02586569069785072,
         0.030177485569313767, 0.4352966485887426, 0.9518404933230397},
        {0.07133267601259985, 0.018172074014147555, 0.5608764707571324,
         0.5370081638237247, 0.6906076351385936, 0.13266926049186845}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ExternalTorques_REPLACED(
        3UL, 5UL);
    ROD00000_ExternalTorques_REPLACED = {
        {0.6018185132266004, 0.003938448160778485, 0.08946545496330316,
         0.38388587015468356, 0.3611628648311712},
        {0.17674353197593184, 0.18411277805131043, 0.38936582363725436,
         0.32013002586294814, 0.04595243267756288},
        {0.09208604065501258, 0.21934003097954202, 0.412093588441637,
         0.39803407751858466, 0.08520800443850063}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalStress_REPLACED(
        3UL, 5UL);
    ROD00000_InternalStress_REPLACED = {
        {0.48845156990161287, 0.42703068123353516, 0.2111597780763791,
         0.7527081614401365, 0.7585091573224211},
        {0.859799916302963, 0.09980576332201718, 0.5912967371146306,
         0.3314992134943602, 0.1323825532380214},
        {0.5542949860767826, 0.6344052832194877, 0.3019910205525258,
         0.6556067934852766, 0.10056167410384631}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalCouple_REPLACED(
        3UL, 4UL);
    ROD00000_InternalCouple_REPLACED = {
        {0.42196851675485547, 0.20095521733492605, 0.20438860427627925,
         0.592736970599767},
        {0.023067352983699885, 0.8262852908468907, 0.609368847923943,
         0.7967177574305179},
        {0.12780115215826082, 0.1801123210891723, 0.01010111051474083,
         0.8292446512357197}};
    blaze::DynamicMatrix<elastica::real_t>
        ROD00000_ReferenceShearStretchStrain_REPLACED(3UL, 5UL);
    ROD00000_ReferenceShearStretchStrain_REPLACED = {
        {0.297597441176643, 0.791076063623359, 0.9813316766668237,
         0.11729731016539668, 0.20564794237709072},
        {0.9357375659253002, 0.4402876689437507, 0.9156671113598859,
         0.9679292352235765, 0.9987855559307337},
        {0.3040655442953162, 0.10165528375219623, 0.5054634070571163,
         0.9126824887816438, 0.11534211862120414}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ReferenceCurvature_REPLACED(
        3UL, 4UL);
    ROD00000_ReferenceCurvature_REPLACED = {
        {0.051148978603062, 0.4646769716719006, 0.9301184173210795,
         0.7944799966795505},
        {0.6036777904356718, 0.19098869154676612, 0.5763017409174203,
         0.5082335545968735},
        {0.30308030515914375, 0.6131410921095497, 0.2555432491263159,
         0.9931735829341407}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ShearStretchStrain_REPLACED(
        3UL, 5UL);
    ROD00000_ShearStretchStrain_REPLACED = {
        {0.9848603196130776, 0.7628387920573857, 0.3981263185067315,
         0.7752103618012558, 0.4981087231895296},
        {0.44372987089924676, 0.47981333159408723, 0.27953052942940315,
         0.039410528091022745, 0.12135498080172424},
        {0.04502860003414588, 0.39606048704701324, 0.638802231688118,
         0.34039507614487385, 0.3222690451538498}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Curvature_REPLACED(3UL,
                                                                       4UL);
    ROD00000_Curvature_REPLACED = {{0.46030989590247284, 0.6483169447319955,
                                    0.25693207884047353, 0.7147848976761505},
                                   {0.5678591352687159, 0.6481755476704774,
                                    0.7111537582140289, 0.6592641079581886},
                                   {0.4768374539110407, 0.10965068835379443,
                                    0.8176421954155958, 0.003583635388100248}};
    blaze::DynamicVector<elastica::real_t>
        ROD00000_ReferenceElementLength_REPLACED(5UL);
    ROD00000_ReferenceElementLength_REPLACED = {
        0.07451700459404914, 0.12877804874902377, 0.12125445792190077,
        0.12557378285646317, 0.06303696469708653};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Position_REPLACED(3UL, 6UL);
    ROD00000_Position_REPLACED = {
        {0.08894214952049244, 0.05762523708273925, 0.07871356476107469,
         0.04363652299969875, 0.08833475012599382, 0.04032049847902628},
        {0.02957220489754161, 0.12892832447280386, 0.1452771836398962,
         0.13722209882380804, 0.1481773167166555, 0.21999128268067816},
        {0.07488558762412101, 0.07839570688697475, 0.08821991684367064,
         0.0886365573296124, 0.012825841069887711, 0.06064545654195839}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_Director_REPLACED(3UL, 3UL,
                                                                      5UL);
    ROD00000_Director_REPLACED = {
        {{0.035527567491191736, 0.02952437215300531, 0.02891160172815559,
          0.03313333391249655, 0.04453392461560597},
         {-0.03022372573360162, -0.0023832872802660252, -0.035840121657047155,
          -0.028279701204238436, -0.014803530160030354},
         {0.9989115668320888, 0.9995612194311614, 0.9989392398765404,
          0.9990507698228563, 0.998898185529002}},
        {{0.9990993054856918, 0.9986340960690273, 0.9992850016191498,
          0.9990432334336197, 0.9987469325612468},
         {0.024280996943764874, 0.043196474014278764, 0.025394843880789095,
          0.02948607942900067, 0.023511776272677427},
         {-0.03479958283406501, -0.029393992602746118, -0.028010488094392143,
          -0.03229843417883682, -0.044178740090724214}},
        {{-0.0232027956541208, -0.04310746591215261, -0.02436400674208259,
          -0.028544700284658744, -0.022831869345976148},
         {0.9992481971913668, 0.9990637540089587, 0.9990348280134573,
          0.9991650662526715, 0.9996139514188987},
         {0.031059148147304438, 0.0036553807132964624, 0.03654870164032295,
          0.02922961624711047, 0.01583205201051463}}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Velocity_REPLACED(3UL, 6UL);
    ROD00000_Velocity_REPLACED = {
        {0.07925380682706683, 0.0860414593881338, 0.0055840582078969915,
         0.08229282255627174, 0.0071962759065840156, 0.06633851975206956},
        {0.06756620000198289, 0.08778845270232372, 0.07191395945987576,
         0.024745306610474773, 0.05689760350195482, 0.08343727453787116},
        {0.045264278461788744, 0.0030062857233150277, 0.010200463945095085,
         0.08437374377569423, 0.07508586841387065, 0.009201572172750961}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Acceleration_REPLACED(3UL,
                                                                          6UL);
    ROD00000_Acceleration_REPLACED = {
        {0.09571408426343082, 0.005107180633195741, 0.0732126259144127,
         0.050441490491986186, 0.05486230099210937, 0.0440326013977175},
        {0.09941018300218248, 0.013618114920622882, 0.051835720941648344,
         0.05022501039227099, 0.09049134977350463, 0.06427073323568147},
        {0.014063593772195671, 0.014344551773854353, 0.027437996786544197,
         0.059461465451936736, 0.07456314681696873, 0.059811903960076684}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularVelocity_REPLACED(
        3UL, 5UL);
    ROD00000_AngularVelocity_REPLACED = {
        {0.09851356354829398, 0.04033172864324111, 0.020292569398976515,
         0.06902762055710024, 0.026847114751602664},
        {0.03866628570001665, 0.08212084544018176, 0.09706746517912329,
         0.021648310689029695, 0.07631296141009615},
        {0.004526510466830236, 0.03302764459795905, 0.005283833550639883,
         0.046620199055542225, 0.07837425516095967}};
    blaze::DynamicMatrix<elastica::real_t>
        ROD00000_AngularAcceleration_REPLACED(3UL, 5UL);
    ROD00000_AngularAcceleration_REPLACED = {
        {0.057039050557071806, 0.07502520173913291, 0.08204218079523058,
         0.023910778516111564, 0.07949498033136443},
        {0.03912707550921284, 0.06589885029187668, 0.040550375265082574,
         0.08510826947147253, 0.03005671672516207},
        {0.09295270631990539, 0.07146242008590545, 0.04433061390912978,
         0.05706921605595977, 0.0643979182090123}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementDimension_PYEL(5UL);
    ROD00000_ElementDimension_PYEL = {
        0.004420986727571907, 0.008465696794771986, 0.0075248939643957945,
        0.004792705630517224, 0.004541929250732981};
    blaze::DynamicTensor<elastica::real_t>
        ROD00000_MassSecondMomentOfInertia_PYEL(3UL, 3UL, 5UL);
    ROD00000_MassSecondMomentOfInertia_PYEL = {
        {{0.0000000007127235497145191, 0.0000000007127235497145191,
          0.000000000712723549714519, 0.0000000007127235497145191,
          0.0000000007127235497145191},
         {0., 0., 0., 0., 0.},
         {0., 0., 0., 0., 0.}},
        {{0., 0., 0., 0., 0.},
         {0.0000000007127235497145191, 0.0000000007127235497145191,
          0.000000000712723549714519, 0.0000000007127235497145191,
          0.0000000007127235497145191},
         {0., 0., 0., 0., 0.}},
        {{0., 0., 0., 0., 0.},
         {0., 0., 0., 0., 0.},
         {0.0000000014254470994290381, 0.0000000014254470994290381,
          0.000000001425447099429038, 0.0000000014254470994290381,
          0.0000000014254470994290381}}};
    blaze::DynamicTensor<elastica::real_t>
        ROD00000_InvMassSecondMomentOfInertia_PYEL(3UL, 3UL, 5UL);
    ROD00000_InvMassSecondMomentOfInertia_PYEL = {
        {{1403068553.579504, 1403068553.579504, 1403068553.5795043,
          1403068553.579504, 1403068553.579504},
         {0., 0., 0., 0., 0.},
         {0., 0., 0., 0., 0.}},
        {{0., 0., 0., 0., 0.},
         {1403068553.579504, 1403068553.579504, 1403068553.5795043,
          1403068553.579504, 1403068553.579504},
         {0., 0., 0., 0., 0.}},
        {{0., 0., 0., 0., 0.},
         {0., 0., 0., 0., 0.},
         {701534276.789752, 701534276.789752, 701534276.7897521,
          701534276.789752, 701534276.789752}}};
    blaze::DynamicTensor<elastica::real_t>
        ROD00000_ShearStretchRigidityMatrix_PYEL(3UL, 3UL, 5UL);
    ROD00000_ShearStretchRigidityMatrix_PYEL = {
        {{0.03822641567117308, 0.03822641567117308, 0.03822641567117308,
          0.03822641567117308, 0.03822641567117308},
         {0., 0., 0., 0., 0.},
         {0., 0., 0., 0., 0.}},
        {{0., 0., 0., 0., 0.},
         {0.03822641567117308, 0.03822641567117308, 0.03822641567117308,
          0.03822641567117308, 0.03822641567117308},
         {0., 0., 0., 0., 0.}},
        {{0., 0., 0., 0., 0.},
         {0., 0., 0., 0., 0.},
         {0.035550566574190964, 0.035550566574190964, 0.035550566574190964,
          0.035550566574190964, 0.035550566574190964}}};
    blaze::DynamicTensor<elastica::real_t>
        ROD00000_BendingTwistRigidityMatrix_PYEL(3UL, 3UL, 4UL);
    ROD00000_BendingTwistRigidityMatrix_PYEL = {
        {{0.0000005028670658104868, 0.0000005028670658104868,
          0.0000005028670658104868, 0.0000005028670658104868},
         {0., 0., 0., 0.},
         {0., 0., 0., 0.}},
        {{0., 0., 0., 0.},
         {0.0000005028670658104868, 0.0000005028670658104868,
          0.0000005028670658104868, 0.0000005028670658104868},
         {0., 0., 0., 0.}},
        {{0., 0., 0., 0.},
         {0., 0., 0., 0.},
         {0.0000008110759125975593, 0.0000008110759125975594,
          0.0000008110759125975593, 0.0000008110759125975593}}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementVolume_PYEL(5UL);
    ROD00000_ElementVolume_PYEL = {
        0.0000063991019833543736, 0.0000063991019833543736,
        0.000006399101983354373, 0.000006399101983354375,
        0.000006399101983354375};
    blaze::DynamicVector<elastica::real_t> ROD00000_Mass_PYEL(6UL);
    ROD00000_Mass_PYEL = {0.000025193264508466167, 0.00005038652901693233,
                          0.000050386529016932326, 0.00005038652901693234,
                          0.00005038652901693235,  0.000025193264508466173};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalLoads_PYEL(3UL,
                                                                       6UL);
    ROD00000_InternalLoads_PYEL = {
        {-0.00638571975743826, -0.049000292625169826, -0.04209564585992403,
         0.18548039415390408, -0.11528498211863526, 0.011933975765490325},
        {0.0715545097241887, -0.21229210371064586, -0.11522046545204513,
         0.2942097655459327, -0.048554007505682104, -0.0070368357917341825},
        {-0.015336665139503613, -0.12815629239681883, 0.1430821214976208,
         0.03201988677244583, -0.049465895573307175, 0.004384716485670621}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalTorques_PYEL(3UL,
                                                                         5UL);
    ROD00000_InternalTorques_PYEL = {
        {5641039856300604., -432925.22817841975, -55511800.76243675,
         40448620769345.305, 5340748916364150.},
        {2624237576279564.5, 309732.67827306985, 51253215.517901145,
         48148422376730.31, 1470623266010772.5},
        {2551295557.2333956, 17843.34119139525, 297076.6357558651,
         376426862.1915939, 2901209577.646068}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ExternalLoads_PYEL(3UL,
                                                                       6UL);
    ROD00000_ExternalLoads_PYEL = {{0., 0., 0., 0., 0., 0.},
                                   {0., 0., 0., 0., 0., 0.},
                                   {0., 0., 0., 0., 0., 0.}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ExternalTorques_PYEL(3UL,
                                                                         5UL);
    ROD00000_ExternalTorques_PYEL = {
        {0., 0., 0., 0., 0.}, {0., 0., 0., 0., 0.}, {0., 0., 0., 0., 0.}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementLength_PYEL(5UL);
    ROD00000_ElementLength_PYEL = {0.10421512091497219, 0.028421286940478633,
                                   0.03597231517542573, 0.0886763172230477,
                                   0.09873893281907023};
    blaze::DynamicVector<elastica::real_t> ROD00000_ReferenceElementLength_PYEL(
        5UL);
    ROD00000_ReferenceElementLength_PYEL = {
        0.07451700459404914, 0.12877804874902377, 0.12125445792190077,
        0.12557378285646317, 0.06303696469708653};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Tangent_PYEL(3UL, 5UL);
    ROD00000_Tangent_PYEL = {
        {-0.30028336250402765, 0.7407362446077099, -0.9745022569269507,
         0.5038730586232227, -0.4859213019673116},
        {0.9532598684389109, 0.5754947278122714, -0.22406542903271293,
         0.12376124611780787, 0.7277229635037671},
        {0.03354885135299215, 0.3465770248945342, 0.011833628212254659,
         -0.8548655419146604, 0.48404522173417597}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementDilatation_PYEL(5UL);
    ROD00000_ElementDilatation_PYEL = {1.3985414669136433, 0.2206997793223986,
                                       0.2966679806411346, 0.7061690362900747,
                                       1.56636559665497};
    blaze::DynamicVector<elastica::real_t> ROD00000_VoronoiDilatation_PYEL(4UL);
    ROD00000_VoronoiDilatation_PYEL = {1.8421723313257061, 0.8943555849431164,
                                       1.7312310055343534, 2.6029895839183044};
    blaze::DynamicVector<elastica::real_t> ROD00000_ReferenceVoronoiLength_PYEL(
        4UL);
    ROD00000_ReferenceVoronoiLength_PYEL = {0.036, 0.03599999999999999, 0.036,
                                            0.036000000000000004};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ShearStretchStrain_PYEL(
        3UL, 5UL);
    ROD00000_ShearStretchStrain_PYEL = {
        {-0.17661627153989062, 0.06652491605391897, -0.23176138193454512,
         0.553261355120083, -0.4296856406278118},
        {-0.014572567138053603, 0.1640729843552772, -0.16137511863690274,
         -0.3490054775572106, 1.1382924349303802},
        {-2.3872680229830574, -0.8682317035788676, -0.9091340346091897,
         -0.733970382512392, -0.01351038261783366}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Curvature_PYEL(3UL, 4UL);
    ROD00000_Curvature_PYEL = {{78.06737488989461, 12.382061597092076,
                                -41.13724918722577, -74.0054207703002},
                               {24.372687192616294, -7.66082430588186,
                                -3.174450014525259, 23.070135896383476},
                               {2.8596166467940645, 25.944214098422112,
                                50.28106847156379, 38.95016532134589}};
    blaze::DynamicMatrix<elastica::real_t>
        ROD00000_ReferenceShearStretchStrain_PYEL(3UL, 5UL);
    ROD00000_ReferenceShearStretchStrain_PYEL = {
        {0.297597441176643, 0.791076063623359, 0.9813316766668237,
         0.11729731016539668, 0.20564794237709072},
        {0.9357375659253002, 0.4402876689437507, 0.9156671113598859,
         0.9679292352235765, 0.9987855559307337},
        {0.3040655442953162, 0.10165528375219623, 0.5054634070571163,
         0.9126824887816438, 0.11534211862120414}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ReferenceCurvature_PYEL(
        3UL, 4UL);
    ROD00000_ReferenceCurvature_PYEL = {
        {0.051148978603062, 0.4646769716719006, 0.9301184173210795,
         0.7944799966795505},
        {0.6036777904356718, 0.19098869154676612, 0.5763017409174203,
         0.5082335545968735},
        {0.30308030515914375, 0.6131410921095497, 0.2555432491263159,
         0.9931735829341407}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalStress_PYEL(3UL,
                                                                        5UL);
    ROD00000_InternalStress_PYEL = {
        {-0.01812749049927247, -0.027696993342014885, -0.04637219950591065,
         0.016665342800123828, -0.02428652563380115},
        {-0.03632695016300756, -0.010558697347560955, -0.04117146397926444,
         -0.0503416937425553, 0.0053328479456285355},
        {-0.09567843315688468, -0.03448003191255449, -0.050289740525638234,
         -0.058539442525521325, -0.004580779423549437}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalCouple_PYEL(3UL,
                                                                        4UL);
    ROD00000_InternalCouple_PYEL = {
        {0.000039231790609619253, 0.000005992860238720051,
         -0.000021154293713669605, -0.000037614406621606695},
        {0.000011952652015296443, -0.000003948418163309577,
         -0.0000018861295298429927, 0.00001134563762971698},
        {0.000002073545046380369, 0.000020545423155670362,
         0.00004057449852296069, 0.00003078600171369033}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Position_PYEL(3UL, 6UL);
    ROD00000_Position_PYEL = {
        {0.08895375763683314, 0.057662543340063196, 0.07870995377241187,
         0.04366070228109599, 0.08833804977251396, 0.040363383086548875},
        {0.029590454562645595, 0.12893034713670298, 0.1452893241613612,
         0.13722958048618028, 0.1482054352829362, 0.22006479785598038},
        {0.07489336853336252, 0.07838711226394685, 0.08824303851588887,
         0.08866799126731656, 0.012862760494601888, 0.0606535372515203}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_Director_PYEL(3UL, 3UL,
                                                                  5UL);
    ROD00000_Director_PYEL = {
        {{0.5147734743711851, -0.40132882032285117, 0.543088609890582,
          0.5847778189973277, 0.9209668594813394},
         {-0.34184433872256836, -0.6902889133576269, -0.5532791669868703,
          -0.6941764425005573, 0.386631050503593},
         {0.7862256153084488, 0.6020269047757043, 0.63161453845316,
          0.41970700385625537, -0.048337092626343114}},
        {{-0.593575832781106, 0.31995379758285936, 0.25773208950853854,
          -0.34806979848972835, 0.06951352865925406},
         {0.5196031110132302, -0.7215305578780988, -0.6060619450049582,
          -0.6820669854105469, -0.04096973960076089},
         {0.6145570256400859, -0.6140221669129097, -0.7525045440755347,
          -0.6431423192360202, 0.9967393589953126}},
        {{-0.6186081157098043, 0.8582335028362473, 0.799142622968312,
          0.7327225380647476, 0.3833900273486758},
         {-0.7830421796485187, -0.053803997469198034, 0.5714639815378416,
          0.23000803049074842, -0.9213239990489412},
         {0.06456736071160186, 0.5104315668782243, -0.18654754343124846,
          -0.640479498595947, -0.06460786102362043}}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Velocity_PYEL(3UL, 6UL);
    ROD00000_Velocity_PYEL = {
        {0.08855222911583079, 0.3738157632890837, -0.15287652267327717,
         0.4322068797355343, 0.006237359511921878, 0.4746477935556004},
        {0.28998668039172504, -0.13000118902879726, 0.1376220882722736,
         0.1784778783555096, 0.2948054877040317, 0.7721823628061198},
        {0.05838723316953755, -0.19699973944893973, 0.37911830425663934,
         0.30609307502207694, 0.41581359125485945, 0.0826262628204716}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularVelocity_PYEL(3UL,
                                                                         5UL);
    ROD00000_AngularVelocity_PYEL = {
        {221382516642551200000., -2686192503.610162, -462604346005.5861,
         801530506644059100., 234749211850705670000.},
        {102988270641952830000., 1872692343.7569525, 426084486809.71204,
         954114460346492200., 64640393524898144000.},
        {49940380430109.93, 52114073.038122706, 1195276121.374677,
         3708426020840.2876, 63613501016116.94}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Acceleration_PYEL(3UL, 6UL);
    ROD00000_Acceleration_PYEL = {
        {-253.46932531480178, -972.4879562293988, -835.4543700713705,
         3681.1504537566693, -2288.0119819306037, 473.69707730730664},
        {2840.2238106198142, -4213.271043919404, -2286.7315471031047,
         5839.056019259906, -963.6307253743471, -279.3141710304935},
        {-608.7605333699308, -2543.463399785924, 2839.6899784372577,
         635.4850670838148, -981.7285798091829, 174.04320445240995}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularAcceleration_PYEL(
        3UL, 5UL);
    ROD00000_AngularAcceleration_PYEL = {
        {11069127937064798000000000., -134058292812616.31, -23106538068710628.,
         40076637792300230000000., 11737461693690264000000000.},
        {5149409011291944000000000., 95910867223599.84, 21333921062592960.,
         47705628700331510000000., 3232015681875986000000000.},
        {2503139283508066300., 2762657039249.3477, 61828408546803.35,
         186482539137375900., 3188020707980629500.}};

    blocks::get<elastica::tags::InternalLoads>(slicedrod_00000) =
        ROD00000_InternalLoads_REPLACED;
    blocks::get<elastica::tags::InternalTorques>(slicedrod_00000) =
        ROD00000_InternalTorques_REPLACED;
    blocks::get<elastica::tags::ExternalLoads>(slicedrod_00000) =
        ROD00000_ExternalLoads_REPLACED;
    blocks::get<elastica::tags::ExternalTorques>(slicedrod_00000) =
        ROD00000_ExternalTorques_REPLACED;
    blocks::get<elastica::tags::InternalStress>(slicedrod_00000) =
        ROD00000_InternalStress_REPLACED;
    blocks::get<elastica::tags::InternalCouple>(slicedrod_00000) =
        ROD00000_InternalCouple_REPLACED;
    blocks::get<elastica::tags::ReferenceShearStretchStrain>(slicedrod_00000) =
        ROD00000_ReferenceShearStretchStrain_REPLACED;
    blocks::get<elastica::tags::ReferenceCurvature>(slicedrod_00000) =
        ROD00000_ReferenceCurvature_REPLACED;
    blocks::get<elastica::tags::ShearStretchStrain>(slicedrod_00000) =
        ROD00000_ShearStretchStrain_REPLACED;
    blocks::get<elastica::tags::Curvature>(slicedrod_00000) =
        ROD00000_Curvature_REPLACED;
    blocks::get<elastica::tags::ReferenceElementLength>(slicedrod_00000) =
        ROD00000_ReferenceElementLength_REPLACED;
    blocks::get<elastica::tags::Position>(slicedrod_00000) =
        ROD00000_Position_REPLACED;
    blocks::get<elastica::tags::Director>(slicedrod_00000) =
        ROD00000_Director_REPLACED;
    blocks::get<elastica::tags::Velocity>(slicedrod_00000) =
        ROD00000_Velocity_REPLACED;
    blocks::get<elastica::tags::Acceleration>(slicedrod_00000) =
        ROD00000_Acceleration_REPLACED;
    blocks::get<elastica::tags::AngularVelocity>(slicedrod_00000) =
        ROD00000_AngularVelocity_REPLACED;
    blocks::get<elastica::tags::AngularAcceleration>(slicedrod_00000) =
        ROD00000_AngularAcceleration_REPLACED;

    std::size_t step = 5;
    while (step--)
      simulator->step(time_stepper, time_v, time_delta);

    auto ROD00000_ElementDimension(
        blocks::get<elastica::tags::ElementDimension>(slicedrod_00000));
    auto ROD00000_MassSecondMomentOfInertia(
        blocks::get<elastica::tags::MassSecondMomentOfInertia>(
            slicedrod_00000));
    auto ROD00000_InvMassSecondMomentOfInertia(
        blocks::get<elastica::tags::InvMassSecondMomentOfInertia>(
            slicedrod_00000));
    auto ROD00000_ShearStretchRigidityMatrix(
        blocks::get<elastica::tags::ShearStretchRigidityMatrix>(
            slicedrod_00000));
    auto ROD00000_BendingTwistRigidityMatrix(
        blocks::get<elastica::tags::BendingTwistRigidityMatrix>(
            slicedrod_00000));
    auto ROD00000_ElementVolume(
        blocks::get<elastica::tags::ElementVolume>(slicedrod_00000));
    auto ROD00000_Mass(blocks::get<elastica::tags::Mass>(slicedrod_00000));
    auto ROD00000_InternalLoads(
        blocks::get<elastica::tags::InternalLoads>(slicedrod_00000));
    auto ROD00000_InternalTorques(
        blocks::get<elastica::tags::InternalTorques>(slicedrod_00000));
    auto ROD00000_ExternalLoads(
        blocks::get<elastica::tags::ExternalLoads>(slicedrod_00000));
    auto ROD00000_ExternalTorques(
        blocks::get<elastica::tags::ExternalTorques>(slicedrod_00000));
    auto ROD00000_ElementLength(
        blocks::get<elastica::tags::ElementLength>(slicedrod_00000));
    auto ROD00000_ReferenceElementLength(
        blocks::get<elastica::tags::ReferenceElementLength>(slicedrod_00000));
    auto ROD00000_Tangent(
        blocks::get<elastica::tags::Tangent>(slicedrod_00000));
    auto ROD00000_ElementDilatation(
        blocks::get<elastica::tags::ElementDilatation>(slicedrod_00000));
    auto ROD00000_VoronoiDilatation(
        blocks::get<elastica::tags::VoronoiDilatation>(slicedrod_00000));
    auto ROD00000_ReferenceVoronoiLength(
        blocks::get<elastica::tags::ReferenceVoronoiLength>(slicedrod_00000));
    auto ROD00000_ShearStretchStrain(
        blocks::get<elastica::tags::ShearStretchStrain>(slicedrod_00000));
    auto ROD00000_Curvature(
        blocks::get<elastica::tags::Curvature>(slicedrod_00000));
    auto ROD00000_ReferenceShearStretchStrain(
        blocks::get<elastica::tags::ReferenceShearStretchStrain>(
            slicedrod_00000));
    auto ROD00000_ReferenceCurvature(
        blocks::get<elastica::tags::ReferenceCurvature>(slicedrod_00000));
    auto ROD00000_InternalStress(
        blocks::get<elastica::tags::InternalStress>(slicedrod_00000));
    auto ROD00000_InternalCouple(
        blocks::get<elastica::tags::InternalCouple>(slicedrod_00000));
    auto ROD00000_Position(
        blocks::get<elastica::tags::Position>(slicedrod_00000));
    auto ROD00000_Director(
        blocks::get<elastica::tags::Director>(slicedrod_00000));
    auto ROD00000_Velocity(
        blocks::get<elastica::tags::Velocity>(slicedrod_00000));
    auto ROD00000_AngularVelocity(
        blocks::get<elastica::tags::AngularVelocity>(slicedrod_00000));
    auto ROD00000_Acceleration(
        blocks::get<elastica::tags::Acceleration>(slicedrod_00000));
    auto ROD00000_AngularAcceleration(
        blocks::get<elastica::tags::AngularAcceleration>(slicedrod_00000));

    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ElementDimension,
                                 ROD00000_ElementDimension_PYEL, approx);
    CHECK_ITERABLE_CUSTOM_APPROX(
        blaze::evaluate(blaze::ravel(ROD00000_MassSecondMomentOfInertia)),
        blaze::evaluate(blaze::ravel(ROD00000_MassSecondMomentOfInertia_PYEL)),
        approx);
    CHECK_ITERABLE_CUSTOM_APPROX(
        blaze::evaluate(blaze::ravel(ROD00000_InvMassSecondMomentOfInertia)),
        blaze::evaluate(
            blaze::ravel(ROD00000_InvMassSecondMomentOfInertia_PYEL)),
        approx);
    CHECK_ITERABLE_CUSTOM_APPROX(
        blaze::evaluate(blaze::ravel(ROD00000_ShearStretchRigidityMatrix)),
        blaze::evaluate(blaze::ravel(ROD00000_ShearStretchRigidityMatrix_PYEL)),
        approx);
    CHECK_ITERABLE_CUSTOM_APPROX(
        blaze::evaluate(blaze::ravel(ROD00000_BendingTwistRigidityMatrix)),
        blaze::evaluate(blaze::ravel(ROD00000_BendingTwistRigidityMatrix_PYEL)),
        approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ElementVolume,
                                 ROD00000_ElementVolume_PYEL, approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_Mass, ROD00000_Mass_PYEL, approx);
    CHECK_ITERABLE_CUSTOM_APPROX(
        blaze::evaluate(blaze::ravel(ROD00000_InternalLoads)),
        blaze::evaluate(blaze::ravel(ROD00000_InternalLoads_PYEL)), approx);
    CHECK_ITERABLE_CUSTOM_APPROX(
        blaze::evaluate(blaze::ravel(ROD00000_InternalTorques)),
        blaze::evaluate(blaze::ravel(ROD00000_InternalTorques_PYEL)), approx);
    CHECK_ITERABLE_CUSTOM_APPROX(
        blaze::evaluate(blaze::ravel(ROD00000_ExternalLoads)),
        blaze::evaluate(blaze::ravel(ROD00000_ExternalLoads_PYEL)), approx);
    CHECK_ITERABLE_CUSTOM_APPROX(
        blaze::evaluate(blaze::ravel(ROD00000_ExternalTorques)),
        blaze::evaluate(blaze::ravel(ROD00000_ExternalTorques_PYEL)), approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ElementLength,
                                 ROD00000_ElementLength_PYEL, approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ReferenceElementLength,
                                 ROD00000_ReferenceElementLength_PYEL, approx);
    CHECK_ITERABLE_CUSTOM_APPROX(
        blaze::evaluate(blaze::ravel(ROD00000_Tangent)),
        blaze::evaluate(blaze::ravel(ROD00000_Tangent_PYEL)), approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ElementDilatation,
                                 ROD00000_ElementDilatation_PYEL, approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_VoronoiDilatation,
                                 ROD00000_VoronoiDilatation_PYEL, approx);
    CHECK_ITERABLE_CUSTOM_APPROX(ROD00000_ReferenceVoronoiLength,
                                 ROD00000_ReferenceVoronoiLength_PYEL, approx);
    CHECK_ITERABLE_CUSTOM_APPROX(
        blaze::evaluate(blaze::ravel(ROD00000_ShearStretchStrain)),
        blaze::evaluate(blaze::ravel(ROD00000_ShearStretchStrain_PYEL)),
        approx);
    CHECK_ITERABLE_CUSTOM_APPROX(
        blaze::evaluate(blaze::ravel(ROD00000_Curvature)),
        blaze::evaluate(blaze::ravel(ROD00000_Curvature_PYEL)), approx);
    CHECK_ITERABLE_CUSTOM_APPROX(
        blaze::evaluate(blaze::ravel(ROD00000_ReferenceShearStretchStrain)),
        blaze::evaluate(
            blaze::ravel(ROD00000_ReferenceShearStretchStrain_PYEL)),
        approx);
    CHECK_ITERABLE_CUSTOM_APPROX(
        blaze::evaluate(blaze::ravel(ROD00000_ReferenceCurvature)),
        blaze::evaluate(blaze::ravel(ROD00000_ReferenceCurvature_PYEL)),
        approx);
    CHECK_ITERABLE_CUSTOM_APPROX(
        blaze::evaluate(blaze::ravel(ROD00000_InternalStress)),
        blaze::evaluate(blaze::ravel(ROD00000_InternalStress_PYEL)), approx);
    CHECK_ITERABLE_CUSTOM_APPROX(
        blaze::evaluate(blaze::ravel(ROD00000_InternalCouple)),
        blaze::evaluate(blaze::ravel(ROD00000_InternalCouple_PYEL)), approx);
    CHECK_ITERABLE_CUSTOM_APPROX(
        blaze::evaluate(blaze::ravel(ROD00000_Position)),
        blaze::evaluate(blaze::ravel(ROD00000_Position_PYEL)), approx);
    CHECK_ITERABLE_CUSTOM_APPROX(
        blaze::evaluate(blaze::ravel(ROD00000_Director)),
        blaze::evaluate(blaze::ravel(ROD00000_Director_PYEL)), approx);
    CHECK_ITERABLE_CUSTOM_APPROX(
        blaze::evaluate(blaze::ravel(ROD00000_Velocity)),
        blaze::evaluate(blaze::ravel(ROD00000_Velocity_PYEL)), approx);
    CHECK_ITERABLE_CUSTOM_APPROX(
        blaze::evaluate(blaze::ravel(ROD00000_AngularVelocity)),
        blaze::evaluate(blaze::ravel(ROD00000_AngularVelocity_PYEL)), approx);
    CHECK_ITERABLE_CUSTOM_APPROX(
        blaze::evaluate(blaze::ravel(ROD00000_Acceleration)),
        blaze::evaluate(blaze::ravel(ROD00000_Acceleration_PYEL)), approx);
    CHECK_ITERABLE_CUSTOM_APPROX(
        blaze::evaluate(blaze::ravel(ROD00000_AngularAcceleration)),
        blaze::evaluate(blaze::ravel(ROD00000_AngularAcceleration_PYEL)),
        approx);

    // clang-format on
  }

}  // namespace
