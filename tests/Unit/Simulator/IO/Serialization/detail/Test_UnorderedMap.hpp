//
// Created by Tejaswin Parthasarathy on 10/11/21.
//
#include <random>
#include <sstream>
#include <unordered_map>

#include "CustomStructs.hpp"
#include "SerializationTestHelpers.hpp"
#include "Simulator/IO/Serialization/STL/unordered_map.hpp"
#include "Simulator/IO/Serialization/Serialize.hpp"
#include "Simulator/IO/Serialization/Types.hpp"

// catch wants << declared and defined, so we inlcude STD helpers
#include "Utilities/StdHelpers.hpp"

template <typename IArchive, typename OArchive>
struct TestUnorderedMap {
  static inline void run() {
    std::random_device rd;
    std::mt19937 gen(rd());

    for (int ii = 0; ii < 10; ++ii) {
      std::unordered_map<std::size_t, int> o_podmap;

      for (std::size_t idx = 0UL; idx < 100UL; ++idx)
        o_podmap[idx] = random_value<int>(gen);

      std::unordered_map<std::size_t, TestA> o_testa;

      for (std::size_t idx = 0UL; idx < 50UL; ++idx)
        o_testa[idx] = TestA{random_value<int>(gen), random_value<int>(gen)};

      std::unordered_map<int, TestB> o_testb;
      for (int idx = -20; idx < 20; ++idx)
        o_testb[idx] =
            TestB{random_value<int>(gen), random_basic_string<char>(gen)};

      std::ostringstream os;
      {
        OArchive oar(os);

        oar(o_podmap);
        oar(o_testa);
        oar(o_testb);
      }

      std::unordered_map<std::size_t, int> i_podmap;
      std::unordered_map<std::size_t, TestA> i_testa;
      std::unordered_map<int, TestB> i_testb;

      std::istringstream is(os.str());
      {
        IArchive iar(is);

        iar(i_podmap);
        iar(i_testa);
        iar(i_testb);
      }

      CHECK(i_podmap.size() == o_podmap.size());
      CHECK(i_testa.size() == o_testa.size());
      CHECK(i_testb.size() == o_testb.size());

      CHECK(i_podmap == o_podmap);
      CHECK(i_testa == o_testa);
      CHECK(i_testb == o_testb);
    }
  }
};
