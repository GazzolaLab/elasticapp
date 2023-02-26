//
// Created by Tejaswin Parthasarathy on 9/30/22.
//

#pragma once

#include "Utilities/DefineTypes.h"
//
#include <cmath>

namespace elastica {

  namespace forcing {

    namespace activation {

      struct Sinusoid {
        real_t operator()(real_t s, real_t t) const {
          // maybe simplified even further
          return std::sin(angular_frequency_ * t - wave_number_ * s +
                          phase_shift_);
        }
        real_t angular_frequency_;
        real_t wave_number_;
        real_t phase_shift_;
      };

    }  // namespace activation

  }  // namespace forcing

}  // namespace elastica
