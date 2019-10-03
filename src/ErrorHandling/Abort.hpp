/// \file
/// Defines function ErrorHandling::abort.

#pragma once

#include <cstdio>
#include <exception>
#include <string>

namespace Parallel {
  /// Abort the program with an error message.
  [[noreturn]] inline void abort(const std::string& message) {
    printf("%s", message.c_str());
    // the following call is never reached, but suppresses the warning that
    // a 'noreturn' functions does return
    std::terminate();  // LCOV_EXCL_LINE
  }
}  // namespace Parallel
