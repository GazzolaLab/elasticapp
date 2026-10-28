//==============================================================================
// \brief Tutorial for construction of a single rod in \elastica simulator
// \details
// A tutorial to demonstrate creating a single rod in the context of an elastica
// simulation.
//==============================================================================

//==============================================================================
// You can compile and run the tutorial using:
// g++ tutorial_rod_construction.cpp -g -I <path/to/elastica.hpp> -L
// <path/to/libElastica.so> -lElastica -o tutorial_rod_construction -std=c++20
// LD_LIBRARY_PATH=<path/to/libElastica.so>  // for linux
// DYLD_LIBRARY_PATH=<path/to/libElastica.so>  // for osx
// ./tutorial_rod_construction

// If you have the entire Elastica source tree, you can also build it by
// running:
//    make tutorial_rod_construction
// in a shell with the current directory set to the build directory specified in
// the CMake file.
//==============================================================================

#include <iostream>

#include "Simulator/Simulator.hpp"
#include "Systems/CosseratRods.hpp"

int main() {
  using ::elastica::cosserat_rod::CosseratRod;

  // TODO : Add CosseratRod to ELASTICA_DEFAULT_ADMISSIBLE_PLUGINS_FOR_SYSTEMS
  namespace ec = elastica::configuration;
  auto system_config = ec::make_systems_configuration<tmpl::list<CosseratRod>>(
      ec::make_blocking_policy(ec::RestrictSizeAcrossBlockTypes{50UL}),
      ec::make_iteration_policy(ec::iterate_across_system_types_in(ec::seq),
                                ec::iterate_over_systems_in(ec::seq)));

  auto simulator_config = ec::make_simulator_configuration(
      ec::DefaultDomainConfiguration{}, system_config,
      ec::DefaultParallelConfiguration{});

  // 1. We first create a simulator. For the broader setup flow, see
  // docs/CoreConcepts.md and docs/Quickstart.md.
  auto simulator = elastica::make_simulator<>(simulator_config);

  /* We will now showcase ways of creating a new CosseratRod using our simulator
   * to create and manage memory and execution, thus allowing us to focus on
   * the high-level configuration of the system we are interested in.
   *
   * Let us assume in this example we want to create a standard Cosserat rod
   * (i.e. with circular cross section, linear hyperelasticity with some
   * damping). We want to initialize it such that the rod is initially straight
   * without any motion. The easiest way to achieve this in \elastica is using
   * Initializers. These are entities that help ease initialization using only
   * higher level details and additionally support loading/saving your initial
   * elastica configuration to disc using YAML/XML files.
   *
   * Initializers in elastica are suffixed by the word "Initializers". For
   * example ::elastica::cosserat_rod::StraightCosseratRodInitializer.
   */

  /*
   * 2. Let us then use StraightCosseratRodInitializer to setup an initializer
   * for a rod. A typical pattern seen in the code is the following
   */
  using StraightInit = ::elastica::cosserat_rod::StraightCosseratRodInitializer;
  // We first create a material for passing into the initializer, see
  // documentation of elastica::create_material
  elastica::MaterialID my_material = elastica::create_material(
      // material density
      7.874,
      // coefficient of restitution
      0.24,
      // Young's modulus
      200,
      // Shear modulus
      200.0 / 1.24,
      // Contact stiffness
      200,
      // normal contact damping
      1e-10,
      // tangent contact damping
      1e-10,
      // coefficient of static friction
      0.1,
      // cofficient of dynamic friction
      0.1);

  // The arguments here are self-explanatory. If there is some confusion, refer
  // to the documentation of
  // ::elastica::cosserat_rod::StraightCosseratRodInitializer.
  StraightInit straight_initializer{
      StraightInit::NElement{10UL},
      StraightInit::Material{my_material},
      StraightInit::Radius{0.007522},
      StraightInit::Length{0.18},
      StraightInit::Origin{::elastica::Vec3{0.0, 0.0, 0.0}},
      StraightInit::Direction{::elastica::Vec3{0.0, 1.0, 0.0}},
      StraightInit::Normal{::elastica::Vec3{0.0, 0.0, 1.0}},
      StraightInit::ForceDampingRate{0.2},
      StraightInit::TorqueDampingRate{0.2},
  };

  // Note that the order of arguments here do not matter, as long as all
  // arguments are present. If all arguments are not present, then a
  // compile-time error is raised. So the following code is also valid.
  StraightInit another_straight_initializer{
      StraightInit::Radius{0.007522},
      StraightInit::Length{0.18},
      StraightInit::NElement{10UL},
      StraightInit::Normal{::elastica::Vec3{0.0, 0.0, 1.0}},
      StraightInit::ForceDampingRate{0.2},
      StraightInit::Material{my_material},
      StraightInit::Origin{::elastica::Vec3{0.0, 0.0, 0.0}},
      StraightInit::Direction{::elastica::Vec3{0.0, 1.0, 0.0}},
      StraightInit::TorqueDampingRate{0.2},
  };

  /*
   * 3. Now that we have an initializer, let us use it to emplace a rod into our
   * simulator. For this, we use the initializer's `initialize` template method
   * and pass our Rod as the type argument.
   *
   * The reason we do it this way (as opposed to directly passing the
   * initializer into the simulator emplace method ) is for reuse. The same
   * initializer can be used to initialize very different rods (e.g. rods with
   * square cross section)
   */
  simulator->emplace_back<CosseratRod>(
      straight_initializer.initialize<CosseratRod>());

  /* Additionally the reuse comes in handy to create new initializers too. Using
   * the initializers `but_with` method, you can quickly add more rods. For
   * example, say we need to add another rod (of the same type) but with a
   * different origin. It can be quickly achieved as shown below. All the other
   * properties are the same.
   */
  simulator->emplace_back<CosseratRod>(
      straight_initializer
          .but_with(StraightInit::Origin{::elastica::Vec3{1.0, 0.0, 0.0}})
          .initialize<CosseratRod>());

  /*
   * 4. Next, the user can override optional details via the `initialize`
   * method.
   *
   * For example, say we need a rod which is initialized with some varying
   * velocity v = 0.1 + 0.002 * i, where i is the index of the node. We can
   * program it as shown below.
   * If you are confused what ::elastica::tags::Velocity means, please consult
   * `tutorial_single_rod` for an introduction to tags in elastica.
   */
  simulator->emplace_back<CosseratRod>(
      straight_initializer.initialize<CosseratRod>(
          ::blocks::initialize<::elastica::tags::Velocity>(
              [](std::size_t index) {
                return 0.1 + 0.002 * static_cast<double>(index);
              })));
  /*
   * Note that we use the ::blocks::initialize method which takes as a template
   * parameter the tag to initialize. We then pass in a functor (in this case
   * a lambda function) with the desired form.
   *
   * The initialize statements before (in parts 2, 3) assumes a (sensible)
   * default initial velocity of 0, but we overrode it in the example above.
   */

  /*
   * 5. You can pass in multiple optional parameters in the `initialize` method
   * as well. In addition to spatially varying initial velocity, let us include
   * a custom angular velocity as well. We do this via the following:
   */
  simulator->emplace_back<CosseratRod>(
      straight_initializer.initialize<CosseratRod>(
          ::blocks::initialize<::elastica::tags::Velocity>(
              [](std::size_t index) {
                return 0.1 + 0.002 * static_cast<double>(index);
              }),
          ::blocks::initialize<::elastica::tags::AngularVelocity>(
              [](std::size_t) {
                return ::elastica::Vec3{0.1, 0.0, 0.0};
              })));

  //============================================================================
  // ADVANCED INITIALIZATION
  //============================================================================
  /*
   * 6. In case you want more control over initializing all members of a
   * CosseratRod you can use
   * ::elastica::cosserat_rod::initialize_cosserat_rod() as shown below. It
   * accepts as arguments ::blocks::initialize (as seen in 4. and 5. above).

   * All tags needed by the CosseratRod must be specified, else a compile-time
   * error is raised. For a list of these tags, either see the example below or
   * consult the documentation of CosseratRod.
   *
   * For the broader walkthrough around initialization and case-study setup,
   * see docs/Quickstart.md.
   */
  auto advanced_initializer =
      elastica::cosserat_rod::initialize_cosserat_rod<CosseratRod>(
          // Variables
          blocks::initialize<elastica::tags::NElement>([](...) { return 5; }),
          // Geometry Variables
          blocks::initialize<elastica::tags::Position>(
              [](std::size_t index) { return 1.0 * index; }),
          blocks::initialize<elastica::tags::Director>([](...) { return 0.5; }),
          blocks::initialize<elastica::tags::ElementDimension>(
              [](...) { return 4.0; }),
          blocks::initialize<elastica::tags::ReferenceCurvature>(
              [](...) { return 1.0; }),
          blocks::initialize<elastica::tags::ReferenceShearStretchStrain>(
              [](...) { return 1.0; }),
          blocks::initialize<elastica::tags::ShearStretchRigidityMatrix>(
              [](...) { return 4.0; }),
          blocks::initialize<elastica::tags::BendingTwistRigidityMatrix>(
              [](...) { return 4.0; }),
          blocks::initialize<elastica::tags::ElementDilatation>(
              [](...) { return 1.0; }),
          blocks::initialize<elastica::tags::ForceDampingRate>(
              [](...) { return 4.0; }),
          blocks::initialize<elastica::tags::TorqueDampingRate>(
              [](...) { return 1.0; }),
          // Kinematics Variables
          blocks::initialize<elastica::tags::Material>(
              [=](...) { return my_material; }),
          blocks::initialize<elastica::tags::Velocity>([](...) { return 0.0; }),
          blocks::initialize<elastica::tags::AngularVelocity>(
              [](...) { return 0.0; }),
          blocks::initialize<elastica::tags::Acceleration>(
              [](...) { return 0.0; }),
          blocks::initialize<elastica::tags::AngularAcceleration>(
              [](...) { return 0.0; }));

  simulator->emplace_back<CosseratRod>(std::move(advanced_initializer));

  simulator->finalize();

  return 0;
}
