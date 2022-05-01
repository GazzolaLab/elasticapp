#pragma once

#include <blaze/math/StaticMatrix.h>

#include "Utilities/DefineTypes.h"
namespace elastica {
  // TODO : customization here based on pe's template
  using Rot3 = blaze::StaticMatrix<real_t, 3UL, 3UL, blaze::rowMajor,
                                   blaze::unaligned, blaze::unpadded>;

}  // namespace elastica
