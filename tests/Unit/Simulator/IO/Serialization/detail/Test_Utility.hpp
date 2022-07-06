#pragma once

#include <ostream>

#include "CustomStructs.hpp"
#include "Framework/TestingFramework.hpp"
#include "SerializationTestHelpers.hpp"
#include "Simulator/IO/Serialization/Serialize.hpp"
#include "Simulator/IO/Serialization/Types.hpp"

template <class IArchive, class OArchive>
struct TestUtility {
  static void run() {
    std::random_device rd;
    std::mt19937 gen(rd());

    auto rng = [&]() { return random_value<int>(gen); };
    auto rngS = [&]() { return random_basic_string<char>(gen); };

    for (int ii = 0; ii < 100; ++ii) {
      std::pair<int, int> o_podpair = {rng(), rng()};
      std::pair<TestA, TestA> o_iserpair = {{rng(), rng()}, {rng(), rng()}};
      std::pair<TestB, TestB> o_isplpair = {{rng(), rngS()}, {rng(), rngS()}};
      std::pair<TestA, TestB> o_eserpair = {{rng(), rng()}, {rng(), rngS()}};
      std::pair<TestB, TestA> o_esplpair = {{rng(), rngS()}, {rng(), rng()}};

      std::ostringstream os;
      {
        OArchive oar(os);

        oar(o_podpair);
        oar(o_iserpair);
        oar(o_isplpair);
        oar(o_eserpair);
        oar(o_esplpair);
      }

      std::pair<int, int> i_podpair;
      std::pair<TestA, TestA> i_iserpair;
      std::pair<TestB, TestB> i_isplpair;
      std::pair<TestA, TestB> i_eserpair;
      std::pair<TestB, TestA> i_esplpair;

      std::istringstream is(os.str());
      {
        IArchive iar(is);

        iar(i_podpair);
        iar(i_iserpair);
        iar(i_isplpair);
        iar(i_eserpair);
        iar(i_esplpair);
      }

      CHECK(i_podpair.first == o_podpair.first);
      CHECK(i_podpair.second == o_podpair.second);

      CHECK(i_iserpair.first == o_iserpair.first);
      CHECK(i_iserpair.second == o_iserpair.second);

      CHECK(i_isplpair.first == o_isplpair.first);
      CHECK(i_isplpair.second == o_isplpair.second);

      CHECK(i_eserpair.first == o_eserpair.first);
      CHECK(i_eserpair.second == o_eserpair.second);

      CHECK(i_esplpair.first == o_esplpair.first);
      CHECK(i_esplpair.second == o_esplpair.second);
    }
  }
};
