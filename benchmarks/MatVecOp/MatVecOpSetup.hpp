//
// Created by Tejaswin Parthasarathy on 10/26/22.
//

#pragma once

#include <blaze/math/DynamicMatrix.h>
#include <blaze_tensor/math/DynamicTensor.h>
#include <tuple>
//
#include "Systems/CosseratRods/Traits/Operations/BlazeBackend/LinearAlgebra/MatVec/Kernels.hpp"
#include "Utilities/DefineTypes.h"
#include "Utilities/Geometry/AxisAngleRotation.hpp"

template <::elastica::cosserat_rod::MatVecKind K>
struct MatVecOpTimingSetup {
  using Mat = blaze::DynamicTensor<::elastica::real_t>;
  using Vec = blaze::DynamicMatrix<::elastica::real_t>;

  // can throw :(
  static auto setup(const std::size_t n_values) {
    Mat m_input(3UL, 3UL, n_values);

    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::normal_distribution<::elastica::real_t> d{1.0, 0.1};

    for (std::size_t k(0UL); k < n_values; ++k) {
      const auto angle = d(gen) * M_PI / 3.0;
      const auto c = std::cos(angle);
      const auto s = std::sin(angle);
      ::blaze::columnslice(m_input, k) = elastica::Rot3{
          {c, -s, s + c}, {s, c, c - s}, {s * c, s - c, 2.0 * s}};
    }

    Vec v_input(3UL, n_values);
    Vec v_output(3UL, n_values);

    for (std::size_t k(0UL); k < n_values; ++k) {
      ::blaze::column(v_input, k) = elastica::Vec3{d(gen), d(gen), d(gen)};
    }

    return std::make_tuple(std::move(m_input), std::move(v_input),
                           std::move(v_output));
  };

  explicit MatVecOpTimingSetup(std::size_t const n_values = 256UL) {
    std::tie(mat_input, vec_input, vec_output) = setup(n_values);
  }

  void run() noexcept {
    ::elastica::cosserat_rod::MatVecOp<K>::apply(vec_output, mat_input,
                                                 vec_input);
  }

  Mat mat_input;
  Vec vec_input;
  Vec vec_output;
};