//
// Created by Tejaswin Parthasarathy on 1/4/21.
//

#pragma once

#include "Simulator/IO/Serialization/Serialize.hpp"
#include "Simulator/IO/Serialization/Types.hpp"

struct TestA {
  int x, y;
  bool operator==(TestA const& other) const {
    return x == other.x && y == other.y;
  }
  bool operator!=(TestA const& other) const {
    return x != other.x || y != other.y;
  }
};

struct TestB {
  int x;
  std::string y;
  bool operator==(TestB const& other) const {
    return x == other.x && y == other.y;
  }
  bool operator!=(TestB const& other) const {
    return x != other.x || y != other.y;
  }
};

template <>
struct elastica::io::Serialize<TestA> {
  using type = TestA;
  template <typename InputArchive>
  static inline void load(InputArchive& ar, type& data) {
    ar(data.x, data.y);
  }
  template <typename OutputArchive>
  static inline void save(OutputArchive& ar, type const& data) {
    ar(data.x, data.y);
  }
};

template <>
struct elastica::io::Serialize<TestB> {
  using type = TestB;
  template <typename InputArchive>
  static inline void load(InputArchive& ar, type& data) {
    ar(data.x, data.y);
  }
  template <typename OutputArchive>
  static inline void save(OutputArchive& ar, type const& data) {
    ar(data.x, data.y);
  }
};
