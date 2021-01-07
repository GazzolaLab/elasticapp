//
// Created by Tejaswin Parthasarathy on 1/1/21.
//

#pragma once

#include "Utilities/NamedType/Traits.hpp"
#include "Utilities/TMPL.hpp"

namespace testing {

  // placeholder
  struct _1 {};

  namespace detail {

    template <typename T>
    struct GetTraitHelper;

    template <template <typename> class A>
    struct GetTraitHelper<A<_1>> {
      using type = named_type::Traits<A>;
    };

    template <typename BoundAffordance>
    using trait_t = typename GetTraitHelper<BoundAffordance>::type;

    template <typename BoundAffordance, typename F>
    using criteria = typename trait_t<BoundAffordance>::template type<F>;

    template <typename BoundAffordance, typename ListToCheck>
    struct CheckGuaranteesHelper {
      using AffordanceTrait = trait_t<BoundAffordance>;

      template <typename F>
      struct Criteria {
        using type = typename AffordanceTrait::template type<F>;
        //      static_assert(type::value, "Wrong!");
      };

      // transform yields a list for evrey single affordance
      using type = tmpl::transform<ListToCheck, Criteria<tmpl::_1>>;
    };

    // check if the typelist satisfies guarantees needed by AffordanceList
  }  // namespace detail

  template <typename AffordanceList, typename ListToCheck>
  using CheckGuarantees = tmpl::flatten<tmpl::transform<
      AffordanceList,
      detail::CheckGuaranteesHelper<tmpl::_1, tmpl::pin<ListToCheck>>>>;

}  // namespace testing
