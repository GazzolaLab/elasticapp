//
// Created by Tejaswin Parthasarathy on 1/4/21.
//

#pragma once

#include <random>
#include <sstream>

#include "CustomStructs.hpp"
#include "SerializationTestHelpers.hpp"
#include "Simulator/IO/Serialization/Custom/TaggedTuple.hpp"
#include "Simulator/IO/Serialization/STL.hpp"
#include "Simulator/IO/Serialization/Serialize.hpp"
#include "Simulator/IO/Serialization/Types.hpp"
#include "Utilities/TaggedTuple.hpp"

struct name {
  using type = std::string;
};
struct moms_age {
  using type = int;
};
struct dads_age {
  using type = int;
};
struct email {
  using type = std::string;
};
struct parents {
  using type = std::vector<std::string>;
};
struct money {
  using type = float;
};

struct B1 {
  using type = TestB;
};
struct B2 {
  using type = TestB;
};

template <typename IArchive, typename OArchive>
struct TestTaggedTuple {
  static inline void run() {
    std::random_device rd;
    std::mt19937 gen(rd());
    auto rng = [&]() { return random_value<int>(gen); };

    for (int ii = 0; ii < 1; ++ii) {
      tuples::TaggedTuple<dads_age, moms_age> o_podtuple(rng(), rng());
      tuples::TaggedTuple<dads_age, money, email> o_spodtuple(18, 304.0,
                                                              "bla@bla.bla");
      tuples::TaggedTuple<name, dads_age, email, parents> o_atuple(
          "bla", 17, "bla@bla.bla", std::vector<std::string>{"Mom", "Dad"});
      tuples::TaggedTuple<B1, B2> o_btuple(
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
