//
// Created by Tejaswin Parthasarathy on 1/3/21.
//

/// \file
/// Commonly used routines, functions and definitions shared amongst unit tests

#pragma once

#include "Utilities/TMPL.hpp"

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
