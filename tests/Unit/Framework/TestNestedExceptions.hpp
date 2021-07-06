//
// Created by Tejaswin Parthasarathy on 6/19/21.
//

#pragma once

#include <exception>
#include <sstream>

#include "Utilities/Invoke.hpp"

// from
// https://github.com/catchorg/Catch2/issues/1918
void full_exception_stack_to_stream(
    const std::exception& ee,
    std::stringstream* full_exception_stack_stream_buffer = nullptr,
    bool exception_ee_is_first_in_stack = true);

#define CHECK_THROWS_NESTED_WITH(ExprToCheckForNestedException, ...)         \
  try {                                                                      \
    ExprToCheckForNestedException;                                           \
    using EXPR_SUPPOSED_TO_THROW_BUT_DOES_NOT_THROW = int;                   \
    throw EXPR_SUPPOSED_TO_THROW_BUT_DOES_NOT_THROW{}; /* NOLINT */          \
  } catch (const std::exception& ee) {                                       \
    std::stringstream full_exception_stack_stream_buffer;                    \
    full_exception_stack_to_stream(ee, &full_exception_stack_stream_buffer); \
    try {                                                                    \
      auto match_against_stream = [&](std::string const& to_be_matched) {    \
        CHECK_THAT(full_exception_stack_stream_buffer.str(),                 \
                   Catch::Contains(to_be_matched));                          \
      };                                                                     \
      cpp17::invoke(                                                         \
          [&](auto const&... test_strings) {                                 \
            (void)std::initializer_list<char>{                               \
                ((void)(match_against_stream(test_strings)), '0')...};       \
          },                                                                 \
          __VA_ARGS__);                                                      \
    } catch (...) {                                                          \
      CHECK(false);                                                          \
    }                                                                        \
  } catch (...) {                                                            \
    constexpr bool expr_supposed_to_throw_but_does_not_throw = false;        \
    REQUIRE(expr_supposed_to_throw_but_does_not_throw);                      \
  }
