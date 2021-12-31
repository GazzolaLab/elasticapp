//==============================================================================
// \brief Beginner tutorial for setting up a case study with \elastica
// \details
// A (beginner) tutorial to demonstrate setting up a case study an elastica
// simulation.
//==============================================================================

//==============================================================================
// You can compile and run the tutorial using:
// g++ tutorial_beginner.cpp -g -I <path/to/elastica.hpp> -L
// <path/to/libElastica.so> -lElastica -o tutorial_beginner -std=c++14
// LD_LIBRARY_PATH=<path/to/libElastica.so>  // for linux
// DYLD_LIBRARY_PATH=<path/to/libElastica.so>  // for osx
// ./tutorial_beginner

// If you have the entire Elastica source tree, you can also build it by
// running:
//    make tutorial_beginner
// in a shell with the current directory set to the build directory specified in
// the CMake file.
//==============================================================================

/*
 * 8. We first include the relevant header.
 *
 * We also include iostream and Get because we will be using them, as we will
 * see shortly
 */
#include "tutorial_beginner.hpp"

#include <iostream>

#include "Utilities/Get.hpp"

/*
 * 9. We first look at the definition of `setup`. It takes in the input
 * parameters and uses `elastica::get` with the name of the parameters that we
 * defined before to access the results. We then store the results in the class
 * variables we declared before.
 */
void BeginnerCaseStudy::setup(
    InputTypes<BeginnerCaseStudy> const& input_parameters) {
  first = ::elastica::get<first_parameter>(input_parameters);
  second = ::elastica::get<second_parameter>(input_parameters);
}

/*
 * 10. We now use the class variables to run some useful computation and then
 * print out the results.
 */
void BeginnerCaseStudy::run() {
  auto computation = first + static_cast<int>(second);

  std::cout << computation << std::endl;
}