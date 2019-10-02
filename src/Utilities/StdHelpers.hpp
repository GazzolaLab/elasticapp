#pragma once

#include <array>
#include <cstdio>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "DataStructures/TaggedTuple.hpp"
#include "Utilities/PrintHelpers.hpp"

namespace StdHelpers_detail {
  // Helper classes for operator<< for tuples
  template <size_t N>
  struct TuplePrinter {
    template <typename... Args>
    static std::ostream& print(std::ostream& os, const std::tuple<Args...>& t) {
      TuplePrinter<N - 1>::print(os, t);
      os << "," << std::get<N - 1>(t);
      return os;
    }
  };

  template <>
  struct TuplePrinter<1> {
    template <typename... Args>
    static std::ostream& print(std::ostream& os, const std::tuple<Args...>& t) {
      os << std::get<0>(t);
      return os;
    }
  };

  template <>
  struct TuplePrinter<0> {
    template <typename... Args>
    static std::ostream& print(std::ostream& os,
                               const std::tuple<Args...>& /*t*/) {
      return os;
    }
  };
}  // namespace StdHelpers_detail

template <typename T>
inline std::ostream& operator<<(std::ostream& os,
                                const std::vector<T>& v) noexcept {
  sequence_print_helper(os, std::begin(v), std::end(v));
  return os;
}

template <typename T, size_t N>
inline std::ostream& operator<<(std::ostream& os,
                                const std::array<T, N>& a) noexcept {
  sequence_print_helper(os, std::begin(a), std::end(a));
  return os;
}

// template <typename... Args>
// inline std::ostream& operator<<(std::ostream& os,
//                                const std::tuple<Args...>& t) noexcept {
//  os << "(";
//  StdHelpers_detail::TuplePrinter<sizeof...(Args)>::print(os, t);
//  os << ")";
//  return os;
//}

template <tuples::TagType TT, class MetaData, typename... Tags>
inline std::ostream& operator<<(
    std::ostream& os, tuples::TaggedTuple<TT, MetaData, Tags...>& tt) noexcept {
  os << "(\n";
  using expander = int[];
  expander{0, ((void)(os << Tags::desc() << '\n'
                         << std::get<Tags>(tt) << '\n'),
               0)...};
  return os << ")\n";
}

// template <typename... Args>
// inline std::ostream& operator<<(std::ostream& os,
//                                TaggedTuple<Args...>& tt) noexcept {
//  os << "(\n";
//  using expander = int[];
//  expander { 0, ( (void) (os << Args::desc() << '\n' << std::get<Args>(tt) <<
//  '\n'), 0) ... };
////  expander {0, ((void) (os << '2'), 0)};
//  os << ")\n";
//  return os;
//}
