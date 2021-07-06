//
// Created by Tejaswin Parthasarathy on 1/4/21.
//

#pragma once

#include <sstream>  // for catch to have << defined

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

/*!
 * \brief Output the items of a TestA
 */
inline std::ostream& operator<<(std::ostream& os,
                                const TestA& test_a) noexcept {
  os << "( " << test_a.x << ", " << test_a.y << " )";
  return os;
}

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

/*!
 * \brief Output the items of a TestA
 */
inline std::ostream& operator<<(std::ostream& os,
                                const TestB& test_b) noexcept {
  os << "( " << test_b.x << ", " << test_b.y << " )";
  return os;
}

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
