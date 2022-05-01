//==============================================================================
/*!
//  \file
//  \brief Utilities for handling type pairs
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once
// TODO : Documentation

#include <functional>

#include "Utilities/TMPL.hpp"

namespace elastica {

  // Compile-time algorithms first
  namespace detail {

    // Helper for removing matrix types
    template <typename S, typename E>
    struct RemovePairDuplicates;

    template <typename S, typename E1, typename E2>
    struct RemovePairDuplicates<S, tmpl::pair<E1, E2>> {
      using type = typename std::conditional<
          std::is_same<tmpl::index_of<S, tmpl::pair<E2, E1>>,
                       tmpl::no_such_type_>::value,
          tmpl::push_back<S, tmpl::pair<E1, E2>>, S>::type;
    };

    template <typename T>
    struct to_pair;  // converts to a meta pair, not as_pair from brigand

    template <typename T1, typename T2>
    struct to_pair<tmpl::list<T1, T2>> {
      using type = tmpl::pair<T1, T2>;
    };

  }  // namespace detail

  // only care about pairs
  template <typename List1, typename List2 = List1>
  using combinations_with_replacement =
      tmpl::transform<tmpl::cartesian_product<List1, List2>,
                      detail::to_pair<tmpl::_1>>;

  // not the most efficient
  template <typename List1, typename List2 = List1>
  using combinations =
      tmpl::fold<combinations_with_replacement<List1, List2>,  // seq
                 tmpl::list<>,                                 // initial value
                 detail::RemovePairDuplicates<tmpl::_state,
                                              tmpl::_element>>;  // meta-lambda

  // Runtime iteration

  // List is a tmpl::<pair<One, Two>>
  template <typename List, typename Functor>
  Functor pairwise_for_each(Functor f) {
    // use ref here maybe
    tmpl::for_each<List>([f = std::ref(f)](auto p) {
      // should be a pair
      using Pair = tmpl::type_from<decltype(p)>;
      f(tmpl::type_<typename Pair::first_type>{},
        tmpl::type_<typename Pair::second_type>{});
    });
    return f;
  }

}  // namespace elastica
