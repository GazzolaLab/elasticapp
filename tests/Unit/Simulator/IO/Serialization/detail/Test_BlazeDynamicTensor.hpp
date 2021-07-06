//
// Created by Tejaswin Parthasarathy on 5/20/21.
//

#pragma once

#include <sstream>
#include <vector>

#include "CustomStructs.hpp"
#include "SerializationTestHelpers.hpp"
#include "Simulator/IO/Serialization/Custom/blaze/DynamicTensor.hpp"
#include "Simulator/IO/Serialization/Serialize.hpp"
#include "Simulator/IO/Serialization/Types.hpp"

template <typename IArchive, typename OArchive>
struct TestBlazeDynamicTensor {
  static inline void run() {
    using type = blaze::DynamicTensor<double>;

    for (int ii = 0; ii < 1; ++ii) {
      type square_tensor(6, 6, 6);
      blaze::randomize(square_tensor);

      type mock_tensor(3, 4, 5);
      blaze::randomize(mock_tensor);

      std::ostringstream os;
      {
        OArchive oar(os);

        oar(square_tensor, mock_tensor);
      }

      type i_square_tensor, i_mock_tensor;

      std::istringstream is(os.str());
      {
        IArchive iar(is);

        iar(i_square_tensor, i_mock_tensor);
      }

      auto to_vec = [](auto&& blaze_struct) -> std::vector<double> {
        return std::vector<double>(
            blaze_struct.data(),
            blaze_struct.data() + blaze::capacity(blaze_struct));
      };

      CHECK_ITERABLE_APPROX(to_vec(square_tensor), to_vec(i_square_tensor));
      CHECK_ITERABLE_APPROX(to_vec(mock_tensor), to_vec(i_mock_tensor));
    }
  }
};
