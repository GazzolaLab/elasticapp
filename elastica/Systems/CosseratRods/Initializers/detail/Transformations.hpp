//
// Created by Tejaswin Parthasarathy on 10/29/21.
//

#pragma once

#include <stdexcept>
#include <string>
#include <tuple>
///
#include "Systems/CosseratRods/BlockInitializer.hpp"

namespace elastica {

  namespace cosserat_rod {

    namespace detail {

      template <typename Tag, typename Tuple>
      inline auto pass_as_wrapped(Tuple const& cache) noexcept {
        return blocks::initialize<typename Tag::TagType>(
            // do a copy here of tuple of initializers
            [value = (std::get<Tag>(cache).value())](
                std::size_t /* dummy index parameter*/) ->
            typename Tag::type { return value; });
      }

      template <typename Tuple, typename... ApplyTags>
      inline auto pass_as_wrapped(Tuple const& cache,
                                  tmpl::list<ApplyTags...> /* meta*/) noexcept {
        // forward_as_tuple makes references to temporary, so make tuple
        // instead. The move constructors should then kick in.
        return std::make_tuple(pass_as_wrapped<ApplyTags>(cache)...);
      }

      template <typename Tuple, typename... ApplyTags>
      inline auto check_lower_bounds(Tuple const& cache,
                                     tmpl::list<ApplyTags...> /* meta*/) {
        using L = tmpl::list<ApplyTags...>;
        tmpl::for_each<L>([&](auto v) {
          using ApplyTag = tmpl::type_from<decltype(v)>;
          if (std::get<ApplyTag>(cache).value() <= ApplyTag ::lower_bound()) {
            throw std::domain_error(std::string(ApplyTag::help) +
                                    " requested is too small!");
          }
        });
      }

      // inline auto concat_helps(char const* one, char const* two)
      //     -> std::string {
      //   std::string buf(one);
      //   buf.append(two);
      //   return buf;
      // }

    }  // namespace detail

  }  // namespace cosserat_rod

}  // namespace elastica
