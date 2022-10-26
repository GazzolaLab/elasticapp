//
// Created by Tejaswin Parthasarathy on 12/11/21.
//

#pragma once

//
#include "Systems/States/Expressions/backends/blaze/SO3PrimitiveAddAssign/BaseTemplate.hpp"
#include "Systems/States/Expressions/backends/blaze/SO3PrimitiveAddAssign/SIMD.hpp"
#include "Systems/States/Expressions/backends/blaze/SO3PrimitiveAddAssign/Scalar.hpp"
#include "Utilities/DefineTypes.h"
//
#include <blaze/math/DynamicMatrix.h>
#include <blaze_tensor/math/DynamicTensor.h>
#include <random>
#include <tuple>

template <::elastica::states::detail::SO3AddAssignKind K>
struct SO3PrimitiveAddAssignBenchmarkSetup {
  using H_mat = blaze::DynamicTensor<::elastica::real_t>;
  using DH_mat = blaze::DynamicMatrix<::elastica::real_t>;

  // can throw :(
  static auto setup(const std::size_t n_values,
                    ::elastica::real_t const inclination) {
    H_mat Q_input_fill(3UL, 3UL, n_values);
    DH_mat w_input_fill(3UL, n_values);

    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::normal_distribution<::elastica::real_t> d{1.0, 0.2};

    //    blaze::randomize(Q_input_fill);
    //    blaze::randomize(w_input_fill);
    for (auto idx = 0UL; idx < n_values; ++idx) {
      w_input_fill(0UL, idx) = d(gen);
      w_input_fill(1UL, idx) = d(gen);
      w_input_fill(2UL, idx) = d(gen);
    }

    for (auto idx = 0UL; idx < n_values; ++idx) {
      const auto angle = d(gen) * inclination;  // around 6.0
      Q_input_fill(0UL, 0UL, idx) = std::cos(angle);
      Q_input_fill(0UL, 1UL, idx) = -std::sin(angle);
      Q_input_fill(0UL, 2UL, idx) = 0.0;

      Q_input_fill(1UL, 0UL, idx) = std::sin(angle);
      Q_input_fill(1UL, 1UL, idx) = std::cos(angle);
      Q_input_fill(1UL, 2UL, idx) = 0.0;

      Q_input_fill(2UL, 0UL, idx) = 0.0;
      Q_input_fill(2UL, 1UL, idx) = 0.0;
      Q_input_fill(2UL, 2UL, idx) = 1.0;
    }

    return std::make_tuple(std::move(Q_input_fill), std::move(w_input_fill));
  };

  explicit SO3PrimitiveAddAssignBenchmarkSetup(
      const std::size_t n_values = 256UL,
      ::elastica::real_t const inclination = 0.0) {
    std::tie(Q_input, w_input) = setup(n_values, inclination);
  }

  void run() noexcept {
    ::elastica::states::detail::SO3AddAssign<K>::apply(Q_input, w_input);
  }

  H_mat Q_input;
  DH_mat w_input;
};