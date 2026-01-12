//
// Created by Tejaswin Parthasarathy on 9/18/22.
//

#pragma once

#include "Simulator/Environments/CollisionSystem/Detection/ContactCache.hpp"  // assume tested
#include "Utilities/Invoke.hpp"
#include "Utilities/TMPL.hpp"
//
#include <algorithm>
#include <iterator>
#include <set>
#include <utility>
#include <vector>

template <typename T1, typename T2, typename Alloc,
          template <typename, typename> class VectorLike>
auto collect_pairs(VectorLike<std::pair<std::reference_wrapper<T1>,
                                        std::reference_wrapper<T2>>,
                              Alloc> const& vec) {
  using SystemID = std::size_t;
  using UserID = std::size_t;
  using IDs = std::pair<UserID, SystemID>;
  // Two pair of IDs for each primitive.
  using RT = std::set<std::pair<IDs, IDs>>;

  RT rt{};

  auto make_IDs = [](auto const& ref_value) {
    return std::make_pair(ref_value.get().getID(),
                          ref_value.get().getSystemID());
  };

  auto make_IDpair = [=](auto const& ref_pair) {
    // HashGrid has no preferential ordering : so here we manually sort such
    // that the ID and systemID are sorted
    std::less<> comp{};
    auto f = make_IDs(ref_pair.first);
    auto s = make_IDs(ref_pair.second);
    if (comp(f, s))
      return std::make_pair(f, s);
    else
      return std::make_pair(s, f);
  };

  using std::begin;
  using std::end;
  // using prev(end()) segfaults
  std::transform(begin(vec), end(vec), std::inserter(rt, begin(rt)),
                 make_IDpair);
  return rt;
}

template <typename T1, typename T2, typename L, typename Strat1>
auto collect_pairs(elastica::ContactCache<L, Strat1> const& lhs) {
  return collect_pairs(lhs.template data<T1, T2>());
}

template <typename L, typename Strat1>
auto collect_pairs(elastica::ContactCache<L, Strat1> const& lhs) {
  using SystemID = std::size_t;
  using UserID = std::size_t;
  using IDs = std::pair<UserID, SystemID>;
  // Two pair of IDs for each primitive.
  using RT = std::set<std::pair<IDs, IDs>>;
  RT rt{};

  auto add_to_collection = [&](auto const& vec) {
    using std::begin;
    using std::end;
    auto results = collect_pairs(vec);
    rt.insert(std::make_move_iterator(begin(results)),
              std::make_move_iterator(end(results)));
  };
  std::apply(
      [=](auto&... v) { EXPAND_PACK_LEFT_TO_RIGHT(add_to_collection(v)); },
      lhs.data());
  return rt;
}
