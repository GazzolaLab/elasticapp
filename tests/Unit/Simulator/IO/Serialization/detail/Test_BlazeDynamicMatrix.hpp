//
// Created by Tejaswin Parthasarathy on 5/20/21.
//

#pragma once

#include <sstream>
#include <vector>

#include "CustomStructs.hpp"
#include "SerializationTestHelpers.hpp"
#include "Simulator/IO/Serialization/Custom/blaze/DynamicMatrix.hpp"
#include "Simulator/IO/Serialization/Serialize.hpp"
#include "Simulator/IO/Serialization/Types.hpp"

template <typename IArchive, typename OArchive>
struct TestBlazeDynamicMatrix {
  static inline void run() {
    using type = blaze::DynamicMatrix<double>;

    for (int ii = 0; ii < 5; ++ii) {
      type square_matrix(20, 20);
      blaze::randomize(square_matrix);

      type mock_matrix(3, 100);
      blaze::randomize(mock_matrix);

      std::ostringstream os;
      {
        OArchive oar(os);

        oar(square_matrix, mock_matrix);
      }

      type i_square_matrix, i_mock_matrix;

      std::istringstream is(os.str());
      {
        IArchive iar(is);

        iar(i_square_matrix, i_mock_matrix);
      }

      auto to_vec = [](auto&& blaze_struct) -> std::vector<double> {
        return std::vector<double>(
            blaze_struct.data(),
            blaze_struct.data() + blaze::capacity(blaze_struct));
      };

      CHECK_ITERABLE_APPROX(to_vec(square_matrix), to_vec(i_square_matrix));
      CHECK_ITERABLE_APPROX(to_vec(mock_matrix), to_vec(i_mock_matrix));
    }
  }
};
