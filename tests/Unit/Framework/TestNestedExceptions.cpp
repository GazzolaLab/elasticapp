//
// Created by Tejaswin Parthasarathy on 6/19/21.
//

#include <catch.hpp> // RWQUIRE

#include "TestNestedExceptions.hpp"

void full_exception_stack_to_stream(const std::exception &ee,
                             std::stringstream *full_exception_stack_stream_buffer,
                             bool exception_ee_is_first_in_stack) {
  constexpr bool kRecursiveCallExceptionFirstInStackValue = false;
  REQUIRE(full_exception_stack_stream_buffer != nullptr);
  if (exception_ee_is_first_in_stack) {
    *full_exception_stack_stream_buffer << ee.what();
  }
  try {
    std::rethrow_if_nested(ee);
  } catch (const std::exception &ee) {
    *full_exception_stack_stream_buffer << ee.what() << '\n';
    full_exception_stack_to_stream(
        ee, full_exception_stack_stream_buffer, kRecursiveCallExceptionFirstInStackValue);
  } catch (...) {
  }
};