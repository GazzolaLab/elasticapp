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

  ELASTICA_TEST_CASE("Unit.z.Integrated.CosseratRods.Stepper:SingleRod,NoStep",
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

    ::elastica::Time time_v{0.0};
    ::elastica::TimeDelta time_delta{0.2};

    //**************************************************************************

    blaze::DynamicMatrix<elastica::real_t> InternalLoads_REPLACED(3UL, 6UL);
    InternalLoads_REPLACED = {
        {0.203957429946802, 0.6553728672957231, 0.07586006916499055,
         0.2266804768321402, 0.023150782148356863, 0.8267331529503883},
        {0.15482201584295285, 0.5341137261531649, 0.8217521666658159,
         0.08207419303514252, 0.2903135892954033, 0.2571735009896835},
        {0.14465615688052758, 0.6064242289540898, 0.5539788198297757,
         0.5294675598323154, 0.04860584329765816, 0.4773015165169989}};
    blaze::DynamicMatrix<elastica::real_t> InternalTorques_REPLACED(3UL, 5UL);
    InternalTorques_REPLACED = {
        {0.7350951313834501, 0.7091748026745681, 0.15380848652812207,
         0.04987070974939489, 0.4252467292498079},
        {0.7562052074199033, 0.8984525966266456, 0.2720759351923555,
         0.8572655759809178, 0.5079311479102631},
        {0.05713725081795762, 0.9519014792912509, 0.35573031120251863,
         0.7560601197761251, 0.6850910122901672}};
    blaze::DynamicMatrix<elastica::real_t> ExternalLoads_REPLACED(3UL, 6UL);
    ExternalLoads_REPLACED = {
        {0.1791075438221752, 0.9899214316476954, 0.7463701769482871,
         0.5090261497759466, 0.9544198771087045, 0.9468784978450715},
        {0.682594016831527, 0.4165951128802907, 0.3851920271071624,
         0.8590050248260485, 0.8718064970595327, 0.7653391073424969},
        {0.023139976989049904, 0.5033417877320233, 0.4084747987104368,
         0.2787462316282022, 0.28563869362760064, 0.9108284397359842}};
    blaze::DynamicMatrix<elastica::real_t> ExternalTorques_REPLACED(3UL, 5UL);
    ExternalTorques_REPLACED = {
        {0.2011706275871653, 0.35044024901157045, 0.3469039633761569,
         0.730724784965762, 0.5070191807639516},
        {0.9066304124156365, 0.8407550791041166, 0.013837335101241854,
         0.33352982437168033, 0.03454364086259509},
        {0.4341406093148956, 0.8678259663477438, 0.8695583275771691,
         0.8563534969675165, 0.38881434776187773}};
    blaze::DynamicMatrix<elastica::real_t> InternalStress_REPLACED(3UL, 5UL);
    InternalStress_REPLACED = {
        {0.7596379212252692, 0.3721425106646684, 0.37886398601387816,
         0.2242284930736973, 0.8432162709655906},
        {0.006054751261704738, 0.48548428952997214, 0.519602535005509,
         0.7863058184251852, 0.862033563570232},
        {0.3951042311864247, 0.2986331591155028, 0.5703814493289531,
         0.909277466665711, 0.9909018868251039}};
    blaze::DynamicMatrix<elastica::real_t> InternalCouple_REPLACED(3UL, 4UL);
    InternalCouple_REPLACED = {{0.4311385046965821, 0.6832921248581908,
                                0.8865673324660336, 0.1672225452678583},
                               {0.2938057843773313, 0.3857070159278747,
                                0.8153466251274433, 0.2208439831421144},
                               {0.09631586788311886, 0.8574195723473468,
                                0.9543850950940012, 0.6935530433914594}};
    blaze::DynamicMatrix<elastica::real_t> ReferenceShearStretchStrain_REPLACED(
        3UL, 5UL);
    ReferenceShearStretchStrain_REPLACED = {
        {0.7184804985324387, 0.6583747305005937, 0.364722264668331,
         0.9093049831040217, 0.45994236798265253},
        {0.3060060659583749, 0.7782948087672685, 0.9241066144615926,
         0.2995753702543501, 0.9044553750545788},
        {0.9642671548467786, 0.45804198361957094, 0.33837614563409246,
         0.41542573531258753, 0.9118989166105574}};
    blaze::DynamicMatrix<elastica::real_t> ReferenceCurvature_REPLACED(3UL,
                                                                       4UL);
    ReferenceCurvature_REPLACED = {{0.8711649329104771, 0.18219259883358419,
                                    0.604351208520874, 0.6482932130639908},
                                   {0.8154536694563059, 0.7224160802374119,
                                    0.929567645128783, 0.1977950177866734},
                                   {0.23536632862462836, 0.28433022419255993,
                                    0.44594753847623225, 0.02922136066212011}};
    blaze::DynamicMatrix<elastica::real_t> ShearStretchStrain_REPLACED(3UL,
                                                                       5UL);
    ShearStretchStrain_REPLACED = {
        {0.17015537742449616, 0.24188911472131192, 0.12951526410122371,
         0.6410513197951541, 0.6697892199549795},
        {0.17516497260853625, 0.31704036797809554, 0.2762522726764465,
         0.3198716032399259, 0.07718805919824767},
        {0.18329113443805112, 0.30371975879604374, 0.9994931973914124,
         0.24676623009938614, 0.6209416000520357}};
    blaze::DynamicMatrix<elastica::real_t> Curvature_REPLACED(3UL, 4UL);
    Curvature_REPLACED = {{0.11181426579214082, 0.2518362108425869,
                           0.14002481988864546, 0.9348913021909042},
                          {0.04802068166268403, 0.40716714832443246,
                           0.6487104333959989, 0.23171343732239702},
                          {0.938828576197833, 0.5867739522581421,
                           0.030625780555092907, 0.795681353275379}};
    blaze::DynamicVector<elastica::real_t> ReferenceElementLength_REPLACED(5UL);
    ReferenceElementLength_REPLACED = {0.10619813470026732, 0.06846781592702295,
                                       0.08932266024355683, 0.05339723069495667,
                                       0.06191007336705552};
    blaze::DynamicMatrix<elastica::real_t> Position_REPLACED(3UL, 6UL);
    Position_REPLACED = {
        {0.08870522268171689, 0.024612313410292953, 0.01793587216973136,
         0.0010584322218191832, 0.013381251500294357, 0.07315923638365135},
        {0.09792447196272275, 0.03980292966236467, 0.09801932960152229,
         0.16346768585830138, 0.19246603367494378, 0.18873080125423047},
        {0.007504401265764327, 0.011626806829448355, 0.038039767086985435,
         0.01195050117458274, 0.08671144050250446, 0.07553838100418793}};
    blaze::DynamicTensor<elastica::real_t> Director_REPLACED(3UL, 3UL, 5UL);
    Director_REPLACED = {
        {{0.02943969992717696, 0.04541644313472981, 0.02015957643685662,
          0.03603802609541951, 0.010014438658309128},
         {-0.02517894880786191, -0.013269490062532379, -0.029224170189883124,
          -0.0175290305472582, -0.008636467209964098},
         {0.9992493805878133, 0.9988800064704819, 0.9993695709569104,
          0.9991966742154514, 0.9999125574031411}},
        {{0.9989597345335528, 0.9987244914988865, 0.9990679485990466,
          0.9987940132621695, 0.9986690289624184},
         {0.03556977895212262, 0.022686080161656193, 0.03874866740700368,
          0.03399167192369593, 0.0506823055525266},
         {-0.02853488401852649, -0.045108002030561936, -0.019020380024440578,
          -0.035427183228149105, -0.00956422998157405}},
        {{-0.034824601201613216, -0.02206211171397978, -0.03816838429877878,
          -0.03334336136017018, -0.050595272601489875},
         {0.9990499543877813, 0.999654571539779, 0.9988215499531906,
          0.9992683820315266, 0.998677483143588},
         {0.026199919635762153, 0.014282885784763959, 0.02997808848353377,
          0.018732883528704277, 0.009132527162258071}}};
    blaze::DynamicMatrix<elastica::real_t> Velocity_REPLACED(3UL, 6UL);
    Velocity_REPLACED = {
        {0.07054612126439226, 0.012027841764838499, 0.0010865578899558637,
         0.09268345190901274, 0.06483176946365873, 0.08031282746334137},
        {0.032171779450055886, 0.08025912067963979, 0.004325978162144239,
         0.02231566995787714, 0.08681653073682254, 0.0937261112329596},
        {0.09683642462618614, 0.013000786308630176, 0.004212250850022337,
         0.003862820481706364, 0.011986298087795312, 0.0344416863076018}};
    blaze::DynamicMatrix<elastica::real_t> Acceleration_REPLACED(3UL, 6UL);
    Acceleration_REPLACED = {
        {0.0640429219321413, 0.0007288269619492249, 0.0781986334077072,
         0.023669101353811985, 0.009110851975167334, 0.06301371619343733},
        {0.048571724002000216, 0.053397382524431394, 0.08479892010142935,
         0.03772285187398736, 0.004565207003110849, 0.09596209846086073},
        {0.07471448935660593, 0.026858259125849195, 0.072565897173148,
         0.08977362823417845, 0.04147658039054533, 0.048065276441950605}};
    blaze::DynamicMatrix<elastica::real_t> AngularVelocity_REPLACED(3UL, 5UL);
    AngularVelocity_REPLACED = {
        {0.06082509135226907, 0.0007186504236994629, 0.09601940820517396,
         0.05070126100005872, 0.07224977770147603},
        {0.022207577772058774, 0.00844527284372777, 0.06436320476061112,
         0.03337980231245532, 0.05555948511703849},
        {0.06683155389284953, 0.09849435093158453, 0.027355999551702884,
         0.034268572069556025, 0.00784288732251256}};
    blaze::DynamicMatrix<elastica::real_t> AngularAcceleration_REPLACED(3UL,
                                                                        5UL);
    AngularAcceleration_REPLACED = {
        {0.08855921743181473, 0.08941407668705303, 0.016310657640645633,
         0.01935356614221394, 0.019692299947268677},
        {0.0952422444147717, 0.006880887603691888, 0.04015261047111745,
         0.07433375528438327, 0.06016618072288619},
        {0.06857668566963625, 0.023246115331522012, 0.08842427315391167,
         0.03348525452550784, 0.07693202348730308}};
    blaze::DynamicVector<elastica::real_t> ElementDimension_PYEL(5UL);
    ElementDimension_PYEL = {0.007522, 0.007522, 0.007522, 0.007522, 0.007522};
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
        {0.203957429946802, 0.6553728672957231, 0.07586006916499055,
         0.2266804768321402, 0.023150782148356863, 0.8267331529503883},
        {0.15482201584295285, 0.5341137261531649, 0.8217521666658159,
         0.08207419303514252, 0.2903135892954033, 0.2571735009896835},
        {0.14465615688052758, 0.6064242289540898, 0.5539788198297757,
         0.5294675598323154, 0.04860584329765816, 0.4773015165169989}};
    blaze::DynamicMatrix<elastica::real_t> InternalTorques_PYEL(3UL, 5UL);
    InternalTorques_PYEL = {
        {0.7350951313834501, 0.7091748026745681, 0.15380848652812207,
         0.04987070974939489, 0.4252467292498079},
        {0.7562052074199033, 0.8984525966266456, 0.2720759351923555,
         0.8572655759809178, 0.5079311479102631},
        {0.05713725081795762, 0.9519014792912509, 0.35573031120251863,
         0.7560601197761251, 0.6850910122901672}};
    blaze::DynamicMatrix<elastica::real_t> ExternalLoads_PYEL(3UL, 6UL);
    ExternalLoads_PYEL = {
        {0.1791075438221752, 0.9899214316476954, 0.7463701769482871,
         0.5090261497759466, 0.9544198771087045, 0.9468784978450715},
        {0.682594016831527, 0.4165951128802907, 0.3851920271071624,
         0.8590050248260485, 0.8718064970595327, 0.7653391073424969},
        {0.023139976989049904, 0.5033417877320233, 0.4084747987104368,
         0.2787462316282022, 0.28563869362760064, 0.9108284397359842}};
    blaze::DynamicMatrix<elastica::real_t> ExternalTorques_PYEL(3UL, 5UL);
    ExternalTorques_PYEL = {
        {0.2011706275871653, 0.35044024901157045, 0.3469039633761569,
         0.730724784965762, 0.5070191807639516},
        {0.9066304124156365, 0.8407550791041166, 0.013837335101241854,
         0.33352982437168033, 0.03454364086259509},
        {0.4341406093148956, 0.8678259663477438, 0.8695583275771691,
         0.8563534969675165, 0.38881434776187773}};
    blaze::DynamicVector<elastica::real_t> ElementLength_PYEL(5UL);
    ElementLength_PYEL = {0.036, 0.036, 0.03599999999999999,
                          0.036000000000000004, 0.036000000000000004};
    blaze::DynamicVector<elastica::real_t> ReferenceElementLength_PYEL(5UL);
    ReferenceElementLength_PYEL = {0.10619813470026732, 0.06846781592702295,
                                   0.08932266024355683, 0.05339723069495667,
                                   0.06191007336705552};
    blaze::DynamicMatrix<elastica::real_t> Tangent_PYEL(3UL, 5UL);
    Tangent_PYEL = {{0.0, 0.0, 0.0, 0.0, 0.0},
                    {1.0, 1.0, 1.0, 1.0, 1.0},
                    {0.0, 0.0, 0.0, 0.0, 0.0}};
    blaze::DynamicVector<elastica::real_t> ElementDilatation_PYEL(5UL);
    ElementDilatation_PYEL = {1.0, 1.0, 1.0, 1.0, 1.0};
    blaze::DynamicVector<elastica::real_t> VoronoiDilatation_PYEL(4UL);
    VoronoiDilatation_PYEL = {1.0, 1.0, 1.0, 1.0};
    blaze::DynamicVector<elastica::real_t> ReferenceVoronoiLength_PYEL(4UL);
    ReferenceVoronoiLength_PYEL = {0.036, 0.03599999999999999, 0.036,
                                   0.036000000000000004};
    blaze::DynamicMatrix<elastica::real_t> ShearStretchStrain_PYEL(3UL, 5UL);
    ShearStretchStrain_PYEL = {
        {0.17015537742449616, 0.24188911472131192, 0.12951526410122371,
         0.6410513197951541, 0.6697892199549795},
        {0.17516497260853625, 0.31704036797809554, 0.2762522726764465,
         0.3198716032399259, 0.07718805919824767},
        {0.18329113443805112, 0.30371975879604374, 0.9994931973914124,
         0.24676623009938614, 0.6209416000520357}};
    blaze::DynamicMatrix<elastica::real_t> Curvature_PYEL(3UL, 4UL);
    Curvature_PYEL = {{0.11181426579214082, 0.2518362108425869,
                       0.14002481988864546, 0.9348913021909042},
                      {0.04802068166268403, 0.40716714832443246,
                       0.6487104333959989, 0.23171343732239702},
                      {0.938828576197833, 0.5867739522581421,
                       0.030625780555092907, 0.795681353275379}};
    blaze::DynamicMatrix<elastica::real_t> ReferenceShearStretchStrain_PYEL(
        3UL, 5UL);
    ReferenceShearStretchStrain_PYEL = {
        {0.7184804985324387, 0.6583747305005937, 0.364722264668331,
         0.9093049831040217, 0.45994236798265253},
        {0.3060060659583749, 0.7782948087672685, 0.9241066144615926,
         0.2995753702543501, 0.9044553750545788},
        {0.9642671548467786, 0.45804198361957094, 0.33837614563409246,
         0.41542573531258753, 0.9118989166105574}};
    blaze::DynamicMatrix<elastica::real_t> ReferenceCurvature_PYEL(3UL, 4UL);
    ReferenceCurvature_PYEL = {{0.8711649329104771, 0.18219259883358419,
                                0.604351208520874, 0.6482932130639908},
                               {0.8154536694563059, 0.7224160802374119,
                                0.929567645128783, 0.1977950177866734},
                               {0.23536632862462836, 0.28433022419255993,
                                0.44594753847623225, 0.02922136066212011}};
    blaze::DynamicMatrix<elastica::real_t> InternalStress_PYEL(3UL, 5UL);
    InternalStress_PYEL = {
        {0.7596379212252692, 0.3721425106646684, 0.37886398601387816,
         0.2242284930736973, 0.8432162709655906},
        {0.006054751261704738, 0.48548428952997214, 0.519602535005509,
         0.7863058184251852, 0.862033563570232},
        {0.3951042311864247, 0.2986331591155028, 0.5703814493289531,
         0.909277466665711, 0.9909018868251039}};
    blaze::DynamicMatrix<elastica::real_t> InternalCouple_PYEL(3UL, 4UL);
    InternalCouple_PYEL = {{0.4311385046965821, 0.6832921248581908,
                            0.8865673324660336, 0.1672225452678583},
                           {0.2938057843773313, 0.3857070159278747,
                            0.8153466251274433, 0.2208439831421144},
                           {0.09631586788311886, 0.8574195723473468,
                            0.9543850950940012, 0.6935530433914594}};
    blaze::DynamicMatrix<elastica::real_t> Position_PYEL(3UL, 6UL);
    Position_PYEL = {
        {0.08870522268171689, 0.024612313410292953, 0.01793587216973136,
         0.0010584322218191832, 0.013381251500294357, 0.07315923638365135},
        {0.09792447196272275, 0.03980292966236467, 0.09801932960152229,
         0.16346768585830138, 0.19246603367494378, 0.18873080125423047},
        {0.007504401265764327, 0.011626806829448355, 0.038039767086985435,
         0.01195050117458274, 0.08671144050250446, 0.07553838100418793}};
    blaze::DynamicTensor<elastica::real_t> Director_PYEL(3UL, 3UL, 5UL);
    Director_PYEL = {
        {{0.02943969992717696, 0.04541644313472981, 0.02015957643685662,
          0.03603802609541951, 0.010014438658309128},
         {-0.02517894880786191, -0.013269490062532379, -0.029224170189883124,
          -0.0175290305472582, -0.008636467209964098},
         {0.9992493805878133, 0.9988800064704819, 0.9993695709569104,
          0.9991966742154514, 0.9999125574031411}},
        {{0.9989597345335528, 0.9987244914988865, 0.9990679485990466,
          0.9987940132621695, 0.9986690289624184},
         {0.03556977895212262, 0.022686080161656193, 0.03874866740700368,
          0.03399167192369593, 0.0506823055525266},
         {-0.02853488401852649, -0.045108002030561936, -0.019020380024440578,
          -0.035427183228149105, -0.00956422998157405}},
        {{-0.034824601201613216, -0.02206211171397978, -0.03816838429877878,
          -0.03334336136017018, -0.050595272601489875},
         {0.9990499543877813, 0.999654571539779, 0.9988215499531906,
          0.9992683820315266, 0.998677483143588},
         {0.026199919635762153, 0.014282885784763959, 0.02997808848353377,
          0.018732883528704277, 0.009132527162258071}}};
    blaze::DynamicMatrix<elastica::real_t> Velocity_PYEL(3UL, 6UL);
    Velocity_PYEL = {
        {0.07054612126439226, 0.012027841764838499, 0.0010865578899558637,
         0.09268345190901274, 0.06483176946365873, 0.08031282746334137},
        {0.032171779450055886, 0.08025912067963979, 0.004325978162144239,
         0.02231566995787714, 0.08681653073682254, 0.0937261112329596},
        {0.09683642462618614, 0.013000786308630176, 0.004212250850022337,
         0.003862820481706364, 0.011986298087795312, 0.0344416863076018}};
    blaze::DynamicMatrix<elastica::real_t> AngularVelocity_PYEL(3UL, 5UL);
    AngularVelocity_PYEL = {
        {0.06082509135226907, 0.0007186504236994629, 0.09601940820517396,
         0.05070126100005872, 0.07224977770147603},
        {0.022207577772058774, 0.00844527284372777, 0.06436320476061112,
         0.03337980231245532, 0.05555948511703849},
        {0.06683155389284953, 0.09849435093158453, 0.027355999551702884,
         0.034268572069556025, 0.00784288732251256}};
    blaze::DynamicMatrix<elastica::real_t> Acceleration_PYEL(3UL, 6UL);
    Acceleration_PYEL = {
        {0.0640429219321413, 0.0007288269619492249, 0.0781986334077072,
         0.023669101353811985, 0.009110851975167334, 0.06301371619343733},
        {0.048571724002000216, 0.053397382524431394, 0.08479892010142935,
         0.03772285187398736, 0.004565207003110849, 0.09596209846086073},
        {0.07471448935660593, 0.026858259125849195, 0.072565897173148,
         0.08977362823417845, 0.04147658039054533, 0.048065276441950605}};
    blaze::DynamicMatrix<elastica::real_t> AngularAcceleration_PYEL(3UL, 5UL);
    AngularAcceleration_PYEL = {
        {0.08855921743181473, 0.08941407668705303, 0.016310657640645633,
         0.01935356614221394, 0.019692299947268677},
        {0.0952422444147717, 0.006880887603691888, 0.04015261047111745,
         0.07433375528438327, 0.06016618072288619},
        {0.06857668566963625, 0.023246115331522012, 0.08842427315391167,
         0.03348525452550784, 0.07693202348730308}};

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
