//
// Created by Tejaswin Parthasarathy on 10/10/21.
//

#pragma once

#include <vector>

template <typename T>
inline void append(std::vector<T> source, std::vector<T>& destination) {
  if (destination.empty())
    destination = std::move(source);
  else
    destination.insert(std::end(destination),
                       std::make_move_iterator(std::begin(source)),
                       std::make_move_iterator(std::end(source)));
}

template <typename T>
std::vector<T> flatten(std::vector<std::vector<T>> v) {
  std::vector<T> result;
  for (auto& sub : v) {
    append(std::move(sub), result);
  }
  return result;
}
