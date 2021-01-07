//
// Created by Tejaswin Parthasarathy on 1/4/21.
//

#pragma once

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

#include <random>
#include <sstream>
#include <string>

#include "CustomStructs.hpp"
#include "SerializationTestHelpers.hpp"

template <typename IArchive, typename OArchive>
struct TestCustomStructs {
  static inline void run() {
    std::random_device rd;
    std::mt19937 gen(rd());

    for (int ii = 0; ii < 100; ++ii) {
      TestA o_testa = {random_value<int>(gen), random_value<int>(gen)};
      TestB o_testb = {random_value<int>(gen), random_basic_string<char>(gen)};

      std::ostringstream os;
      {
        OArchive oar(os);
        oar(o_testa, o_testb);
      }

      TestA i_testa;
      TestB i_testb;

      std::istringstream is(os.str());
      {
        IArchive iar(is);
        iar(i_testa, i_testb);
      }

      CHECK(i_testa == o_testa);
      CHECK(i_testb == o_testb);
    }
  }
};
