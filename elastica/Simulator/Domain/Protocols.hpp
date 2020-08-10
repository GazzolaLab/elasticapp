#pragma once

#include <utility>
#include <type_traits>

namespace elastica {

  namespace protocols {

    /*!
     * \brief Has domain support functions.
     *
     * Requires the class has these member functions:
     * - `get_lower_bound`: Returns the lower bound of the domain
     * - `get_upper_bound`: Returns the upper bound of the domain
     */
    struct ComputationalDomain{
      template <typename ConformingType>
      struct test {
        // Try calling the `ConformingType::get_lower_bound` member function
        using lower_domain_return_type =
            decltype(std::declval<ConformingType&>().get_lower_bound());

        // Try calling the `ConformingType::get_lower_bound` member function
        using upper_domain_return_type =
            decltype(std::declval<ConformingType&>().get_upper_bound());

        // Try calling the `ConformingType::get_span` member function
        using span_return_type =
        decltype(std::declval<ConformingType&>().get_span());

        using bound_type = typename ConformingType::bound_type;

        // Check the return type of the member functions
        static_assert(
            std::is_same<lower_domain_return_type, bound_type>::value,
            "Not a conforming domain, doesn't properly implement `get_lower_bound`");
        static_assert(
            std::is_same<upper_domain_return_type, bound_type>::value,
            "Not a conforming domain, doesn't properly implement `get_upper_bound`");
        static_assert(
            std::is_same<span_return_type , bound_type>::value,
            "Not a conforming domain, doesn't properly implement `get_span`");
      };
    };

  }  // namespace protocols

}  // namespace elastica
