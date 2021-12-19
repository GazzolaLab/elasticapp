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
     * rng_seed=300
     * NROD=1, N=5, n_step=3, time=0.0, time_step=2e-05
     * tag=300
     */

    namespace ec = elastica::configuration;
    namespace et = elastica::timestepper;

    Approx approx = Approx::custom()
                        .epsilon(7e-10)
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
        ec::make_blocking_policy(ec::RestrictSizeAcrossBlockTypes{10UL}),
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
        0.0044207077229210115, 0.00846496896077867, 0.007523342974842949,
        0.004792526359576932, 0.004541893851439805};
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
        {-0.05629280085772673, 0.02621330179266366, -0.22324652295009645,
         0.19540272745085394, 0.04534738680066891, -0.002065286748248083},
        {0.022896482167265156, -0.23462237973851124, 0.31196406817546957,
         -0.18699375821443287, 0.12722778036283114, -0.058632274294443204},
        {-0.0010796748930827383, 0.024300321980913028, -0.10320810488128508,
         0.050416082062340856, 0.02230954165580001, -0.006829950015671479}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalTorques_PYEL(3UL,
                                                                         5UL);
    ROD00000_InternalTorques_PYEL = {
        {401368.704009042, 0.016659330415432105, 312.97351633496993,
         45119.22957039335, 273134.61898449354},
        {124323.7850008521, 221.12450783341407, 1382.0005170358716,
         40338.87932760631, 39018.00197214552},
        {15322.91194728353, 64.71722760769752, 354.1991640698091,
         12328.380702175362, 15473.505178607566}};
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
    ROD00000_ElementLength_PYEL = {0.10422827601266507, 0.028426174582282082,
                                   0.0359871485909009, 0.08868295146265474,
                                   0.09874047195766138};
    blaze::DynamicVector<elastica::real_t> ROD00000_ReferenceElementLength_PYEL(
        5UL);
    ROD00000_ReferenceElementLength_PYEL = {
        0.07451700459404914, 0.12877804874902377, 0.12125445792190077,
        0.12557378285646317, 0.06303696469708653};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Tangent_PYEL(3UL, 5UL);
    ROD00000_Tangent_PYEL = {
        {-0.3003639504662542, 0.7413196567654572, -0.9745599335660403,
         0.5039547691119618, -0.4860848300150847},
        {0.9532321116426931, 0.57508540602919, -0.22381973254048199,
         0.12362319734921888, 0.7275280876773474},
        {0.033616046666999515, 0.34600858698215353, 0.011732996772443093,
         -0.8548373504746183, 0.4841739560010926}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementDilatation_PYEL(5UL);
    ROD00000_ElementDilatation_PYEL = {1.398718005111395, 0.22073773332038915,
                                       0.2967903135906145, 0.7062218676968869,
                                       1.5663900131001234};
    blaze::DynamicVector<elastica::real_t> ROD00000_VoronoiDilatation_PYEL(4UL);
    ROD00000_VoronoiDilatation_PYEL = {1.8424229249298216, 0.8946294885164306,
                                       1.7315291674104951, 2.603103103059946};
    blaze::DynamicVector<elastica::real_t> ROD00000_ReferenceVoronoiLength_PYEL(
        4UL);
    ROD00000_ReferenceVoronoiLength_PYEL = {0.036, 0.03599999999999999, 0.036,
                                            0.036000000000000004};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ShearStretchStrain_PYEL(
        3UL, 5UL);
    ROD00000_ShearStretchStrain_PYEL = {
        {0.233445579731708, 0.10683467549548874, -0.11694472813492245,
         -0.5724631767187108, 0.6295560236255329},
        {-1.1481509132918835, 0.18767143573333075, -0.27076564555816246,
         0.33621490789745, 0.8212586595839833},
        {-0.23602048493854244, -0.9542732984681036, -1.0330815052277809,
         -0.7591789597607634, -2.175912879559931}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Curvature_PYEL(3UL, 4UL);
    ROD00000_Curvature_PYEL = {{17.755942772416635, 15.099208226757929,
                                -18.913499121562243, -77.41985859875909},
                               {-12.84886255328309, 83.37863231821197,
                                -64.1021592935299, -10.669849314476421},
                               {-13.07425560450411, 14.173098170249588,
                                -21.597934740452878, 1.1027590018280162}};
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
        {-0.002452295721673588, -0.026156095721996443, -0.0419831703717931,
         -0.026367071085182624, 0.01620448652017236},
        {-0.07965958721892479, -0.00965661313598751, -0.04535307173184517,
         -0.024148174461804006, -0.006786216932564141},
        {-0.019200364338068737, -0.037538859359813846, -0.05469614333156704,
         -0.05943562172923019, -0.08145541335128624}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalCouple_PYEL(3UL,
                                                                        4UL);
    ROD00000_InternalCouple_PYEL = {
        {0.000008903157705874748, 0.0000073592237917569715,
         -0.00000997870172684377, -0.000039331414953796144},
        {-0.000006764839490343051, 0.000041832326262229576,
         -0.000032524667921530424, -0.000005621089733757356},
        {-0.000010850034931054225, 0.000010998154561936169,
         -0.000017724829603828622, 0.0000000888820936367359}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Position_PYEL(3UL, 6UL);
    ROD00000_Position_PYEL = {
        {0.0889500105169077, 0.05764677224505835, 0.0787148462444855,
         0.0436465309987664, 0.08833649861905635, 0.04034456869780046},
        {0.029581119973906923, 0.1289328251982211, 0.14528242374504424,
         0.1372262472418608, 0.14819270419383906, 0.22003359506040357},
        {0.0748905248680998, 0.07839271189263927, 0.08823096012889206,
         0.0886552095911954, 0.012845875324062705, 0.06065027512938321}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_Director_PYEL(3UL, 3UL,
                                                                  5UL);
    ROD00000_Director_PYEL = {
        {{0.2713698961902898, -0.043376484538473384, 0.04433983315536455,
          0.6048433947091046, 0.2935449181827158},
         {-0.29903478950813434, -0.11601641826644832, 0.1884117879612192,
          -0.5926922151667917, 0.020446823958828908},
         {0.9148423766450134, 0.9922996882402553, 0.9810886694651879,
          0.5318650260709423, 0.9557265866340119}},
        {{0.2954395928709026, 0.9953883423986808, 0.9766578585637498,
          0.465364537866762, -0.8080627535589942},
         {0.9305028996380283, 0.08004267228507717, 0.19843099731539243,
          0.8050411404162924, 0.5394734578782977},
         {0.2165174374721263, 0.05286982532853577, -0.0822469854164034,
          0.3678921161608216, 0.23664947613609466}},
        {{-0.9160097305192484, -0.08556008652338845, -0.21017470471353292,
          -0.6462200204054481, -0.5107503962991692},
         {0.21152434476685317, 0.9900168490011103, 0.961834776591924,
          0.024994005639569966, -0.8417543083553705},
         {0.3408571917460547, 0.1120094205324802, -0.17521545604455424,
          0.7627417550582017, 0.174882008924339}}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Velocity_PYEL(3UL, 6UL);
    ROD00000_Velocity_PYEL = {
        {0.09891653218289612, 0.4167626923688799, -0.06403581414735661,
         0.2677536119020467, 0.044882787948795085, 0.47644337726656594},
        {0.17710778511660705, -0.02633342648487101, 0.18570610499566043,
         0.03145303765387855, 0.35014721758356804, 0.7925599024207015},
        {0.08485440542960125, -0.07070218688802418, 0.18407138604679613,
         0.38530778973425533, 0.40180713696109643, 0.08529515342567362}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularVelocity_PYEL(3UL,
                                                                         5UL);
    ROD00000_AngularVelocity_PYEL = {
        {15734344321.669044, -151.84108311654313, 2559927.362377648,
         891523076.8965601, 11991784129.32581},
        {4874085480.96106, 1330797.7514383143, 11318526.987743283,
         797247587.3271068, 1713312754.7289276},
        {299976660.3095924, 189052.93325241195, 1425733.9974012063,
         121464092.98404661, 339285813.4715869}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Acceleration_PYEL(3UL, 6UL);
    ROD00000_Acceleration_PYEL = {
        {-2234.4385277584647, 520.2442459145124, -4430.678741039589,
         3878.0747803681625, 899.9902887819472, -81.97773446764094},
        {908.8334764861783, -4656.450529856237, 6191.41810841216,
         -3711.1855462715803, 2525.0356165648236, -2327.2995952842825},
        {-42.8556963199396, 482.2781496369185, -2048.3273385750017,
         1000.5865267162694, 442.7679796777212, -271.10222311111295}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularAcceleration_PYEL(
        3UL, 5UL);
    ROD00000_AngularAcceleration_PYEL = {
        {787684977170380., 5159564.091871045, 130327541577.40273,
         44707638170523.445, 600282310835087.8},
        {243985085962071.72, 68484509400.99493, 575488724902.8866,
         39970895743451.15, 85751914111397.31},
        {15035586266222.525, 10021791853.675188, 73747304280.82465,
         6107958723025.185, 17003467886764.977}};

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

    std::size_t step = 3;
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
