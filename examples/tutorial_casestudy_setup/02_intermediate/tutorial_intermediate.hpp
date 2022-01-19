//==============================================================================
// \brief Beginner tutorial for setting up a case study with \elastica
// \details
// A (beginner) tutorial to demonstrate setting up a case study
//==============================================================================

/*
 * This tutorial continues [tutorial_beginner] to demonstrate setting up a case
 * study that can communicate with the outside world. All the lessons learnt in
 * the previous tutorial hold, here we show how data buffers can be exposed
 * using views on arrays. Note that all our rod interface obtained from the
 * simulator uses similar views
 */

#pragma once

#include <blaze/math/DynamicMatrix.h>
#include <blaze/math/DynamicVector.h>
#include <blaze_tensor/math/DynamicTensor.h>
#include <elastica/CaseStudy.hpp>
#include <elastica/Utilities/TMPL.hpp>

using String = const char* const;

/// [case_study_interface_eg]
struct IntermediateCaseStudy
    : public ::elastica::CaseStudyInterface<IntermediateCaseStudy> {
  /// [case_study_interface_eg]
  /* Help string */
  static constexpr String help = {"An intermediate case study"};

  //============================================================================
  /* Parameters */
  struct n_rows {
    // unsigned integer parameter
    using type = std::size_t;
    // documentation
    static constexpr String help = {
        "Number of rows in the simulation matrix, >=3 "};
  };

  struct n_cols {
    using type = std::size_t;
    static constexpr String help = {
        "Number of columns in the simulation matrix, >=8"};
  };

  using Parameters = tmpl::list<n_rows, n_cols>;
  //============================================================================

  // setup routine
  void setup(InputTypes<IntermediateCaseStudy> const& inputs);
  // simulation progression routines
  /*
   * 1. Here we demonstrate the use of step(). It steps forth the simulation
   * by one time-step.
   */
  void step();
  void run();

  /*
   * 2. We now declare buffers that can be exposed to python. Here we show
   * one each of an elastica vector, matrix and tensor type
   */
  using VectorType = ::blaze::DynamicVector<double>;
  using MatrixType = ::blaze::DynamicMatrix<double>;
  using TensorType = ::blaze::DynamicTensor<double>;

  // We store these buffers the case-study class
  VectorType vector;
  MatrixType matrix;
  TensorType tensor;

  /*
   * 3. This routine then communicates the buffers to the outside world.
   * one each of an elastica vector, matrix and tensor type
   */
  inline auto communicate() {
    const std::size_t first_size = 4UL;
    const std::size_t second_size = 8UL;
    // more than one array can be communicated using a tuple of instances
    return std::make_tuple(
        blaze::subvector(vector, 0UL, first_size),
        blaze::subvector(vector, 0UL, second_size),
        blaze::submatrix(matrix, 0UL, 0UL, 2UL, first_size),
        blaze::submatrix(matrix, 0UL, 0UL, 3UL, second_size),
        blaze::subtensor(tensor, 0UL, 0UL, 0UL, 2UL, 3UL, first_size),
        blaze::subtensor(tensor, 0UL, 0UL, 0UL, 3UL, 3UL, second_size));
  }
};

/*
 * Register as a case study
 */
ELASTICA_REGISTER_CASE_STUDY(IntermediateCaseStudy);
