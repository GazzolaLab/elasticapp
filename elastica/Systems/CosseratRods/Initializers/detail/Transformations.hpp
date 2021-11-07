//
// Created by Tejaswin Parthasarathy on 10/29/21.
//

#pragma once

#include <tuple>

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

    }  // namespace detail

  }  // namespace cosserat_rod

}  // namespace elastica
