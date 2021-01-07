//
// Created by Tejaswin Parthasarathy on 1/4/21.
//

#pragma once

#include <random>
#include <type_traits>

/*
 * Copyright (c) 2014, Randolph Voorhies, Shane Grant
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of cereal nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL RANDOLPH VOORHIES OR SHANE GRANT BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// Random Number Generation ===============================================
template <class T>
inline typename std::enable_if<std::is_floating_point<T>::value, T>::type
random_value(std::mt19937& gen) {
  return std::uniform_real_distribution<T>(-10000.0, 10000.0)(gen);
}

template <class T>
inline typename std::enable_if<
    std::is_integral<T>::value && sizeof(T) != sizeof(char), T>::type
random_value(std::mt19937& gen) {
  return std::uniform_int_distribution<T>(std::numeric_limits<T>::lowest(),
                                          std::numeric_limits<T>::max())(gen);
}

template <class T>
inline typename std::enable_if<
    std::is_integral<T>::value && sizeof(T) == sizeof(char), T>::type
random_value(std::mt19937& gen) {
  return static_cast<T>(std::uniform_int_distribution<int64_t>(
      std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max())(gen));
}

template <class T>
inline typename std::enable_if<std::is_same<T, std::string>::value,
                               std::string>::type
random_value(std::mt19937& gen) {
  std::string s(std::uniform_int_distribution<unsigned int>(3, 30)(gen), ' ');
  for (char& c : s)
    c = static_cast<char>(std::uniform_int_distribution<int>('A', 'Z')(gen));
  return s;
}

template <class C>
inline std::basic_string<C> random_basic_string(std::mt19937& gen) {
  std::basic_string<C> s(std::uniform_int_distribution<unsigned int>(3, 30)(gen), ' ');
  for (C& c : s)
    c = static_cast<C>(std::uniform_int_distribution<int>('A', 'Z')(gen));
  return s;
}
