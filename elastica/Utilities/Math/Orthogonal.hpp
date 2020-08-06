//
// Created by Tejaswin Parthasarathy on 7/17/20.
//

#pragma once

#include <iostream>

#include "Equal.hpp"
#include "Identity.hpp"
#include "Normalize.hpp"
#include "Rot3.hpp"
#include "Length.hpp"
#include "Trans.hpp"
#include "elastica/ErrorHandling/Assert.hpp"

namespace elastica {

  auto orthogonalize(const Rot3& input_R) -> Rot3 {
    // TODO : very general, need not do a QR
    //  https://stackoverflow.com/a/23082112
    throw std::runtime_error("Not implemented");
    //    Rot3 Q, R;
    //    blaze::qr(input_R, Q, R);
    //    ELASTICA_ASSERT(input_R == Q * R, "QR fails");
    return input_R;
  }

  auto make_orthogonal_bases(const Vec3& arg_normal) noexcept -> Rot3 {
    // Normal is in the 2nd row, d2
    Rot3 R;
    // TODO : Doesn't work, seems like a bug in blaze
    Vec3 normal = normalize(arg_normal);
    //        Vec3 normal = arg_normal;

    // Any initial guess thats not in the normal direction
    //    Vec3 binormal{0.8, 0.6, 0.0};
    Vec3 binormal{1.0, 0.0, 0.0};

    Vec3 tangent(binormal % normal);

    // If its collinear, iterate till its not
    // should be parallel threshold but thats in simluator
    // and we want to make it not depend on simluator
    while (equal(length(tangent), 0.0)) {
      // TODO : Arbitrary, revisit
      binormal = normalize(binormal + 100 * tangent);
      tangent = binormal % normal;
    }

    // TODO : Doesn't work, seems like a bug in blaze
    //    blaze::row<0UL>(R) = binormal;
    //    blaze::row<1UL>(R) = normal;
    //    blaze::row<2UL>(R) = tangent;

    for (int j = 0UL; j < 3UL; ++j) {
      R(0, j) = binormal[j];
      R(1, j) = normal[j];
      R(2, j) = tangent[j];
    }

    std::cout << "R from orthgonal is " << std::endl << R << std::endl;

    return R;
  }

  inline auto is_orthogonal(const Rot3& input_R) -> bool {
    return is_identity(input_R * trans(input_R));
  }
}  // namespace elastica