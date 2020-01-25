#pragma once

#include "DataStructures/Tags.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits.hpp"

/*
 *  Groups contain a logical aggregation of tags
 *  Useful to quickly prototype new simulations
 */
namespace Groups {

  struct GenericBodyGroup {
    using construct_tags =
        tmpl::list<Tags::Position, Tags::Velocity, Tags::OneDimension>;
  };

  struct RadiusGroup {
    using construct_tags = Tags::OneDimension;
  };

  namespace detail {
    template <typename G, typename = cpp17::void_t<>>
    struct IsValidGroup : std::false_type {};

    template <typename G>
    struct IsValidGroup<G, cpp17::void_t<typename G::construct_tags>>
        : std::true_type {};
  }  // namespace detail

}  // namespace Groups

template <typename Group>
constexpr bool is_valid_group_v = Groups::detail::IsValidGroup<Group>::value;
