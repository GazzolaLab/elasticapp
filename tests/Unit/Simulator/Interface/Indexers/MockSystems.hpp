//
// Created by Tejaswin Parthasarathy on 1/12/21.
//

#pragma once

#include <stdexcept>

#include "Utilities/End.hpp"

/// [nothrow_mock_system]
struct NoThrowSystem {
  std::size_t check(std::size_t i) const noexcept { return i + 1; };
  std::size_t check(elastica::from_end f) const noexcept { return 200 - f.i; };
};
/// [nothrow_mock_system]

struct ThrowSystem {
  // Assume we have b indices
  std::size_t check(std::size_t i) const {
    if (i < n_indices) {
      return i;
    } else {
      throw std::out_of_range("index exceeds n_indices for MockSystem!");
    }
  };

  std::size_t check(elastica::from_end fe) const {
    if (fe.i < n_indices) {
      return n_indices - fe.i;
    } else {
      throw std::out_of_range("index is less than zero for MockSystem!");
    }
  };

  std::size_t n_indices = {4};
};
