//
// Created by Tejaswin Parthasarathy on 6/12/21.
//

#pragma once

#include <array>
#include <random>
#include <sstream>

#include "CustomStructs.hpp"
#include "SerializationTestHelpers.hpp"
#include "Simulator/IO/Serialization/STL/array.hpp"
#include "Simulator/IO/Serialization/Serialize.hpp"
#include "Simulator/IO/Serialization/Types.hpp"

template <typename IArchive, typename OArchive>
struct TestArray {
  static inline void run() {
    std::random_device rd;
    std::mt19937 gen(rd());

    constexpr std::size_t array_size = 20UL;

    for (int ii = 0; ii < 10; ++ii) {
      std::array<int, array_size> o_podarray{};
      for (auto& elem : o_podarray)
        elem = random_value<int>(gen);

      std::array<bool, array_size> o_boolarray{};
      for (size_t i = 0; i < array_size; ++i)
        o_boolarray[i] = (random_value<int>(gen) % 2) == 0;

      std::array<TestA, array_size> o_testa{};
      for (auto& elem : o_testa)
        elem = TestA{random_value<int>(gen), random_value<int>(gen)};

      std::array<TestB, array_size> o_testb{};
      for (auto& elem : o_testb)
        elem = TestB{random_value<int>(gen), random_basic_string<char>(gen)};

      std::ostringstream os;
      {
        OArchive oar(os);

        oar(o_podarray, o_boolarray);
        oar(o_testa, o_testb);
      }

      std::array<int, array_size> i_podarray;
      std::array<bool, array_size> i_boolarray;
      std::array<TestA, array_size> i_testa;
      std::array<TestB, array_size> i_testb;

      std::istringstream is(os.str());
      {
        IArchive iar(is);

        iar(i_podarray, i_boolarray);
        iar(i_testa, i_testb);
      }

      CHECK(i_podarray == o_podarray);
      CHECK(i_boolarray == o_boolarray);
      CHECK(i_testa == o_testa);
      CHECK(i_testb == o_testb);
    }
  }
};
