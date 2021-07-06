//
// Created by Tejaswin Parthasarathy on 5/20/21.
//

#pragma once

#include <sstream>
#include <vector>

#include "CustomStructs.hpp"
#include "SerializationTestHelpers.hpp"
#include "Simulator/IO/Serialization/Custom/blaze/DynamicVector.hpp"
#include "Simulator/IO/Serialization/Serialize.hpp"
#include "Simulator/IO/Serialization/Types.hpp"

template <typename IArchive, typename OArchive>
struct TestBlazeDynamicVector {
  static inline void run() {
    using type = blaze::DynamicVector<double>;

    for (int ii = 0; ii < 5; ++ii) {
      type short_vector(20, 0.0);
      blaze::randomize(short_vector);

      type long_vector(100, 0.0);
      blaze::randomize(long_vector);

      std::ostringstream os;
      {
        OArchive oar(os);

        oar(short_vector, long_vector);
      }

      type i_short_vector, i_long_vector;

      std::istringstream is(os.str());
      {
        IArchive iar(is);

        iar(i_short_vector, i_long_vector);
      }

      auto to_vec = [](auto&& blaze_struct) -> std::vector<double> {
        return std::vector<double>(
            blaze_struct.data(),
            blaze_struct.data() + blaze::capacity(blaze_struct));
      };

      CHECK_ITERABLE_APPROX(to_vec(short_vector), to_vec(i_short_vector));
      CHECK_ITERABLE_APPROX(to_vec(long_vector), to_vec(i_long_vector));
    }
  }
};
