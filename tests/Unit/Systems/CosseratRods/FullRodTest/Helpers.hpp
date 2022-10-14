#pragma once

#include "Utilities/DefineTypes.h"
#include "blaze/Blaze.h"
#include "blaze_tensor/Blaze.h"

// Helpers to expand a (3, n) to (3, 3, n) by filling only the diagonal
// entries and vice-versa
blaze::DynamicTensor<elastica::real_t> expand_to_diagonal(
    blaze::DynamicMatrix<elastica::real_t> const& mat);

inline blaze::DynamicTensor<elastica::real_t> const& expand_to_diagonal(
    blaze::DynamicTensor<elastica::real_t> const& t) {
  return t;
}

blaze::DynamicMatrix<elastica::real_t> shrink_using_diagonal(
    blaze::DynamicTensor<elastica::real_t> const& tensor);

inline blaze::DynamicMatrix<elastica::real_t> const& shrink_using_diagonal(
    blaze::DynamicMatrix<elastica::real_t> const& mat) {
  return mat;
}
