//
// Created by Seung Hyun Kim on 11/9/21.
//

#include <iostream>

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

  using real_type = typename Traits::DataType::real_type;

  ELASTICA_TEST_CASE("Unit.z.Integrated.CosseratRods.Stepper:SingleRod,OneStep",
                     "[Systems][Integrated]") {
    /*
     */
    namespace ec = elastica::configuration;
    namespace et = elastica::timestepper;

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
        ec::make_blocking_policy(ec::RestrictSizeAcrossBlockTypes{50UL}),
        ec::make_iteration_policy(ec::iterate_across_system_types_in(ec::seq),
                                  ec::iterate_over_systems_in(ec::seq)));

    auto simulator_config = ec::make_simulator_configuration(
        ec::DefaultDomainConfiguration{}, system_config,
        ec::DefaultParallelConfiguration{});

    auto simulator = elastica::make_simulator<>(simulator_config);

    auto slicedrod = *(simulator->emplace_back<Plugin>(
        straight_initializer.initialize<Plugin>()));

    simulator->finalize();

    using Algorithm = et::symplectic::PositionVerlet;
    using TimeStepper = et::symplectic::SymplecticStepper<Algorithm>;
    const TimeStepper time_stepper{};

    ::elastica::Time time_v{0.0};
    ::elastica::TimeDelta time_delta{0.2};

    //**************************************************************************
    blaze::DynamicMatrix<elastica::real_t> InternalLoads_REPLACED(3UL, 6UL);
    InternalLoads_REPLACED = {
        {0.6703700886687535, 0.5599270904792274, 0.37978721939595683,
         0.2517851088005173, 0.7532804498119788, 0.831240507511035},
        {0.47781402266344997, 0.6950211678952081, 0.5153977473752267,
         0.5560888125838666, 0.2995297923411575, 0.4569164183770765},
        {0.9061628803336391, 0.9266585640581266, 0.35913849984522517,
         0.4168057452312872, 0.5179665933465903, 0.10411282840829195}};
    blaze::DynamicMatrix<elastica::real_t> InternalTorques_REPLACED(3UL, 5UL);
    InternalTorques_REPLACED = {
        {0.005446580181096006, 0.4128528364721107, 0.7959898243412399,
         0.6432890641383698, 0.8782875840719243},
        {0.10076240092266242, 0.49634830812601183, 0.32439486332849543,
         0.3715252187945519, 0.9236619681106076},
        {0.8906049364593669, 0.8929947521390232, 0.8007821567530773,
         0.9908026910504705, 0.9343406228935561}};
    blaze::DynamicMatrix<elastica::real_t> ExternalLoads_REPLACED(3UL, 6UL);
    ExternalLoads_REPLACED = {
        {0.1395213064814601, 0.8474314503230967, 0.2696127183057031,
         0.009970972559999569, 0.08155526350714615, 0.44619971524211255},
        {0.12251946914166034, 0.2611447683110364, 0.02586569069785072,
         0.030177485569313767, 0.4352966485887426, 0.9518404933230397},
        {0.07133267601259985, 0.018172074014147555, 0.5608764707571324,
         0.5370081638237247, 0.6906076351385936, 0.13266926049186845}};
    blaze::DynamicMatrix<elastica::real_t> ExternalTorques_REPLACED(3UL, 5UL);
    ExternalTorques_REPLACED = {
        {0.6018185132266004, 0.003938448160778485, 0.08946545496330316,
         0.38388587015468356, 0.3611628648311712},
        {0.17674353197593184, 0.18411277805131043, 0.38936582363725436,
         0.32013002586294814, 0.04595243267756288},
        {0.09208604065501258, 0.21934003097954202, 0.412093588441637,
         0.39803407751858466, 0.08520800443850063}};
    blaze::DynamicMatrix<elastica::real_t> InternalStress_REPLACED(3UL, 5UL);
    InternalStress_REPLACED = {
        {0.48845156990161287, 0.42703068123353516, 0.2111597780763791,
         0.7527081614401365, 0.7585091573224211},
        {0.859799916302963, 0.09980576332201718, 0.5912967371146306,
         0.3314992134943602, 0.1323825532380214},
        {0.5542949860767826, 0.6344052832194877, 0.3019910205525258,
         0.6556067934852766, 0.10056167410384631}};
    blaze::DynamicMatrix<elastica::real_t> InternalCouple_REPLACED(3UL, 4UL);
    InternalCouple_REPLACED = {{0.42196851675485547, 0.20095521733492605,
                                0.20438860427627925, 0.592736970599767},
                               {0.023067352983699885, 0.8262852908468907,
                                0.609368847923943, 0.7967177574305179},
                               {0.12780115215826082, 0.1801123210891723,
                                0.01010111051474083, 0.8292446512357197}};
    blaze::DynamicMatrix<elastica::real_t> ReferenceShearStretchStrain_REPLACED(
        3UL, 5UL);
    ReferenceShearStretchStrain_REPLACED = {
        {0.297597441176643, 0.791076063623359, 0.9813316766668237,
         0.11729731016539668, 0.20564794237709072},
        {0.9357375659253002, 0.4402876689437507, 0.9156671113598859,
         0.9679292352235765, 0.9987855559307337},
        {0.3040655442953162, 0.10165528375219623, 0.5054634070571163,
         0.9126824887816438, 0.11534211862120414}};
    blaze::DynamicMatrix<elastica::real_t> ReferenceCurvature_REPLACED(3UL,
                                                                       4UL);
    ReferenceCurvature_REPLACED = {{0.051148978603062, 0.4646769716719006,
                                    0.9301184173210795, 0.7944799966795505},
                                   {0.6036777904356718, 0.19098869154676612,
                                    0.5763017409174203, 0.5082335545968735},
                                   {0.30308030515914375, 0.6131410921095497,
                                    0.2555432491263159, 0.9931735829341407}};
    blaze::DynamicMatrix<elastica::real_t> ShearStretchStrain_REPLACED(3UL,
                                                                       5UL);
    ShearStretchStrain_REPLACED = {
        {0.9848603196130776, 0.7628387920573857, 0.3981263185067315,
         0.7752103618012558, 0.4981087231895296},
        {0.44372987089924676, 0.47981333159408723, 0.27953052942940315,
         0.039410528091022745, 0.12135498080172424},
        {0.04502860003414588, 0.39606048704701324, 0.638802231688118,
         0.34039507614487385, 0.3222690451538498}};
    blaze::DynamicMatrix<elastica::real_t> Curvature_REPLACED(3UL, 4UL);
    Curvature_REPLACED = {{0.46030989590247284, 0.6483169447319955,
                           0.25693207884047353, 0.7147848976761505},
                          {0.5678591352687159, 0.6481755476704774,
                           0.7111537582140289, 0.6592641079581886},
                          {0.4768374539110407, 0.10965068835379443,
                           0.8176421954155958, 0.003583635388100248}};
    blaze::DynamicVector<elastica::real_t> ReferenceElementLength_REPLACED(5UL);
    ReferenceElementLength_REPLACED = {0.07451700459404914, 0.12877804874902377,
                                       0.12125445792190077, 0.12557378285646317,
                                       0.06303696469708653};
    blaze::DynamicMatrix<elastica::real_t> Position_REPLACED(3UL, 6UL);
    Position_REPLACED = {
        {0.08894214952049244, 0.05762523708273925, 0.07871356476107469,
         0.04363652299969875, 0.08833475012599382, 0.04032049847902628},
        {0.02957220489754161, 0.12892832447280386, 0.1452771836398962,
         0.13722209882380804, 0.1481773167166555, 0.21999128268067816},
        {0.07488558762412101, 0.07839570688697475, 0.08821991684367064,
         0.0886365573296124, 0.012825841069887711, 0.06064545654195839}};
    blaze::DynamicTensor<elastica::real_t> Director_REPLACED(3UL, 3UL, 5UL);
    Director_REPLACED = {
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
    blaze::DynamicMatrix<elastica::real_t> Velocity_REPLACED(3UL, 6UL);
    Velocity_REPLACED = {
        {0.07925380682706683, 0.0860414593881338, 0.0055840582078969915,
         0.08229282255627174, 0.0071962759065840156, 0.06633851975206956},
        {0.06756620000198289, 0.08778845270232372, 0.07191395945987576,
         0.024745306610474773, 0.05689760350195482, 0.08343727453787116},
        {0.045264278461788744, 0.0030062857233150277, 0.010200463945095085,
         0.08437374377569423, 0.07508586841387065, 0.009201572172750961}};
    blaze::DynamicMatrix<elastica::real_t> Acceleration_REPLACED(3UL, 6UL);
    Acceleration_REPLACED = {
        {0.09571408426343082, 0.005107180633195741, 0.0732126259144127,
         0.050441490491986186, 0.05486230099210937, 0.0440326013977175},
        {0.09941018300218248, 0.013618114920622882, 0.051835720941648344,
         0.05022501039227099, 0.09049134977350463, 0.06427073323568147},
        {0.014063593772195671, 0.014344551773854353, 0.027437996786544197,
         0.059461465451936736, 0.07456314681696873, 0.059811903960076684}};
    blaze::DynamicMatrix<elastica::real_t> AngularVelocity_REPLACED(3UL, 5UL);
    AngularVelocity_REPLACED = {
        {0.09851356354829398, 0.04033172864324111, 0.020292569398976515,
         0.06902762055710024, 0.026847114751602664},
        {0.03866628570001665, 0.08212084544018176, 0.09706746517912329,
         0.021648310689029695, 0.07631296141009615},
        {0.004526510466830236, 0.03302764459795905, 0.005283833550639883,
         0.046620199055542225, 0.07837425516095967}};
    blaze::DynamicMatrix<elastica::real_t> AngularAcceleration_REPLACED(3UL,
                                                                        5UL);
    AngularAcceleration_REPLACED = {
        {0.057039050557071806, 0.07502520173913291, 0.08204218079523058,
         0.023910778516111564, 0.07949498033136443},
        {0.03912707550921284, 0.06589885029187668, 0.040550375265082574,
         0.08510826947147253, 0.03005671672516207},
        {0.09295270631990539, 0.07146242008590545, 0.04433061390912978,
         0.05706921605595977, 0.0643979182090123}};
    blaze::DynamicVector<elastica::real_t> ElementDimension_PYEL(5UL);
    ElementDimension_PYEL = {0.004385484267964851, 0.009548190359889389,
                             0.008075465362449064, 0.0048541653807997525,
                             0.0046313571524989605};
    blaze::DynamicTensor<elastica::real_t> MassSecondMomentOfInertia_PYEL(
        3UL, 3UL, 5UL);
    MassSecondMomentOfInertia_PYEL = {
        {{7.127235497145191e-10, 7.127235497145191e-10, 7.12723549714519e-10,
          7.127235497145191e-10, 7.127235497145191e-10},
         {0.0, 0.0, 0.0, 0.0, 0.0},
         {0.0, 0.0, 0.0, 0.0, 0.0}},
        {{0.0, 0.0, 0.0, 0.0, 0.0},
         {7.127235497145191e-10, 7.127235497145191e-10, 7.12723549714519e-10,
          7.127235497145191e-10, 7.127235497145191e-10},
         {0.0, 0.0, 0.0, 0.0, 0.0}},
        {{0.0, 0.0, 0.0, 0.0, 0.0},
         {0.0, 0.0, 0.0, 0.0, 0.0},
         {1.4254470994290381e-09, 1.4254470994290381e-09, 1.425447099429038e-09,
          1.4254470994290381e-09, 1.4254470994290381e-09}}};
    blaze::DynamicTensor<elastica::real_t> InvMassSecondMomentOfInertia_PYEL(
        3UL, 3UL, 5UL);
    InvMassSecondMomentOfInertia_PYEL = {
        {{1403068553.579504, 1403068553.579504, 1403068553.5795043,
          1403068553.579504, 1403068553.579504},
         {0.0, 0.0, 0.0, 0.0, 0.0},
         {0.0, 0.0, 0.0, 0.0, 0.0}},
        {{0.0, 0.0, 0.0, 0.0, 0.0},
         {1403068553.579504, 1403068553.579504, 1403068553.5795043,
          1403068553.579504, 1403068553.579504},
         {0.0, 0.0, 0.0, 0.0, 0.0}},
        {{0.0, 0.0, 0.0, 0.0, 0.0},
         {0.0, 0.0, 0.0, 0.0, 0.0},
         {701534276.789752, 701534276.789752, 701534276.7897521,
          701534276.789752, 701534276.789752}}};
    blaze::DynamicTensor<elastica::real_t> ShearStretchRigidityMatrix_PYEL(
        3UL, 3UL, 5UL);
    ShearStretchRigidityMatrix_PYEL = {
        {{0.03822641567117308, 0.03822641567117308, 0.03822641567117308,
          0.03822641567117308, 0.03822641567117308},
         {0.0, 0.0, 0.0, 0.0, 0.0},
         {0.0, 0.0, 0.0, 0.0, 0.0}},
        {{0.0, 0.0, 0.0, 0.0, 0.0},
         {0.03822641567117308, 0.03822641567117308, 0.03822641567117308,
          0.03822641567117308, 0.03822641567117308},
         {0.0, 0.0, 0.0, 0.0, 0.0}},
        {{0.0, 0.0, 0.0, 0.0, 0.0},
         {0.0, 0.0, 0.0, 0.0, 0.0},
         {0.035550566574190964, 0.035550566574190964, 0.035550566574190964,
          0.035550566574190964, 0.035550566574190964}}};
    blaze::DynamicTensor<elastica::real_t> BendingTwistRigidityMatrix_PYEL(
        3UL, 3UL, 4UL);
    BendingTwistRigidityMatrix_PYEL = {
        {{5.028670658104868e-07, 5.028670658104868e-07, 5.028670658104868e-07,
          5.028670658104868e-07},
         {0.0, 0.0, 0.0, 0.0},
         {0.0, 0.0, 0.0, 0.0}},
        {{0.0, 0.0, 0.0, 0.0},
         {5.028670658104868e-07, 5.028670658104868e-07, 5.028670658104868e-07,
          5.028670658104868e-07},
         {0.0, 0.0, 0.0, 0.0}},
        {{0.0, 0.0, 0.0, 0.0},
         {0.0, 0.0, 0.0, 0.0},
         {8.110759125975593e-07, 8.110759125975594e-07, 8.110759125975593e-07,
          8.110759125975593e-07}}};
    blaze::DynamicVector<elastica::real_t> ElementVolume_PYEL(5UL);
    ElementVolume_PYEL = {6.3991019833543736e-06, 6.3991019833543736e-06,
                          6.399101983354373e-06, 6.399101983354375e-06,
                          6.399101983354375e-06};
    blaze::DynamicVector<elastica::real_t> Mass_PYEL(6UL);
    Mass_PYEL = {2.5193264508466167e-05, 5.038652901693233e-05,
                 5.0386529016932326e-05, 5.038652901693234e-05,
                 5.038652901693235e-05,  2.5193264508466173e-05};
    blaze::DynamicMatrix<elastica::real_t> InternalLoads_PYEL(3UL, 6UL);
    InternalLoads_PYEL = {
        {-0.03620163086771273, -0.0353089165032898, -0.09882457404609565,
         0.13374494279847832, -0.008568828315390326, 0.041457291433268775},
        {2.0932929973587052e-05, -0.20653343803580024, -0.01484778414589721,
         0.12421803640909575, 0.09443290568023649, -0.001633143033198246},
        {-0.008870610683550299, -0.14736799435378053, 0.01401855919973691,
         0.09909483014523232, 0.052543079110871374, -0.012432862388289065}};
    blaze::DynamicMatrix<elastica::real_t> InternalTorques_PYEL(3UL, 5UL);
    InternalTorques_PYEL = {
        {0.0014410585813778983, -0.0018401305476610276, 0.004068710258553392,
         -0.0046448085879460255, 0.002629722565895596},
        {-0.001823187157499998, -0.0003911773462122025, 0.0026622752273588704,
         -0.007940773146067748, -0.0007863096622657886},
        {-0.00018021698372160106, 0.0011328471971337965, -0.005134388882457037,
         0.0035247687020530404, -0.0026539958587741956}};
    blaze::DynamicMatrix<elastica::real_t> ExternalLoads_PYEL(3UL, 6UL);
    ExternalLoads_PYEL = {{0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
                          {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
                          {0.0, 0.0, 0.0, 0.0, 0.0, 0.0}};
    blaze::DynamicMatrix<elastica::real_t> ExternalTorques_PYEL(3UL, 5UL);
    ExternalTorques_PYEL = {{0.0, 0.0, 0.0, 0.0, 0.0},
                            {0.0, 0.0, 0.0, 0.0, 0.0},
                            {0.0, 0.0, 0.0, 0.0, 0.0}};
    blaze::DynamicVector<elastica::real_t> ElementLength_PYEL(5UL);
    ElementLength_PYEL = {0.10590928695904903, 0.022342255874381996,
                          0.031234462845741835, 0.08644502848184751,
                          0.09496260336199729};
    blaze::DynamicVector<elastica::real_t> ReferenceElementLength_PYEL(5UL);
    ReferenceElementLength_PYEL = {0.07451700459404914, 0.12877804874902377,
                                   0.12125445792190077, 0.12557378285646317,
                                   0.06303696469708653};
    blaze::DynamicMatrix<elastica::real_t> Tangent_PYEL(3UL, 5UL);
    Tangent_PYEL = {
        {-0.28928669110474764, 0.5837632347262933, -0.8774335406979703,
         0.4301990885356174, -0.44333269910402245},
        {0.9572186515097149, 0.6606946910751847, -0.4089057066262134,
         0.16392437865841053, 0.7841816718497352},
        {-0.006757481157156297, 0.47191419873421986, 0.2508116918063044,
         -0.8877260513832959, 0.4341833983930021}};
    blaze::DynamicVector<elastica::real_t> ElementDilatation_PYEL(5UL);
    ElementDilatation_PYEL = {1.4212767614052328, 0.17349428797391503,
                              0.25759434647639723, 0.688400289578425,
                              1.5064590089057113};
    blaze::DynamicVector<elastica::real_t> VoronoiDilatation_PYEL(4UL);
    VoronoiDilatation_PYEL = {1.7812714282420976, 0.7441210933350534,
                              1.6344373795498521, 2.5195504422756216};
    blaze::DynamicVector<elastica::real_t> ReferenceVoronoiLength_PYEL(4UL);
    ReferenceVoronoiLength_PYEL = {0.036, 0.03599999999999999, 0.036,
                                   0.036000000000000004};
    blaze::DynamicMatrix<elastica::real_t> ShearStretchStrain_PYEL(3UL, 5UL);
    ShearStretchStrain_PYEL = {
        {-0.07078633722088204, 0.08399007298653167, 0.06259769389163937,
         -0.6025981350595794, 0.5916546028481369},
        {-0.3638871812247525, 0.1038537991936856, -0.23057423810603558,
         0.3223280288383566, -0.66959289707798},
        {0.3720798250516266, -0.8892736526732212, -1.0962906889469175,
         -0.9170838487412987, 0.21285156908193348}};
    blaze::DynamicMatrix<elastica::real_t> Curvature_PYEL(3UL, 4UL);
    Curvature_PYEL = {{0.36575131739371536, -0.5470056360632829,
                       0.24329541116635353, -0.2938694276482442},
                      {-0.6594926148006188, 0.9707115562343315,
                       -0.41597169376563564, -0.21082234751431958},
                      {-0.05422417113591109, -0.132991692542048,
                       0.24010631499156723, 0.41487659535940563}};
    blaze::DynamicMatrix<elastica::real_t> ReferenceShearStretchStrain_PYEL(
        3UL, 5UL);
    ReferenceShearStretchStrain_PYEL = {
        {0.297597441176643, 0.791076063623359, 0.9813316766668237,
         0.11729731016539668, 0.20564794237709072},
        {0.9357375659253002, 0.4402876689437507, 0.9156671113598859,
         0.9679292352235765, 0.9987855559307337},
        {0.3040655442953162, 0.10165528375219623, 0.5054634070571163,
         0.9126824887816438, 0.11534211862120414}};
    blaze::DynamicMatrix<elastica::real_t> ReferenceCurvature_PYEL(3UL, 4UL);
    ReferenceCurvature_PYEL = {{0.051148978603062, 0.4646769716719006,
                                0.9301184173210795, 0.7944799966795505},
                               {0.6036777904356718, 0.19098869154676612,
                                0.5763017409174203, 0.5082335545968735},
                               {0.30308030515914375, 0.6131410921095497,
                                0.2555432491263159, 0.9931735829341407}};
    blaze::DynamicMatrix<elastica::real_t> InternalStress_PYEL(3UL, 5UL);
    InternalStress_PYEL = {
        {-0.014081991439541102, -0.02702936299334656, -0.03511990711679657,
         -0.02751902252895415, 0.014755651055007585},
        {-0.04967999580110113, -0.012860660950927293, -0.04381669828417068,
         -0.024679020073092216, -0.0637761282415398},
        {0.00241794621602293, -0.03522808512468405, -0.056943265625474464,
         -0.06504922999732295, 0.0034665162102169344}};
    blaze::DynamicMatrix<elastica::real_t> InternalCouple_PYEL(3UL, 4UL);
    InternalCouple_PYEL = {{1.5820315500477254e-07, -5.087418644832934e-07,
                            -3.4538066983616496e-07, -5.472950815882506e-07},
                           {-6.352067952998171e-07, 3.9209694911078325e-07,
                            -4.989816305807621e-07, -3.615895316483683e-07},
                           {-2.898010541862046e-07, -6.051703292302529e-07,
                            -1.252052544104967e-08, -4.6904275694959764e-07}};
    blaze::DynamicMatrix<elastica::real_t> Position_PYEL(3UL, 6UL);
    Position_PYEL = {
        {82.12645832786824, 322.4318403181735, 67.87102282080157,
         57.10546739731317, 29.060388419818302, 387.18643199208975},
        {97.32337401603917, 21.823442670187543, 4.533014031434772,
         61.42661563002596, 210.42589770442297, 754.5711159999963},
        {49.670259666641265, -51.20293209927687, 228.2842004033743,
         252.59482000809734, 295.0077627467627, 95.51371088963793}};
    blaze::DynamicTensor<elastica::real_t> Director_PYEL(3UL, 3UL, 5UL);
    Director_PYEL = {
        {{0.06680704756130182, 0.1314687738202628, -0.14687073092702368,
          0.9219575496111007, 0.005446771159919682},
         {0.20655174265401474, -0.1584480223207004, 0.3077103848005619,
          0.386759573725485, 0.09524257031165695},
         {0.9761522401770799, 0.9785755901987618, 0.940076224293994,
          0.02028075064626205, 0.9954391922585529}},
        {{0.6965420384077518, 0.9906683489948107, 0.9858494847769395,
          -0.2740630444009588, 0.7352328791578983},
         {-0.7101433967141464, 0.05679504353272528, -0.03210316259179969,
          0.6145199397940789, -0.6751028604095581},
         {0.10259407796782619, -0.12389731768691213, 0.16453018056468321,
          0.7397666465103437, 0.06057013514942613}},
        {{0.7143990531397113, -0.035946958287070216, 0.08080706505005984,
          0.27364890719455237, 0.6777927014132851},
         {0.6730770637229319, 0.9857324927447477, 0.9509383292503752,
          -0.6875916489655488, 0.7315497116855841},
         {-0.19131455554452725, 0.16443621540625705, -0.2986411093612209,
          0.6725578041061337, -0.07370259997869617}}};
    blaze::DynamicMatrix<elastica::real_t> Velocity_PYEL(3UL, 6UL);
    Velocity_PYEL = {
        {820.2959079766504, 3223.6561093515193, 677.9175085021969,
         570.5360159205784, 289.7133404210165, 3871.394776416355},
        {972.8704519114142, 216.85735500444508, 43.80545451848888,
         612.869190005411, 2102.720306273561, 7543.427809898618},
        {495.90847651170964, -512.8162843473617, 2281.9496044013613,
         2524.9774607639015, 2949.8742831885143, 954.5214527587871}};
    blaze::DynamicMatrix<elastica::real_t> AngularVelocity_PYEL(3UL, 5UL);
    AngularVelocity_PYEL = {
        {240597861.87798795, 102156.37300145919, 6761070.893617938,
         73259725.16360116, 153787150.9851524},
        {69763436.72797117, 8944463.41928048, 28337557.335877284,
         60307021.46345562, 19093213.12953441},
        {18327388.15861992, 5366847.393710957, 14708422.239261435,
         38785476.70608941, 17449154.101139784}};
    blaze::DynamicMatrix<elastica::real_t> Acceleration_PYEL(3UL, 6UL);
    Acceleration_PYEL = {
        {4101.083270849116, 16117.850339460654, 3389.559622219945,
         2852.2686154901107, 1448.5307207255494, 19356.642189483013},
        {4864.0144285570605, 1083.8478327587138, 218.667702795145,
         3064.2222234940027, 10513.317043350296, 37716.7218631204},
        {2479.3160611662393, -2564.0964531654254, 11409.69701968708,
         12624.465435100627, 14748.995986600503, 4772.561255933071}};
    blaze::DynamicMatrix<elastica::real_t> AngularAcceleration_PYEL(3UL, 5UL);
    AngularAcceleration_PYEL = {
        {1202989308.8973718, 510781.6633486527, 33805354.366626844,
         366298625.4728677, 768935754.7915263},
        {348817183.4465244, 44722316.685798176, 141687786.1940491,
         301535107.2090365, 95466065.26610722},
        {91636940.77046706, 26834236.80341656, 73542111.169888,
         193927383.29734606, 87245770.11382763}};

    blocks::get<elastica::tags::InternalLoads>(slicedrod) =
        InternalLoads_REPLACED;
    blocks::get<elastica::tags::InternalTorques>(slicedrod) =
        InternalTorques_REPLACED;
    blocks::get<elastica::tags::ExternalLoads>(slicedrod) =
        ExternalLoads_REPLACED;
    blocks::get<elastica::tags::ExternalTorques>(slicedrod) =
        ExternalTorques_REPLACED;
    blocks::get<elastica::tags::InternalStress>(slicedrod) =
        InternalStress_REPLACED;
    blocks::get<elastica::tags::InternalCouple>(slicedrod) =
        InternalCouple_REPLACED;
    blocks::get<elastica::tags::ReferenceShearStretchStrain>(slicedrod) =
        ReferenceShearStretchStrain_REPLACED;
    blocks::get<elastica::tags::ReferenceCurvature>(slicedrod) =
        ReferenceCurvature_REPLACED;
    blocks::get<elastica::tags::ShearStretchStrain>(slicedrod) =
        ShearStretchStrain_REPLACED;
    blocks::get<elastica::tags::Curvature>(slicedrod) = Curvature_REPLACED;
    blocks::get<elastica::tags::ReferenceElementLength>(slicedrod) =
        ReferenceElementLength_REPLACED;
    blocks::get<elastica::tags::Position>(slicedrod) = Position_REPLACED;
    blocks::get<elastica::tags::Director>(slicedrod) = Director_REPLACED;
    blocks::get<elastica::tags::Velocity>(slicedrod) = Velocity_REPLACED;
    blocks::get<elastica::tags::Acceleration>(slicedrod) =
        Acceleration_REPLACED;
    blocks::get<elastica::tags::AngularVelocity>(slicedrod) =
        AngularVelocity_REPLACED;
    blocks::get<elastica::tags::AngularAcceleration>(slicedrod) =
        AngularAcceleration_REPLACED;

    simulator->step(time_stepper, time_v, time_delta);

    auto ElementDimension(
        blocks::get<elastica::tags::ElementDimension>(slicedrod));
    auto MassSecondMomentOfInertia(
        blocks::get<elastica::tags::MassSecondMomentOfInertia>(slicedrod));
    auto InvMassSecondMomentOfInertia(
        blocks::get<elastica::tags::InvMassSecondMomentOfInertia>(slicedrod));
    auto ShearStretchRigidityMatrix(
        blocks::get<elastica::tags::ShearStretchRigidityMatrix>(slicedrod));
    auto BendingTwistRigidityMatrix(
        blocks::get<elastica::tags::BendingTwistRigidityMatrix>(slicedrod));
    auto ElementVolume(blocks::get<elastica::tags::ElementVolume>(slicedrod));
    auto Mass(blocks::get<elastica::tags::Mass>(slicedrod));
    auto InternalLoads(blocks::get<elastica::tags::InternalLoads>(slicedrod));
    auto InternalTorques(
        blocks::get<elastica::tags::InternalTorques>(slicedrod));
    auto ExternalLoads(blocks::get<elastica::tags::ExternalLoads>(slicedrod));
    auto ExternalTorques(
        blocks::get<elastica::tags::ExternalTorques>(slicedrod));
    auto ElementLength(blocks::get<elastica::tags::ElementLength>(slicedrod));
    auto ReferenceElementLength(
        blocks::get<elastica::tags::ReferenceElementLength>(slicedrod));
    auto Tangent(blocks::get<elastica::tags::Tangent>(slicedrod));
    auto ElementDilatation(
        blocks::get<elastica::tags::ElementDilatation>(slicedrod));
    auto VoronoiDilatation(
        blocks::get<elastica::tags::VoronoiDilatation>(slicedrod));
    auto ReferenceVoronoiLength(
        blocks::get<elastica::tags::ReferenceVoronoiLength>(slicedrod));
    auto ShearStretchStrain(
        blocks::get<elastica::tags::ShearStretchStrain>(slicedrod));
    auto Curvature(blocks::get<elastica::tags::Curvature>(slicedrod));
    auto ReferenceShearStretchStrain(
        blocks::get<elastica::tags::ReferenceShearStretchStrain>(slicedrod));
    auto ReferenceCurvature(
        blocks::get<elastica::tags::ReferenceCurvature>(slicedrod));
    auto InternalStress(blocks::get<elastica::tags::InternalStress>(slicedrod));
    auto InternalCouple(blocks::get<elastica::tags::InternalCouple>(slicedrod));
    auto Position(blocks::get<elastica::tags::Position>(slicedrod));
    auto Director(blocks::get<elastica::tags::Director>(slicedrod));
    auto Velocity(blocks::get<elastica::tags::Velocity>(slicedrod));
    auto AngularVelocity(
        blocks::get<elastica::tags::AngularVelocity>(slicedrod));
    auto Acceleration(blocks::get<elastica::tags::Acceleration>(slicedrod));
    auto AngularAcceleration(
        blocks::get<elastica::tags::AngularAcceleration>(slicedrod));

    CHECK_ITERABLE_APPROX(ElementDimension, ElementDimension_PYEL);
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK((blaze::pageslice(MassSecondMomentOfInertia, i) -
             blaze::pageslice(MassSecondMomentOfInertia_PYEL, i)) ==
            Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK((blaze::pageslice(InvMassSecondMomentOfInertia, i) -
             blaze::pageslice(InvMassSecondMomentOfInertia_PYEL, i)) ==
            Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK((blaze::pageslice(ShearStretchRigidityMatrix, i) -
             blaze::pageslice(ShearStretchRigidityMatrix_PYEL, i)) ==
            Approx(0.0).margin(1e-6));
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK((blaze::pageslice(BendingTwistRigidityMatrix, i) -
             blaze::pageslice(BendingTwistRigidityMatrix_PYEL, i)) ==
            Approx(0.0).margin(1e-6));
    CHECK_ITERABLE_APPROX(ElementVolume, ElementVolume_PYEL);
    CHECK_ITERABLE_APPROX(Mass, Mass_PYEL);
    CHECK_MATRIX_APPROX(InternalLoads, InternalLoads_PYEL);
    CHECK_MATRIX_APPROX(InternalTorques, InternalTorques_PYEL);
    CHECK_MATRIX_APPROX(ExternalLoads, ExternalLoads_PYEL);
    CHECK_MATRIX_APPROX(ExternalTorques, ExternalTorques_PYEL);
    CHECK_ITERABLE_APPROX(ElementLength, ElementLength_PYEL);
    CHECK_ITERABLE_APPROX(ReferenceElementLength, ReferenceElementLength_PYEL);
    CHECK_MATRIX_APPROX(Tangent, Tangent_PYEL);
    CHECK_ITERABLE_APPROX(ElementDilatation, ElementDilatation_PYEL);
    CHECK_ITERABLE_APPROX(VoronoiDilatation, VoronoiDilatation_PYEL);
    CHECK_ITERABLE_APPROX(ReferenceVoronoiLength, ReferenceVoronoiLength_PYEL);
    CHECK_MATRIX_APPROX(ShearStretchStrain, ShearStretchStrain_PYEL);
    CHECK_MATRIX_APPROX(Curvature, Curvature_PYEL);
    CHECK_MATRIX_APPROX(ReferenceShearStretchStrain,
                        ReferenceShearStretchStrain_PYEL);
    CHECK_MATRIX_APPROX(ReferenceCurvature, ReferenceCurvature_PYEL);
    CHECK_MATRIX_APPROX(InternalStress, InternalStress_PYEL);
    CHECK_MATRIX_APPROX(InternalCouple, InternalCouple_PYEL);
    CHECK_MATRIX_APPROX(Position, Position_PYEL);
    for (std::size_t i(0UL); i < 3UL; ++i)
      CHECK((blaze::pageslice(Director, i) -
             blaze::pageslice(Director_PYEL, i)) == Approx(0.0).margin(1e-6));
    CHECK_MATRIX_APPROX(Velocity, Velocity_PYEL);
    CHECK_MATRIX_APPROX(AngularVelocity, AngularVelocity_PYEL);
    CHECK_MATRIX_APPROX(Acceleration, Acceleration_PYEL);
    CHECK_MATRIX_APPROX(AngularAcceleration, AngularAcceleration_PYEL);
  }

}  // namespace
