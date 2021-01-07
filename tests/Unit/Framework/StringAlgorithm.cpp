//
// Created by Tejaswin Parthasarathy on 1/2/21.
//

#include "StringAlgorithm.hpp"

// drop in replacement for boost:algorithm::replace_all_copy
// https://stackoverflow.com/a/41894946
std::string replace_all_copy(std::string str, const std::string& old,
                             const std::string& new_s) {
  if (!old.empty()) {
    size_t pos = str.find(old);
    while ((pos = str.find(old, pos)) != std::string::npos) {
      str = str.replace(pos, old.length(), new_s);
      pos += new_s.length();
    }
  }
  return str;
}

