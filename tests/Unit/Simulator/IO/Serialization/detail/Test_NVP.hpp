//
// Created by Tejaswin Parthasarathy on 7/4/22.
//

#pragma once

#include <ostream>

#include "Framework/TestingFramework.hpp"
#include "SerializationTestHelpers.hpp"
#include "Simulator/IO/Serialization/Serialize.hpp"
#include "Simulator/IO/Serialization/Types.hpp"

struct ordered_naming {
  int x;
  int xx;
  int y;
  int z;

  bool operator==(ordered_naming const& other) const {
    return x == other.x && xx == other.xx && y == other.y && z == other.z;
  }
};

template <>
struct elastica::io::Serialize<ordered_naming> {
  using type = ordered_naming;

  template <class Archive>
  static void save(Archive& ar, type const& data) {
    ar(MAKE_NVP(data.x), MAKE_NVP(data.z), MAKE_NVP(data.y), MAKE_NVP(data.xx));
  }

  template <class Archive>
  static void load(Archive& ar, type& data) {
    ar(MAKE_NVP(data.x), MAKE_NVP(data.z), MAKE_NVP(data.y), MAKE_NVP(data.xx));
  }
};

std::ostream& operator<<(std::ostream& os, ordered_naming const& s) {
  os << "[x: " << s.x << " xx: " << s.xx << " y: " << s.y << " z: " << s.z
     << "]";
  return os;
}

template <class IArchive, class OArchive>
struct TestNVP {
  static inline void run() {
    std::random_device rd;
    std::mt19937 gen(rd());

    auto rngB = [&]() { return random_value<int>(gen) % 2 == 0; };
    auto rngI = [&]() { return random_value<int>(gen); };
    auto rngF = [&]() { return random_value<float>(gen); };
    auto rngD = [&]() { return random_value<double>(gen); };
    auto rngS = [&]() { return random_basic_string<char>(gen); };

    for (int ii = 0; ii < 100; ++ii) {
      auto const name1 = rngS();
      auto const name2 = rngS();
      auto const name3 = rngS();
      auto const name4 = rngS();
      auto const name5 = rngS();
      auto const name6 = rngS();
      auto const name7 = rngS();

      int o_int1 = rngI();
      double o_double2 = rngD();
      std::vector<bool> o_vecbool3 = {rngB(), rngB(), rngB(), rngB(), rngB()};
      int o_int4 = rngI();
      int o_int5 = rngI();
      int o_int6 = rngI();
      std::pair<float, ordered_naming> o_un7;
      o_un7.first = rngF();
      o_un7.second.x = rngI();
      o_un7.second.xx = rngI();
      o_un7.second.y = rngI();
      o_un7.second.z = rngI();

      std::ostringstream os;
      {
        OArchive oar(os);

        oar(elastica::io::make_nvp(name1, o_int1),
            elastica::io::make_nvp(name2, o_double2),
            elastica::io::make_nvp(name3, o_vecbool3),
            elastica::io::make_nvp(name4, o_int4),
            elastica::io::make_nvp(name5, o_int5),
            elastica::io::make_nvp(name6, o_int6),
            elastica::io::make_nvp(name7, o_un7));
      }

      decltype(o_int1) i_int1;
      decltype(o_double2) i_double2;
      decltype(o_vecbool3) i_vecbool3;
      decltype(o_int4) i_int4;
      decltype(o_int5) i_int5;
      decltype(o_int6) i_int6;
      decltype(o_un7) i_un7;

      std::istringstream is(os.str());
      {
        IArchive iar(is);

        iar(elastica::io::make_nvp(name1, i_int1),
            elastica::io::make_nvp(name2, i_double2),
            elastica::io::make_nvp(name3, i_vecbool3),
            elastica::io::make_nvp(name4, i_int4),
            elastica::io::make_nvp(name5, i_int5),
            elastica::io::make_nvp(name6, i_int6),
            elastica::io::make_nvp(name7, i_un7));
      }

      CHECK(o_int1 == i_int1);
      CHECK(o_double2 == approx(o_double2));
      CHECK(o_vecbool3.size() == i_vecbool3.size());
      CHECK(i_vecbool3 == o_vecbool3);
      CHECK(o_int4 == i_int4);
      CHECK(o_int5 == i_int5);
      CHECK(o_int6 == i_int6);
      CHECK(o_un7.first == i_un7.first);
      CHECK(o_un7.second == i_un7.second);
    }
  }
};