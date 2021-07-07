//
// Created by Tejaswin Parthasarathy on 1/3/21.
//

/// \file
/// Commonly used routines, functions and definitions shared amongst unit tests

#pragma once

#include <sstream>

#include "Simulator/IO/Serialization/Archives.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits/HasEquivalence.hpp"

template <typename T>
struct counter {
  static int objects_created;
  static int objects_alive;
  static int objects_copied;
  static int objects_moved;

  counter() {
    ++objects_created;
    ++objects_alive;
  }

  counter(const counter&) {
    ++objects_created;
    ++objects_alive;
    ++objects_copied;
  }

  counter(counter&&) noexcept {
    ++objects_created;
    ++objects_alive;
    ++objects_moved;
  }

 protected:
  ~counter()  // objects should never be removed through pointers of this type
  {
    --objects_alive;
  }
};

template <typename T>
int counter<T>::objects_alive = 0;
template <typename T>
int counter<T>::objects_created = 0;
template <typename T>
int counter<T>::objects_copied = 0;
template <typename T>
int counter<T>::objects_moved = 0;

namespace lists {

  using arithmetics =
      tmpl::list<char, signed char, unsigned char, wchar_t, char16_t, char32_t,
                 int, unsigned int, long, unsigned long, long long,
                 unsigned long long, short, unsigned short, float, double,
                 long double, bool>;
  using signed_integers = tmpl::list<signed char, int, long, long long, short>;
  using unsigned_integers =
      tmpl::list<bool, char, unsigned char, unsigned int, unsigned long,
                 unsigned long long, unsigned short>;
  using integers = tmpl::append<unsigned_integers, signed_integers>;
  using floating_points = tmpl::list<float, double, long double>;

}  // namespace lists

template <typename T>
std::stringstream serialize(T const& t) {
  std::stringstream ss;                            // any stream can be used
  elastica::io::BinaryOutputArchive oarchive(ss);  //  Create an output archive
  oarchive(t);
  return ss;
}

template <typename T>
T deserialize(std::stringstream ss) {
  elastica::io::BinaryInputArchive iarchive(ss);  //  Create an output archive
  T result{};
  iarchive(result);
  return result;
}

/*!
 * \ingroup TestingFrameworkGroup
 * \brief Serializes and deserializes an object `t` of type `T`
 */
template <typename T>
T serialize_and_deserialize(const T& t) {
  static_assert(
      std::is_default_constructible<T>::value,
      "Cannot use serialize_and_deserialize if a class is not default "
      "constructible.");
  return deserialize<T>(serialize<T>(t));
}

/// \ingroup TestingFrameworkGroup
/// \brief Tests the serialization of comparable types
/// \example
/// \snippet Test_PupStlCpp11.cpp example_serialize_comparable
template <typename T>
void test_serialization(const T& t) {
  static_assert(::tt::has_equivalence_v<T>, "No operator== for T");
  CHECK(t == serialize_and_deserialize(t));
}

struct NonCopyable {
  constexpr NonCopyable() = default;
  constexpr NonCopyable(const NonCopyable&) = delete;
  constexpr NonCopyable& operator=(const NonCopyable&) = delete;
  constexpr NonCopyable(NonCopyable&&) = default;
  NonCopyable& operator=(NonCopyable&&) = default;
  ~NonCopyable() = default;
};
inline bool operator==(const NonCopyable& /*a*/,
                       const NonCopyable& /*b*/) noexcept {
  return true;
}
inline bool operator!=(const NonCopyable& a, const NonCopyable& b) noexcept {
  return not(a == b);
}
inline std::ostream& operator<<(std::ostream& os,
                                const NonCopyable& /*v*/) noexcept {
  return os << "NC";
}
