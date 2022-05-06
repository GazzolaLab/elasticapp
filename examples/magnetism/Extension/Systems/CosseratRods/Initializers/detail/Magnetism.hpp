//
// Created by Tejaswin Parthasarathy on 5/3/22.
//

#pragma once

#include <functional>
#include <stdexcept>
#include <string>
#include <tuple>
//
#include <elastica/Options/CustomTypes.hpp>  // Vec3 support
#include <elastica/Options/Options.hpp>
#include <elastica/Options/Parameter.hpp>
#include <elastica/Systems/CosseratRods/Initializers/detail/Common.hpp>
#include <elastica/Systems/CosseratRods/Initializers/detail/Transformations.hpp>
#include <elastica/Utilities/DefineTypes.h>
#include <elastica/Utilities/Math/Normalize.hpp>
#include <elastica/Utilities/Math/Vec3.hpp>
#include <elastica/Utilities/Math/Zero.hpp>
//
#include "Extension/Systems/CosseratRods/Tags.hpp"
//

namespace elastica {

  namespace cosserat_rod {

    namespace detail {

      namespace option_tags {

        // can be used for torque damping as well
        struct MagnetizationDensity : Options::Parameter<real_t> {
          using P = Options::Parameter<real_t>;
          using P::P;
          static constexpr Options::String help = {"Density of Magnetization"};
          static type lower_bound() noexcept { return 0.0; }
        };

        struct UniformMagnetizationDirectionInWorldFrame
            : Options::Parameter<Vec3> {
          using P = Options::Parameter<Vec3>;
          using P::P;
          static constexpr Options::String help = {
              "Direction of magnetization in world frame"};
        };

      }  // namespace option_tags

      class UniformMagnetizedRod {
       public:
        using MagnetizationDensity = option_tags::MagnetizationDensity;
        using UniformMagnetizationDirectionInWorldFrame =
            option_tags::UniformMagnetizationDirectionInWorldFrame;

        // the other required parameter comes from common
        using RequiredParameters =
            tmpl::list<MagnetizationDensity,
                       UniformMagnetizationDirectionInWorldFrame>;
        using DefaultParameters = tmpl::list<>;

        template <typename OptionsTuple>
        decltype(auto) get_required_parameters(
            OptionsTuple const& options_cache) const /*noexcept*/ {
          check_lower_bounds(options_cache, tmpl::list<MagnetizationDensity>{});

          auto zero_check = [&](auto v) {
            using Variable = tmpl::type_from<decltype(v)>;
            const Vec3 input = (std::get<Variable>(options_cache).value());
            if (::elastica::is_zero(input)) {
              throw std::logic_error(std::string(Variable::help) + " is zero!");
            }
            return input;
          };
          const Vec3 magnetization_direction = ::elastica::normalize(zero_check(
              tmpl::type_<UniformMagnetizationDirectionInWorldFrame>{}));

          // check if they are in the same direction
          if (::elastica::is_zero_length(magnetization_direction)) {
            throw std::logic_error(" Magnetization direction is singular!");
          }

          const auto magnetization_density =
              std::get<MagnetizationDensity>(options_cache).value();

          return std::make_tuple(blocks::initialize<tags::Magnetization>(
              [=](std::size_t /* meta index */) -> Vec3 {
                return Vec3{magnetization_density * magnetization_direction};
              }));
        }
      };

      namespace option_tags {

        using VariableMagnetization = std::function<Vec3(std::size_t)>;

        // can be used for torque damping as well
        struct NonUniformMagnetizationInWorldFrame
            : Options::Parameter<VariableMagnetization> {
          using P = Options::Parameter<VariableMagnetization>;
          using P::P;
          static constexpr Options::String help = {
              "Non-uniform magnetization vector in world frame"};
        };

      }  // namespace option_tags

      class NonUniformMagnetizedRod {
       public:
        using NonUniformMagnetizationInWorldFrame =
            option_tags::NonUniformMagnetizationInWorldFrame;

        // the other required parameter comes from common
        using RequiredParameters =
            tmpl::list<NonUniformMagnetizationInWorldFrame>;
        using DefaultParameters = tmpl::list<>;

        template <typename OptionsTuple>
        decltype(auto) get_required_parameters(
            OptionsTuple const& options_cache) const /*noexcept*/ {
          return std::make_tuple(blocks::initialize<tags::Magnetization>(
              std::get<NonUniformMagnetizationInWorldFrame>(options_cache)
                  .value()));
        }
      };

    }  // namespace detail

    struct MagnetismInitializers {
      using UniformMagnetizedRod = detail::UniformMagnetizedRod;
      using NonUniformMagnetizedRod = detail::NonUniformMagnetizedRod;
    };

  }  // namespace cosserat_rod

}  // namespace elastica
