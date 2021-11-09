//
// Created by Tejaswin Parthasarathy on 10/29/21.
//

#pragma once

#include <tuple>

#include "Options/CustomTypes.hpp"  // Vec3 support
#include "Options/Options.hpp"
#include "Options/Parameter.hpp"
#include "Systems/CosseratRods/Tags.hpp"
#include "Utilities/DefineTypes.h"
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/TMPL.hpp"
///
#include "Systems/CosseratRods/Initializers/detail/Common.hpp"
#include "Systems/CosseratRods/Initializers/detail/Transformations.hpp"

namespace elastica {

  namespace cosserat_rod {

    namespace detail {

      namespace option_tags {

        struct Radius : Options::Parameter<real_t> {
          using P = Options::Parameter<real_t>;
          using P::P;

          static constexpr Options::String help = {
              "Constant radius of the cosserat rod"};
          static type lower_bound() noexcept { return 0.0; }
          using TagType = ::elastica::tags::ElementDimension;
        };

        struct Length : Options::Parameter<real_t> {
          using P = Options::Parameter<real_t>;
          using P::P;
          static constexpr Options::String help = {
              "Length of the cosserat rod"};
          static type lower_bound() noexcept { return 0.0; }
        };

        struct Origin : Options::Parameter<Vec3> {
          using P = Options::Parameter<Vec3>;
          using P::P;
          static constexpr Options::String help = {
              "Position of first node of the cosserat rod"};
        };

        struct Direction : Options::Parameter<Vec3> {
          using P = Options::Parameter<Vec3>;
          using P::P;
          static constexpr Options::String help = {
              "Direction from start to end of the cosserat rod"};
        };

        struct Normal : Options::Parameter<Vec3> {
          using P = Options::Parameter<Vec3>;
          using P::P;
          static constexpr Options::String help = {
              "Common normal of all elements of the cosserat rod"};
        };

        struct TotalTwist : Options::Parameter<real_t> {
          using P = Options::Parameter<real_t>;
          using P::P;
          static constexpr Options::String help = {
              "(Total) Twist for the cosserat rod"};
        };

      }  // namespace option_tags

      class StraightRod {
       public:
        using Radius = option_tags::Radius;
        using Length = option_tags::Length;
        using Origin = option_tags::Origin;
        using Direction = option_tags::Direction;
        using Normal = option_tags::Normal;

        using RequiredParameters =
            tmpl::list<Radius, Length, Origin, Direction, Normal>;
        using DefaultParameters = tmpl::list<>;

       protected:
        template <typename OptionsTuple>
        decltype(auto) get_required_parameters(
            OptionsTuple const& options_cache) const /*noexcept*/ {
          // pass directly as references
          // return detail::pass_as_wrapped<tmpl::list<NElement,
          // Radius>>(data());
          return std::tuple_cat(
              // for things that don't need to be processed, pass as wrapped
              pass_as_wrapped(options_cache, tmpl::list<Radius>{}),
              // for things that do need processing, process them here and put
              // into a tuple
              // std::make_tuple(blocks::initialize<tags::Position>(
              //     [origin = std::get<Origin>(options_cache).value(),
              //      direction = std::get<Direction>(options_cache).value(),
              //      rod_length = std::get<Length>(options_cache).value(),
              //      n_elements =
              //          std::get<option_tags::NElement>(options_cache).value()](
              //         std::size_t index) {
              //       return static_cast<double>(index) + r;
              //     }))
              std::make_tuple()
              );
        }
      };

      class StraightRodWithTwist : public StraightRod {
       public:
        using TotalTwist = option_tags::TotalTwist;
        using RequiredParameters =
            tmpl::push_back<typename StraightRod::RequiredParameters,
                            TotalTwist>;
      };

    }  // namespace detail

    struct GeometryInitializers {
      using StraightRod = detail::StraightRod;
      using StraightRodWithTwist = detail::StraightRodWithTwist;
    };

  }  // namespace cosserat_rod

}  // namespace elastica