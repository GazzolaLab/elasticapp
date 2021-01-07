//
// Created by Tejaswin Parthasarathy on 1/4/21.
//

#pragma once

#include <random>
#include <sstream>

#include "Simulator/IO/Serialization/Serialize.hpp"
#include "Simulator/IO/Serialization/Types.hpp"
#include "Simulator/IO/Serialization/STL/string.hpp"
#include "SerializationTestHelpers.hpp"

template <class IArchive, class OArchive>
struct TestStringBasic {
  static inline void run() {
    std::random_device rd;
    std::mt19937 gen(rd());

    for (size_t i = 0; i < 10; ++i) {
      std::basic_string<char> o_string = random_basic_string<char>(gen);
      std::basic_string<char> o_string2 = "";
      std::basic_string<char> o_string3;

      std::ostringstream os;
      {
        OArchive oar(os);
        oar(o_string);
        oar(o_string2);
        oar(o_string3);
      }

      std::basic_string<char> i_string;
      std::basic_string<char> i_string2;
      std::basic_string<char> i_string3;

      std::istringstream is(os.str());
      {
        IArchive iar(is);

        iar(i_string);
        iar(i_string2);
        iar(i_string3);
      }

      CHECK(i_string == o_string);
      CHECK(i_string2 == o_string2);
      CHECK(i_string3 == o_string3);
    }
  }
};

template <class IArchive, class OArchive>
struct TestStringAll {
  static inline void run() {
    std::random_device rd;
    std::mt19937 gen(rd());

    for (size_t i = 0; i < 10; ++i) {
      std::basic_string<char> o_string = random_basic_string<char>(gen);
      std::basic_string<wchar_t> o_wstring = random_basic_string<wchar_t>(gen);
      std::basic_string<char16_t> o_u16string =
          random_basic_string<char16_t>(gen);
      std::basic_string<char32_t> o_u32string =
          random_basic_string<char32_t>(gen);

      std::ostringstream os;
      {
        OArchive oar(os);
        oar(o_string);
        oar(o_wstring);
        oar(o_u16string);
        oar(o_u32string);
      }

      std::basic_string<char> i_string;
      std::basic_string<wchar_t> i_wstring;
      std::basic_string<char16_t> i_u16string;
      std::basic_string<char32_t> i_u32string;

      std::istringstream is(os.str());
      {
        IArchive iar(is);

        iar(i_string);
        iar(i_wstring);
        iar(i_u16string);
        iar(i_u32string);
      }

      CHECK(i_string == o_string);
      CHECK(i_wstring == o_wstring);
      CHECK(i_u16string == o_u16string);
      CHECK(i_u32string == o_u32string);
    }
  }
};