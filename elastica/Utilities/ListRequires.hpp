#pragma once

#include "elastica/Utilities/Requires.hpp"

// Constraints a typelist by requiring predicate checks on types
template <template <typename...> class Predicate>
struct ListRequires {
  template <typename ToCheck, typename... Args>
  struct apply {
    using type = Requires<Predicate<ToCheck, Args...>::value>;
  };
};
