//
// Created by Tejaswin Parthasarathy on 1/20/22.
//

#pragma once

#include <blaze/math/DynamicMatrix.h>
#include <blaze_tensor/math/DynamicTensor.h>
#include <tuple>
//
#include "Systems/CosseratRods/Traits/Operations/BlazeBackend/InvRotateDivide/BaseTemplate.hpp"
#include "Systems/CosseratRods/Traits/Operations/BlazeBackend/InvRotateDivide/Blaze.hpp"
#include "Systems/CosseratRods/Traits/Operations/BlazeBackend/InvRotateDivide/Scalar.hpp"
#include "Utilities/DefineTypes.h"
#include "Utilities/Geometry/AxisAngleRotation.hpp"

template <::elastica::cosserat_rod::detail::InvRotateDivideKind K>
struct InvRotateDivideTimingSetup {
  using R = blaze::DynamicTensor<::elastica::real_t>;
  using Kappa = blaze::DynamicMatrix<::elastica::real_t>;
  using Length = blaze::DynamicVector<::elastica::real_t>;

  // can throw :(
  static auto setup(const std::size_t n_values) {
    R Q(3UL, 3UL, n_values);

    const ::elastica::Vec3 axis{1.0, 0.0, 1.0};
    for (std::size_t k(0UL); k < n_values; ++k) {
      ::blaze::columnslice(Q, k) =
          ::elastica::geometry::rotate_via_axis_angle(axis, k * M_PI / 20);
    }

    Kappa kappa(3UL, n_values - 1UL);

    Length temp(n_values - 1UL);
    blaze::randomize(temp);
    Length w(temp * temp);

    return std::make_tuple(std::move(Q), std::move(kappa), std::move(w));
  };

  explicit InvRotateDivideTimingSetup(const std::size_t n_values = 256UL) {
    std::tie(Q_input, kappa_output, length_input) = setup(n_values);
  }

  void run() noexcept {
    ::elastica::cosserat_rod::detail::InvRotateDivideOp<K>::apply(
        kappa_output, Q_input, length_input);
  }

  R Q_input;
  Kappa kappa_output;
  Length length_input;
};
