//
// Created by Yashraj Bhosale on 9/16/21.
//

#pragma once

#include "blaze/Blaze.h"
#include "blaze_tensor/Blaze.h"

namespace elastica {
  //**************************************************************************
  /*!\brief Simple 2-point difference rule with zero at end points.
  //
  // \details
  // Discrete 2-point difference in \elastica of a function f:[a,b]-> R, i.e
  // D f[a,b] -> df[a,b] where f satisfies the conditions
  // f(a) = f(b) = 0.0. Operates from rod's elemental space to nodal space.
  //
  // \example
  // The following shows a typical use of the difference kernel
  // with the expected (correct) result also shown.
  // \snippet test_calculus.cpp difference_kernel_example
  //
  // \param[out] out_matrix(3, n_nodes) difference values
  // \param[in] in_matrix(3, n_elems) vector batch \n
  // where n_nodes = n_elems + 1
  //
  // \return void/None
  //
  // \see fill later?
  */
  template <typename MT1,  // blaze Matrix expression type 1
      typename MT2>  // blaze Matrix expression type 2
  void difference_kernel(MT1& out_matrix, const MT2& in_matrix) {
    constexpr std::size_t dimension(3UL);
    assert(in_matrix.rows() == dimension);
    assert(out_matrix.rows() == dimension);
    const std::size_t n_elems = in_matrix.columns();
    const std::size_t n_nodes = n_elems + 1UL;
    assert(out_matrix.columns() == n_nodes);

    blaze::column(out_matrix, 0UL) = blaze::column(in_matrix, 0UL);
    blaze::column(out_matrix, n_nodes - 1UL) =
        -blaze::column(in_matrix, n_elems - 1UL);
    blaze::submatrix(out_matrix, 0UL, 1UL, dimension, n_elems - 1UL) =
        blaze::submatrix(in_matrix, 0UL, 1UL, dimension, n_elems - 1UL) -
        blaze::submatrix(in_matrix, 0UL, 0UL, dimension, n_elems - 1UL);
  }

  //**************************************************************************
  /*!\brief Simple 2-point difference rule with zero at end points.
  //
  // \details
  // Discrete 2-point difference in \elastica of a function f:[a,b]-> R, i.e
  // D f[a,b] -> df[a,b] where f satisfies the conditions
  // f(a) = f(b) = 0.0. Operates from rod's elemental space to nodal space.
  //
  // \example
  // The following shows a typical use of the difference kernel
  // with the expected (correct) result also shown.
  // \snippet test_calculus.cpp difference_kernel_example
  //
  // \param[out] out_matrix(3, n_nodes) difference values
  // \param[in] in_matrix(3, n_elems) vector batch \n
  // where n_nodes = n_elems + 1
  //
  // 0.5*(ref_length[1:] + ref_length[:-1])
  //
  // \return void/None
  //
  // \see fill later?
  */
  template <typename MT1,
            typename MT2>  // blaze Matrix expression type 2
  void voronoi_length(MT1& out_matrix, const MT2& in_matrix) {
    constexpr std::size_t dimension(3UL);
    assert(in_matrix.rows() == dimension);
    assert(out_matrix.rows() == dimension);
    const std::size_t n_elems = in_matrix.columns();
    const std::size_t n_nodes = n_elems + 1UL;
    assert(out_matrix.columns() == n_nodes);

    blaze::column(out_matrix, 0UL) = blaze::column(in_matrix, 0UL);
    blaze::column(out_matrix, n_nodes - 1UL) =
        -blaze::column(in_matrix, n_elems - 1UL);
    blaze::submatrix(out_matrix, 0UL, 1UL, dimension, n_elems - 1UL) =
        blaze::submatrix(in_matrix, 0UL, 1UL, dimension, n_elems - 1UL) -
        blaze::submatrix(in_matrix, 0UL, 0UL, dimension, n_elems - 1UL);
  }
  //**************************************************************************
} // namespace elastica