#pragma once

#include <limits>

#include "Utilities/DefineTypes.h"

struct Time {
  real value_ = std::numeric_limits<double>::signaling_NaN();
  double value() const noexcept { return value_; };
  Time() noexcept: value_(0.0){};
};