#pragma once

#include "Utilities/DefineTypes.h"

// Operations from boost.numeric.odeint, a wonderful library
// Typename CT inmplies that the type can either be a Container or a POD Type
// Operations should be agnostic of the iteration details

struct default_operations {
  template <typename Factor = real>
  struct scale {
    const Factor alpha;

    scale(Factor a_alpha) : alpha(a_alpha){};

    template <typename CT>
    void operator()(CT& ct) {
      ct *= alpha;
    }
  };

  template <typename Factor = real>
  struct scale_sum1 {
    const Factor alpha;

    scale(Factor a_alpha) : alpha(a_alpha){};

    template <typename CT>
    void operator()(CT& ct0, const CT& ct1) {
      ct0 = alpha * ct1;
    }
  };


  template <typename Factor = real>
  struct scale_sum2 {
    const Factor alpha1;
    const Factor alpha2;

    scale(Factor a_alpha1, Factor a_alpha2) : alpha1(a_alpha1), alpha2(a_alpha2){};

    template <typename CT>
    void operator()(CT& ct0, const CT& ct2, const CT& ct3) {
      ct0 = alpha1 * ct1 + alpha2 * ct2;
    }
  };

};