//
// Created by Tejaswin Parthasarathy on 12/11/21.
//

#pragma once

#include <blaze/math/DynamicMatrix.h>
#include <blaze_tensor/math/DynamicTensor.h>
#include <tuple>
//
#include "Systems/States/Expressions/backends/blaze/SO3PrimitiveAddAssign/BaseTemplate.hpp"
#include "Systems/States/Expressions/backends/blaze/SO3PrimitiveAddAssign/SIMD.hpp"
#include "Systems/States/Expressions/backends/blaze/SO3PrimitiveAddAssign/Scalar.hpp"
#include "Utilities/DefineTypes.h"

template <::elastica::states::detail::SO3AddAssignKind K>
struct SO3PrimitiveAddAssignBenchmarkSetup {
  using H_mat = blaze::DynamicTensor<::elastica::real_t>;
  using DH_mat = blaze::DynamicMatrix<::elastica::real_t>;

  // can throw :(
  static auto setup(const std::size_t n_values) {
    H_mat Q_input(3UL, 3UL, n_values);
    blaze::randomize(Q_input);
    DH_mat w_input(3UL, n_values);
    blaze::randomize(w_input);
    return std::make_tuple(std::move(Q_input), std::move(w_input));
  };

  explicit SO3PrimitiveAddAssignBenchmarkSetup(
      const std::size_t n_values = 256UL) {
    std::tie(Q_input, w_input) = setup(n_values);
  }

  void run() noexcept {
    ::elastica::states::detail::SO3AddAssign<K>::apply(Q_input, w_input);
  }

  H_mat Q_input;
  DH_mat w_input;
};