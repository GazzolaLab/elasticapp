//
// Created by Tejaswin Parthasarathy on 10/29/21.
//

#pragma once

#include <cstddef>

#include "Options/Options.hpp"
#include "Options/Parameter.hpp"
#include "Simulator/Materials.hpp"
#include "Systems/CosseratRods/BlockInitializer.hpp"
#include "Systems/CosseratRods/Tags.hpp"
#include "Utilities/DefineTypes.h"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace cosserat_rod {

    namespace detail {

      namespace option_tags {

        // can be used for torque damping as well
        struct Material : Options::Parameter<::elastica::MaterialID> {
          using P = Options::Parameter<::elastica::MaterialID>;
          using P::P;
          static constexpr Options::String help = {
              "Material of the cosserat rod"};
          static type lower_bound() noexcept { return 0; }
        };

      }  // namespace option_tags

      namespace {

        struct VelocityDefaultParameter {
          using TagType = tags::Velocity;
          static auto initialize(...) -> real_t { return 0.0; }
          using type = decltype(blocks::initialize<TagType>(initialize));
          type data = blocks::initialize<TagType>(initialize);
        };

        struct AngularVelocityDefaultParameter {
          using TagType = tags::AngularVelocity;
          static auto initialize(...) -> real_t { return 0.0; }
          using type = decltype(blocks::initialize<TagType>(initialize));
          type data = blocks::initialize<TagType>(initialize);
        };

        struct AccelerationDefaultParameter {
          using TagType = tags::Acceleration;
          static auto initialize(...) -> real_t { return 0.0; }
          using type = decltype(blocks::initialize<TagType>(initialize));
          type data = blocks::initialize<TagType>(initialize);
        };

        struct AngularAccelerationDefaultParameter {
          using TagType = tags::AngularAcceleration;
          static auto initialize(...) -> real_t { return 0.0; }
          using type = decltype(blocks::initialize<TagType>(initialize));
          type data = blocks::initialize<TagType>(initialize);
        };

      }  // namespace

      class DefaultRodKinematics {
       public:
        using Material = option_tags::Material;
        using RequiredParameters = tmpl::list<Material>;
        using DefaultParameters =
            tmpl::list<VelocityDefaultParameter, AccelerationDefaultParameter,
                       AngularVelocityDefaultParameter,
                       AngularAccelerationDefaultParameter>;

       protected:
        template <typename OptionsTuple>
        decltype(auto) get_required_parameters(
            OptionsTuple const& options_cache) const noexcept {
          // pass directly as references
          // return detail::pass_as_wrapped<tmpl::list<NElement,
          // Radius>>(data());
          return std::tuple_cat(
              // for things that do need processing, process them here and put
              // into a tuple
              std::make_tuple());
        }
      };

    }  // namespace detail

    struct KinematicsInitializer {
      using DefaultRodKinematics = detail::DefaultRodKinematics;
    };

  }  // namespace cosserat_rod

}  // namespace elastica
