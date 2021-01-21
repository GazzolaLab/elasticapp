//
// Created by Tejaswin Parthasarathy on 1/12/21.
//

#pragma once

#include <stdexcept>

#include "Utilities/End.hpp"

/// [nothrow_mock_system]
struct NoThrowSystem {
  int check(int i) const noexcept { return i + 1; };
  int check(elastica::from_end f) const noexcept { return f.i + 1; };
};
/// [nothrow_mock_system]

struct ThrowSystem {
  // Assume we have b indices
  int check(int i) const {
    if (i < n_indices) {
      return i;
    } else {
      throw std::out_of_range("index exceeds n_indices for MockSystem!");
    }
  };

  int check(elastica::from_end fe) const {
    if (fe.i < n_indices) {
      return n_indices - fe.i;
    } else {
      throw std::out_of_range("index is less than zero for MockSystem!");
    }
  };

  int n_indices = {4};
};
