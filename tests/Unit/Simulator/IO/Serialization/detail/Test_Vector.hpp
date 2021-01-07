//
// Created by Tejaswin Parthasarathy on 1/4/21.
//

#pragma once

#include <random>
#include <sstream>
#include <vector>

#include "CustomStructs.hpp"
#include "SerializationTestHelpers.hpp"
#include "Simulator/IO/Serialization/Serialize.hpp"
#include "Simulator/IO/Serialization/Types.hpp"
#include "Simulator/IO/Serialization/STL/vector.hpp"

template <typename IArchive, typename OArchive>
struct TestVector {
  static inline void run() {
    std::random_device rd;
    std::mt19937 gen(rd());

    for (int ii = 0; ii < 100; ++ii) {
      std::vector<int> o_podvector(100);
      for (auto& elem : o_podvector)
        elem = random_value<int>(gen);

      std::vector<bool> o_boolvector;
      o_boolvector.resize(100);
      for (size_t i = 0; i < 100; ++i)
        o_boolvector[i] = (random_value<int>(gen) % 2) == 0;

      std::vector<TestA> o_testa(100);
      for (auto& elem : o_testa)
        elem = TestA{random_value<int>(gen), random_value<int>(gen)};

      std::vector<TestB> o_testb(100);
      for (auto& elem : o_testb)
        elem = TestB{random_value<int>(gen), random_basic_string<char>(gen)};

      std::ostringstream os;
      {
        OArchive oar(os);

        oar(o_podvector);
        oar(o_boolvector);
        oar(o_testa);
        oar(o_testb);
      }

      std::vector<int> i_podvector;
      std::vector<bool> i_boolvector;
      std::vector<TestA> i_testa;
      std::vector<TestB> i_testb;

      std::istringstream is(os.str());
      {
        IArchive iar(is);

        iar(i_podvector);
        iar(i_boolvector);
        iar(i_testa);
        iar(i_testb);
      }

      CHECK(i_podvector.size() == o_podvector.size());
      CHECK(i_boolvector.size() == o_boolvector.size());
      CHECK(i_testa.size() == o_testa.size());
      CHECK(i_testb.size() == o_testb.size());

      CHECK(i_podvector == o_podvector);
      CHECK(i_boolvector == o_boolvector);
      CHECK(i_testa == o_testa);
      CHECK(i_testb == o_testb);
    }
  }
};
