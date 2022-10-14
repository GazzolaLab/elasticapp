//
// Created by Tejaswin Parthasarathy on 9/7/22.
//
#include "Helpers.hpp"

blaze::DynamicTensor<elastica::real_t> expand_to_diagonal(
    blaze::DynamicMatrix<elastica::real_t> const& mat) {
  auto const n_elem = mat.columns();
  blaze::DynamicTensor<elastica::real_t> ret(3UL, 3UL, n_elem);
  ret = elastica::real_t(0.0);  // initialize just in case.
  for (auto idx = 0U; idx < n_elem; ++idx) {
    ret(0UL, 0UL, idx) = mat(0UL, idx);
    ret(1UL, 1UL, idx) = mat(1UL, idx);
    ret(2UL, 2UL, idx) = mat(2UL, idx);
  }
  return ret;
}

blaze::DynamicMatrix<elastica::real_t> shrink_using_diagonal(
    blaze::DynamicTensor<elastica::real_t> const& tensor) {
  auto const n_elem = tensor.columns();
  blaze::DynamicMatrix<elastica::real_t> ret(3UL, n_elem);
  ret = elastica::real_t(0.0);
  for (auto idx = 0U; idx < n_elem; ++idx) {
    ret(0UL, idx) = tensor(0UL, 0UL, idx);
    ret(1UL, idx) = tensor(1UL, 1UL, idx);
    ret(2UL, idx) = tensor(2UL, 2UL, idx);
  }
  return ret;
}
