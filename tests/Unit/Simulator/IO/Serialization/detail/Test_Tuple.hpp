//
// Created by Tejaswin Parthasarathy on 1/4/21.
//

#pragma once

#include <random>
#include <sstream>
#include <tuple>

#include "CustomStructs.hpp"
#include "SerializationTestHelpers.hpp"
#include "Simulator/IO/Serialization/STL/tuple.hpp"
#include "Simulator/IO/Serialization/Serialize.hpp"
#include "Simulator/IO/Serialization/Types.hpp"

template <typename IArchive, typename OArchive>
struct TestTuple {
  static inline void run() {
    std::random_device rd;
    std::mt19937 gen(rd());
    auto rng = [&]() { return random_value<int>(gen); };

    for (int ii = 0; ii < 1; ++ii) {
      auto o_podtuple = std::make_tuple(rng(), rng(), rng(), rng());
      auto o_spodtuple = std::make_tuple(rng(), random_value<float>(gen),
                                         random_value<char>(gen));
      auto o_atuple = std::make_tuple(TestA{rng(), rng()}, TestA{rng(), rng()},
                                      TestA{rng(), rng()}, TestA{rng(), rng()});
      auto o_btuple =
          std::make_tuple(TestB{rng(), random_basic_string<char>(gen)},
                          TestB{rng(), random_basic_string<char>(gen)},
                          TestB{rng(), random_basic_string<char>(gen)},
                          TestB{rng(), random_basic_string<char>(gen)});

      std::ostringstream os;
      {
        OArchive oar(os);

        oar(o_podtuple);
        oar(o_spodtuple);
        oar(o_atuple);
        oar(o_btuple);
      }

      decltype(o_podtuple) i_podtuple;
      decltype(o_spodtuple) i_spodtuple;
      decltype(o_atuple) i_atuple;
      decltype(o_btuple) i_btuple;

      std::istringstream is(os.str());
      {
        IArchive iar(is);

        iar(i_podtuple);
        iar(i_spodtuple);
        iar(i_atuple);
        iar(i_btuple);
      }

      CHECK(o_podtuple == i_podtuple);
      CHECK(o_spodtuple == i_spodtuple);
      CHECK(o_atuple == i_atuple);
      CHECK(o_btuple == i_btuple);
    }
  }
};
