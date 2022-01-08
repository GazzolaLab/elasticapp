//==============================================================================
// \brief Intermediate tutorial for setting up a case study with \elastica
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
#include "tutorial_intermediate.hpp"

#include <iostream>

#include "Utilities/Get.hpp"

void IntermediateCaseStudy::setup(
    InputTypes<IntermediateCaseStudy> const& input_parameters) {
  const auto n_row = ::elastica::get<n_rows>(input_parameters);
  const auto n_col = ::elastica::get<n_cols>(input_parameters);
  data.resize(n_row, n_col);

  // fill in data
  // (0, 0, .... , 0)
  // (1, 1, ....., 1)
  // (2, 2, ....., 2)
  for (std::size_t i_row = 0; i_row < n_row; ++i_row) {
    for (std::size_t j_col = 0; j_col < n_col; ++j_col) {
      data(i_row, j_col) = (double)(i_row);
    }
  }
}

void IntermediateCaseStudy::run() {
  for (std::size_t i_step = 0UL; i_step < 500UL; ++i_step)
    step();
}

void IntermediateCaseStudy::step() {
  const auto n_row = data.rows();
  const auto n_col = data.columns();

  for (std::size_t i_row = 0; i_row < n_row; ++i_row) {
    for (std::size_t j_col = 0; j_col < n_col; ++j_col) {
      data(i_row, j_col) += static_cast<double>(i_row % 2);
    }
  }
}
