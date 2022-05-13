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
     * rng_seed=300
     * NROD=1, N=5, n_step=2, time=0.0, time_step=2e-05
     * tag=300
     */

    namespace ec = elastica::configuration;
    namespace et = elastica::timestepper;

    Approx approx =
        Approx::custom()
            //                .epsilon(std::numeric_limits<double>::epsilon()
            //                * 50000)
            .epsilon(6e-12)
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
        0.004420634630043581, 0.00846443223191415, 0.007522988515158648,
        0.004792482063235909, 0.0045419061592667205};
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
        {-0.021104455347224046, -0.031845357495265236, -0.11121447494277789,
         0.13858184076858868, -0.019195143389503663, 0.03028970283871808},
        {-0.006996918464576146, -0.1529674293354309, -0.02378178546010236,
         0.08069847699321443, 0.08425021137224341, 0.0007449533193218307},
        {-0.013075075376848403, -0.1128500378156369, -0.012476889770918435,
         0.08268380537849458, 0.05703087128891422, -0.015426607611823932}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalTorques_PYEL(3UL,
                                                                         5UL);
    ROD00000_InternalTorques_PYEL = {
        {-493.7144298976936, -0.04222039666226981, -5.692460589584668,
         -133.07083915645683, -315.73506165424584},
        {-143.259448604311, -6.461476288036018, -23.350832270413342,
         -109.82589360634722, -39.30214371168313},
        {-37.5461297619313, -3.896216254770047, -12.224515907127184,
         -70.54818792819488, -35.734595714425964}};
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
    ROD00000_ElementLength_PYEL = {0.10423172276102752, 0.028429779698777868,
                                   0.035990539874514844, 0.08868459084215102,
                                   0.09873993681715025};
    blaze::DynamicVector<elastica::real_t> ROD00000_ReferenceElementLength_PYEL(
        5UL);
    ROD00000_ReferenceElementLength_PYEL = {
        0.07451700459404914, 0.12877804874902377, 0.12125445792190077,
        0.12557378285646317, 0.06303696469708653};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Tangent_PYEL(3UL, 5UL);
    ROD00000_Tangent_PYEL = {
        {-0.3004044349850557, 0.7414942292477226, -0.9745601357852607,
         0.5039822824549645, -0.4861788560290695},
        {0.9532182690182781, 0.5750159403815838, -0.22382298183665875,
         0.12357716837812366, 0.7274227619260456},
        {0.03364679852685588, 0.34574987534257895, 0.011653949543442335,
         -0.8548277852451505, 0.48423779838231223}};
    blaze::DynamicVector<elastica::real_t> ROD00000_ElementDilatation_PYEL(5UL);
    ROD00000_ElementDilatation_PYEL = {1.3987642596325103, 0.2207657281264202,
                                       0.29681828191171433, 0.7062349228064726,
                                       1.5663815237873258};
    blaze::DynamicVector<elastica::real_t> ROD00000_VoronoiDilatation_PYEL(4UL);
    ROD00000_VoronoiDilatation_PYEL = {1.842520867497297, 0.8947266607401767,
                                       1.7315990377314705, 2.6031184397125178};
    blaze::DynamicVector<elastica::real_t> ROD00000_ReferenceVoronoiLength_PYEL(
        4UL);
    ROD00000_ReferenceVoronoiLength_PYEL = {0.036, 0.03599999999999999, 0.036,
                                            0.036000000000000004};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_ShearStretchStrain_PYEL(
        3UL, 5UL);
    ROD00000_ShearStretchStrain_PYEL = {
        {-0.20001494723553256, 0.0803563088926905, -0.008713055171612145,
         -0.5610303568647546, 0.6347312723963133},
        {0.2679788450880327, 0.16562346754268714, -0.29155552197116985,
         0.42276988871906857, -0.3806694437149416},
        {0.35820573312474835, -0.8781425532211206, -1.054959555626998,
         -1.0726520891656726, 0.3804919646728182}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Curvature_PYEL(3UL, 4UL);
    ROD00000_Curvature_PYEL = {{-12.650274753643016, -0.03245369839588621,
                                3.7960397830991024, 4.457755966080651},
                               {-3.914663865135914, 2.1077267225242986,
                                1.3868272586934733, -2.845157414208832},
                               {-0.6872399315306906, 0.4901238621117893,
                                1.5247827970129457, -0.33244230989610873}};
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
        {-0.019021938002569055, -0.027168268770048715, -0.037845861452311134,
         -0.02593003536115165, 0.01640231773088594},
        {-0.025526022430776212, -0.010499427932747857, -0.04614779418936111,
         -0.0208394877865064, -0.0527316202161335},
        {0.0019247143873199982, -0.0348323682325682, -0.0554739204187979,
         -0.07057976908535939, 0.009426227254194392}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_InternalCouple_PYEL(3UL,
                                                                        4UL);
    ROD00000_InternalCouple_PYEL = {
        {-0.000006387127683850267, -0.00000024999064138138916,
         0.0000014411774680523774, 0.000001842140837986798},
        {-0.0000022721252106665806, 0.0000009638644295650417,
         0.0000004075865888885694, -0.0000016863098769987108},
        {-0.0000008032248897370384, -0.00000009977631208565736,
         0.000001029449624607062, -0.000001075175120031123}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Position_PYEL(3UL, 6UL);
    ROD00000_Position_PYEL = {
        {0.08894758529855848, 0.05763854104006016, 0.07871524082502024,
         0.043641951541484425, 0.08833578096135512, 0.04033502343470823},
        {0.029577759584899888, 0.12893242057664484, 0.145279947906566,
         0.13722487594399846, 0.1481862062566107, 0.2200172784024361},
        {0.07488881920885194, 0.07839422239200697, 0.0882268690357034,
         0.08864770355270606, 0.012837927734919418, 0.060648515005870075}};
    blaze::DynamicTensor<elastica::real_t> ROD00000_Director_PYEL(3UL, 3UL,
                                                                  5UL);
    ROD00000_Director_PYEL = {
        {{0.04949775826943573, -0.08548343494601021, 0.03653354864992828,
          0.9493317190908913, 0.09900228730506494},
         {0.00629091571067672, 0.35073098164288646, 0.96072506146834,
          0.3036069831251458, 0.15385905130112848},
         {0.9987544224211582, 0.9325665450066545, -0.27508663378957127,
          0.08119166783346077, 0.9831205111485988}},
        {{0.9953000658846858, 0.9958358059568446, 0.866413494035693,
          -0.16169649382821608, 0.25511960999606076},
         {-0.08364429135254481, 0.059837919037121205, 0.10672255262987447,
          0.25032706459935405, -0.9588770150786124},
         {-0.04879970670065584, 0.06877841972296447, 0.4877888417266467,
          0.9545630438125765, 0.12437384994196848}},
        {{0.0832331110570782, -0.031680118751554256, 0.4979889127037904,
          0.26948753405601705, 0.9618277037514685},
         {0.9964758185242888, 0.9345625725231691, -0.256159428924251,
          -0.9193253833799627, 0.2385000257581668},
         {-0.010401553735298052, -0.3543856206376936, -0.8284862037458913,
          0.2867356072441803, -0.1341834788993437}}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Velocity_PYEL(3UL, 6UL);
    ROD00000_Velocity_PYEL = {
        {0.1436053027380654, 0.4063578074505897, 0.024577760673435187,
         0.19019211629468344, 0.02688298217315614, 0.4780829319559188},
        {0.15893111558688347, 0.06679558411225374, 0.06187774282741722,
         0.10567674857931017, 0.29964650525227154, 0.8391058943263872},
        {0.08571151935600005, -0.08034774988076256, 0.22503793281829618,
         0.36529605919992997, 0.392951777367542, 0.09071719788789588}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularVelocity_PYEL(3UL,
                                                                         5UL);
    ROD00000_AngularVelocity_PYEL = {
        {-19355221.73855501, -255.03236495396405, -46623.469170407,
         -2629686.5139088207, -13862087.375946516},
        {-5616238.280375362, -38892.436581584334, -191247.51031445016,
         -2170327.541916237, -1725527.4990186275},
        {-735065.0148581009, -11382.90382109183, -49212.0882152868,
         -695081.4764570851, -783544.2637127228}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_Acceleration_PYEL(3UL, 6UL);
    ROD00000_Acceleration_PYEL = {
        {-837.7022890436417, -632.0212587885076, -2207.2263581681605,
         2750.37482185007, -380.9578425823528, 1202.2936856213792},
        {-277.7297266189874, -3035.879476516955, -471.98697596555064,
         1601.5883325897641, 1672.0780934112609, 29.569543044788414},
        {-518.9909141173245, -2239.686678511111, -247.62352188866973,
         1640.9903002191077, 1131.8674336497568, -612.3306333182758}};
    blaze::DynamicMatrix<elastica::real_t> ROD00000_AngularAcceleration_PYEL(
        3UL, 5UL);
    ROD00000_AngularAcceleration_PYEL = {
        {-968945251328.302, -13077744.680480562, -2370661629.9229126,
         -131859363784.31183, -693903782448.5513},
        {-281155570968.06366, -2001438731.8494568, -9724603485.445225,
         -108826039948.1654, -86375919218.43098},
        {-36843306510.32235, -603425422.6388593, -2545488927.7962613,
         -34952959022.865204, -39267686966.08028}};

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

    std::size_t step = 2;
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
