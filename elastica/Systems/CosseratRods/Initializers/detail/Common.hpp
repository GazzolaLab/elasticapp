//
// Created by Tejaswin Parthasarathy on 10/29/21.
//

#pragma once

#include "Options/Options.hpp"
#include "Options/Parameter.hpp"
#include "Systems/CosseratRods/Tags.hpp"
///
#include "Systems/CosseratRods/Initializers/detail/Transformations.hpp"

namespace elastica {

  namespace cosserat_rod {

    namespace detail {

      namespace option_tags {

        // struct Name : Options::Parameter<std::size_t> {
        //   static constexpr Options::String help = {
        //       "Number of elements in the cosserat rod"};
        //   static type lower_bound() noexcept { return 2UL; }
        //   using TagType = tags::NElement;
        // };

        struct NElement : public Options::Parameter<std::size_t> {
          using P = Options::Parameter<std::size_t>;
          using P::P;
          static constexpr Options::String help = {
              "Number of elements in the cosserat rod"};
          static type lower_bound() noexcept { return 2UL; }
          using TagType = tags::NElement;
        };

      }  // namespace option_tags

      class CommonInitializerToAllCosseratRods {
       public:
        using NElement = option_tags::NElement;
        using RequiredParameters = tmpl::list<NElement>;
        using DefaultParameters = tmpl::list<>;

       protected:
        template <typename OptionsTuple>
        decltype(auto) get_required_parameters(
            OptionsTuple const& options_cache) const noexcept {
          // for things that don't need to be processed, pass as wrapped
          return pass_as_wrapped(options_cache, tmpl::list<NElement>{});
        }
      };

    }  // namespace detail

  }  // namespace cosserat_rod

}  // namespace elastica