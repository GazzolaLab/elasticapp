//
// Created by Tejaswin Parthasarathy on 1/3/21.
//

/// \file
/// Commonly used routines, functions and definitions shared amongst unit tests

#pragma once

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
