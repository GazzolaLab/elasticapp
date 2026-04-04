//==============================================================================
/*!
//  \file
//  \brief Defines MakeString helper for making strings by streaming operation
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
//
// Reused from SpECTRE : https://spectre-code.org/
// Distributed under the MIT License.
// See LICENSE.txt for details.
*/
//==============================================================================


#pragma once

#include <ostream>
#include <sstream>
#include <string>

#include "Utilities/StlStreamDeclarations.hpp"

/*!
 * \ingroup UtilitiesGroup
 * \brief Make a string by streaming into object.
 *
 * \details
 * Stream values into the helper and convert with `static_cast<std::string>`.
 */
class MakeString {
 public:
  MakeString() = default;
  MakeString(const MakeString&) = delete;
  MakeString& operator=(const MakeString&) = delete;
  MakeString(MakeString&&) = default;
  MakeString& operator=(MakeString&&) = delete;
  ~MakeString() = default;

  // NOLINTNEXTLINE(google-explicit-constructor)
  operator std::string() const noexcept { return stream_.str(); }

  template <class T>
  friend MakeString operator<<(MakeString&& ms, const T& t) noexcept {
    // clang-tidy: can get unintentional pointer casts
    ms.stream_ << t;  // NOLINT
    return std::move(ms);
  }

  template <class T>
  friend MakeString& operator<<(MakeString& ms, const T& t) noexcept {
    // clang-tidy: can get unintentional pointer casts
    ms.stream_ << t;  // NOLINT
    return ms;
  }

 private:
  std::stringstream stream_{};
};

inline std::ostream& operator<<(std::ostream& os,
                                const MakeString& t) noexcept {
  return os << std::string{t};
}
