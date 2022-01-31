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
 * 4. We first include the relevant header.
 *
 * We also include iostream and Get because we will be using them, as we will
 * see shortly
 */
#include "tutorial_intermediate.hpp"

#include <iostream>

#include "Utilities/Get.hpp"

/*
 * Definition of setup
 */
void IntermediateCaseStudy::setup(
    InputTypes<IntermediateCaseStudy> const& input_parameters) {
  const auto n_row = ::elastica::get<n_rows>(input_parameters);
  const auto n_col = ::elastica::get<n_cols>(input_parameters);
  vector.resize(n_col);
  matrix.resize(n_row, n_col);
  tensor.resize(n_row, n_row, n_col);

  for (std::size_t j_col = 0UL; j_col < n_col; ++j_col) {
    vector[j_col] = static_cast<double>(j_col);
  }

  // fill in matrix
  // (0, 0, .... , 0)
  // (1, 1, ....., 1)
  // (2, 2, ....., 2)
  for (std::size_t i_row = 0UL; i_row < n_row; ++i_row) {
    for (std::size_t j_col = 0UL; j_col < n_col; ++j_col) {
      matrix(i_row, j_col) = static_cast<double>(i_row);
    }
  }

  for (std::size_t i_page = 0UL; i_page < n_row; ++i_page) {
    for (std::size_t j_row = 0UL; j_row < n_row; ++j_row) {
      for (std::size_t k_col = 0UL; k_col < n_col; ++k_col) {
        tensor(i_page, j_row, k_col) = static_cast<double>(i_page + j_row);
      }
    }
  }
}

/*
 * Definition of run()
 */
void IntermediateCaseStudy::run() {
  // we have a current_iteration() method that exposes (as a value) the current
  // iteration --- this is useful to access the current iteration from Python
  // get_current_iteration() can be used to change the underlying iterate, for
  // example in incrementing the counter
  for (; current_iteration() < 500UL; ++get_current_iteration())
    step();
}

/*
 * step() modifies these arrays, which is exposed to the world outside
 */
void IntermediateCaseStudy::step() {
  const auto n_row = matrix.rows();
  const auto n_col = matrix.columns();

  for (std::size_t j_col = 0UL; j_col < n_col; ++j_col) {
    vector[j_col] += static_cast<double>(j_col % 2);
  }

  for (std::size_t i_row = 0UL; i_row < n_row; ++i_row) {
    for (std::size_t j_col = 0UL; j_col < n_col; ++j_col) {
      matrix(i_row, j_col) += static_cast<double>(i_row % 2);
    }
  }

  for (std::size_t i_page = 0UL; i_page < n_row; ++i_page) {
    for (std::size_t j_row = 0UL; j_row < n_row; ++j_row) {
      for (std::size_t k_col = 0UL; k_col < n_col; ++k_col) {
        tensor(i_page, j_row, k_col) +=
            static_cast<double>((i_page + j_row) % 2);
      }
    }
  }
}
