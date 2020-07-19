#pragma once

#include <blaze/math/StaticVector.h>

#include "elastica/Utilities/DefineTypes.h"

namespace elastica {
  //  using namespace blaze;
    using Vec3 = blaze::StaticVector<real_t, 3UL, blaze::rowVector>;

  // Mock blaze's interface to find the source of problems
//  struct Vec3 {
//    Vec3(real_t one, real_t two, real_t three) {
//      v_[0UL] = one;
//      v_[1UL] = two;
//      v_[2UL] = three;
//    }
//    Vec3(const Vec3& other) {
//      for (int i = 0; i < 3; ++i)
//        v_[i] = other.v_[i];
//    }
//
//    real_t operator[](::std::size_t idx) const noexcept { return v_[idx]; }
//
//    real_t v_[3UL];
//  };

}  // namespace elastica
