//==============================================================================
// \brief Tutorial for blocking systems in \elastica simulator
// \details
// A tutorial to demonstrate how to create multiple "blocks" of systems in an
// elastica simulation. Each block can be independently executed and is a
// central token of parallelism in elastica.
//==============================================================================

//==============================================================================
// You can compile and run the tutorial using:
// g++ tutorial_blocking.cpp -g -I <path/to/elastica.hpp> -L
// <path/to/libElastica.so> -lElastica -o tutorial_blocking -std=c++14
// LD_LIBRARY_PATH=<path/to/libElastica.so>  // for linux
// DYLD_LIBRARY_PATH=<path/to/libElastica.so>  // for osx
// ./tutorial_blocking

// If you have the entire Elastica source tree, you can also build it by
// running:
//    make tutorial_blocking
// in a shell with the current directory set to the build directory specified in
// the CMake file.
//==============================================================================

#include "Simulator/Simulator.hpp"
#include "Systems/CosseratRods.hpp"
#include "Utilities/IgnoreUnused.hpp"

int main() {
  using ::elastica::cosserat_rod::CosseratRod;
  using ::elastica::cosserat_rod::CosseratRodWithoutDamping;

  /*
   * 1. Here we show how to customize addition of simulation blocks in
   * \elastica. Each block carries with it an independent chunk of data and can
   * be safely executed in parallel---hence blocks are central tokens of
   * parallelism in elastica. Effective blocking is key to achieve good
   * parallel performance---finding *optimal* block sizes is more of an art!
   *
   * To control blocking, we need to look at a Systems configuration.
   * SystemsConfiguration is the compile-time customization point of the
   * systems modeled (such as types of cosserat rods, rigid bodies etc.)
   * within an elastica Simulator. It offers the ability to
   * 1) customize the simulation with custom systems via the template parameter.
   * 2) control (via policy classes) how new systems will be added to blocks
   * 3) control (via policy classes) how systems will be iterated upon (in
   * serial/parallel) when executing some kernel over them.
   *
   * To idiomatically makes a systems configuration, we use the
   * `make_systems_configuration` helper function.
   */

  // alias a namespace
  namespace ec = elastica::configuration;

  // 1.1 the list of systems (such as types of Cosserat rods) to be added are
  // passed as a template parameters. Here we only pass one system, which is
  // the CosseratRod and CosseratRodWithoutDamping class built within
  // elastica
  auto system_config = ec::make_systems_configuration<
      tmpl::list<CosseratRod, CosseratRodWithoutDamping>>(
      // 1.2 We now need two arguments (passed in any order)
      /* 1.3. The first one is a BlockingPolicy, which is a policy class that
       * determines how systems added to an \elastica simulator are to be
       * distributed in blocks.
       *
       * We use the ec::make_blocking_policy to idiomatically construct blocking
       * policies.
       */
      ec::make_blocking_policy(ec::RestrictSizeAcrossBlockTypes{50UL}),
      /* 1.4. The second one is a IterationPolicy, which is a policy class that
       * determines how kernels operating upon systems will be iterated over,
       * within a \elastica simulator.
       *
       * We use the ec::make_iteration_policy to idiomatically construct
       * iteration policies.
       */
      ec::make_iteration_policy(ec::iterate_across_system_types_in(ec::seq),
                                ec::iterate_over_systems_in(ec::seq)));

  /*
   * 2.1 Before focusing on the blocking policy, we first see how the
   * constructed systems configuration can be mixed into the simulator, via
   * simulator configuration. DefaultDomainConfiguration and
   * DefaultParallelConfiguration are default values and can be ignored for the
   * purposes of this tutorial. Refer to their documentation for more details.
   *
   * Customizations for other modules are possible, see the XXX
   * (TODO : fill link) tutorials for more details.
   */
  auto simulator_config = ec::make_simulator_configuration(
      ec::DefaultDomainConfiguration{}, system_config,
      ec::DefaultParallelConfiguration{});

  /*
   * 2.2 This simulator configuration can then be used to make a new simulator
   * as shown below
   */
  auto simulator = elastica::make_simulator<>(simulator_config);

  /*
   * 3 We now focus on different blocking policies to add systems, by using
   * examples.
   */

  /*
   * 3.1 RestrictSizeAcrossBlockTypes restricts size in a block to a maximum
   * value specified by the parameter, across all system types (in this case
   * CosseratRod and CosseratRodWithoutDamping).
   */
  ::elastica::ignore_unused(
      ec::make_blocking_policy(ec::RestrictSizeAcrossBlockTypes{50UL}));
  /*
   * The following demonstrates the pattern of fill with this policy, with
   * the parameter 50
   * 0. Create simulator
   * CosseratRod               : 0 blocks
   * CosseratRodWithoutDamping : 0 blocks
   *
   * 1. Add CosseratRod of size 20
   * CosseratRod               : 1 block of size 20
   * CosseratRodWithoutDamping : 0 block
   *
   * 2. Add CosseratRod of size 20
   * CosseratRod               : 1 block of size 40
   * CosseratRodWithoutDamping : 0 block
   *
   * 3. Add CosseratRod of size 20
   * CosseratRod               : 1 block of size 40, 1 block of size 20
   * CosseratRodWithoutDamping : 0 block
   *
   * 4. Add CosseratRod of size 30
   * CosseratRod               : 1 block of size 40, 1 block of size 50
   * CosseratRodWithoutDamping : 0 block
   *
   * 5. Add CosseratRodWithoutDamping of size 60
   * CosseratRod               : 1 block of size 40, 1 block of size 50
   * CosseratRodWithoutDamping : 1 block of size 60
   *
   * 6. Add CosseratRodWithoutDamping of size 20
   * CosseratRod               : 1 block of size 40, 1 block of size 50
   * CosseratRodWithoutDamping : 1 block of size 60, 1 block of size 20
   */

  /*
   * 3.2 LooselyRestrictSizeAcrossBlockTypes restricts size in a block to have
   * equal or slightly more than maximum value specified by the parameter,
   * across all system types (in this case CosseratRod and
   * CosseratRodWithoutDamping).
   */
  ::elastica::ignore_unused(
      ec::make_blocking_policy(ec::LooselyRestrictSizeAcrossBlockTypes{50UL}));
  /*
   * The following demonstrates the pattern of fill with this policy, with
   * the parameter 50
   * 0. Create simulator
   * CosseratRod               : 0 blocks
   * CosseratRodWithoutDamping : 0 blocks
   *
   * 1. Add CosseratRod of size 20
   * CosseratRod               : 1 block of size 20
   * CosseratRodWithoutDamping : 0 block
   *
   * 2. Add CosseratRod of size 20
   * CosseratRod               : 1 block of size 40
   * CosseratRodWithoutDamping : 0 block
   *
   * 3. Add CosseratRod of size 20
   * CosseratRod               : 1 block of size 60
   * CosseratRodWithoutDamping : 0 block
   *
   * 4. Add CosseratRod of size 30
   * CosseratRod               : 1 block of size 60, 1 block of size 30
   * CosseratRodWithoutDamping : 0 block
   *
   * 5. Add CosseratRodWithoutDamping of size 60
   * CosseratRod               : 1 block of size 60, 1 block of size 30
   * CosseratRodWithoutDamping : 1 block of size 60
   *
   * 6. Add CosseratRodWithoutDamping of size 20
   * CosseratRod               : 1 block of size 60, 1 block of size 30
   * CosseratRodWithoutDamping : 1 block of size 60, 1 block of size 20
   */

  /*
   * 3.3 AlwaysNewAcrossBlockTypes always creates a new block across all system
   * types (in this case CosseratRod and CosseratRodWithoutDamping).
   */
  ::elastica::ignore_unused(
      ec::make_blocking_policy(ec::AlwaysNewAcrossBlockTypes{0UL}));
  /*
   * The following demonstrates the pattern of fill with this policy
   * 0. Create simulator
   * CosseratRod               : 0 blocks
   * CosseratRodWithoutDamping : 0 blocks
   *
   * 1. Add CosseratRod of size 20
   * CosseratRod               : 1 block of size 20
   * CosseratRodWithoutDamping : 0 block
   *
   * 2. Add CosseratRod of size 20
   * CosseratRod               : 2 blocks of size 20
   * CosseratRodWithoutDamping : 0 block
   *
   * 3. Add CosseratRod of size 20
   * CosseratRod               : 3 blocks of size 20
   * CosseratRodWithoutDamping : 0 block
   *
   * 4. Add CosseratRod of size 30
   * CosseratRod               : 3 blocks of size 60, 1 block of size 30
   * CosseratRodWithoutDamping : 0 block
   *
   * 5. Add CosseratRodWithoutDamping of size 60
   * CosseratRod               : 3 blocks of size 60, 1 block of size 30
   * CosseratRodWithoutDamping : 1 block of size 60
   *
   * 6. Add CosseratRodWithoutDamping of size 20
   * CosseratRod               : 3 blocks of size 60, 1 block of size 30
   * CosseratRodWithoutDamping : 1 block of size 60, 1 block of size 20
   */

  /*
   * 3.4 AlwaysSameAcrossBlockTypes always adds to the same block across all
   * system types (in this case CosseratRod and CosseratRodWithoutDamping).
   */
  ::elastica::ignore_unused(
      ec::make_blocking_policy(ec::AlwaysSameAcrossBlockTypes{0UL}));
  /*
   * The following demonstrates the pattern of fill with this policy
   * 0. Create simulator
   * CosseratRod               : 0 blocks
   * CosseratRodWithoutDamping : 0 blocks
   *
   * 1. Add CosseratRod of size 20
   * CosseratRod               : 1 block of size 20
   * CosseratRodWithoutDamping : 0 block
   *
   * 2. Add CosseratRod of size 20
   * CosseratRod               : 1 block of size 40
   * CosseratRodWithoutDamping : 0 block
   *
   * 3. Add CosseratRod of size 20
   * CosseratRod               : 1 block of size 60
   * CosseratRodWithoutDamping : 0 block
   *
   * 4. Add CosseratRod of size 30
   * CosseratRod               : 1 block of size 90
   * CosseratRodWithoutDamping : 0 block
   *
   * 5. Add CosseratRodWithoutDamping of size 60
   * CosseratRod               : 1 block of size 90
   * CosseratRodWithoutDamping : 1 block of size 60
   *
   * 6. Add CosseratRodWithoutDamping of size 20
   * CosseratRod               : 1 block of size 90
   * CosseratRodWithoutDamping : 1 block of size 80
   */

  /*
   * 4. Finally, for more fine-grained control over blocking, one can mix and
   * match policies, depending on the use-case.
   *
   * Here we show how for a CosseratRod we have strict restrictions over the
   * block size, while for a CosseratRodWithoutDamping we always add to a new
   * block. We also have the policies LooselyRestrictSizeOf and AlwaysSame
   * mirroring the use cases in 3.2 and 3.4 above. For more examples, consult
   * documentation of make_blocking_policy
   */
  ::elastica::ignore_unused(
      ec::make_blocking_policy(ec::RestrictSizeOf<CosseratRod>{50UL},
                               ec::AlwaysNew<CosseratRodWithoutDamping>{}));
}