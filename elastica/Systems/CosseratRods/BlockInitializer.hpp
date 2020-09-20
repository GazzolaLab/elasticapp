#pragma once

#include <cstddef>
#include <tuple>
#include <type_traits>

///
#include "Systems/CosseratRods/Types.hpp"
///
#include "Systems/CosseratRods/CosseratRodTraits.hpp"
#include "Systems/CosseratRods/TypeTraits/Aliases.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  // We don't have a piecewise constructor for std::tuple
  // and hence as an alternative, we use this class
  template <typename CRT>  // Cosserat rod traits
  struct CosseratBlockInitializer {
    // Nested struct to have limited instantiations for different
    // rod traits
    template <typename... Initializers>
    struct Initializer {
      using InitializerType = std::tuple<Initializers...>;
      std::size_t n_elems;
      InitializerType initializers;

      // logic which was in traits class now gets shifted here
      template <typename PropertyTag>
      constexpr decltype(auto) get() noexcept {
        // Here initializers can be a ref too (in the case its not an
        // immediate lambda, hence we first remove reference here

        // TODO Is there no alternative than doing an O(n) search here?
        using find_result = tmpl::find<
            // find
            InitializerType,
            // condition
            std::is_same<
                // first
                tmpl::pin<
                    typename CRT::template initializer_tag_t<PropertyTag>>,
                // second
                tmpl::bind<CRT::template parameter_type_t,
                           tmpl::_1>>  // condition
            >;                         // find

        static_assert(tmpl::size<find_result>::value,
                      "Valid initializer not found!");

        using PropertyTagInitializer = tmpl::front<find_result>;

        // The trailing get is to return the generator from the enclosing
        // strong type
        return std::get<PropertyTagInitializer>(initializers).get();
      }
    };
  };

  namespace initializers{

  template <typename PropertyTag,
            typename CRT,  // Cosserat Rod Traits
            typename... Initializers>
  inline constexpr decltype(auto) get(
      typename CosseratBlockInitializer<CRT>::template Initializer<
          Initializers...>& initializers) noexcept {
    return initializers.template get<PropertyTag>();
  }

  template <typename PropertyTag,
            typename CRT,  // Cosserat Rod Traits
            typename... Initializers>
  inline constexpr decltype(auto) get(
      typename CosseratBlockInitializer<CRT>::template Initializer<
          Initializers...>&& initializers) noexcept {
    return initializers.template get<PropertyTag>();
  }

  }
  /*
  template <typename PropertyTag,
            typename CRT,  // Cosserat Rod Traits
            typename... Initializers>
  inline constexpr decltype(auto) get(
      typename CosseratBlockInitializer<CRT>::template Initializer<
          Initializers...> const& initializers) noexcept {
    return initializers.template get<PropertyTag>();
  }
  */

  // Ease in syntax compared to forward_as_tuple?
  template <typename CRT = ::elastica::detail_cosserat_rod::CosseratRodTraits,
            typename... Initializers>
  constexpr auto make_cosserat_rod(std::size_t n_elems,
                                   Initializers&&... initializers) {
    // for temporary initializers, Initializers get deduced as T&&, so we remove
    // cv-rvalue reference
    // for reference initializers, Initializers get deduced as T&, so we need to
    // remove cv-lvalue reference here
    // this is covered by decay (including array operations, which we really
    // should not be using)
    using ReturnType = typename CosseratBlockInitializer<
        CRT>::template Initializer<std::decay_t<Initializers>...>;
    return ReturnType{n_elems, std::forward_as_tuple(initializers...)};
  }

}  // namespace elastica