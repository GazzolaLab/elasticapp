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

  ELASTICA_TEST_CASE("Unit.z.Integrated.CosseratRods.Stepper:SingleRod,OneStep",
                     "[Systems][Integrated]") {
    /*
     * rng_seed=300
     * NROD=1, N=5, n_step=1, time=0.0, time_step=2e-05
     * tag=300
     */

    namespace ec = elastica::configuration;
    namespace et = elastica::timestepper;

    Approx approx = Approx::custom()
                        .epsilon(std::numeric_limits<double>::epsilon() * 200)
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

    auto slicedrod_00000 = (simulator->emplace_back<Plugin>(
                                straight_initializer.initialize<Plugin>()))
                               .get();

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
        0.0044205848500514285, 0.008463835986916503, 0.007522854530181701,
        0.004792450020013244, 0.004541944257309467};
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
         {0.0000008110759125975593, 0.0000008110759125975593,
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
        {-0.037355638234172775, -0.008604644515494989, -0.11054690634198425,
         0.12328073141426117, -0.012762880290219481, 0.04216987624589443},
        {-0.00043352642088004376, -0.16106522809371587, -0.027368411266327924,
         0.0930167598947168, 0.0920168691325536, -0.0006974756883796055},
        {-0.007307698711475705, -0.11531806288067394, -0.007153863021021925,
         0.0880430506791343, 0.05316948582187895, -0.014560416869305868}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalTorques_PYEL(3UL,
                                                                         5UL);
    ROD00000_InternalTorques_PYEL = {
        {0.0015421537837970213, -0.00289223424276732, 0.005348192965846609,
         -0.005410613382729141, 0.0025860228299563024},
        {-0.0016421845107677715, -0.0007232904214997996, 0.00015235635027877948,
         -0.007627376309818388, -0.0006594990605193937},
        {-0.0003146661162255835, 0.0019598036344589008, -0.004457976206107236,
         0.0033808180042260378, -0.002876743819877462}};
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
    ROD00000_ElementLength_PYEL = {0.10423407027102072, 0.0284337853791872,
                                   0.0359918218970712, 0.08868577676984132,
                                   0.0987382803535992};
    blaze::DynamicVector<elastica::real_t> ROD00000_ReferenceElementLength_PYEL(
        5UL);
    ROD00000_ReferenceElementLength_PYEL = {
        0.07451700459404914, 0.12877804874902377, 0.12125445792190077,
        0.12557378285646317, 0.06303696469708653};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Tangent_PYEL(3UL, 5UL);
    ROD00000_Tangent_PYEL = {
        {-0.30044729597338127, 0.7416361494997835, -0.9745623540270615,
         0.5039982485221745, -0.4862719914969523},
        {0.9532038952278391, 0.57497446098495, -0.22381630264935753,
         0.12353209065584835, 0.727319040837592},
        {0.03367130031285107, 0.3455143860538962, 0.01159658491125207,
         -0.8548248873686212, 0.4843000754910876}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementDilatation_PYEL(5UL);
    ROD00000_ElementDilatation_PYEL = {1.3987957626432121, 0.22079683343084314,
                                       0.29682885490489186, 0.7062443668772279,
                                       1.5663552461332697};
    blaze::DynamicVector<elastica::real_t> ROD00000_VoronoiDilatation_PYEL(4UL);
    ROD00000_VoronoiDilatation_PYEL = {1.8426091062528878, 0.8948001010591446,
                                       1.7316333148182297, 2.603111904492229};
    blaze::DynamicVector<elastica::real_t> ROD00000_ReferenceVoronoiLength_PYEL(
        4UL);
    ROD00000_ReferenceVoronoiLength_PYEL = {0.036, 0.03599999999999999, 0.036,
                                            0.036000000000000004};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ShearStretchStrain_PYEL(
        3UL, 5UL);
    ROD00000_ShearStretchStrain_PYEL = {
        {-0.008181930333816185, 0.08078704346471492, -0.002543861754938874,
         -0.5938155880673655, 0.7069635133559449},
        {-0.3891488202850203, 0.1667687461106827, -0.29085493365169834,
         0.37767722111203733, -0.7674483118810647},
        {0.3435497152228475, -0.8799463429045646, -1.0591971742736699,
         -0.9406361240558314, 0.16820136811405018}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Curvature_PYEL(3UL, 4UL);
    ROD00000_Curvature_PYEL = {{0.5281082038797649, -0.4939806430037735,
                                0.10981846103956955, -0.17309770411861997},
                               {-0.7786751795243776, 0.9284725466899326,
                                -0.2068462862506748, -0.365884935319509},
                               {-0.1410727644308197, -0.04840070483916087,
                                0.12285520678655558, 0.32644277337333294}};
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
        {-0.011688849359028874, -0.027151803331254568, -0.037610035300409524,
         -0.02718329723697817, 0.01916349739866915},
        {-0.05064565771635406, -0.010455648038148371, -0.04612101320904662,
         -0.02256321884217482, -0.06751679000347757},
        {0.0014036846471859373, -0.034896493977795325, -0.05562457016261245,
         -0.06588652672886591, 0.001879176268157193}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalCouple_PYEL(3UL,
                                                                        4UL);
    ROD00000_InternalCouple_PYEL = {
        {0.00000023984708612613854, -0.0000004820773418088365,
         -0.0000004125018320997535, -0.0000004865629593440331},
        {-0.0000006951397819182221, 0.00000037085634231845023,
         -0.00000039381935051729137, -0.0000004395654001949449},
        {-0.00000036024185625068693, -0.0000005365606166816047,
         -0.00000010762007503150466, -0.0000005407692998214417}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Position_PYEL(3UL, 6UL);
    ROD00000_Position_PYEL = {
        {0.0889445456520459, 0.05763028747965939, 0.07871430782453513,
         0.043638697774122895, 0.08833516711014314, 0.040325702234806235},
        {0.029574525416642825, 0.1289304774890673, 0.14527861595431427,
         0.1372230827266934, 0.14818054774212433, 0.22000050219845818},
        {0.074887001180282, 0.07839538140966888, 0.08822231875234265,
         0.0886407258295821, 0.012830295072858796, 0.06064657819578566}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_Director_PYEL(3UL, 3UL,
                                                                  5UL);
    ROD00000_Director_PYEL = {
        {{0.06287016476569678, 0.03685881463612332, 0.04676980092893851,
          0.07675044891690759, 0.06451733916492058},
         {-0.0953803272203454, -0.01343990115507131, -0.06772251116902618,
          -0.08724204712608215, -0.03270930433611285},
         {0.9934535397096704, 0.9992301020488533, 0.9966073686271985,
          0.993226154410114, 0.9973803759634166}},
        {{0.9652612847094606, 0.998403699651845, 0.9984237510129981,
          0.992034820213461, 0.9814762687463161},
         {0.25876073490361967, 0.043316285708095474, 0.03415761538582303,
          0.10647450370261186, 0.18275295844856615},
         {-0.0362427139323193, -0.03624571580149887, -0.04453392778869833,
          -0.06730598446890505, -0.05749513080344688}},
        {{-0.25360992611365873, -0.04279579775081062, -0.031025781766174293,
          -0.09988134998636708, -0.180393588674563},
         {0.9612208254357987, 0.9989710048091468, 0.9971193102092222,
          0.9904806940443276, 0.9826146027757493},
         {0.10833526722700272, 0.015015033981818355, 0.06921330850120097,
          0.09471911449315762, 0.04389414057671454}}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Velocity_PYEL(3UL, 6UL);
    ROD00000_Velocity_PYEL = {
        {0.16035934851893824, 0.4189982326263598, 0.0687222878367984,
         0.13518461985768204, 0.034502139024803195, 0.4540370582434912},
        {0.16448571011926322, 0.12751317364259285, 0.07131748234672823,
         0.07364498192751488, 0.2662049433840463, 0.8385145034654914},
        {0.09609133763834654, -0.03555401631054033, 0.22999040325606956,
         0.3324762531955478, 0.37031442869454684, 0.1029638105542614}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularVelocity_PYEL(3UL,
                                                                         5UL);
    ROD00000_AngularVelocity_PYEL = {
        {23683.28801102806, 6.522528655647213, 789.763428051254,
         7500.761777415915, 15988.273024510336},
        {6873.138985911597, 1136.3380554048058, 3244.5593944543366,
         6193.257047071088, 1990.8853499923573},
        {1801.11534834618, 685.6046316853578, 1697.6903406384301,
         3977.704000218989, 1809.475608882819}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Acceleration_PYEL(3UL, 6UL);
    ROD00000_Acceleration_PYEL = {
        {4055.2770845935697, 16647.8386619113, 3156.9114814450704,
         2644.5898650705158, 1365.2931559109588, 19384.926924571082},
        {4845.975505864017, 1986.2360470134568, -29.82385565737656,
         2444.9837658520055, 10465.366994104575, 37753.86144638101},
        {2541.35295882789, -1928.0151016927678, 10989.496965548724,
         12405.125470992678, 14761.428014033809, 4688.111919075522}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularAcceleration_PYEL(
        3UL, 5UL);
    ROD00000_AngularAcceleration_PYEL = {
        {1184159474.8732255, 324109.8463501986, 39487156.77409275,
         375034637.4897679, 799412308.8697791},
        {343655015.9812948, 56812796.727968276, 162223116.34945786,
         309661769.93801993, 99540451.85154735},
        {90055541.09178565, 34278580.20203799, 84884252.84024397,
         198882869.00099665, 90469861.73138289}};

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

    std::size_t step = 1;
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
