//==============================================================================
// \brief Tutorial for programmatically querying a single rod in \elastica
// \details A tutorial to demonstrate the API afforded by an \elastica
// Cosserat rod
//==============================================================================

//==============================================================================
// You can compile and run the tutorial using:
// g++ tutorial_single_rod.cpp -g -I <path/to/elastica.hpp> -L
// <path/to/libElastica.so> -lElastica -o tutorial_single_rod -std=c++14
// LD_LIBRARY_PATH=<path/to/libElastica.so>  // for linux
// DYLD_LIBRARY_PATH=<path/to/libElastica.so>  // for osx
// ./tutorial_single_rod

// If you have the entire Elastica source tree, you can also build it by
// running:
//    make tutorial_single_rod
// in a shell with the current directory set to the build directory specified in
// the CMake file.
//==============================================================================

#include <iostream>

#include "Simulator/Simulator.hpp"
#include "Systems/CosseratRods.hpp"
#include "Utilities/Get.hpp"
#include "Utilities/TMPL.hpp"

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

  /*
   * 1. Simulator creation
   * We first create a simulator. To customize the simulator
   * itself see XXXX (TODO : fill link)
   */
  auto simulator = elastica::make_simulator<>(simulator_config);

  /*
   * 2. Rod Initialization
   * We then construct a Cosserat rod using an initializer. In this tutorial
   * we dont focus on the initializers. To know more about them, please look at
   * `tutorial_rod_construction`.
   */
  elastica::MaterialID my_material = elastica::create_material(
      7.874, 0.24, 200, 200.0 / 1.24, 200, 1e-10, 1e-10, 0.1, 0.1);
  using StraightInit = ::elastica::cosserat_rod::StraightCosseratRodInitializer;
  StraightInit straight_initializer{
      // StraightInitialization Wrapper
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
  //

  /*
   * 3. Rod creation
   * Now that we have an initializer, let us use it to emplace a rod into our
   * simulator. More details can be found in `tutorial_rod_construction`.
   *
   * Here, observe that the `emplace_back` method returns a wrapped reference
   * to the emplaced rod (as opposed to a direct reference!). These reference
   * wrappers (similar to std::ref) are NEVER invalidated to ensure a
   * memory-safe programming experience in __elastica__.
   */
  auto rod_ref = simulator->emplace_back<CosseratRod>(
      straight_initializer.initialize<CosseratRod>());

  /*
   * 4. Rod API
   * Now begins the main lesson in the tutorial. Upon construction of a rod,
   * we now have access to its members and member functions. There are three
   * APIs, designed for users who may/may not be familiar with C++.
   */

  /*
   * 4.1
   * The first API, to access rod data, is to use the get_*() member function.
   * In case of position, the member function is get_position()
   */
  std::cout << "Position using member function" << std::endl;
  // we get the wrapped reference to get the rod, and then get its position!
  std::cout << rod_ref.get().get_position() << std::endl;

  /*
   * 4.2
   * The second API, is to use the elastica free functions. Free functions are
   * very useful to write generic code. For example, the ::elastica::position
   * free function will work with all cosserat rods, spheres, boxes etc.
   */
  std::cout << "Position using free function" << std::endl;
  // we dereference the iterator to get the rod!
  std::cout << ::elastica::position(rod_ref.get()) << std::endl;
  // The call below also works, use it if you are comfortable with it
  std::cout << ::elastica::position(rod_ref) << std::endl;

  /*
   * 4.3
   * Finally, the last API is the most generic and most powerful. Users familiar
   * with C++-11 tuples will get good mileage using this API for writing generic
   * code. The trick here is to use the free functino ::elastica::get, along
   * with a custom type. This type acts as a compile-time key to look at the
   * value stored in the rod. This API is useful in metaprogramming, and power
   * users can take advantage of this.
   */
  std::cout << "Position using elastica::get free function" << std::endl;

  /* But what type do we use to access the position? For this we reserve special
   * types called `Tags` aptly placed in the namespace elastica::tags. Tags
   * are simple empty types whose sole purpose is to access members without
   * explicitly specifying the member's types (which might be a vector,
   * matrix etc.). Each data entry of a Cosserat rod has a unique tag associated
   * with it. To obtain the positions, the corresponding tag entry is
   * elastica::tags::Position
   *
   * Note that by convention, all tags are Capitalized.
   */
  auto rod = rod_ref.get();
  std::cout << ::elastica::get<elastica::tags::Position>(rod) << std::endl;

  /*
   * 5. Now that we have access to the position, let's analyze its contents.
   * Firstly, the positions are a (3, n + 1) array, where n corresponds to the
   * number of elements. You can programatically obtain the shape as shown below
   */
  /// [tag_example]
  auto rod_position = ::elastica::get<::elastica::tags::Position>(rod);
  /// [tag_example]
  std::cout << "Shape of position : (" << rod_position.rows() << ", "
            << rod_position.columns() << ")" << std::endl;

  /*
   * all operations like +,-,*, / are defined. Furthermore, you can access
   * data points using the () syntax. For example, to access the (1, 4) index
   * (with zero indexing), we do
   */
  std::cout << "Rod position at (1, 4) : " << rod_position(1UL, 4UL)
            << std::endl;

  /*
   * You can use this to set new values of position, which are useful to change
   * the state of the rod once it is created
   */
  rod_position(1UL, 4UL) = 0.08;
  std::cout << "Updated rod position at (1, 4) : "
            << ::elastica::position(rod)(1UL, 4UL) << std::endl
            << std::endl;

  /*
   * Lastly, you can also get access to the i'th element positions by passing
   * in an index to the functions. For example, to get the 4th location index
   * we do
   */
  // TODO : Bug, look at issue #52.
  //   const std::size_t rod_index = 4UL;
  //   std::cout << "Rod position at the 5th node : "
  //             << ::elastica::position(*rod_it, rod_index) << std::endl;

  /*
   * Many more operations are possible on these arrays, see the documentation
   * and API of blaze library. In our experience, we rarely need to use
   * them---most geometry and algebra kernels are provided by \elastica itself.
   * For more examples, see XXX (TODO : Link examples)
   */

  /*
   * 5. Full rod API
   *
   * Finally, one can access other members using the Cosserat rod accessor
   * functions. While the full list can be found in the API documentation on
   * CosseratRod, here we highlight a few commonly used ones
   */
  // Velocity is of shape (3, n + 1)
  std::cout << "Velocity using member function" << std::endl;
  std::cout << "Shape of velocity : ("            //
            << rod.get_velocity().rows() << ", "  //
            << rod.get_velocity().columns()       //
            << ")" << std::endl
            << std::endl;

  // Directors is of shape (3, 3, n) and is a tensor
  std::cout << "Director using member function" << std::endl;
  std::cout << "Shape of director : ("
            // follows the blaze_tensor API
            << rod.get_director().pages() << ", "  //
            << rod.get_director().rows() << ", "   //
            << rod.get_director().columns()        //
            << ")" << std::endl
            << std::endl;

  // Angular velocity is of shape (3, n)
  std::cout << "Angular velocity using member function" << std::endl;
  std::cout << "Shape of angular velocity : ("            //
            << rod.get_angular_velocity().rows() << ", "  //
            << rod.get_angular_velocity().columns()       //
            << ")" << std::endl
            << std::endl;

  // TODO : Doesn't compile, need to fix #50 first
  // Curvature is of shape (3, n)
  // std::cout << "Reference curvature using member function" << std::endl;
  // std::cout << "Shape of reference curvature : ("                  //
  //           << (*rod_it).get_reference_curvature().rows() << ", "  //
  //           << (*rod_it).get_reference_curvature().columns()       //
  //           << ")" << std::endl;

  /*
   * For a full list of member functions, consult the API for a CosseratRod.
   *
   * Additionally, we have access to the following free functions to access
   * the most used members of the CosseratRod.
   */
  std::cout << "Velocity using free function" << std::endl;
  // we dereference the iterator to get the rod!
  std::cout << "Shape of velocity : ("                             //
            << ::elastica::velocity(rod_ref.get()).rows() << ", "  //
            << ::elastica::velocity(rod_ref.get()).columns()       //
            << ")" << std::endl
            << std::endl;

  std::cout << "Angular Velocity using free function" << std::endl;
  // we dereference the iterator to get the rod!
  std::cout << "Shape of angular_velocity : ("                             //
            << ::elastica::angular_velocity(rod_ref.get()).rows() << ", "  //
            << ::elastica::angular_velocity(rod_ref.get()).columns()       //
            << ")" << std::endl
            << std::endl;

  std::cout << "Director using free function" << std::endl;
  // we dereference the iterator to get the rod!
  std::cout << "Shape of director : ("
            // follows the blaze_tensor API
            << ::elastica::director(rod_ref.get()).pages() << ", "  //
            << ::elastica::director(rod_ref.get()).rows() << ", "   //
            << ::elastica::director(rod_ref.get()).columns()        //
            << ")" << std::endl
            << std::endl;

  /*
   * Finally all tags used by the CosseratRod are accessible to the user. So
   * to access members that do not have access functions, such as the tangent,
   * we simply need to use the tag and elastica::get
   */
  std::cout << "Tangent using elastica::get " << std::endl;
  std::cout
      << "Shape of tangent : ("  //
      << ::elastica::get<elastica::tags::Tangent>(rod_ref.get()).rows()
      << ", "                                                               //
      << ::elastica::get<elastica::tags::Tangent>(rod_ref.get()).columns()  //
      << ")" << std::endl
      << std::endl;
  /*
   * To see a list of all tags supported by the CosseratRod, please consult
   * its API documentation.
   * To programatically obtain and print the list of tags, see the
   * XXX (TODO) tutorial.
   */

  return 0;
}
